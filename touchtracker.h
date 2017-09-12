#ifndef TOUCHTRACKER_H
#define TOUCHTRACKER_H

#pragma once

#include <dscore.h>
#include <opencv2/opencv.hpp>

#include <fpstracker.h>
#include <backgroundupdaterthread.h>
#include <touch.h>

class TouchTracker: public QThread {
private:
    /* Forbid copying */
    TouchTracker &operator=(const TouchTracker &);
    TouchTracker(const TouchTracker &);

protected:
    const int w, h;
    uint16_t* depthStream;
    uint16_t* irStream;
    BackgroundUpdaterThread &background;

    QMutex touchLock;
    bool touchesUpdated;
    vector<FingerTouch> touches;
    int nextTouchId;

public:
    FPSTracker fps;

    TouchTracker(uint16_t * depthStream, uint16_t * irStream, BackgroundUpdaterThread &background)
            : w(FORMAT_QVGA_WIDTH), h(FORMAT_QVGA_HEIGHT), background(background) {
            touchesUpdated = false;
            nextTouchId = 1;
        }

    /* The responsibility of stopping the thread is in the subclass: it must be the first thing the destructor does. */
    virtual ~TouchTracker() {}

    virtual void drawDebug() {}
    virtual bool update(vector<FingerTouch> &retTouches) = 0;
};

#endif // TOUCHTRACKER_H
