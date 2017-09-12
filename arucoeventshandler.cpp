#include "arucoeventshandler.h"

ArUcoEventsHandler::ArUcoEventsHandler(procamcalibration * procam)
{
    procamInstance = procam;
}

void ArUcoEventsHandler::arucoUpdate(vector< vector< cv::Point2f > > pointsP, vector< int > idsP){
    mutex.lock();
    vector< vector< cv::Point2f > > points = pointsP;
    vector< int > ids = idsP;
    if(ids.size() > 0) {
        for(unsigned long i=0; i < ids.size(); i++) {
            markerPoint = points[i][0];
            if(currentMarkers.isEmpty()) {
                currentMarkers.insert(ids[i], points[i]);
                sendSensorData("detected",ids[i],points[i][0].x,points[i][0].y);
            }
            else if(!currentMarkers.contains(ids[i])) {
                currentMarkers.insert(ids[i], points[i]);
                sendSensorData("detected",ids[i],points[i][0].x,points[i][0].y);
            }
            else {
                if(currentMarkers.contains(ids[i])) {
                    double curr_x = points[i][0].x;
                    double past_x = currentMarkers.value(ids[i])[0].x;
                    double diff_x = abs(curr_x - past_x);
                    double change_x = diff_x / past_x;

                    double curr_y = points[i][0].y;
                    double past_y = currentMarkers.value(ids[i])[0].y;
                    double diff_y = abs(curr_y - past_y);
                    double change_y = diff_y / past_y;

                    if(change_x > 0.1 || change_y > 0.1) {
                        int temp = 0;
                        currentMarkers[ids[i]] =  points[i];
                        sendSensorData("moved",ids[i],points[i][0].x,points[i][0].y);
                    }
                }
            }
            if(previousMarkers.contains(ids[i]))previousMarkers.remove(ids[i]);
        }
    }
    QMapIterator<int , vector< cv::Point2f >> j(previousMarkers);
    while (j.hasNext()) {
        j.next();
        if(previousMarkers.contains(j.key())){
            currentMarkers.remove(j.key());
            sendSensorData("lost",j.key(),-1,-1);
        }
    }
    previousMarkers.clear();
    previousMarkers = currentMarkers;
    QMapIterator<int , vector< cv::Point2f >> i(currentMarkers);
    while (i.hasNext()) {
        i.next();
        if(currentMarkers.contains(i.key())){
            previousMarkers.insert(i.key(), i.value());
        }
    }
    mutex.unlock();
}

void ArUcoEventsHandler::sendSensorData(char * event , int id, float x, float y) {
    cv::Point2d worldPoint = procamInstance->getDisplayPoint(cv::Point2f(x,y));
    QJsonObject arucoEvent = {};
    QJsonObject data = {};
    data.insert("event", event);
    data.insert("id", id);
    data.insert("x", (int)worldPoint.x);
    data.insert("y", (int)worldPoint.y);
    arucoEvent.insert("aruco", data);
    emit sendArUcoSensorData(arucoEvent);
}
