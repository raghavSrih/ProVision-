
#include "procamcalibration.h"
using namespace std;

int displayWidth = 1920;
int displayHeight = 1080;
int markerSize = 250;
int offset = 50;
int calibeMarkerSize = 0;
cv::Ptr<cv::aruco::Dictionary> dictionary;
vector< int > ids;
vector< vector< cv::Point2f > > corners, rejected;
vector< cv::Vec3d > rvecs, tvecs;
cv::Ptr<cv::aruco::DetectorParameters> detectorParams;
cv::Point2f camPtTemp[3] , displayPtTemp[3];
procamcalibration::MATRIX * matrixPtr;
cv::Mat cv_ColorAcqImage_Original;
cv::Mat cv_ColorAcqImage;
cv::Point2f gridPoints[200][4], cameraPoints[200][4];

procamcalibration::procamcalibration(CameraSensor cameraSens)
{
    cameraSensor = cameraSens;
}

void procamcalibration::start() {
    colorFrameHeight = cameraSensor.getColorFrameHeight();
    colorframeWidth = cameraSensor.getColorFrameWidth();
    this->pixelsColorStream = cameraSensor.getColorStreamPtr();
    initArUco();
    displayMarkers();
    detectMarkers();
    //setCalibrationMatrix();
    cv::Point2f test1 = cv::Point2f(228, 134);
    cv::Point2f test2 = cv::Point2f(200, 134);
    cv::Point2f test3 = cv::Point2f(145, 77);
    cv::Point2f test4 = cv::Point2f(117, 76);
    cv::Point2f r1 = getDisplayPoint(test1);
    cv::Point2f r2 = getDisplayPoint(test2);
    cv::Point2f r3 = getDisplayPoint(test3);
    cv::Point2f r4 = getDisplayPoint(test4);
    qInfo("(%.0f,%.0f) = (%.0f,%.0f)", test1.x , test1.y, r1.x, r1.y);
    qInfo("(%.0f,%.0f) = (%.0f,%.0f)", test2.x , test2.y, r2.x, r2.y);
    qInfo("(%.0f,%.0f) = (%.0f,%.0f)", test3.x , test3.y, r3.x, r3.y);
    qInfo("(%.0f,%.0f) = (%.0f,%.0f)", test4.x , test4.y,r4.x, r4.y);
}

void procamcalibration::initArUco()
{
    matrixPtr = new MATRIX();
    dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100);
}

void procamcalibration::displayMarkers() {

    int colSize = displayWidth / (markerSize+offset);
    int rowSize = displayHeight / (markerSize+offset);
    calibeMarkerSize = colSize * rowSize;
    cv::Mat calibWindow = cv::Mat(displayHeight,displayWidth, CV_8UC1, cv::Scalar(255));

    int currX = offset , currY = offset ;
    for (int i=0; i < calibeMarkerSize; i++) {
        if (currX < displayWidth-offset-markerSize) {
            gridPoints[i][0].x = currX;
            gridPoints[i][1].x = currX+markerSize;
            gridPoints[i][2].x = currX+markerSize;
            gridPoints[i][3].x = currX;
            currX += offset+markerSize;
        }
        else if (currX > displayWidth-offset-markerSize) {
            gridPoints[i][0].x = offset;
            gridPoints[i][1].x = offset+markerSize;
            gridPoints[i][2].x = offset+markerSize;
            gridPoints[i][3].x = offset;
            currX = offset+offset+markerSize;
        }
    }
    currX = offset;
    currY = offset;

    for (int i=0; i < calibeMarkerSize; i++) {

        if (currX < displayWidth-offset-markerSize) {
            gridPoints[i][0].y = currY;
            gridPoints[i][1].y = currY;
            gridPoints[i][2].y = currY+markerSize;
            gridPoints[i][3].y = currY+markerSize;
            currX += offset+markerSize;
        }
        else if (currX > displayWidth-offset-markerSize) {
            currX = offset+offset+markerSize;
            currY += offset+markerSize;
            gridPoints[i][1].y = currY;
            gridPoints[i][2].y = currY;
            gridPoints[i][3].y = currY+markerSize;
            gridPoints[i][0].y = currY+markerSize;
        }
    }


    for (int i=0; i < calibeMarkerSize; i++) {
        cv::aruco::drawMarker(dictionary, i, markerSize, calibeMarker[i], 1);
    }

    for (int i=0; i < calibeMarkerSize; i++) {
        calibeMarker[i].copyTo(calibWindow(cv::Rect(gridPoints[i][0].x,gridPoints[i][0].y,markerSize,markerSize)));

    }
//cv::imwrite( "Gray_Image.png", calibWindow );
    //cv::startWindowThread();
    cvNamedWindow("calibWindow", CV_WINDOW_NORMAL);
    cvSetWindowProperty("calibWindow", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    cv::imshow("calibWindow", calibWindow);
    cv::waitKey(0);
}

void procamcalibration::detectMarkers() {
    //cv::Mat testImg = cv::imread("Gray_Image.png", CV_8UC1);
    detectorParams = cv::aruco::DetectorParameters::create();
    detectorParams->adaptiveThreshWinSizeMin = 20;
    detectorParams->minMarkerPerimeterRate = 0.1;
    //detectorParams->maxMarkerPerimeterRate = 5.0;
    //detectorParams->doCornerRefinement = true;
    //detectorParams->cornerRefinementWinSize = 11;
    int frameCount = 0;
    int frameCountPrevious = -1;
    cv_ColorAcqImage_Original=cv::Mat(colorFrameHeight,colorframeWidth,CV_8UC3);

    while(true) {
        // Check if the depth frame is new
        int frameCount = getFrameCount();
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
        if((int)ids.size() > 0) {
            for(int i=0; i < ids.size(); i++) {
                qInfo("Detected Marker Id %d" , ids[i]);
            }
        }
        if((int)ids.size() > (calibeMarkerSize/2)) {
            for(int i=0; i < ids.size(); i++) {
                qInfo("Detected Marker Id %d : Cornors (%.0f, %.0f) => GridPoints (%.0f, %.0f)", ids[i],corners[i][0].x,corners[i][0].y, gridPoints[ids[i]][0].x, gridPoints[ids[i]][0].y);
                cameraPoints[ids[i]][0] = corners[i][0];
                cameraPoints[ids[i]][1] = corners[i][1];
                cameraPoints[ids[i]][2] = corners[i][2];
                cameraPoints[ids[i]][3] = corners[i][3];
                continue; //only top left cornor point of each marker is taken
            }
            emit proCamCalibCompleted();
            cv::destroyWindow("calibWindow");
            break;
        }
        if(frameCount > 200) {
            qInfo("Detected markers : %d", ids.size());
            qInfo("Markers couldn't be detected :(");
            cv::destroyWindow("calibWindow");
            break;
        }
    }
}

void procamcalibration::drawDebug() {
    //cv::aruco::drawDetectedMarkers(cv_ColorAcqImage, corners, cv::noArray(), cv::Scalar(100, 0, 255));
    //cv::imshow("Acq Color",cv_ColorAcqImage);
}

int procamcalibration::setCalibrationMatrix() {
    uint8_t retTHRESHOLD = 0 ;
    // K = (Xs0 - Xs2)*(Ys1 - Ys2) - (Xs1 - Xs2)*(Ys0 - Ys2)
    matrixPtr->Divider = ((camPtTemp[0].x - camPtTemp[2].x) * (camPtTemp[1].y - camPtTemp[2].y)) -
                         ((camPtTemp[1].x - camPtTemp[2].x) * (camPtTemp[0].y - camPtTemp[2].y)) ;
    if( matrixPtr->Divider == 0 )
    {
        retTHRESHOLD = 1;
    }
    else
    {
        //                 (Xd0 - Xd2)*(Ys1 - Ys2) - (Xd1 - Xd2)*(Ys0 - Ys2)
        //            A = ---------------------------------------------------
        //                                   K
        matrixPtr->An = ((displayPtTemp[0].x - displayPtTemp[2].x) * (camPtTemp[1].y - camPtTemp[2].y)) -
                          ((displayPtTemp[1].x - displayPtTemp[2].x) * (camPtTemp[0].y - camPtTemp[2].y)) ;
        //                 (Xs0 - Xs2)*(Xd1 - Xd2) - (Xd0 - Xd2)*(Xs1 - Xs2)
        //            B = ---------------------------------------------------
        //                                   K
        matrixPtr->Bn = ((camPtTemp[0].x - camPtTemp[2].x) * (displayPtTemp[1].x - displayPtTemp[2].x)) -
                        ((displayPtTemp[0].x - displayPtTemp[2].x) * (camPtTemp[1].x - camPtTemp[2].x)) ;
        //                 Ys0*(Xs2*Xd1 - Xs1*Xd2) + Ys1*(Xs0*Xd2 - Xs2*Xd0) + Ys2*(Xs1*Xd0 - Xs0*Xd1)
        //            C = ----------------------------------------------------------------------------
        //                                   K
        matrixPtr->Cn = (camPtTemp[2].x * displayPtTemp[1].x - camPtTemp[1].x * displayPtTemp[2].x) * camPtTemp[0].y +
                        (camPtTemp[0].x * displayPtTemp[2].x - camPtTemp[2].x * displayPtTemp[0].x) * camPtTemp[1].y +
                        (camPtTemp[1].x * displayPtTemp[0].x - camPtTemp[0].x * displayPtTemp[1].x) * camPtTemp[2].y ;
        //                 (Yd0 - Yd2)*(Ys1 - Ys2) - (Yd1 - Yd2)*(Ys0 - Ys2)
        //            D = ---------------------------------------------------
        //                                   K
        matrixPtr->Dn = ((displayPtTemp[0].y - displayPtTemp[2].y) * (camPtTemp[1].y - camPtTemp[2].y)) -
                        ((displayPtTemp[1].y - displayPtTemp[2].y) * (camPtTemp[0].y - camPtTemp[2].y)) ;
        //                 (Xs0 - Xs2)*(Yd1 - Yd2) - (Yd0 - Yd2)*(Xs1 - Xs2)
        //            E = ---------------------------------------------------
        //                                   K
        matrixPtr->En = ((camPtTemp[0].x - camPtTemp[2].x) * (displayPtTemp[1].y - displayPtTemp[2].y)) -
                        ((displayPtTemp[0].y - displayPtTemp[2].y) * (camPtTemp[1].x - camPtTemp[2].x)) ;
        //                 Ys0*(Xs2*Yd1 - Xs1*Yd2) + Ys1*(Xs0*Yd2 - Xs2*Yd0) + Ys2*(Xs1*Yd0 - Xs0*Yd1)
        //            F = ----------------------------------------------------------------------------
        //                                   K
        matrixPtr->Fn = (camPtTemp[2].x * displayPtTemp[1].y - camPtTemp[1].x * displayPtTemp[2].y) * camPtTemp[0].y +
                        (camPtTemp[0].x * displayPtTemp[2].y - camPtTemp[2].x * displayPtTemp[0].y) * camPtTemp[1].y +
                        (camPtTemp[1].x * displayPtTemp[0].y - camPtTemp[0].x * displayPtTemp[1].y) * camPtTemp[2].y ;
    }
    return( retTHRESHOLD ) ;

}

cv::Point2f procamcalibration::getDisplayPoint(cv::Point2f cameraPoints) {

    SetNearestNeighbour(cameraPoints);

    setCalibrationMatrix();

    cv::Point2f retDisplayPoint = cv::Point2f(0,0);
    if( matrixPtr->Divider != 0 )
    {
        // XD = AX+BY+C
        retDisplayPoint.x = ( (matrixPtr->An * cameraPoints.x) +
                          (matrixPtr->Bn * cameraPoints.y) +
                           matrixPtr->Cn
                         ) / matrixPtr->Divider ;
        // YD = DX+EY+F
        retDisplayPoint.y = ( (matrixPtr->Dn * cameraPoints.x) +
                          (matrixPtr->En * cameraPoints.y) +
                           matrixPtr->Fn
                         ) / matrixPtr->Divider ;
    }
    return retDisplayPoint;
}

void procamcalibration::SetNearestNeighbour(cv::Point2f point) {
    cv::Point2f nearest1 = cameraPoints[0][0];
    cv::Point2f nearest2 = cameraPoints[0][1];
    cv::Point2f nearest3 = cameraPoints[0][2];
    displayPtTemp[0] = gridPoints[0][0];
    displayPtTemp[1] = gridPoints[0][1];
    displayPtTemp[2] = gridPoints[0][2];
    for (int i = 1; i < calibeMarkerSize; i++) {
        for (int j = 0; j < 4; j++) {
            float temp = dist(cameraPoints[i][j], point);
            float d1p = dist(nearest1 , point);
            float d2p = dist(nearest2 , point);
            float d3p = dist(nearest3 , point);
            if( temp < d1p && area(gridPoints[i][j] , displayPtTemp[1] , displayPtTemp[2]) > 0) {
                camPtTemp[0] = cameraPoints[i][j];
                displayPtTemp[0] =  gridPoints[i][j];
            }
            else if( temp < d2p && area(gridPoints[i][j] , displayPtTemp[0] , displayPtTemp[2]) > 0) {
                camPtTemp[1] = cameraPoints[i][j];
                displayPtTemp[1] =  gridPoints[i][j];
            }
            else if( temp < d3p && area(gridPoints[i][j] , displayPtTemp[1] , displayPtTemp[0]) > 0) {
                camPtTemp[2] = cameraPoints[i][j];
                displayPtTemp[2] =  gridPoints[i][j];
            }
        }
    }
}

float procamcalibration::dist(cv::Point2f p1, cv::Point2f p2){
          return (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y);
}

float procamcalibration::area(cv::Point2f p0, cv::Point2f p1, cv::Point2f p2){
    float dArea = ((p1.x - p0.x)*(p2.y - p0.y) - (p2.x - p0.x)*(p1.y - p0.y))/2.0;
    return (dArea > 0.0) ? dArea : -dArea;
}
