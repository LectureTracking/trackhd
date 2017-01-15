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
    std::vector<std::vector<cv::Rect>> areasOfMotion;
    std::vector<MetaFrame> metaFrameVector;

    int fps; //Frame Rate
    int totalFrames; //Number of frames
    cv::Size videoDimension;

    void setVideoInfo(int f, int t, cv::Size s);

    std::string saveFileExtension = "flv";
    std::string inputFileName = "Videos/PanningTest40s.mp4";


    //Charles Testing

    //std::string inputFileName = "Videos/BUD_1.mp4";
    //std::string inputFileName = "Videos/BUD_2.mp4";
    //std::string inputFileName = "Videos/BUD_3.mp4";
    //std::string inputFileName = "Videos/BUD_4.mp4";
    //std::string inputFileName = "Videos/BUD_5.mp4";
    //std::string inputFileName = "Videos/BUD_6.mp4";

    //std::string inputFileName = "Videos/BD_1.mp4";
    //std::string inputFileName = "Videos/BD_2.mp4";
    //std::string inputFileName = "Videos/BD_3.mp4";
    //std::string inputFileName = "Videos/BD_4.mp4";


    //std::string inputFileName = "Videos/cs203-presenters-4K.mp4";
    //std::string inputFileName = "Videos/test1.avi";
    //std::string inputFileName = "Videos/presenter-MAM1000W-20160811.mp4";

    //50min long video
    //std::string inputFileName = "Videos/presenter-MAM1000W-201608112.mp4";


    //std::string inputFileName = "Videos/Test Cases Input/CLIP_1_NORM_LECT.mp4";
    //std::string inputFileName = "Videos/Test Cases Input/CLIP_2_LECT_WAVE.mp4";
    //std::string inputFileName = "Videos/Test Cases Input/CLIP_3_LECT_PACE.mp4";
    //std::string inputFileName = "Videos/Test Cases Input/CLIP_4_LIGHTS.mp4";
    //std::string inputFileName = "Videos/Test Cases Input/CLIP_5_MOVE_BOARDS.mp4";
    //std::string inputFileName = "Videos/cs203-presenters-4K_SHORT_CLIP.flv";
    //std::string inputFileName = "Videos/Test Cases Input/CLIP_6_MOVE_SCREEN.mp4";
    //std::string inputFileName = "Videos/Test Cases Input/CLIP_7_MOVE_SCREEN_2.mp4";
    //std::string inputFileName = "Videos/Test Cases Input/CLIP_8_OFF_ON.mp4";
    //std::string inputFileName = "Videos/Test Cases Input/CLIP_9_LECT_CROSS.mp4";
    //std::string inputFileName = "Videos/Test Cases Input/CLIP_10_BOTH_MOVE.mp4";
    //std::string inputFileName = "Videos/Test Cases Input/CLIP_11_BOTH_MOVE_2.mp4";
    //std::string inputFileName = "Videos/Test Cases Input/CLIP_12_RUNNING.mp4";
    //std::string inputFileName = "Videos/Test Cases Input/CLIP_13_THROWING.mp4";
    //std::string inputFileName = "Videos/Test Cases Input/CLIP_14_MULTI_STUDENT.mp4";
    //std::string inputFileName = "Videos/Test Cases Input/CLIP_15_MOVE_3.mp4";
    //std::string inputFileName = "Videos/Test Cases Input/CLIP_16_MOVE_CHAIRS.mp4";
    //std::string inputFileName = "Videos/Test Cases Input/CLIP_17_NO_MOVE.mp4";    std::string blackboardSaveFileName = "bb.flv";
    //std::string blackboardSaveFileName = "bb.flv";

    cv::Size panOutputVideoSize = cv::Size(1280, 720);
    std::string panOutputVideoSaveFileName = "finalPanOutput.flv";

    vector<Rect> vals;
    int skipFrameMovementDetection;

    int boardDetectionSkipFrames = 28*2;

    bool boardsFound = false;
    cv::Rect boardCropRegion;

};


#endif //TRACK4K_PERSISTENTDATA_H
