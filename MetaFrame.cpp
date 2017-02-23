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

#include "MetaFrame.h"

/*
 * This file will contain all data that needs to pass through to the following stages.
 * This data includes:
 * 1) Video information such as:
 *      - Length (float)
 *      - Frame Rate (int)
 *      - Processing/Detection rate (e.g Every 60 frames) and this will be the interval of the metadata
 * 2) Areas of motion
 *      - Stored as an array of rectangles
 *      - [rect(sx,sy,ex,ey);...]
 *
 * 3) Blackboard columns
 *      - Stored as a vector of rectangles
 *
 * 4) Blackboard column usage
 *      - Detect writing on blackboard
 *      - Used boards stored as index from 3 above
 *      - E.g [0,1,3] Index in rectangle vector
 *
 * 5) Grayscale Video
 *
 */
MetaFrame::MetaFrame(cv::Mat frame) : colourFrame(cv::Mat(frame))
{

}

void MetaFrame::setColourFrame(cv::Mat cFrame)
{

    colourFrame = cv::Mat(cFrame);
}

void MetaFrame::setBoardUsage(bool hasBoardData, bool lB, bool rB, bool lP, bool rP)
{

    hasBoardUsageData = hasBoardData;
    leftBoard = lB;
    rightBoard = rB;
    leftProjector = lP;
    rightProjector = rP;
}

MetaFrame::MetaFrame(bool hasBoardData, bool lB, bool rB, bool lP, bool rP)
{
    hasBoardUsageData = hasBoardData;
    leftBoard = lB;
    rightBoard = rB;
    leftProjector = lP;
    rightProjector = rP;
}



