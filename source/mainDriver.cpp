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
#include <stdexcept>
#include <regex>

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
    string crop_y_str = "";
    int cropWidth = 0;
    int cropHeight = 0;
    cv::Size saveDimensions;

    if ((argc < 5) || (argc > 6)) {
        cerr << endl
		<< "track4k build UCT " << __DATE__ << " " << __TIME__ << endl << endl
                << "Usage: track4k <input video> <output crop data file> <output-width> <output-height> [crop-y-top]" << endl << endl;
        return EXIT_FAILURE;
    }

    cout << "track4k build UCT " << __DATE__ << " " << __TIME__ << endl;

    if (argc == 6) {
        try {
          persistentData.y_top = stoi(argv[5]);
        } catch (...) {
          cerr << "Invalid value for crop top y position: " << argv[5] << endl;
          return EXIT_FAILURE;
        }
    }

    // Unused (until cropvid incorporated into track4k)
    // string codecInput = argv[5];
    // persistentData.codec = CV_FOURCC(codecInput[0], codecInput[1], codecInput[2], codecInput[3]);
    persistentData.codec = CV_FOURCC('X', '2', '6', '4');

    //Get filenames from the command line and store them
    inputFilename = argv[1];
    outputFilename = argv[2];

    //Extract the extensions from the filenames
    std::regex rx(".*\\.([a-z]+)$");
	std::smatch match;
    if (std::regex_match(inputFilename, match, rx) && match.size()>1) {
        inputFileExtension = match[1].str();
    }
    if (std::regex_match(outputFilename, match, rx) && match.size()>1) {
        outputFileExtension = match[1].str();
    }
 
    //Extract the crop dimensions from the parameters
    cropWidth = stoi(argv[3]);
    cropHeight = stoi(argv[4]);
    saveDimensions = cv::Size(cropWidth, cropHeight);

    //Update this information in PersistentData
    persistentData.inputFile = inputFilename;
    persistentData.outputFile = outputFilename;

    persistentData.panOutputVideoSize = saveDimensions;

    cout << "\n----------------------------------------" << endl;
    cout << "Stage [1 of 3] - Board Segmentation (skip)" << endl;
    cout << "----------------------------------------\n" << endl;

    Track4KPreProcess pre;

    if (!pre.preProcessDriver(persistentData)) {
	return EXIT_FAILURE;
    }

    cout << "\nStage 1 Complete" << endl;
    cout << "----------------------------------------\n" << endl;

    // Check that input resolution is larger than output resolution, otherwise there's nothing to do
    if ((persistentData.videoDimension.width <= cropWidth) || (persistentData.videoDimension.height < cropHeight)) {
        cerr << "Input video size is the same or smaller than output size: nothing to do." << endl;
        return EXIT_FAILURE;
    }

    vector<Rect> r;
    cout << "\n----------------------------------------" << endl;
    cout << "Stage [2 of 3] - Lecturer Tracking" << endl;
    cout << "----------------------------------------\n" << endl;

    MovementDetection move(persistentData, &r);
    vector<Rect> *rR = new vector<Rect>();
    move.getLecturer(rR, persistentData);

    cout << "\nStage 2 Complete" << endl;
    cout << "----------------------------------------\n" << endl;

    for (int i = 0; i < rR->size(); i++) {
        persistentData.lecturerTrackedLocationRectangles.push_back(std::move(rR->at(i)));
    }

    persistentData.skipFrameMovementDetection = move.getFrameSkipReset();

    cout << "\n----------------------------------------" << endl;
    cout << "Stage [3 of 3] - Virtual Cinematographer" << endl;
    cout << "----------------------------------------\n" << endl;
    if (outputFileExtension=="json") {
        VirtualCinematographer vc(new JsonVirtualCinematographerOutput());
        vc.cinematographerDriver(persistentData);
    }
    else {
        VirtualCinematographer vc(new DefaultVirtualCinematographerOutput());
        vc.cinematographerDriver(persistentData);
    }
    cout << "\nStage 3 Complete" << endl;
    cout << "----------------------------------------\n" << endl;

}
