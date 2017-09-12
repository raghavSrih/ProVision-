#include "provisionmain.h"

ProVisionMain::ProVisionMain()
{
}

void ProVisionMain::startProVisionApp() {

    browser.show();
    mqttClient = new mqttclientwrapper("m21.cloudmqtt.com", 13462, 3600, 0,  true, "ilamiyvz", "hC1RJUurBdMP");
    mqttClient->start();
    camerafactory = new CameraFactory();
    cameraSensor = camerafactory->getCamera(DS325);
    cameraSensor.initCamera();
    cameraSensor.startCamera();
    proCamCalib = new  procamcalibration(cameraSensor);
    aruco = new aruco_thread(cameraSensor, cv::aruco::DICT_4X4_50);

    bgthread = new BackgroundUpdaterThread();
    touchTracker = new IRDepthTouchTracker(getPixelsDepthAcqQVGA(), getPixelsConfidenceQVGA(), *bgthread);
    touchEventHandler = new TouchEventHandler(proCamCalib);
    //arUcoEventsHandler = new ArUcoEventsHandler(proCamCalib);

    //emitted by aruco_thread -> received by arucoeventhandler
    QObject::connect(aruco, SIGNAL(detectedMarkers(Point2fArray , numArray)), arUcoEventsHandler, SLOT(arucoUpdate(Point2fArray , numArray)), Qt::DirectConnection);

    QObject::connect(this, SIGNAL(sendSensorData_mqtt(char *, char *)), mqttClient, SLOT(publish(char *, char *)), Qt::DirectConnection);
    //emitted by touchtracker -> received by touchEventHandler
    QObject::connect(touchTracker, SIGNAL(sendTouchPoints(vector<FingerTouch>)), touchEventHandler, SLOT(receiveTouchPoints(vector<FingerTouch>)), Qt::DirectConnection);
    QObject::connect(touchEventHandler, SIGNAL(sendTouchSensorData(QJsonObject)), &browser, SLOT(receiveTouchEvents(QJsonObject)), Qt::DirectConnection);
    QObject::connect(arUcoEventsHandler, SIGNAL(sendArUcoSensorData(QJsonObject)), &browser, SLOT(receiveArUcoEvents(QJsonObject)), Qt::DirectConnection);
    //emitted by arucoEvenHhandler -> received by proVisionMain
    QObject::connect(arUcoEventsHandler, SIGNAL(sendArUcoSensorData(QJsonObject)), this, SLOT(receiveSensorData_internal(QJsonObject)), Qt::DirectConnection);
    //emitted by toucheventhandler -> received by provisionMain
    QObject::connect(touchEventHandler, SIGNAL(sendTouchSensorData(QJsonObject)), this, SLOT(receiveSensorData_internal(QJsonObject)), Qt::DirectConnection);
    QObject::connect(proCamCalib, SIGNAL(proCamCalibCompleted()), this, SLOT(calibrationCompleted()), Qt::DirectConnection);
    QObject::connect(mqttClient, SIGNAL(received(char *, char *)), this, SLOT(receiveSensorData_mqtt(char *, char *)) , Qt::DirectConnection);

    proCamCalib->start();
}

void ProVisionMain::calibrationCompleted(){
    /* Setup worker threads */
    bgthread->setDynamicUpdate(false);
    bgthread->start();
    bgthread->captureBackground();
    touchTracker->start();
    aruco->start();
}

//SLOT
void ProVisionMain::receiveSensorData_internal(QJsonObject data) {
    QJsonObject sensorData;
    sensorData.insert("magiclamp", data);
    sendSensorData(sensorData);
}

void ProVisionMain::sendSensorData(QJsonObject sensorData) {
    QJsonDocument doc(sensorData);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    QByteArray ba = strJson.toLatin1();
    char * c_str2 = ba.data();
    emit sendSensorData_mqtt("magiclamp/sensordata", c_str2);
}

//SLOT
void ProVisionMain::receiveSensorData_mqtt(char *topic, char *message) {
//    QJsonDocument doc(data);
//    QString strJson(doc.toJson(QJsonDocument::Compact));
//    qDebug("Received Data from MQTT : %s", strJson.toLatin1().constData());
    qDebug("Received Data from MQTT : %s", message);
}

ProVisionMain::~ProVisionMain() {
    delete camerafactory;
    delete mqttClient;
    delete aruco;
    delete bgthread;
    delete proCamCalib;
    delete touchTracker;
    delete touchEventHandler;
    delete arUcoEventsHandler;
    cameraSensor.stopCamera();
}


