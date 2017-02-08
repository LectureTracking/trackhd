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
// Created by Charles Fitzhenry on 2017/01/18.
//

#include "HelperMethods.h"

using namespace cv;

bool HelperMethods::RectA_inside_RectB(cv::Rect r1, cv::Rect r2)
{

    //Create intersection rectangle
    Rect r3 = r1 & r2;
    bool status = false;

    if (r3.area() > 0)
    {
        if (r3.area() == r2.area())
        {
            //cout << "r2 is inside r1" << endl;
            status = false;
        } else if (r3.area() == r1.area())
        {
            // cout << "r1 is inside r2" << endl;
            status = true;
        } else
        {
            //  cout << "Overlapping Rectangles" << endl;
            status = false;
        }
    } else
    {
        //  cout << "Non-overlapping Rectangles" << endl;
        status = false;

    }

    return status;
}
