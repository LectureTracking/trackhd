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

#ifndef TRACK4K_PANLOGIC_H
#define TRACK4K_PANLOGIC_H

#include <math.h>
#include "opencv2/opencv.hpp"
#include "PresenterMotion.h"

class PanLogic
{
    //Take in a vector of line segments
    //Decide if pan size contains line segments
    //Else pan from start to end of line segment
    //Need to consider how long the lecturer is allowed to be in a margin

    public:
        //Variables
        enum Position
        {
            LEFT, RIGHT, CENTER
        };


        //Methods
        long double smooth(double x);

        void smoothMove(int start, int end, int numFrames, bool right, Position panOffsetType, std::vector<cv::Rect> &croppingRectangles);

        void doPan(std::vector<PresenterMotion::Movement> &motionLines, std::vector<cv::Rect> &croppingRectangles);

        void rePosition(Position moveToPosition, int numFrames, PresenterMotion::Movement &movement, std::vector<cv::Rect> &croppingRectangles);

        void
        initialise(cv::Size inputFrameSize, cv::Size cropSize, int yPanLevel);

        bool checkIfPanRequired(int start, int end);

        void setPan(int currentPosX, cv::Rect &crop);

        int getPan(cv::Rect &crop);

        bool inBounds(int x);

        bool inBounds(cv::Rect &crop);


        //Size of the crop frame and input frame
        int cropFrameWidth;
        int cropFrameHeight;
        int inputFrameWidth;
        int inputFrameHeight;

        int yLevelOfPanWindow;


        int boardOffSet;

        cv::Rect currentPan;

};


#endif //TRACK4K_PANLOGIC_H
