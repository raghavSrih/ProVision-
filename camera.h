#ifndef CAMERA_H
#define CAMERA_H
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <memory>

class Camera {
  public:
    virtual void initCamera() = 0;
    virtual void startCamera() = 0;
    virtual void stopCamera() = 0;
    //virtual void startColorStreamAcquisition() = 0;
    //virtual void stopColorStreamAcquisition()  = 0;
    //virtual void startDepthStreamAcquisition() = 0;
    //virtual void stopDepthStreamAcquisition()  = 0;
    virtual uint8_t  * getColorStreamPtr() = 0;
    virtual uint16_t * getDepthStreamPtr() = 0;
    virtual uint16_t * getIRStreamPtr() = 0;
    virtual int getCurrentFrameCount() = 0;
    virtual int getDepthFrameHeight() = 0;
    virtual int getDepthFrameWidth() = 0;
    virtual int getColorFrameHeight() = 0;
    virtual int getColorFrameWidth() = 0;
    //virtual unsigned long long int getCurrentColorFrameCount() = 0;
    //virtual bool isDepthStreamAvailable() = 0;
    //virtual bool isColorStreamAvailable() = 0;
    //virtual void setDepthFrameRate() = 0;
    //virtual void setColorFrameRate() = 0;
};

class Kinect2 : public Camera {
public:
    Kinect2();
    ~Kinect2();

    void initCamera();
    virtual void startCamera();
    virtual void stopCamera();
    virtual uint8_t  * getColorStreamPtr();
    virtual uint16_t * getDepthStreamPtr();
    virtual uint16_t * getIRStreamPtr();
    virtual int getCurrentFrameCount();
    virtual int getDepthFrameHeight();
    virtual int getDepthFrameWidth();
    virtual int getColorFrameHeight();
    virtual int getColorFrameWidth();
};

class RealsenseR200 : public Camera {
public:
    RealsenseR200();
    ~RealsenseR200();

    void initCamera();
    virtual void startCamera();
    virtual void stopCamera();
    virtual uint8_t  * getColorStreamPtr();
    virtual uint16_t * getDepthStreamPtr();
    virtual uint16_t * getIRStreamPtr();
    virtual int getCurrentFrameCount();
    virtual int getDepthFrameHeight();
    virtual int getDepthFrameWidth();
    virtual int getColorFrameHeight();
    virtual int getColorFrameWidth();
};

class RealsenseSR300 : public Camera {
public:
    void initCamera();
    virtual void startCamera();
    virtual void stopCamera();
    virtual uint8_t  * getColorStreamPtr();
    virtual uint16_t * getDepthStreamPtr();
    virtual uint16_t * getIRStreamPtr();
    virtual int getCurrentFrameCount();
    virtual int getDepthFrameHeight();
    virtual int getDepthFrameWidth();
    virtual int getColorFrameHeight();
    virtual int getColorFrameWidth();
};

class DepthSense_DS325 : public Camera {
public:
    DepthSense_DS325();
    ~DepthSense_DS325();

    void initCamera();
    virtual void startCamera();
    virtual void stopCamera();
    virtual uint8_t  * getColorStreamPtr();
    virtual uint16_t * getDepthStreamPtr();
    virtual uint16_t * getIRStreamPtr();
    virtual int getCurrentFrameCount();
    virtual int getDepthFrameHeight();
    virtual int getDepthFrameWidth();
    virtual int getColorFrameHeight();
    virtual int getColorFrameWidth();

protected:
  uint16_t* pixelsDepthAcq = NULL;
  uint8_t* pixelsColorSync = NULL;
  uint8_t* pixelsColorAcq = NULL;
  uint16_t* pixelsDepthSync = NULL;
  uint16_t* pixelsConfidenceQVGA = NULL;
  bool interpolateDepthFlag = 0;

  bool dispColorAcqFlag   = 1;
  bool dispDepthAcqFlag   = 0;
  bool dispColorSyncFlag  = 0;
  bool dispDepthSyncFlag  = 0;
  bool dispConfidenceFlag = 1;

  bool buildColorSyncFlag = 0;
  bool buildDepthSyncFlag = 0;
  bool buildConfidenceFlag = 0;

  bool saveColorAcqFlag   = 0;
  bool saveDepthAcqFlag   = 0;
  bool saveColorSyncFlag  = 0;
  bool saveDepthSyncFlag  = 0;
  bool saveConfidenceFlag = 0;
  int widthDepthAcq, heightDepthAcq;
  int widthColor, heightColor;
  int flagColorFormat;
};

class CameraSensor
{
public:
    CameraSensor() {
    }

    ~CameraSensor() {
    }

    void setDeviceType(Camera* newCamera)
    {
        _camera = newCamera;
    }

    void initCamera() {
        _camera->initCamera();
    }

    void startCamera() {
        _camera->startCamera();
    }

    void stopCamera() {
        _camera->stopCamera();
    }

    uint8_t  * getColorStreamPtr(){
        return _camera->getColorStreamPtr();
    }

    uint16_t * getDepthStreamPtr(){
        return _camera->getDepthStreamPtr();
    }

    uint16_t * getIRStreamPtr() {
        return _camera->getIRStreamPtr();
                                }

    int getCurrentFrameCount(){
        return _camera->getCurrentFrameCount();
    }
    int getDepthFrameHeight(){
        return _camera->getDepthFrameHeight();
    }
    int getDepthFrameWidth(){
        return _camera->getDepthFrameWidth();
    }
    int getColorFrameHeight(){
        return _camera->getColorFrameHeight();
    }
    int getColorFrameWidth(){
        return _camera->getColorFrameWidth();
    }

private:
    Camera*  _camera = NULL;
};

#endif // CAMERA_H
