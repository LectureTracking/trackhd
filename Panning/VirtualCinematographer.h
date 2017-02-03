/**
 *  Copyright 2016 Charles Fitzhenry / Mohamed Tanweer Khatieb / Maximilian Hahn
 *  Licensed under the Educational Community License, Version 2.0
 *  (the "License"); you may not use this file except in compliance
 *  with the License. You may obtain a copy of the License at
 *
 *  http://www.osedu.org/licenses/ECL-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS IS"
 *  BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 *  or implied. See the License for the specific language governing
 *  permissions and limitations under the License.
 *
 */

//
// Created by Mohamed Tanweer Khatieb on 2016/07/21.
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
