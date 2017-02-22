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
// Created by Charles Fitzhenry on 2016/09/05.
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
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "IlluminationCorrection.h"


class BoardDetection
{

public:
    BoardDetection();

    enum type
    {
        BLACKBOARD, PROJECTORSCREEN, BOUNDING
    };

    ///Rectangle struct for storing boards
    struct BoardRectangleStruct
    {
        int ItemID;
        cv::Rect boundingRectangle;
        int numFeatures;
        type boardType;
        std::vector<KeyPoint> featurePoints;

        BoardRectangleStruct(int id, cv::Rect rect, std::vector<KeyPoint> f, BoardDetection::type bT)
        {

            ItemID = id;
            boundingRectangle = rect;
            featurePoints = f;
            numFeatures = featurePoints.size();
            boardType = bT;
        }
    };

    /**
     *
     * @param frames
     * @param pD
     */
    void extractBoards(std::vector<cv::Mat> &frames, PersistentData &pD);

    /**
     * This method acts as the main driver for detecting the boards in the input frames
     * @param frame is the input frame
     * @param cropArea is the overall area that includes all the boards
     * @param boardRects is the areas in the frame that contain boards
     * @param boardColumnRects is the areas in the frame tht contain the board columns
     */
    void findEdgesInImage(cv::Mat &frame, cv::Rect &cropArea, std::vector<BoardRectangleStruct> &boardRects,
                          std::vector<BoardRectangleStruct> &boardColumnRects);

    /**
     * This method uses the contours provided by the findRectangles method
     * @param contours
     * @param allBoundingRectangles is the output vector containing all the rectangles found
     */
    void boundContoursWithRectangles(std::vector<std::vector<cv::Point>> contours,
                                     std::vector<cv::Rect> &allBoundingRectangles);

    /**
     * This is a helper method to check if a vector contains a number
     * @param v is the vector of number
     * @param val is the value to check for in the vecctor
     * @return true if contained
     */
    bool checkContainment(std::vector<int> &v, int val);

    /**
     *
     * @param allRectangles is a vector containing all unclassified rectangles that has been detected in the image
     * @param possibleBoardColumns contains all rectangles that are classified as not being boards
     * @param boards is a vector to which all rectngle are added that are classified as being black boards.
     */
    void findBoards(std::vector<cv::Rect> &allRectangles, std::vector<cv::Rect> &possibleBoardColumns,
                    std::vector<BoardRectangleStruct> &boards);

    /**
     * This method culls all the smaller rectangles that are contained and only keeps the largest rectangles that are
     * not contained
     * @param boards is a vector containing rectangles.
     */
    void removeOverlappingRectangles(std::vector<BoardRectangleStruct> &boards);

    /**
     * This method finds the area containing the board columns and places these  into a vector.
     * @param allRectangles is all the rectangles that is found in the image
     * @param boardColumnsR is all the rectangles that meet the criteria of a board column's size
     */
    void findBoardColumns(std::vector<cv::Rect> &allRectangles, std::vector<BoardRectangleStruct> &boardColumnsR);

    /**
     * This method takes in vectors of board columns or boards (rectangles enclosing them) and then finds the overall
     * cropping rectangle that includes all the boards.
     * @param boardColumns
     * @param finalCrop is the rectangle that is achieved in the end representing the overall crop region
     * @param Boards
     */
    void findOverallCropOfBoards(std::vector<BoardRectangleStruct> &boardColumns, cv::Rect &finalCrop,
                                 std::vector<BoardRectangleStruct> &Boards);

    /**
     * This function simply divides the width by the height of the rectangle
     * @param r is the input rectangle
     * @return returns width divided by height
     */
    int calculateAspectRatio(cv::Rect &r);

    /**
     * This method takes the following parameters
     * @param cropImg is the image that will be passed through the Speeded Up Robust Features(SURF) function
     * @return the features as a vector
     */
    std::vector<KeyPoint> countFeatures(cv::Mat cropImg);

    /**
     * Returns a ratio of dark pixels (0) to light (255) of a region in a binary image
     * @param r is a rectangle depicting the region to consider in the image
     * @return
     */
    bool isDark(cv::Rect &r);

    /**
     * This method checks if
     * @param r1 is contained in
     * @param r2 and then
     * @return true if so.
     */
    bool isContained(cv::Rect r1, cv::Rect r2);

};


#endif //TRACK4K_BOARDDETECTION_H
