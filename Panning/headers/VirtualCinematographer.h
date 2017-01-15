//
// Created by Tanweer Khatieb on 2016/07/21.
//

#ifndef TRACK4K_VIRTUALCINEMATOGRAPHER_H
#define TRACK4K_VIRTUALCINEMATOGRAPHER_H

#include "VideoOutput.h"
#include "FrameCropper.h"
#include "opencv2/opencv.hpp"
#include "PresenterMotion.h"
#include "PanLogic.h"

class VirtualCinematographer {
public:
    int cinematographerDriver(PersistentData &pD);
};


#endif //TRACK4K_VIRTUALCINEMATOGRAPHER_H
