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

#include "opencv2/opencv.hpp"
#include "PresenterMotion.h"
#include "PanLogic.h"

#include <fstream>

#include <memory>

class VirtualCinematographerOutput
{
    public:
        virtual void outputHeader(ofstream & stream, const PersistentData & persistentData) = 0;
        virtual void outputFrames(ofstream & stream, const PersistentData & persistentData, const std::vector<Rect> & cropRectangles, long int y) = 0;
};

class DefaultVirtualCinematographerOutput : public VirtualCinematographerOutput
{
    public:
        virtual void outputHeader(ofstream & stream, const PersistentData & persistentData);
        virtual void outputFrames(ofstream & stream, const PersistentData & persistentData, const std::vector<Rect> & cropRectangles, long int y);
};

class JsonVirtualCinematographerOutput : public VirtualCinematographerOutput
{
    public:
        virtual void outputHeader(ofstream & stream, const PersistentData & persistentData);
        virtual void outputFrames(ofstream & stream, const PersistentData & persistentData, const std::vector<Rect> & cropRectangles, long int y);
};

class VirtualCinematographer
{
    public:
        VirtualCinematographer(VirtualCinematographerOutput * output = nullptr);

        int cinematographerDriver(PersistentData &persistentData);

    protected:
        std::unique_ptr<VirtualCinematographerOutput> _output;
};


#endif //TRACK4K_VIRTUALCINEMATOGRAPHER_H
