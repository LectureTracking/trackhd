//
// Created by Charles on 2016/09/22.
//

#include "Board Segmentation/headers/Track4KPreProcess.h"
#include "PersistentData.h"
#include "Panning/headers/VirtualCinematographer.h"
#include "PointPlotter.h"
#include "Tracking/headers/MovementDetection.h"
#include "Tracking/headers/MovementDetection.h"
#include "Board Segmentation/headers/BoardSegmentationTesting.h"
#include "CodeTimer.h"

int main()
{

    CodeTimer ct;
    int vcTime = 0;


    //Create object of persistent data to share between sections
    PersistentData persistentData;

    // BoardSegmentationTesting bT;

    // bT.colourCorrectionTest("C:\\Users\\Charles\\Desktop\\Final 4k\\track4k\\Videos\\vlcsnap-2016-11-05-11h14m28s013.png");

    Track4KPreProcess pre;
    pre.preProcessDriver(persistentData);

    //PointPlotter pp;
    //pp.plotPoints(persistentData);

    vector<Rect> r;
    MovementDetection move(persistentData.inputFileName, &r);

    vector<Rect> *rR = new vector<Rect>();
    move.getLecturer(rR);

    for (int i = 0; i < rR->size(); i++)
    {
        persistentData.vals.push_back(std::move(rR->at(i)));
    }

    persistentData.skipFrameMovementDetection = move.getFrameSkipReset();


    ct.start();
    VirtualCinematographer vc;
    vc.cinematographerDriver(persistentData);
    vcTime = ct.stop();

    cout << "VC took a total of " << vcTime << endl;
}