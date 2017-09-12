#ifndef PROVISIONMAIN_H
#define PROVISIONMAIN_H

#include <QtWidgets>
#include "webdisplaywindow.h"
#include <qtwebenginewidgetsglobal.h>
#include "opencv2/opencv.hpp"
#include <unistd.h>
#include "backgroundupdaterthread.h"
#include <touchtracker.h>
#include <irdepthtouchtracker.h>
#include <inttypes.h>
#include <procamcalibration.h>
#include <camera.h>
#include <camerafactory.h>
#include <mqttclientwrapper.h>
#include <aruco_thread.h>
#include <toucheventhandler.h>
#include <QJsonObject>
#include <arucoeventshandler.h>

using namespace std;
typedef vector< vector< cv::Point2f > > Point2fArray;
typedef vector< int > numArray;

class ProVisionMain : public QObject
{
    Q_OBJECT
public:
    ProVisionMain();
    ~ProVisionMain();

    void startProVisionApp();
    void sendSensorData_mqtt(QJsonObject);    
    void sendSensorData(QJsonObject);

public slots:
    void calibrationCompleted();
    void receiveSensorData_internal(QJsonObject);
    void receiveSensorData_mqtt(char *, char *);

signals:
    void sendSensorData_mqtt(char *topic, char * message);


private:
    CameraFactory * camerafactory = NULL;
    CameraSensor cameraSensor;
    mqttclientwrapper * mqttClient = NULL;
    aruco_thread * aruco = NULL;
    BackgroundUpdaterThread *bgthread = NULL;
    procamcalibration * proCamCalib = NULL;
    //WebDisplayWindow *browser = NULL;
    TouchTracker *touchTracker = NULL;
    TouchEventHandler *touchEventHandler = NULL;
    ArUcoEventsHandler *arUcoEventsHandler = NULL;
    WebDisplayWindow browser;

};

#endif // PROVISIONMAIN_H
