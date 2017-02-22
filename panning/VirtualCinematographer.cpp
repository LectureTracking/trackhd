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
#include <fstream>

using namespace cv;
using namespace std;

/**
 * Main method for the Virtual Cinematographer module
 * @param persistentData
 * @return
 */
int VirtualCinematographer::cinematographerDriver(PersistentData &persistentData) {
    //Vector of points representing the lecturers positions
    vector<Point> lectPoints;

    //Set a fixed y-value for the crop window
    long int y_value = 0;

    //Generate this fixed y-value from average y-value of all lecture positions
    for (int i = 0; i < persistentData.vals.size(); i++) {
        y_value += ((persistentData.vals.at(i).tl().y + (persistentData.vals.at(i).height / 2)));
    }

    y_value = y_value / persistentData.vals.size();

    //Add an offset to the y-value
    int y = y_value - 600;

    //Remove every second point as we dont need that accuracy, only general direction of lecturer
    for (int i = 0; i < persistentData.vals.size(); i += 2) {
        if (i > persistentData.vals.size()) {
            break;
        }
        int x = ((persistentData.vals.at(i).tl().x + (persistentData.vals.at(i).width / 2)));
        int y = persistentData.vals.at(i).y;

        lectPoints.push_back(Point(x, y));
    }

    PresenterMotion presenterMotion;
    presenterMotion.generateMotionLines(lectPoints, 4 * 2);
    presenterMotion.cullMotion(150, y);

    vector<PresenterMotion::Movement> movementLines;
    presenterMotion.getMotionLines(movementLines);

    PanLogic panLogic;
    panLogic.initialise(persistentData.videoDimension, persistentData.panOutputVideoSize, y);
    vector<Rect> cropRectangles;
    panLogic.doPan(movementLines, cropRectangles);


    cout << persistentData.outputVideoFilenameSuffix + "." + persistentData.saveFileExtension << endl;
    cout << "BoardSegment-" << persistentData.outputVideoFilenameSuffix + persistentData.saveFileExtension << endl;
    cout << persistentData.codec << endl;

    //Create video writer object for writing the cropped output video
    VideoWriter outputVideo;
    outputVideo.open(persistentData.outputVideoFilenameSuffix + "." + persistentData.saveFileExtension,
                     persistentData.codec, persistentData.fps, persistentData.panOutputVideoSize, 1);

    //Create video writer object to write the segmented board stream
    VideoWriter outputBoardSegment;
    outputBoardSegment.open(
            "BoardSegment-" + persistentData.outputVideoFilenameSuffix + "." + persistentData.saveFileExtension,
            persistentData.codec, persistentData.fps,
            Size(persistentData.boardCropRegion.width, persistentData.boardCropRegion.height), 1);

    //Open original input video file
    FileReader fileReader;
    fileReader.readFile(persistentData.inputFileName, persistentData);

    Mat drawing;

    //Loop over all frames in the input video and save the cropped frames to a stream as well as the board segment
    for (int i = 0; i < cropRectangles.size(); i++) {

        fileReader.getNextFrame(drawing);

        outputVideo.write(drawing(cropRectangles[i]));
        outputBoardSegment.write(drawing(persistentData.boardCropRegion));

        drawing.release();

    }

    //Close all file writers
    outputVideo.release();
    outputBoardSegment.release();
    fileReader.getInputVideo().release();
}