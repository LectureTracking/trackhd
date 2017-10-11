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
// Created by Charles Fitzhenry on 2016/07/20.
//

#ifndef TRACK4K_MOTIONDETECTION_H
#define TRACK4K_MOTIONDETECTION_H

//opencv
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "../FileReader.h"

#include "../MetaFrame.h"

//C
#include <stdio.h>
//C++
#include <iostream>
#include <sstream>

class MotionDetection
{
public:
/**
 * This method bounds the merged difference image with rectangles
 * @param threshold_output is the image containing the merged motion from the subtract method.
 * @return a rectangle that bounds all motion
 */
    Rect boundMotion(cv::Mat threshold_output);

    /**
     * This method loops over all frames and takes two consecutive frames and differences them. This difference is then
     * accumulated in a final frame containing all motion for the frames in the vector.
     *
     * @param frames is the vector containing the input frames
     * @param persistentData is the link to the central class sharing all data between the different modules.
     */
    void subtract(std::vector<cv::Mat> &frames, PersistentData &persistentData);
};

#endif //TRACK4K_MOTIONDETECTION_H
