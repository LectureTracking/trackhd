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

bool debugOn = true;
bool showDebugWindows = true;
bool printText = false;
bool saveDebugVideo = true;


/**
 * Main method for the Virtual Cinematographer module
 * @param persistentData
 * @return
 */
int VirtualCinematographer::cinematographerDriver(PersistentData &persistentData)
{
    vector<Point> lectPoints;


    cout << "Max count " << persistentData.vals.size() << endl;
    cout << "Max Skip " << persistentData.skipFrameMovementDetection << endl;


    ofstream myfile;
    ofstream rectFile;
    myfile.open("TrackingCoordinatesPanningTest.txt");
    rectFile.open("RectFile.txt");
    for (int i = 0; i < persistentData.vals.size(); i++)
    {
        myfile << to_string(((persistentData.vals.at(i).tl().x + (persistentData.vals.at(i).width / 2)))) << "\n";
        rectFile << persistentData.vals.at(i).tl().x << " " << persistentData.vals.at(i).tl().y << " " << persistentData.vals.at(i).br().x << " "
                 << persistentData.vals.at(i).br().y << endl;
    }

    long int y_value = 0;


    for (int i = 0; i < persistentData.vals.size(); i++)
    {
        y_value += ((persistentData.vals.at(i).tl().y + (persistentData.vals.at(i).height / 2)));
    }

    y_value = y_value / persistentData.vals.size();

    myfile.close();
    rectFile.close();
    int y = y_value - 600;
    for (int i = 0; i < persistentData.vals.size(); i += 2)
    {
        if (i > persistentData.vals.size())
        {
            break;
        }
        int x = ((persistentData.vals.at(i).tl().x + (persistentData.vals.at(i).width / 2)));
        int y = persistentData.vals.at(i).y;

        lectPoints.push_back(Point(x, y));
    }

    PresenterMotion presenterMotion;

    presenterMotion.generateMotionLines(lectPoints, 4 * 2);

    presenterMotion.cullMotion(150, y);

    cout << "Debug 1" << endl;

    Size sS(3840, presenterMotion.motion.size() * 11);

    Mat lineDrawing = Mat::zeros(sS, CV_8UC3);

    cout << "Debug 2" << endl;

    presenterMotion.generateMotionImage(lineDrawing);

    cout << "Debug 3" << endl;

    vector<PresenterMotion::Movement> movementLines;
    presenterMotion.getMotionLines(movementLines);

    cout << "Debug 4" << endl;

    imwrite("Line Image.jpg", lineDrawing);

    PanLogic panLogic;

    panLogic.initialise(persistentData.videoDimension, persistentData.panOutputVideoSize, y);

    vector<Rect> cropRectangles;


    panLogic.doPan(movementLines, cropRectangles);

    cout << "Debug 5" << endl;

    VideoWriter outputVideo;
    outputVideo.open(persistentData.outputVideoFilenameSuffix, CV_FOURCC('F', 'L', 'V', '1'), persistentData.fps, persistentData.panOutputVideoSize, 1);

    VideoWriter outputVideoAll;
    outputVideoAll.open("finalOverviewDebugView.flv", CV_FOURCC('F', 'L', 'V', '1'), persistentData.fps, persistentData.videoDimension, 1);

    VideoWriter outputBoardSegment;
    outputBoardSegment.open("BoardSegment-" + persistentData.outputVideoFilenameSuffix, CV_FOURCC('F', 'L', 'V', '1'), persistentData.fps, Size(persistentData.boardCropRegion.width, persistentData.boardCropRegion.height), 1);

    FileReader fileReader;
    fileReader.readFile(persistentData.inputFileName, persistentData);

    int lineindex = 0;
    int accumulatedLineCount = 0;

    Mat drawing;

    RNG randNumGen(12345);
    Scalar color = Scalar(randNumGen.uniform(0, 255), randNumGen.uniform(0, 255), randNumGen.uniform(0, 255));

    int secondIteration = 0;
    int sIndex = 0;
    int boardCounterWriter = persistentData.fps;

    for (int i = 0; i < cropRectangles.size(); i++)
    {


        if (i > (movementLines.at(lineindex).numFrames) + accumulatedLineCount)
        {
            accumulatedLineCount += movementLines.at(lineindex).numFrames;
            lineindex++;
        }

        fileReader.getNextFrame(drawing);


        outputVideo.write(drawing(cropRectangles[i]));

        if (boardCounterWriter == 0)
        {
            boardCounterWriter = persistentData.fps;
        }

        boardCounterWriter--;

        if (saveDebugVideo)
        {
            rectangle(drawing, cropRectangles[i].tl(), cropRectangles[i].br(), color, 2, 8, 0);

            if (secondIteration > 3)
            {
                secondIteration = 0;
                sIndex++;
            }
            rectangle(drawing, persistentData.vals.at(sIndex).tl(), persistentData.vals.at(sIndex).br(), color, 2, 8, 0);


            secondIteration++;

            if (movementLines.at(lineindex).isDropData)
            {
                line(drawing, Point(movementLines.at(lineindex).start.x, 500), Point(movementLines.at(lineindex).end.x, 500), Scalar(0, 0, 0), 10, 8);

            }
            else if (movementLines.at(lineindex).right)
            {
                arrowedLine(drawing, Point(movementLines.at(lineindex).start.x, 500), Point(movementLines.at(lineindex).end.x, 500), Scalar(0, 0, 255), 10, 8);
            }
            else if (!movementLines.at(lineindex).right)
            {
                arrowedLine(drawing, Point(movementLines.at(lineindex).start.x, 500), Point(movementLines.at(lineindex).end.x, 500), Scalar(255, 170, 0), 10, 8);
            }


            if (movementLines.at(lineindex).boardUsed)
            {
                if (movementLines.at(lineindex).rightBoardUsed)
                {
                    if (movementLines.at(lineindex).isDropData)
                    {
                        putText(drawing, "Dropped Data & Right Board Flagged as Used", Point(100, 100), FONT_HERSHEY_PLAIN, 6.0,
                                color, 4.0);

                    }
                    else if (movementLines.at(lineindex).right)
                    {
                        putText(drawing, "Moving Right & Right Board Flagged as Used", Point(100, 100), FONT_HERSHEY_PLAIN, 6.0, color, 4.0);

                    }
                    else if (!movementLines.at(lineindex).right)
                    {
                        putText(drawing, "Moving Left & Right Board Flagged as Used", Point(100, 100), FONT_HERSHEY_PLAIN, 6.0, color, 4.0);

                    }
                }
                else
                {
                    if (movementLines.at(lineindex).isDropData)
                    {
                        putText(drawing, "Dropped Data & Left Board Flagged as Used", Point(100, 100), FONT_HERSHEY_PLAIN, 6.0, color, 4.0);

                    }
                    else if (movementLines.at(lineindex).right)
                    {
                        putText(drawing, "Moving Right & Left Board Flagged as Used", Point(100, 100), FONT_HERSHEY_PLAIN, 6.0, color, 4.0);

                    }
                    else if (!movementLines.at(lineindex).right)
                    {
                        putText(drawing, "Moving Left & Left Board Flagged as Used", Point(100, 100), FONT_HERSHEY_PLAIN, 6.0, color, 4.0);

                    }
                }
            }
            else
            {
                if (movementLines.at(lineindex).isDropData)
                {
                    putText(drawing, "Dropped Data", Point(100, 100), FONT_HERSHEY_PLAIN, 6.0, color, 4.0);

                }
                else if (movementLines.at(lineindex).right)
                {
                    putText(drawing, "Moving Right", Point(100, 100), FONT_HERSHEY_PLAIN, 6.0, color, 4.0);

                }
                else if (!movementLines.at(lineindex).right)
                {
                    putText(drawing, "Moving Left", Point(100, 100), FONT_HERSHEY_PLAIN, 6.0, color, 4.0);

                }
            }
            if (debugOn && showDebugWindows)
            {

                imshow("Frame", drawing);
                waitKey(1);
            }

            outputVideoAll.write(drawing);
        }


        drawing.release();

    }
    outputVideo.release();
    outputVideoAll.release();
    outputBoardSegment.release();
    fileReader.getInputVideo().release();
}