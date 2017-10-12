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

#ifndef TRACK4K_PERSISTENTDATA_H
#define TRACK4K_PERSISTENTDATA_H

#include <vector>
#include "opencv2/opencv.hpp"
#include "MetaFrame.h"
#include "PersistentData.h"
#include "tracking/Ghost.h"

class PersistentData
{
private:
    bool videoInfoSet = false;
public:
    //Store the areas of motion. Each vector stores x frames worth of motion
    int segmentationNumFramesToProcessPerIteration = 29; // number of frames that will be read into memory
    std::vector<Rect> areasOfMotion;
    std::vector<MetaFrame> metaFrameVector;

    float fps; //Frame Rate
    int totalFrames; //Number of frames
    cv::Size videoDimension;

    void setVideoInfo(float f, int t, cv::Size s, int ext);

    std::string saveFileExtension = "mp4"; //Default save extension
    std::string inputFileName = "";
    int ext_int; //The int version of the file extension
    int codec; //Default codec for mp4
    cv::Size panOutputVideoSize = cv::Size(1280, 720);
    std::string outputVideoFilenameSuffix = "";

    vector<Rect> lecturerTrackedLocationRectangles;
    int skipFrameMovementDetection;

    int boardDetectionSkipFrames = 28*2;

    bool boardsFound = false;
    cv::Rect boardCropRegion;

    // Number of frames to repeat at the start of the output video
    int outputPadding = 0;
};

#endif //TRACK4K_PERSISTENTDATA_H
