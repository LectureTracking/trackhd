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
// Created by Mohamed Tanweer Khatieb on 2016/09/16.
//

#ifndef TRACK4K_PRESENTERMOTION_H
#define TRACK4K_PRESENTERMOTION_H

#include "opencv2/opencv.hpp"
#include "../MetaFrame.h"
#include "../PersistentData.h"
#include <vector>
#include <stdlib.h>


class PresenterMotion
{

public:

    /**
     * Struct stores data of each line segment. It also can be used to store noise by enabling the @isDropData boolean
     */
    struct Movement
    {
        bool isDropData = false;
        cv::Point start;
        cv::Point end;
        int numFrames;
        bool right;
        bool boardUsed = false;
        bool rightBoardUsed = false;

        int length()
        {
            return (int) abs(start.x - end.x);  // This line was complaining about returning a double into an int returning function. I convert it to int before returning now.
                                                // Was this a possible cause for the co-ordinate mix-up? it said double may be too big to fit into an int
        }
    };

    std::vector<Movement> motion;

    void generateMotionLines(std::vector<cv::Point> lecturerPositions, int skipFrames);

    void DrawLine(cv::Mat &img, cv::Point start, cv::Point end, cv::Scalar s);

    void cullMotion(int ignoreThresh, int yFix);

    void repairCulling(int ignoreThresh);

    void generateMotionImage(cv::Mat &img);

    void getMotionLines(std::vector<Movement> &outMotionVec);

    void relinkMotion();

    void attatchBoardUsage(PersistentData &persistentData);

};


#endif //TRACK4K_PRESENTERMOTION_H
