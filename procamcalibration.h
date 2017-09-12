#ifndef PROCAMCALIBRATION_H
#define PROCAMCALIBRATION_H
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <stdio.h>
#include <QtDebug>
#include <dscore.h>
#include <applicationconstants.h>
#include <camera.h>
#include <QThread>

class procamcalibration : public QObject
{
    Q_OBJECT
public:
    procamcalibration(CameraSensor cameraSensor);
    struct MATRIX {
        float Divider;
        float An, Bn, Cn, Dn, En, Fn;
    };
    cv::Point2f getDisplayPoint(cv::Point2f camPoint);
    void drawDebug();
    void start();
private:

    CameraSensor cameraSensor;
    uint8_t* pixelsColorStream;
    IplImage *cv_confidenceImage=NULL;
    CvSize cv_szConfidence=cvSize(FORMAT_QVGA_WIDTH,FORMAT_QVGA_HEIGHT);
    cv::Mat calibeMarker[200];
    int colorFrameHeight;
    int colorframeWidth;

    void initArUco();
    void createMarkers();
    void displayMarkers();
    void detectMarkers();
    int setCalibrationMatrix();
    void SetNearestNeighbour(cv::Point2f camPoint);
    float dist(cv::Point2f p1, cv::Point2f p2);
    float area(cv::Point2f p1, cv::Point2f p2, cv::Point2f p3);
    bool readCameraParameters(string filename, cv::Mat &camMatrix, cv::Mat &distCoeffs);

signals:
    void proCamCalibCompleted();
};

#endif // PROCAMCALIBRATION_H
