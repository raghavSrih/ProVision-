#ifndef IRDEPTHTOUCHTRACKER_H
#define IRDEPTHTOUCHTRACKER_H

#pragma once

#include <dscore.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include <touchtracker.h>

struct IRDepthTip {
    vector<unsigned> pixels;
    vector<unsigned> roots; // pixels next to midconf/highconf pixels
};

struct IRDepthFinger {
    float x, y, z;
};

struct IRDepthHand {
    vector<IRDepthFinger> fingers;
};

struct IRDepthArm {
    vector<IRDepthHand> hands;
};

class IRDepthTouchTracker : public TouchTracker {
    Q_OBJECT
protected:
    void run();

    /* Edgemap construction */
    void buildEdgeImage();
    // Fill holes in the irCanny image
    void fillIrCannyHoles();

    /* Touch tracking stages */
    void buildDiffImage();

    void rejectBlob(const vector<unsigned> &blob, int reason=0);

    int nextBlobId;
    vector<IRDepthArm> detectTouches();
    bool floodArm(IRDepthArm &arm, unsigned idx);
    bool floodHand(IRDepthHand &hand, unsigned idx);
    bool floodFinger(IRDepthFinger &finger, unsigned idx);
    bool floodTip(IRDepthTip &tip, unsigned idx);
    void refloodFinger(const vector<unsigned> &blob, vector<unsigned> &roots);
    bool computeFingerMetrics(IRDepthFinger &finger, vector<unsigned> &px);

    vector<FingerTouch> mergeTouches(vector<FingerTouch> &curTouches, vector<FingerTouch> &newTouches);
private:
    /* Double-buffered images for display's sake */
    int front;
    cv::Mat diffIm[2]; // depth difference image; A=valid B=zone [0=noise/negative 64=close 128=medium 192=far] GR=diff
    cv::Mat edgeIm[2]; // edge image; B=IRedge G=depthedge R=depthabs
    cv::Mat blobIm[2]; // blob image; B=flags G=blobidx R=dist
    cv::Mat irCanny; // temporary image for canny purposes

public:
    IRDepthTouchTracker(uint16_t * depthStream, uint16_t * irStream, BackgroundUpdaterThread &background);
    virtual ~IRDepthTouchTracker();

    virtual void drawDebug();
    virtual bool update(vector<FingerTouch> &retTouches);

signals:
    void sendTouchPoints(vector<FingerTouch>);
};

#endif // IRDEPTHTOUCHTRACKER_H
