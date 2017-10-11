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
// Created by Charles Fitzhenry on 2016/07/15.
//

#ifndef TRACK4K_METADATA_H
#define TRACK4K_METADATA_H

#include <vector>
#include "opencv2/opencv.hpp"

class MetaFrame
{
    public:
        cv::Mat colourFrame;

        //Board used
        bool hasBoardUsageData = false;
        bool leftBoard = false;
        bool rightBoard = false;
        bool rightProjector = false;
        bool leftProjector = false;

        void setBoardUsage(bool hasBoardData, bool lB, bool rB, bool lP, bool rP);

        //Contructors
        MetaFrame(cv::Mat frame);

        MetaFrame(bool hasBoardData, bool lB, bool rB, bool lP, bool rP);

        void setColourFrame(cv::Mat cFrame);

};

#endif //TRACK4K_METADATA_H
