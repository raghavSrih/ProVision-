#include "toucheventhandler.h"

TouchEventHandler::TouchEventHandler(procamcalibration * procam)
{
    procamInstance = procam;
    previousTouches.clear();
}

void TouchEventHandler::receiveTouchPoints(vector<FingerTouch> touchPoints){
    map<int, FingerTouch> newTouchMap;
    set<int> touchDown, touchUp;
    map<int, FingerTouch> allTouches;

    for(auto &i : touchMap) {
        allTouches[i.first] = i.second;
    }

    for(auto touch : touchPoints) {
        newTouchMap[touch.id] = touch;
        allTouches[touch.id] = touch;

        if(!touchMap.count(touch.id) || (!touchMap[touch.id].touched && touch.touched))
            touchDown.insert(touch.id);
    }

    for(auto &i : touchMap) {
        if(!newTouchMap.count(i.first) || (i.second.touched && !newTouchMap[i.first].touched))
            touchUp.insert(i.first);
    }

    touchMap = newTouchMap;
    processTouchPoints(touchMap);
}

void TouchEventHandler::processTouchPoints(map<int, FingerTouch> touchPoints){
    lostTouches.clear();
    lostTouches = previousTouches;
    QJsonArray touchStartArray = {};
    QJsonArray touchMoveArray= {};
    QJsonArray touchEndArray= {};
    for(auto &entry : touchPoints) {
        auto &touch = entry.second;
        if(touch.touched) {
            lostTouches.erase(touch.id);
            if(!(previousTouches.find(touch.id) != previousTouches.end())){
                previousTouches.insert(pair<int,FingerTouch>(touch.id,touch));
                touchStartArray.push_back(getTouchObject(touch.tip.x, touch.tip.y, touch.id));
            }
            else if((percentageChange(touch.tip.x,previousTouches.at(touch.id).tip.x )  > 5) &&
                    (percentageChange(touch.tip.y,previousTouches.at(touch.id).tip.y) > 5)){
                touchMoveArray.push_back(getTouchObject(touch.tip.x, touch.tip.y, touch.id));
            }
        }
    }
    for(auto elem : lostTouches) {
        touchEndArray.push_back(getTouchObject(elem.second.tip.x, elem.second.tip.y, elem.second.id));
        previousTouches.erase(elem.first);
    }
    sendSensorData("touchstart", touchStartArray);
    sendSensorData("touchmove", touchMoveArray);
    sendSensorData("touchend", touchEndArray);
}

int TouchEventHandler::percentageChange(float newVal, float oldVal){
    int retVal = ((newVal - oldVal) / oldVal ) * 100 ;
    return abs(retVal);
}

void TouchEventHandler::sendSensorData(char * event ,QJsonArray touchArray) {
    QJsonObject touchEvent = {};
    QJsonObject data = {};
    data.insert("event", event);
    data.insert("touchArray", touchArray);
    touchEvent.insert("touch", data);
    emit sendTouchSensorData(touchEvent);
}

QJsonObject TouchEventHandler::getTouchObject(float x, float y, int id) {
    cv::Point2d worldPoint = procamInstance->getDisplayPoint(cv::Point2f(x,y));
    QJsonObject data = {};
    data.insert("x", (int)worldPoint.x);
    data.insert("y", (int)worldPoint.y);
    data.insert("id", (int)id);
    return data;
}
