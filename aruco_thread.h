#ifndef ARUCO_THREAD_H
#define ARUCO_THREAD_H
#include <QObject>
#include <QThread>
#include <opencv2/aruco.hpp>
#include <camera.h>
#include <acquisitionparameters.h>
using namespace std;
typedef vector< vector< cv::Point2f > > Point2fArray;
typedef vector< int > numArray;

class aruco_thread : public QThread
{
    Q_OBJECT
public:
    aruco_thread(CameraSensor camSensor , int dict);
    ~aruco_thread();
private:
    void run();
protected:    
    cv::Ptr<cv::aruco::DetectorParameters> detectorParams;
    vector< vector< cv::Point2f > > corners, rejected;
    vector< int > ids;
    cv::Ptr<cv::aruco::Dictionary> dictionary;
    CameraSensor cameraSensor;
    uint8_t * pixelsColorStream = NULL;
    cv::Mat cv_ColorAcqImage_Original, cv_ColorAcqImage;

signals:
    void detectedMarkers(Point2fArray , numArray);
};

#endif // ARUCO_THREAD_H
