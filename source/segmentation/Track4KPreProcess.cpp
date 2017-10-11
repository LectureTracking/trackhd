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
// Created by Charles Fitzhenry on 2016/07/19.
//

#include "IlluminationCorrection.h"

#include "MotionDetection.h"
#include "Track4KPreProcess.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

void Track4KPreProcess::preProcessDriver(PersistentData &persistentData)
{

    vector<Mat> frameVector;

    //Read in video file
    FileReader fileReader;
    fileReader.readFile(persistentData.inputFileName, persistentData);

    //Create objects
    MotionDetection motionDetection; //Detects and segments overall merged motion over a given number of frames
    BoardDetection boardDetection; //Detects boards

    //Keep reading in frames from the video file until the end is reached.
    //Number of frames to read on each iteration is defined in the PersistentData class
    while (!fileReader.isEndOfFile())
    {
        //Read in frames
        fileReader.getNextSegment(persistentData.segmentationNumFramesToProcessPerIteration, frameVector);

        ///Unused feature - intended to speed up tracking section search space in future
        //Detect areas of motion
        //motionDetection.subtract(frameVector, persistentData);

        //Detect the boards
        //boardDetection.extractBoards(frameVector, persistentData);

    }

    //If board crop was found, write this coordinates to text file
    if(persistentData.boardsFound){
        int out_w = persistentData.boardCropRegion.width;
        int out_h = persistentData.boardCropRegion.height;
        int out_x = persistentData.boardCropRegion.tl().x;
        int out_y = persistentData.boardCropRegion.tl().y;


        ofstream outTextFile;
        outTextFile.open ("boardCropCoordinates.txt");
        outTextFile << out_w << ":" << out_h << ":" << out_x << ":" << out_y;
        outTextFile.close();

    }

    fileReader.getInputVideo().release();
}
