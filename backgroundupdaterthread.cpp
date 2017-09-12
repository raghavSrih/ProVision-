#include "backgroundupdaterthread.h"
#include "fixedqueue.h"

typedef uint16_t depth_t;


/* Background updater configuration */
static const int HIST_SIZE = 128; // depth value history length, in frames
static const int HIST_QUEUE_SIZE = 128; // fixed queue capacity; use a power-of-two for best performance
static const int HIST_MIN = 10; // minimum number of frames required for stability
static const depth_t MIN_DEPTH = 100; // minimum valid depth value (below this = invalid)
static const depth_t MAX_DEPTH = 5000; // maximum valid depth value

/* Heuristic threshold for z-increase destabilization.
 * When the current pixel value is further than [THRESHOLD] z-values from the stable mean,
 * the pixel will be marked unstable (reflecting the fact that the previous stable object must
 * have moved off that point). */
static const float HEUR_Z_INCREASE_THRESHOLD = 5; /* z-values */
/* Heuristic threshold for stability - a window stdev below this value will be considered stable. */
static const float HEUR_STABLE_FACTOR = 40; /* mm / m^2 */
/* Heuristic threshold for rejecting halos. Increases in the stable mean which are less than this threshold will just be rejected.
 * Halos are an effect caused by multipath interference, and manifest as depth values which are greater than the true surface depth
 * in a radius around the halo-causing object (hovering over the surface). */
static const float HEUR_HALO_THRESHOLD = 5; /* mm */

static const int PIXELSKIP = 1; // 1/N pixels will be updated each frame; increase this to reduce CPU usage but increase latency

static const float INVALID_MEAN = 0;
static const float INVALID_STDEV = 1e6;

/* bgPixelState maintains a history window of pixel values for each pixel.
 * It uses the window state to determine if the pixel is "stable" or not,
 * and if it is stable, determines the mean and standard deviation of the pixel
 * values for background subtraction purposes. */
struct bgPixelState {
    /* I'd have loved to just use std::queue, but it is too damn slow. */
    fixedqueue<depth_t, HIST_QUEUE_SIZE> window;
    uint64_t cur_sum, cur_ssum;
    float cur_mean, cur_stdev;
    bool stable;

    bgPixelState() : window(), cur_sum(0), cur_ssum(0),
        cur_mean(INVALID_MEAN), cur_stdev(INVALID_STDEV),
        stable(false) {
    }

private:
    void remove_one() {
        if(window.empty())
            return;
        uint64_t ival = window.pop();
        cur_sum -= ival;
        cur_ssum -= ival * ival;
    }

    void add_one(depth_t val) {
        uint64_t ival = val;
        window.push(val);
        cur_sum += ival;
        cur_ssum += ival * ival;
    }
public:
    /* Update depth windows (fast) */
    void update(depth_t val) {
        if(val < MIN_DEPTH || val > MAX_DEPTH) {
            remove_one();
            return;
        }
        /* Valid depth value */
        if(window.size() == HIST_SIZE) {
            remove_one();
        }
        add_one(val);
    }

    /* Update window statistics (slow; call me less frequently) */
    bool update_stats(float *stable_mean, float *stable_stdev) {
        size_t n = window.size();
        if(n == 0) {
            cur_mean = INVALID_MEAN;
            cur_stdev = INVALID_STDEV;
            stable = false;
            return stable;
        }

        cur_mean = cur_sum / (float)n;
        cur_stdev = sqrtf(cur_ssum * n - cur_sum * cur_sum) / n;
        float temp1= *stable_mean + *stable_stdev * HEUR_Z_INCREASE_THRESHOLD;
        float temp2= HEUR_STABLE_FACTOR * powf((cur_mean / 1000.0f), 2);
        if(cur_mean > *stable_mean + *stable_stdev * HEUR_Z_INCREASE_THRESHOLD) {
            /* Current value is further than the stable value: mark the current pixel as unstable */
            stable = false;
            *stable_mean = INVALID_MEAN;
            *stable_stdev = INVALID_STDEV;
        }
        else if(cur_stdev > HEUR_STABLE_FACTOR * powf((cur_mean / 1000.0f), 2) || n < HIST_MIN) {
            stable = false;
        }
        else {
            stable = true;
            if(cur_mean > *stable_mean + HEUR_HALO_THRESHOLD || cur_mean < *stable_mean)  {
                *stable_mean = cur_mean;
                *stable_stdev = cur_stdev;
            }
        }
        return stable;
    }
};

void BackgroundUpdaterThread::run() {
    int lastDepthTimestamp = 0;
    int curDepthFrame = 0;
    fps.fps = 30; // estimated fps
    int frameCountPrevious = -1;
    uint16_t *depthPixels = pixelsDepthAcq;
    while(isRunning()) {
        // Check if the depth frame is new
        int frameCount = getFrameCount();
        if (frameCount > frameCountPrevious) {

            frameCountPrevious = frameCount;
            curDepthFrame++;
            fps.update();

            if(curFrame >= HIST_SIZE) {
                continue;
            }
            else if(curFrame == HIST_SIZE-1) {
                qInfo("Background updating Completed ! \n");
            }
            if(curFrame >= 0)
                curFrame++; // manual capture mode

            // Update background pixels based on new depth data            
            uint16_t *debugpx = (uint16_t *)backgroundStateDebug.data;
            float *means = (float *)bgmean.data;
            float *stdevs = (float *)bgstdev.data;
            const int n = width * height;
            int unstablePixels = 0;
            for(int i=0; i<n; i++) {
                bgpixels[i].update(depthPixels[i]);
                /* Update mean & stdev for a subset of pixels each frame to save CPU */
                if((i+curDepthFrame) % PIXELSKIP == 0) {
                    bool stable = bgpixels[i].update_stats(&means[i], &stdevs[i]);
                    if(!stable) {
                        unstablePixels++;
                    }
                    if(debugpx) {
                        // ABGR
                        debugpx[i] = ((stable ? 255 : 64) << 24) | (((int)(means[i]) & 0xff) << 8) | (((int)(stdevs[i] * 5) & 0xff));
                    }
                }
            }
           qInfo("Unstable pixel rate per frame : %f", (((float)unstablePixels/n)*100));
           means = NULL;
           stdevs = NULL;
        }
    }
}

BackgroundUpdaterThread::BackgroundUpdaterThread() {
        width = FORMAT_QVGA_WIDTH;
        height = FORMAT_QVGA_HEIGHT;
        pixelsDepthAcq = getPixelsDepthAcqQVGA();
        bgpixels = new bgPixelState[width * height];
        bgmean = cv::Mat(height, width, CV_32FC1);
        bgstdev = cv::Mat(height, width, CV_32FC1);
        curFrame = -1; //start off dynamic
        backgroundStateDebug = cv::Mat(height, width,CV_16UC1);
    }

/* update() and drawDebug() functions called from the main thread */
void BackgroundUpdaterThread::drawDebug() {
    cv::imshow( "Background", backgroundStateDebug );
}

void BackgroundUpdaterThread::update() {
    fps.tick();
}

void BackgroundUpdaterThread::setDynamicUpdate(bool dynamic) {
    if(dynamic)
        curFrame = -1;
    else
        curFrame = 0;
}

void BackgroundUpdaterThread::captureBackground() {
    qInfo("Background updating started ...\n");
    curFrame = 0;
}

BackgroundUpdaterThread::~BackgroundUpdaterThread() {
    terminate();
    wait();
    bgmean.release();
    bgstdev.release();
    delete depthPixels;

    delete pixelsDepthAcq;
    backgroundStateDebug.release();
    delete [] bgpixels;
    bgpixels = NULL;
    bgmean = NULL;
    bgstdev = NULL;
}
