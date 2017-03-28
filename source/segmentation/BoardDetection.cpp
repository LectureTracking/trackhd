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

#include "BoardDetection.h"

using namespace cv;
using namespace std;

Mat sourceFrame;
Mat sourceFrameGray;
Mat thresholdFrame;
int cannyThreshold = 100;
RNG rng(12345);

int keyPointChangeThresh = 150;
int leftKeyPointInitial = 0;
int rightKeyPointInitial = 0;
Rect rightHalf;
Rect leftHalf;
bool initialLoop = true;

double boardFoundThresh = 0.4;

IlluminationCorrection illuminationCorrection;

BoardDetection::BoardDetection()
{


}

///----------------------------//
/// Main method to find boards //
///----------------------------//
void BoardDetection::extractBoards(std::vector<Mat> &frames, PersistentData &pD)
{
    //Initialise parameters needed
    Size sizeOfFrames = pD.videoDimension;
    leftHalf = Rect(Point(0, 0), Point(sizeOfFrames.width / 2, sizeOfFrames.height));
    rightHalf = Rect(Point(sizeOfFrames.width / 2, 0), Point(sizeOfFrames.width, sizeOfFrames.height));

    vector<BoardDetection::BoardRectangleStruct> boardRectangles; //Store all board rectangles found
    vector<BoardDetection::BoardRectangleStruct> boardColumnRectangles;

    Rect cropRegion;
    Rect finalCrop;


    //Find board crop region
    for (int i = 0; i < frames.size(); i += pD.boardDetectionSkipFrames)
    {
        if (i >= frames.size())
        {
            break;
        }
        Mat frame = frames[i];
        illuminationCorrection.applyCLAHE(frame);

        //Find the cropping area
        boardRectangles.clear();
        boardColumnRectangles.clear();
        findEdgesInImage(frame, cropRegion, boardRectangles, boardColumnRectangles);

        int leftK = 0;
        int rightK = 0;

        bool projectorUsedLeft = false;
        bool projectorUsedRight = false;

        bool leftBoardUsed = false;
        bool rightBoardUsed = false;

        Scalar color;

        for (int a = 0; a < boardRectangles.size(); a++)
        {
            BoardDetection::BoardRectangleStruct b = boardRectangles.at(a);

            switch (b.boardType)
            {
                case BLACKBOARD:
                    if (isContained(b.boundingRectangle, rightHalf))
                    {
                        //Board is on the right
                        rightK += b.numFeatures;
                    } else if (isContained(b.boundingRectangle, leftHalf))
                    {
                        //Board is on the left
                        leftK += b.numFeatures;
                    }

                    break;
                case PROJECTORSCREEN:
                    if (isContained(b.boundingRectangle, rightHalf))
                    {
                        projectorUsedRight = true;
                    } else if (isContained(b.boundingRectangle, leftHalf))
                    {
                        projectorUsedLeft = true;
                    }

            }
        }

        if (initialLoop)
        {
            leftKeyPointInitial = leftK;
            rightKeyPointInitial = rightK;
            initialLoop = false;

        } else
        {
            //Now check if change in number of keypoints is significant
            if (leftK > leftKeyPointInitial + keyPointChangeThresh)
            {
                //Left board used
                leftBoardUsed = true;
            }

            if (rightK > rightKeyPointInitial + keyPointChangeThresh)
            {
                //right board used
                rightBoardUsed = true;
            }

            //Now apply this update back to the metaframe
            pD.metaFrameVector.push_back(
                    MetaFrame(true, leftBoardUsed, rightBoardUsed, projectorUsedLeft, projectorUsedRight));
        }


        //If we find an area big enough we assume the boards are contained in the crop region
        if (cropRegion.area() > int(boardFoundThresh * sizeOfFrames.area()) && !pD.boardsFound)
        {
            finalCrop = cropRegion;
            pD.boardsFound = true;
            pD.boardCropRegion = cropRegion;
        } else
        {
            //If the new area found is bigger than current crop area --> Update current crop area to new
            if (cropRegion.area() > pD.boardCropRegion.area())
            {
                // pD.boardCropRegion = cropRegion;
            }
        }

    }
}

///-------------------------------------------//
/// Main method to find rectangles in a frame //
///-------------------------------------------//
void BoardDetection::findEdgesInImage(cv::Mat &frame, Rect &cropArea,
                                      vector<BoardDetection::BoardRectangleStruct> &boardRects,
                                      vector<BoardDetection::BoardRectangleStruct> &boardColumnRects)
{


    sourceFrame = frame;
    // Convert image to gray and blur it
    cvtColor(sourceFrame, sourceFrameGray, CV_BGR2GRAY);
    blur(sourceFrameGray, sourceFrameGray, Size(3, 3));


    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    //cv::Mat grayOverlay = sourceFrameGray.clone();


    //Threshold the image to detect blackboards easier
    threshold(sourceFrameGray, thresholdFrame, 200, 255, THRESH_BINARY);


    // Detect edges using canny
    Canny(sourceFrameGray, canny_output, cannyThreshold, cannyThreshold * 2, 3);


    // Find contours
    findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    // Draw contours
    Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
    for (int i = 0; i < contours.size(); i++)
    {
        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
    }


    //Bound contours with rectangle
    //Vector to store bounding rectangles
    vector<Rect> rects;
    BoardDetection::boundContoursWithRectangles(contours, rects);

    //Rect cropArea;
    vector<Rect> possibleBoardColumnsVec;


    BoardDetection::findBoards(rects, possibleBoardColumnsVec, boardRects);
    BoardDetection::findBoardColumns(possibleBoardColumnsVec, boardColumnRects);
    BoardDetection::findOverallCropOfBoards(boardColumnRects, cropArea, boardRects);

    BoardDetection::removeOverlappingRectangles(boardRects);
    BoardDetection::removeOverlappingRectangles(boardColumnRects);

}

void BoardDetection::boundContoursWithRectangles(vector<vector<Point> > contours, vector<Rect> &allBoundingRectangles)
{

    // Approximate contours to polygons + get bounding rectangles
    vector<vector<Point>> contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());


    for (int i = 0; i < contours.size(); i++)
    {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        boundRect[i] = boundingRect(Mat(contours_poly[i]));
    }

    allBoundingRectangles = boundRect;
}

//Simple method to check if a value is contained in a vector of values
bool BoardDetection::checkContainment(vector<int> &v, int val)
{

    bool found = false;
    for (int i = 0; i < v.size(); i++)
    {
        if (v.at(i) == val)
        {
            found = true;
        }
    }

    return found;
}

void BoardDetection::findBoards(std::vector<cv::Rect> &allRectangles, std::vector<cv::Rect> &possibleBoardColumns,
                                std::vector<BoardDetection::BoardRectangleStruct> &boards)
{

    int boardIdCounter = 0;
    int projectorIdCounter = 0;

    vector<Rect> rectanglesToKeep;
    //Loop over all rectangles
    for (int i = 0; i < allRectangles.size(); i++)
    {
        Rect r = allRectangles.at(i);
        //Ignore really small rectangles
        if (r.width > 400 && r.height > 300 && r.height <= 720)
        {

            //Based on aspect ratio, classify as Projector screen or blackboard
            if (calculateAspectRatio(r) >= 165 && calculateAspectRatio(r) <= 200 && isDark(r))
            {
                //Close to a 16:9 ratio and classified as being a dark area... So assume blackboard
                rectanglesToKeep.push_back(r);

                BoardRectangleStruct b(boardIdCounter, r, countFeatures(sourceFrameGray(r)), BLACKBOARD);
                boards.push_back(b);
                boardIdCounter++;

            } else if (calculateAspectRatio(r) >= 100 && calculateAspectRatio(r) <= 145 && !isDark(r))
            {
                //Close to a 4:3 ratio and classified as being a light area... So assume projector screen

                BoardRectangleStruct p(projectorIdCounter, r, countFeatures(sourceFrameGray(r)), PROJECTORSCREEN);
                boards.push_back(p);

                projectorIdCounter++;
            }
        } else if (r.width > 400 && r.height > 300 && r.height > 1000)
        {
            //Possibly the board column
            possibleBoardColumns.push_back(r);
        }
    }
}

void BoardDetection::removeOverlappingRectangles(vector<BoardDetection::BoardRectangleStruct> &boards)
{
///-------------------------------------------///
    /// STAGE 1 - Remove all contained rectangles ///
    ///-------------------------------------------///
    vector<int> toRemove;
    //Remove rectangles contained in another
    for (int i = 0; i < boards.size(); i++)
    {

        Rect r1 = boards.at(i).boundingRectangle;
        for (int j = i + 1; j < boards.size(); j++)
        {

            //cout << "Comparing rect " << i << " with " << j << endl;

            Rect r2 = boards.at(j).boundingRectangle;
            Rect r3 = r1 & r2;

            if (r3.area() > 0)
            {
                if (r3.area() == r2.area())
                {
                    //cout << "r2 is inside r1" << endl;
                    toRemove.push_back(boards.at(i).ItemID);
                } else if (r3.area() == r1.area())
                {
                    //cout << "r1 is inside r2" << endl;
                    toRemove.push_back(boards.at(j).ItemID);
                } else
                {
                    //cout << "Overlapping Rectangles" << endl;
                    if (r1.area() > r2.area())
                    {
                        toRemove.push_back(boards.at(i).ItemID); //Remove smaller rectangle R2
                    } else
                    {
                        toRemove.push_back(boards.at(j).ItemID);//Remove smaller rectangle R1
                    }
                }
            } else
            {
                //cout << "Non-overlapping Rectangles" << endl;

            }
        }
    }

    vector<BoardDetection::BoardRectangleStruct> toKeep;
    for (int i = 0; i < boards.size(); i++)
    {
        if (!checkContainment(toRemove, boards.at(i).ItemID))
        {
            toKeep.push_back(boards.at(i));
        }
    }

    boards.clear();
    boards = move(toKeep);

}

void
BoardDetection::findBoardColumns(std::vector<cv::Rect> &allRectangles, std::vector<BoardRectangleStruct> &boardColumnsR)
{

    ///-------------------------------------------///
    /// STAGE 1 - Remove all contained rectangles ///
    ///-------------------------------------------///
    vector<int> toRemove;
    //Remove rectangles contained in another
    for (int i = 0; i < allRectangles.size(); i++)
    {

        Rect r1 = allRectangles[i];
        for (int j = i + 1; j < allRectangles.size(); j++)
        {

            //cout << "Comparing rect " << i << " with " << j << endl;

            Rect r2 = allRectangles[j];
            Rect r3 = r1 & r2;

            if (r3.area() > 0)
            {
                if (r3.area() == r2.area())
                {
                    //cout << "r2 is inside r1" << endl;
                    toRemove.push_back(j);
                } else if (r3.area() == r1.area())
                {
                    //cout << "r1 is inside r2" << endl;
                    toRemove.push_back(i);
                } else
                {
                    //cout << "Overlapping Rectangles" << endl;
                    if (r1.area() > r2.area())
                    {
                        toRemove.push_back(j); //Remove smaller rectangle R2
                    } else
                    {
                        toRemove.push_back(i); //Remove smaller rectangle R1
                    }
                }
            } else
            {
                //cout << "Non-overlapping Rectangles" << endl;
            }
        }
    }

    ///---------------------------------------------------///
    /// STAGE 2 - Keep only possible candidate rectangles ///
    ///---------------------------------------------------///
    //Evaluate all rectangles and keep only candidates that pass a certain criteria
    //vector<Rect> filtered;
    int boardColumnId = 0;
    for (int i = 0; i < allRectangles.size(); i++)
    {

        if (!checkContainment(toRemove, i))
        {
            Rect t = allRectangles.at(i);


            if (t.width > 400 && t.width < 2000 && t.height > 500)
            {
                //Check to see if the rectangle is not unreasonably small
                //filtered.push_back(t);
                BoardRectangleStruct bc(boardColumnId, t, countFeatures(sourceFrameGray(t)), BOUNDING);
                boardColumnsR.push_back(bc);
                boardColumnId++;
            }
        }
    }

    //allRectangles.clear();
    //allRectangles = move(filtered);
}

void BoardDetection::findOverallCropOfBoards(std::vector<BoardRectangleStruct> &boardColumns, Rect &finalCrop,
                                             std::vector<BoardRectangleStruct> &Boards)
{
    ///-------------------------------------///
    /// STAGE 1 - Decide on area to segment ///
    ///-------------------------------------///
    //Bound overall crop region
    int minX = INT_MAX, minY = INT_MAX;
    int maxX = INT_MIN, maxY = INT_MIN;

    for (BoardRectangleStruct boardRect:boardColumns)
    {

        if (boardRect.boundingRectangle.tl().x < minX)
        {
            minX = boardRect.boundingRectangle.tl().x;
        }
        if (boardRect.boundingRectangle.tl().y < minY)
        {
            minY = boardRect.boundingRectangle.tl().y;
        }
        if (boardRect.boundingRectangle.br().x > maxX)
        {
            maxX = boardRect.boundingRectangle.br().x;
        }
        if (boardRect.boundingRectangle.br().y > maxY)
        {
            maxY = boardRect.boundingRectangle.br().y;
        }
    }

    //Extend further incase boards do extend
    for (BoardRectangleStruct boardRect:Boards)
    {

        if (boardRect.boundingRectangle.tl().x < minX)
        {
            minX = boardRect.boundingRectangle.tl().x;
        }
        if (boardRect.boundingRectangle.tl().y < minY)
        {
            minY = boardRect.boundingRectangle.tl().y;
        }
        if (boardRect.boundingRectangle.br().x > maxX)
        {
            maxX = boardRect.boundingRectangle.br().x;
        }
        if (boardRect.boundingRectangle.br().y > maxY)
        {
            maxY = boardRect.boundingRectangle.br().y;
        }
    }

    ///-----------------------------------///
    /// STAGE 2 - Set overall crop region ///
    ///-----------------------------------///
    finalCrop = Rect(Point(minX, minY), Point(maxX, maxY));
}

int BoardDetection::calculateAspectRatio(cv::Rect &r)
{
    //Returns 177 for 16:9 and 133 for 4:3
    // cout << "Width = " << boundingRectangle.width << " Height = " << boundingRectangle.height << " Aspect Ratio = " << (double) (boundingRectangle.width / boundingRectangle.height)<< endl;
    return (int) ((r.width / r.height) * 100);
}

bool BoardDetection::isDark(cv::Rect &r)
{

    Mat tmpImg = (thresholdFrame(r));
    int TotalNumberOfPixels = tmpImg.rows * tmpImg.cols;
    int ZeroPixels = TotalNumberOfPixels - countNonZero(tmpImg);
    float percent = ((float) ZeroPixels / (float) TotalNumberOfPixels) * 100;
    ///cout << "Percentage black: " << percent << endl;
    return percent > 75;
}

std::vector<KeyPoint> BoardDetection::countFeatures(Mat cropImg)
{

    //Perform sift function on image to find key featurePoints such as writing on the boards

    //-- Step 1: Detect the keypoints using SURF Detector
    int minHessian = 400;

    Ptr<cv::xfeatures2d::SURF> detector = xfeatures2d::SURF::create(minHessian);

    std::vector<KeyPoint> keypoints_1;

    detector->detect(cropImg, keypoints_1);

    return keypoints_1;
}

bool BoardDetection::isContained(cv::Rect r1, cv::Rect r2)
{

    Rect r3 = r1 & r2;
    //Is r1 contained in r2?
    return r3.area() == r1.area();
}
