#ifndef BACKGROUNDUPDATERTHREAD_H
#define BACKGROUNDUPDATERTHREAD_H

#include <QThread>
#include <dscore.h>
#include <opencv2/opencv.hpp>
#include <fpstracker.h>

struct bgPixelState;

class BackgroundUpdaterThread : public QThread
{

public:
    BackgroundUpdaterThread();

private:
    int width, height;
    bgPixelState *bgpixels;
    cv::Mat bgmean;
    cv::Mat  bgstdev;
    uint16_t* depthPixels;

    uint16_t* pixelsDepthAcq;

    int curFrame;

    /* Debugging */
    cv::Mat backgroundStateDebug;

    /* Forbid copying */
    //BackgroundUpdaterThread &operator=(const BackgroundUpdaterThread &);
    //BackgroundUpdaterThread(const BackgroundUpdaterThread &);

protected:
    void run();
public:
    FPSTracker fps;

    /* Public methods */
    //BackgroundUpdaterThread(uint16_t* depthPixels);
    virtual ~BackgroundUpdaterThread();

    void setDynamicUpdate(bool dynamic);
    void captureBackground();

    void drawDebug();
    void update();
    cv::Mat &getBackgroundMean() { return bgmean; }
    cv::Mat &getBackgroundStdev()  { return bgstdev; }
};

#endif // BACKGROUNDUPDATERTHREAD_H
