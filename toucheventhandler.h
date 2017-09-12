#ifndef TOUCHEVENTHANDLER_H
#define TOUCHEVENTHANDLER_H

#include <touchtracker.h>
#include<QWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <procamcalibration.h>

class TouchEventHandler : public QObject
{
    Q_OBJECT
public:
    TouchEventHandler(procamcalibration * );

public slots:
    void receiveTouchPoints(vector<FingerTouch> touchPoints);
signals:
    void sendTouchSensorData(QJsonObject touchEvents);

private:
    void processTouchPoints(map<int, FingerTouch>);
    void sendSensorData(char * event ,QJsonArray touchArray);
    int percentageChange(float newVal, float oldVal);
    QJsonObject getTouchObject(float x, float y, int id);
private:
    map<int, FingerTouch> touchMap;
    map<int, FingerTouch> previousTouches, lostTouches;
    procamcalibration * procamInstance;    
};

#endif // TOUCHEVENTHANDLER_H
