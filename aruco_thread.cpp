#include "aruco_thread.h"

aruco_thread::aruco_thread(CameraSensor camSensor , int dict)
{
    cameraSensor = camSensor;
    dictionary = cv::aruco::getPredefinedDictionary(dict);
    detectorParams = cv::aruco::DetectorParameters::create();
    detectorParams->adaptiveThreshWinSizeMin = 20;
    detectorParams->minMarkerPerimeterRate = 0.1;

    qRegisterMetaType<Point2fArray>("Point2fArray");
    qRegisterMetaType<numArray>("numArray");
}

aruco_thread::~aruco_thread()
{
    cv_ColorAcqImage_Original.release();
    cv_ColorAcqImage.release();
    delete pixelsColorStream;
    delete detectorParams;
    pixelsColorStream = NULL;
}

void aruco_thread::run() {
    int frameCount = 0;
    int frameCountPrevious = -1;
    int colorFrameHeight = cameraSensor.getColorFrameHeight();
    int colorframeWidth = cameraSensor.getColorFrameWidth();
    pixelsColorStream = cameraSensor.getColorStreamPtr();
    cv_ColorAcqImage_Original=cv::Mat(colorFrameHeight,colorframeWidth,CV_8UC3);
    while(isRunning()) {
        frameCount = cameraSensor.getCurrentFrameCount();
        if (frameCount == frameCountPrevious) {
            continue;
        }
        frameCountPrevious = frameCount;
        frameCount++;

        int countColor = 0;
        for (int i=0; i<colorFrameHeight; i++) {
            for (int j=0; j<colorframeWidth; j++) {
                cv_ColorAcqImage_Original.at<cv::Vec3b>(i,j)[2] = pixelsColorStream[3*countColor];
                cv_ColorAcqImage_Original.at<cv::Vec3b>(i,j)[1] = pixelsColorStream[3*countColor+1];
                cv_ColorAcqImage_Original.at<cv::Vec3b>(i,j)[0] = pixelsColorStream[3*countColor+2];
                countColor++;
            }
        }

        //Resize Mar to QVGA resolution
        cv::resize(cv_ColorAcqImage_Original,cv_ColorAcqImage, cv::Size(FORMAT_QVGA_WIDTH, FORMAT_QVGA_HEIGHT));
        cv::aruco::detectMarkers(cv_ColorAcqImage, dictionary, corners, ids, detectorParams, rejected);

        emit detectedMarkers(corners, ids);
    }
    cv_ColorAcqImage_Original.release();
    cv_ColorAcqImage.release();
    delete pixelsColorStream;
}
