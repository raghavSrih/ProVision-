#ifndef FPSTRACKER_H
#define FPSTRACKER_H

#pragma once

#include <QElapsedTimer>
#include <QMutex>

class FPSTracker {
    QMutex mutex;

public:
    double fps;
    double lastTime;

    FPSTracker() : fps(0.0), lastTime(0.0) {}
    QElapsedTimer timer;

    void reset() {
        mutex.lock();
        fps = 0.0;
        timer.restart();
        lastTime = timer.elapsed();
        mutex.unlock();
    }

    /* Tick the FPS tracker.
     Call with a fixed period (e.g. every 1/60th of a second) */
    void tick(double alpha=0.10) {
        mutex.lock();

        double now = timer.elapsed();
        if(now - lastTime < 0.00001){
            mutex.unlock();
            return;
        }

        double newfps = 1000.0 / (now - lastTime);
        if(newfps < fps)
            fps += alpha * (newfps - fps);
        mutex.unlock();
    }

    /* Update the FPS tracker.
     Call when a new frame arrives. */
    void update(double alpha=0.10) {
        mutex.lock();

        double now = timer.elapsed();
        if(now - lastTime < 0.00001) {
            mutex.unlock();
            return;
        }
        double newfps = 1000.0 / (now - lastTime);
        fps += alpha * (newfps - fps);
        lastTime = now;
        mutex.unlock();
    }
};

#endif // FPSTRACKER_H
