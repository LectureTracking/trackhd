//
// Created by Charles on 2016/09/05.
//

#ifndef TRACK4K_BOARDDETECTION_H
#define TRACK4K_BOARDDETECTION_H

#include "../MetaFrame.h"
#include "../FileReader.h"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <vector>
#include "../panning/VideoOutput.h"
#include "../panning/FrameCropper.h"
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "IlluminationCorrection.h"


class BoardDetection {

public:
    BoardDetection();

    enum type {
        BLACKBOARD, PROJECTORSCREEN, BOUNDING
    };

    ///Rectangle struct for storing boards
    struct BoardRect {
        int ID;
        cv::Rect r;
        int numFeatures;
        type boardType;
        std::vector<KeyPoint> features;

        BoardRect(int id, cv::Rect rect, std::vector<KeyPoint> f, BoardDetection::type bT) {

            ID = id;
            r = rect;
            features = f;
            numFeatures = features.size();
            boardType = bT;
        }
    };

    void extractBoards(std::vector<cv::Mat> &frames, PersistentData &pD);

    void findRectangles(cv::Mat &frame, cv::Rect &cropArea, std::vector<BoardRect> &boardRects,
                            std::vector<BoardRect> &boardColumnRects);

     void boundContoursWithRectangles(std::vector<std::vector<cv::Point>> contours, std::vector<cv::Rect> &vR);

     bool checkContainment(std::vector<int> &v, int val);

     void findBoards(std::vector<cv::Rect> &allRectangles, std::vector<cv::Rect> &possibleBoardColumns,
                               std::vector<BoardRect> &boards);

     void removeOverlappingRectangles(std::vector<BoardRect> &boards);

     void findBoardColumns(std::vector<cv::Rect> &allRectangles, std::vector<BoardRect> &boardColumnsR);

     void findAreaToSegment(std::vector<BoardRect> &boardColumns, cv::Rect &finalCrop,
                                     std::vector<BoardRect> &Boards);

     int calculateAspectRatio(cv::Rect &r);

    std::vector<KeyPoint> countFeatures(cv::Mat cropImg);

     bool isDark(cv::Rect &r);

    bool isContained(cv::Rect r1, cv::Rect r2);


};


#endif //TRACK4K_BOARDDETECTION_H
