//
// Created by Charles on 2016/07/15.
//

#ifndef TRACK4K_PERSISTENTDATA_H
#define TRACK4K_PERSISTENTDATA_H

#include <vector>
#include "opencv2/opencv.hpp"
#include "MetaFrame.h"
#include "PersistentData.h"
#include "Tracking/headers/Ghost.h"

class PersistentData
{
private:
    bool videoInfoSet = false;
public:
    //Store the areas of motion. Each vector stores x frames worth of motion
    int areasOfMotionOverNumberOfFrames = 290;
    std::vector<Rect> areasOfMotion;
    std::vector<MetaFrame> metaFrameVector;

    int fps; //Frame Rate
    int totalFrames; //Number of frames
    cv::Size videoDimension;

    void setVideoInfo(int f, int t, cv::Size s);

    std::string saveFileExtension = "flv";
    std::string inputFileName = "Videos/PanningTest40s.mp4";

    cv::Size panOutputVideoSize = cv::Size(1280, 720);
    std::string panOutputVideoSaveFileName = "finalPanOutput.flv";

    vector<Rect> vals;
    int skipFrameMovementDetection;

    int boardDetectionSkipFrames = 28*2;

    bool boardsFound = false;
    cv::Rect boardCropRegion;

};


#endif //TRACK4K_PERSISTENTDATA_H
