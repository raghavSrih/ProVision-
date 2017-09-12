#include<camera.h>
#include <dscore.h>
#include <unistd.h>


DepthSense_DS325::DepthSense_DS325() {

}

DepthSense_DS325::~DepthSense_DS325() {

}

void DepthSense_DS325::initCamera(){

    buildColorSyncFlag = dispColorSyncFlag || saveColorSyncFlag;
    buildDepthSyncFlag = dispDepthSyncFlag || saveDepthSyncFlag;
    buildConfidenceFlag = 1;//dispConfidenceFlag || saveConfidenceFlag;

    flagColorFormat = FORMAT_VGA_ID; // VGA, WXGA or NHD


    switch (flagColorFormat) {
        case FORMAT_VGA_ID:
            widthColor = FORMAT_VGA_WIDTH;
            heightColor = FORMAT_VGA_HEIGHT;
            break;
        case FORMAT_WXGA_ID:
            widthColor = FORMAT_WXGA_WIDTH;
            heightColor = FORMAT_WXGA_HEIGHT;
            break;
        case FORMAT_NHD_ID:
            widthColor = FORMAT_NHD_WIDTH;
            heightColor = FORMAT_NHD_HEIGHT;
            break;
        case FORMAT_QVGA_ID:
            widthColor = FORMAT_QVGA_WIDTH;
            heightColor = FORMAT_QVGA_HEIGHT;
            break;
        default:
            printf("Unknown flagColorFormat");
            exit(EXIT_FAILURE);
    }


    if (interpolateDepthFlag) {
        widthDepthAcq = FORMAT_VGA_WIDTH;
        heightDepthAcq = FORMAT_VGA_HEIGHT;
    } else {
        widthDepthAcq = FORMAT_QVGA_WIDTH;
        heightDepthAcq = FORMAT_QVGA_HEIGHT;
    }


}

void DepthSense_DS325::startCamera(){
    start_capture(flagColorFormat,
                  interpolateDepthFlag,
                  buildColorSyncFlag, buildDepthSyncFlag, buildConfidenceFlag);

    pixelsColorAcq = getPixelsColorsAcq();
    pixelsDepthSync = getPixelsDepthSync();
    pixelsConfidenceQVGA = getPixelsConfidenceQVGA();
    if (interpolateDepthFlag) {
        pixelsDepthAcq = getPixelsDepthAcqVGA();
        pixelsColorSync = getPixelsColorSyncVGA();
    } else {
        pixelsDepthAcq = getPixelsDepthAcqQVGA();
        pixelsColorSync = getPixelsColorSyncQVGA();
    }
    usleep(100);
};

uint8_t  * DepthSense_DS325::getColorStreamPtr(){
    return pixelsColorAcq;
}
uint16_t * DepthSense_DS325::getDepthStreamPtr(){
    return pixelsDepthAcq;
}
uint16_t * DepthSense_DS325::getIRStreamPtr(){
    return pixelsConfidenceQVGA;
}
int DepthSense_DS325::getCurrentFrameCount(){
    return getFrameCount();
}
int DepthSense_DS325::getDepthFrameHeight() {
    return heightDepthAcq;
}
int DepthSense_DS325::getDepthFrameWidth(){
    return widthDepthAcq;
}
int DepthSense_DS325::getColorFrameHeight(){
    return heightColor;
}
int DepthSense_DS325::getColorFrameWidth(){
    return widthColor;
}
void DepthSense_DS325::stopCamera(){
    stop_capture();
    delete pixelsDepthAcq;
    delete pixelsColorSync;
    delete pixelsColorAcq;
    delete pixelsDepthSync;
    delete pixelsConfidenceQVGA;
    pixelsDepthAcq = NULL;
    pixelsColorSync = NULL;
    pixelsColorAcq = NULL;
    pixelsDepthSync = NULL;
    pixelsConfidenceQVGA = NULL;
};
