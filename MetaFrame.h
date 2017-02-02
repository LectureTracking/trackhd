//
// Created by Charles on 2016/07/15.
//

#ifndef TRACK4K_METADATA_H
#define TRACK4K_METADATA_H

#include <vector>
#include "opencv2/opencv.hpp"

class MetaFrame {
public:
    cv::Mat colourFrame;

    //Blackboards
    std::vector<cv::Rect> blackBoardColumns; //Size of this array will indicate number of board columns

    //Board used
    cv::Rect boardCropRegion;
    bool hasBoardUsageData = false;
    bool leftBoard = false;
    bool rightBoard =false;
    bool rightProjector = false;
    bool leftProjector = false;

    void setBoardUsage(bool hasBoardData, bool lB, bool rB, bool lP, bool rP);

    //Contructors
    MetaFrame(cv::Mat frame);
    MetaFrame(bool hasBoardData, bool lB, bool rB, bool lP, bool rP);

    cv::Mat getColourFrame(){

        return colourFrame;
    }

    void setColourFrame(cv::Mat cFrame);

};

#endif //TRACK4K_METADATA_H
