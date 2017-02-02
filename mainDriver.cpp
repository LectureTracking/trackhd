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

#include "Board Segmentation/headers/Track4KPreProcess.h"
#include "PersistentData.h"
#include "Panning/headers/VirtualCinematographer.h"
#include "PointPlotter.h"
#include "Tracking/headers/MovementDetection.h"
#include "Tracking/headers/MovementDetection.h"
#include "CodeTimer.h"

int main(int argc, char* argv[])
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