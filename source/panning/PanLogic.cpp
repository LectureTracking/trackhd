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
// Created by Mohamed Tanweer Khatieb on 2016/09/19.
//

#include "PanLogic.h"

using namespace std;
using namespace cv;

bool initialCrop = true;

/**
 * Cos function over with which the pan operation is divided
 * @param x
 * @return
 */
long double PanLogic::smooth(double x)
{
    return (cos(x - (135)) + 1);
}

/**
 * This method, given a start and end point, creates a vector of rectangles that will pan over that line.
 * @param start
 * @param end
 * @param numFrames
 * @param right
 * @param panOffsetType
 * @param croppingRectangles
 */
void PanLogic::smoothMove(int start, int end, int numFrames, bool right, Position panOffsetType, std::vector<cv::Rect> &croppingRectangles)
{

    if (initialCrop)
    {
        setPan(start, currentPan);
        initialCrop = false;
    }

    if (!checkIfPanRequired(start, end) && numFrames > 59)
    {

        switch (panOffsetType)
        {
            case LEFT:
                end -= (cropFrameWidth / 2) - boardOffSet;
                break;
            case RIGHT:
                end += (cropFrameWidth / 2) - boardOffSet;
                break;
            case CENTER:
                break;
        }


        int currentPos = start;

        int pixels = (int) abs(end - start);

        double sampleSize = 2 * CV_PI / numFrames;
        double normal = pixels / numFrames;

        for (int i = 0; i < numFrames; i++)
        {

            long double outP = smooth(i * sampleSize) * normal;

            //Needed to move rectangle left/right depending on the direction the lecturer is moving in
            if (right)
            {
                currentPos += outP;
            }
            else
            {
                currentPos -= outP;
            }

            //Creates a cropping rectangle
            setPan(currentPos, currentPan);


            croppingRectangles.push_back(currentPan);

        }
    }
    else
    {

        //Don't pan. just push back current crop
        for (int i = 0; i < numFrames; i++)
        {
            croppingRectangles.push_back(currentPan);
        }
    }
}

//Essentially the main method for this class
/**
 * Makes the crop window move over the pan operations.
 * @param motionLines
 * @param croppingRectangles
 */
void PanLogic::doPan(std::vector<PresenterMotion::Movement> &motionLines, std::vector<cv::Rect> &croppingRectangles)
{
    int startingPosition;

    //Loop over all motion lines
    for (int i = 0; i < motionLines.size(); i++)
    {
        PresenterMotion::Movement movement = motionLines.at(i);
        if (!movement.isDropData)
        {
            //This is an actual motion line
            if (i > 0)
            {
                startingPosition = getPan(currentPan);
            }
            else
            {
                startingPosition = movement.start.x;
            }

            //If boards used in this motion line
            if (motionLines.at(i).boardUsed)
            {
                //True should be board used
                if (!motionLines.at(i).rightBoardUsed)
                {
                    smoothMove(startingPosition, movement.end.x, movement.numFrames, movement.right, LEFT, croppingRectangles);

                }
                else
                {
                    smoothMove(startingPosition, movement.end.x, movement.numFrames, movement.right, RIGHT, croppingRectangles);
                }

            }
            else
            {
                smoothMove(startingPosition, movement.end.x, movement.numFrames, movement.right, CENTER, croppingRectangles);
            }

        }
        else
        {
            //This is dropped frames (noise)
            //Where noise is reduced (jittery motions dropped), fill up vacant frames with last known data
            if (movement.numFrames > 50)
            {
                int startPosition = getPan(currentPan);
                int endPosition = int (movement.start.x + abs(movement.end.x - movement.start.x));
                bool right = false;
                if (startPosition < endPosition)
                {
                    right = true;
                }

                smoothMove(startPosition, endPosition, movement.numFrames, right, CENTER, croppingRectangles);
            }
            else
            {
                int speed = motionLines.at(i).numFrames; //Number of frames to reposition over

                /*

                if (motionLines.at(i).boardUsed)
                {
                    //True should be board used
                    if (!motionLines.at(i).rightBoardUsed)
                    {
                        cout << "Reposition LEFT" << endl;
                        rePosition(LEFT, speed, m, croppingRectangles);

                    } else
                    {
                        cout << "Reposition RIGHT" << endl;
                        rePosition(RIGHT, speed, m, croppingRectangles);
                    }

                } else
                {
                    cout << "Reposition CENTER" << endl;
                    rePosition(CENTER, speed, m, croppingRectangles);
                }
                */

                int remain = speed;
                for (int j = 0; j < remain; j++)
                {
                    croppingRectangles.push_back(currentPan);
                }
            }

        }
    }
}

/**
 * Adjusts the crop window position.
 * @param moveToPosition
 * @param numFrames
 * @param movement
 * @param croppingRectangles
 */
void PanLogic::rePosition(Position moveToPosition, int numFrames, PresenterMotion::Movement &movement, std::vector<cv::Rect> &croppingRectangles)
{

    int startPos = getPan(currentPan);
    int endPos;
    bool right;

    switch (moveToPosition)
    {
        case LEFT:
            if (movement.start.x > movement.end.x)
            {
                endPos = movement.start.x;
            }
            else
            {
                endPos = movement.end.x;
            }
            break;

        case RIGHT:
            if (movement.start.x < movement.end.x)
            {
                endPos = movement.start.x;
            }
            else
            {
                endPos = movement.end.x;
            }
            break;
        case CENTER:
            if (movement.start.x < movement.end.x)
            {
                endPos = movement.start.x + movement.length() / 2;
            }
            else
            {
                endPos = movement.start.x - movement.length() / 2;
            }
            break;
    }

    right = startPos > endPos;

    smoothMove(startPos, endPos, numFrames, right, moveToPosition, croppingRectangles);

}

//Method to initialise the dimensions of the panning class
/**
 * Initialise the dimensions of the frame and cropping window
 * @param inputFrameSize
 * @param cropSize
 * @param yPanLevel
 */
void PanLogic::initialise(cv::Size inputFrameSize, cv::Size cropSize, int yPanLevel)
{

    cropFrameWidth = cropSize.width;
    cropFrameHeight = cropSize.height;
    inputFrameWidth = inputFrameSize.width;
    inputFrameHeight = inputFrameSize.height;

    yLevelOfPanWindow = yPanLevel;

    boardOffSet = (int) (0.1 * cropFrameWidth);

}

/**
 * Checks if a pan is necessary
 * @param start
 * @param end
 * @return
 */
bool PanLogic::checkIfPanRequired(int start, int end)
{

    //Add some buffer padding
    return start >= currentPan.tl().x + 1.5 * boardOffSet && end >= currentPan.tl().x + 1.5 * boardOffSet &&
           start <= currentPan.br().x - 1.5 * boardOffSet && end <= currentPan.br().x - 1.5 * boardOffSet;

}

/**
 * Checks edge cases of the pan operation and adjusts coordinates accordingly. Leaves pan operation in an orderly state
 * @param currentPosX
 * @param crop
 */
void PanLogic::setPan(int currentPosX, Rect &crop)
{
    Rect tempRect = Rect(currentPosX - (cropFrameWidth / 2), yLevelOfPanWindow, cropFrameWidth, cropFrameHeight);
    if (!inBounds(tempRect))
    {
        if (initialCrop)
        {
            if (currentPosX > cropFrameWidth)
            {
                tempRect = Rect(inputFrameWidth - cropFrameWidth, yLevelOfPanWindow, cropFrameWidth, cropFrameHeight);
            }
            else
            {
                tempRect = Rect(0, yLevelOfPanWindow, cropFrameWidth, cropFrameHeight);
            }

            initialCrop = false;
            currentPan = tempRect;
        }
        crop = currentPan;
    }
    else
    {
        crop = tempRect;
    }
}

/**
 * Checks that a crop window falls within the bounds of the original frame
 * @param crop
 * @return
 */
bool PanLogic::inBounds(cv::Rect &crop)
{
    return crop.tl().x >= 0 && crop.tl().x + cropFrameWidth <= inputFrameWidth;
}

/**
 * returns crop window coordinates (central x-value of rectangle)
 * @param crop
 * @return
 */
int PanLogic::getPan(cv::Rect &crop)
{
    return crop.x + (cropFrameWidth / 2);
}


