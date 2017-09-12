#ifndef CAMERATYPEFACTORY_H
#define CAMERATYPEFACTORY_H

#include <stdio.h>
#include <iostream>
#include <memory>
#include <camera.h>
#include <cameratypes.h>



class CameraFactory {

public:
    CameraSensor cameraSensor;
public:
    CameraSensor getCamera(int cameraType) {
        switch (cameraType) {
        case DS325:
            cameraSensor.setDeviceType(new DepthSense_DS325());
            break;
        /*case KINECT2:
            cameraSensor.setDeviceType(new RealsenseR200());
            break;
        case SR300:
            cameraSensor.setDeviceType(new RealsenseSR300());
            break;
        case R200:
            cameraSensor.setDeviceType(new Kinect2());
            break;*/
        default: throw std::invalid_argument("Invalid camera type");
        }

        return cameraSensor;
    }
};

#endif // CAMERATYPEFACTORY_H
