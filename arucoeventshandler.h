#ifndef ARUCOEVENTSHANDLER_H
#define ARUCOEVENTSHANDLER_H
#include <QWidget>
#include <aruco_thread.h>
#include <opencv2/core.hpp>
#include <qmutex.h>
#include <QMap>
#include <QJsonObject>
#include <procamcalibration.h>

class ArUcoEventsHandler : public QObject
{
    Q_OBJECT
public:
    ArUcoEventsHandler(procamcalibration * );


public slots:
    void arucoUpdate(Point2fArray , numArray);

private:
    void sendSensorData(char * event , int id, float x, float y);

signals:
    void sendArUcoSensorData(QJsonObject data);

private:
    QMap< int , vector< cv::Point2f >> currentMarkers;
    QMap< int , vector< cv::Point2f >> previousMarkers;
    QMutex mutex;
    cv::Point2f markerPoint;
    procamcalibration * procamInstance;
};

#endif // ARUCOEVENTSHANDLER_H
