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
// Created by Charles Fitzhenry on 2016/09/22.
//

#include <stdlib.h>
#include "segmentation/Track4KPreProcess.h"
#include "panning/VirtualCinematographer.h"
#include "tracking/MovementDetection.h"

using namespace std;

int main(int argc, char *argv[]) {

    //Create object of persistent data to share between sections
    PersistentData persistentData;

    //Store input parameters from the input command
    string inputFilename = "";
    string outputFilename = "";
    string inputFileExtension = "";
    string outputFileExtension = "";
    int cropWidth = 0;
    int cropHeight = 0;
    cv::Size saveDimensions;

    //Check if input of command line parameters are valid
    if (argc == 6) {
        string codecInput = argv[5];
        persistentData.codec = CV_FOURCC(codecInput[0], codecInput[1], codecInput[2], codecInput[3]);
    } else if (argc == 5) {
        //Use default codec
        persistentData.codec = CV_FOURCC('X', '2', '6', '4');
    } else {
        cerr << endl
		<< "track4k build UCT " << __DATE__ << " " << __TIME__ << endl << endl
                << "Parameters:" << endl << " track4k <inputFileName> <outputFileName> <output-width> <output-height>" << endl << endl;
        return -1;
    }

    //Get filenames from the command line and store them
    inputFilename = argv[1];
    outputFilename = argv[2];

    //Extract the extensions from the filenames
    inputFileExtension = inputFilename.substr(inputFilename.find_first_of('.') + 1);

    //Extract the crop dimensions from the parameters
    cropWidth = stoi(argv[3]);
    cropHeight = stoi(argv[4]);
    saveDimensions = cv::Size(cropWidth, cropHeight);

    //Update this information in PersistentData
    persistentData.inputFile = inputFilename;
    persistentData.outputFile = outputFilename;

    persistentData.panOutputVideoSize = saveDimensions;

    cout << "track4k build UCT " << __DATE__ << " " << __TIME__ << endl;

    cout << "\n----------------------------------------" << endl;
    cout << "Stage [1 of 3] - Board Segmentation (skip)" << endl;
    cout << "----------------------------------------\n" << endl;
    Track4KPreProcess pre;
    pre.preProcessDriver(persistentData);
    cout << "\nStage 1 Complete" << endl;
    cout << "----------------------------------------\n" << endl;

    vector<Rect> r;
    cout << "\n----------------------------------------" << endl;
    cout << "Stage [2 of 3] - Lecturer Tracking" << endl;
    cout << "----------------------------------------\n" << endl;
    MovementDetection move(persistentData, &r);
    vector<Rect> *rR = new vector<Rect>();
    move.getLecturer(rR);
    cout << "\nStage 2 Complete" << endl;
    cout << "----------------------------------------\n" << endl;

    for (int i = 0; i < rR->size(); i++) {
        persistentData.lecturerTrackedLocationRectangles.push_back(std::move(rR->at(i)));
    }

    persistentData.skipFrameMovementDetection = move.getFrameSkipReset();

    cout << "\n----------------------------------------" << endl;
    cout << "Stage [3 of 3] - Virtual Cinematographer" << endl;
    cout << "----------------------------------------\n" << endl;
    VirtualCinematographer vc;
    vc.cinematographerDriver(persistentData);
    cout << "\nStage 3 Complete" << endl;
    cout << "----------------------------------------\n" << endl;

}
