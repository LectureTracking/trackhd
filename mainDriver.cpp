//
// Created by Charles on 2016/09/22.
//

#include <stdlib.h>
#include "Board Segmentation/headers/Track4KPreProcess.h"
#include "PersistentData.h"
#include "Panning/headers/VirtualCinematographer.h"
#include "PointPlotter.h"
#include "Tracking/headers/MovementDetection.h"
#include "Tracking/headers/MovementDetection.h"
#include "CodeTimer.h"

using namespace std;

int main(int argc, char *argv[])
{

    //Store input parameters from the input command
    string inputFilename = "";
    string outputFilename = "";
    string inputFileExtension = "";
    string outputFileExtension = "";
    int cropWidth = 0;
    int cropHeight = 0;
    cv::Size saveDimensions;



    //Check if input of command line parameters are valid
    if (argc != 5)
    {
        cerr
                << "The number of parameters entered were incorrect. Expected track4k.exe <inputFileName.extension> <outputFileName.extension> <crop width> <crop height>"
                << endl;
        return -1;
    }

    //Get filenames from the command line and store them
    inputFilename = argv[1];
    outputFilename = argv[2];

    //Extract the extensions from the filenames
    inputFileExtension = inputFilename.substr(inputFilename.find_first_of('.') + 1);
    outputFileExtension = outputFilename.substr(outputFilename.find_first_of('.') + 1);

    //Extract the crop dimensions from the parameters
    cropWidth = stoi(argv[3]);
    cropHeight = stoi(argv[4]);
    saveDimensions = cv::Size(cropWidth, cropHeight);


    //Update this information in PersistantData
    //Create object of persistent data to share between sections
    PersistentData persistentData;

    persistentData.inputFileName = inputFilename;
    persistentData.outputVideoFilenameSuffix = outputFilename;
    persistentData.saveFileExtension = outputFileExtension;
    persistentData.panOutputVideoSize = saveDimensions;


    Track4KPreProcess pre;
    pre.preProcessDriver(persistentData);

    vector<Rect> r;
    MovementDetection move(persistentData.inputFileName, &r);

    vector<Rect> *rR = new vector<Rect>();
    move.getLecturer(rR);

    for (int i = 0; i < rR->size(); i++)
    {
        persistentData.vals.push_back(std::move(rR->at(i)));
    }

    persistentData.skipFrameMovementDetection = move.getFrameSkipReset();

    VirtualCinematographer vc;
    vc.cinematographerDriver(persistentData);

}