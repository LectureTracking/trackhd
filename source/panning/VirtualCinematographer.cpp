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
// Created by Mohamed Tanweer Khatieb on 2016/07/21.
//

#include "../segmentation/Track4KPreProcess.h"
#include "VirtualCinematographer.h"

#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

int skipLecturePosition = 2; //Only process every second point of lecturers position (Helps filter noisy movement/jitter out)

/**
 * Main method for the Virtual Cinematographer module
 * @param persistentData
 * @return
 */
int VirtualCinematographer::cinematographerDriver(PersistentData &persistentData) {
    //Vector of points representing the lecturers positions
    vector<Point> lectPoints;

    long int y = persistentData.y_top;

    if (y < 0) {
        //Set a fixed y-value for the crop window
        long int y_value = 0;

        //Generate this fixed y-value from average y-value of all lecture positions
        for (int i = 0; i < persistentData.lecturerTrackedLocationRectangles.size(); i++) {
            y_value += ((persistentData.lecturerTrackedLocationRectangles.at(i).tl().y +
                         (persistentData.lecturerTrackedLocationRectangles.at(i).height / 2)));
        }

        y_value = y_value / persistentData.lecturerTrackedLocationRectangles.size();

        //Add an offset to the y-value
        y = y_value - 500;

        int topY = persistentData.topAndBottomCrop.first;
        int bottomY = persistentData.topAndBottomCrop.second;
        int availableHeight = bottomY - topY;
        int cropFrameHeight = persistentData.panOutputVideoSize.height;

        if (cropFrameHeight <= availableHeight) {
            //Crop frame is small enough to fit inside the margins
            if (y < topY) {
                y = topY;
            }

            if (y + cropFrameHeight > bottomY) {
                y -= (y + cropFrameHeight - bottomY);
            }

        } else {
            //Center the crop window
            int buffer = (cropFrameHeight - availableHeight) / 2;
            y = topY - buffer;
        }

    }

    //Remove every second point as we dont need that accuracy, only general direction of lecturer
    for (int i = 0; i < persistentData.lecturerTrackedLocationRectangles.size(); i += skipLecturePosition) {
        if (i > persistentData.lecturerTrackedLocationRectangles.size()) {
            break;
        }
        int x = ((persistentData.lecturerTrackedLocationRectangles.at(i).tl().x +
                  (persistentData.lecturerTrackedLocationRectangles.at(i).width / 2)));
        int y = persistentData.lecturerTrackedLocationRectangles.at(i).y;

        lectPoints.push_back(Point(x, y));
    }

    PresenterMotion presenterMotion;
    presenterMotion.generateMotionLines(lectPoints,
                                        (persistentData.skipFrameMovementDetection + 1) * skipLecturePosition);
    //4*2 because tracking section evaluates every 4th frame
    // and here we evaluate every 2nd one of those points, so essentially
    // we evaluating every 8th frame from the original video file

    presenterMotion.cullMotion(150, y);

    vector<PresenterMotion::Movement> movementLines;
    presenterMotion.getMotionLines(movementLines);

    PanLogic panLogic;
    panLogic.initialise(persistentData.videoDimension, persistentData.panOutputVideoSize, y);
    vector<Rect> cropRectangles;
    panLogic.doPan(movementLines, cropRectangles);

    ofstream cropdata;
    cropdata.open(persistentData.outputFile);

    if (cropdata.is_open()) {
        cout << "Writing cropping data to output file " << persistentData.outputFile << endl;
    } else {
        cerr << "Unable to write cropping data to output file " << persistentData.outputFile << endl;
        return 1;
    }

    //Loop over all frames in the input video and save the cropped frames to a stream as well as the board segment
    cout << "Crop rectangles : " << cropRectangles.size() << endl;
    cout << "Frames processed: " << persistentData.processedFrames << endl;

    cropdata << "# track4k " << persistentData.inputFile << " " << persistentData.processedFrames
             << " frames (frame top-left-x top-left-y) output frame size "
             << persistentData.panOutputVideoSize.width << " " << persistentData.panOutputVideoSize.height << endl;

    int last_x = -1;

    // Write out the pan x position and the fixed y position
    for (int i = 0; i < persistentData.processedFrames - 1; i++) {
        if (cropRectangles[i].x != last_x) {
            cropdata << i << " " << cropRectangles[i].x << " " << y << endl;
            last_x = cropRectangles[i].x;
        }
    }

    // Always write out the last frame
    int i = persistentData.processedFrames - 1;
    cropdata << i << " " << cropRectangles[i].x << " " << y << endl;

    // Close all file writers
    cropdata.close();

    return 0;
}
