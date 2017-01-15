//
// Created by Tanweer Khatieb on 2016/07/21.
//
#include "../../Board Segmentation/headers/Track4KPreProcess.h"
#include "../headers/VirtualCinematographer.h"
#include "../headers/VideoOutput.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include "../headers/PresenterMotion.h"
#include "../../Tracking/headers/Ghost.h"
#include "../../CodeTimer.h"

using namespace cv;
using namespace std;

bool debugOn = true;
bool showDebugWindows = true;
bool printText = false;
bool saveDebugVideo = true;

CodeTimer mainTimer;

int VirtualCinematographer::cinematographerDriver(PersistentData &pD)
{


    //This is the main method for the virtual cinematography

    //For debugging
    if (debugOn && showDebugWindows)
    {
        namedWindow("Frame", WINDOW_NORMAL);
        resizeWindow("Frame", 1080, 720);
    }


    //Read in text file of lecturer positions
    vector<Point> lectPoints; //Stores coordinates


    cout << "Max count " << pD.vals.size() << endl;
    cout << "Max Skip " << pD.skipFrameMovementDetection << endl;


    ofstream myfile;
    ofstream rectFile;
    myfile.open("TrackingCoordinatesPanningTest.txt");
    rectFile.open("RectFile.txt");
    for (int i = 0; i < pD.vals.size(); i++)
    {
        myfile << to_string(((pD.vals.at(i).tl().x + (pD.vals.at(i).width / 2)))) << "\n";
        rectFile << pD.vals.at(i).tl().x << " " << pD.vals.at(i).tl().y << " " << pD.vals.at(i).br().x << " "
                 << pD.vals.at(i).br().y << endl;
    }

    //Generate y-value
    long int ySum = 0;


    mainTimer.start();
    for (int i = 0; i < pD.vals.size(); i++)
    {
        ySum += ((pD.vals.at(i).tl().y + (pD.vals.at(i).height / 2)));
    }
    mainTimer.stop(true);

    ySum = ySum / pD.vals.size();

    myfile.close();
    rectFile.close();
    int y = ySum -600;
    for (int i = 0; i < pD.vals.size(); i += 2)
    {
        if (i > pD.vals.size())
        {
            break;
        }
        //int x = pD.vals.at(i).tl().x;
        int x = ((pD.vals.at(i).tl().x + (pD.vals.at(i).width / 2)));
        int y = pD.vals.at(i).y;

        lectPoints.push_back(Point(x, y));
    }

/*

    lectPoints.clear();

    ifstream input("TrackingCoordinatesPanningTest40s.txt");
    int x;
    int y = 500;
    int sv = 1;
    while (input >> x)
    {

        // lectPoints.push_back(Point(x, y));

        if (sv == 1)
        {
            lectPoints.push_back(Point(x, y));
            sv = 0;
        } else
        {
            sv++;
        }

    }

    //Read in rectangles from file

    int tx;
    int ty;
    int bx;
    int by;

    ifstream rectIn("RectFile.txt");
    while (rectIn >> tx >> ty >> bx >> by)
    {
        pD.vals.push_back(Rect(Point(tx, ty), Point(bx, by)));
    }

*/
    //---------------------//
    // Create Motion Lines //
    //---------------------//

    //Instantiate PresenterMotion Class
    PresenterMotion pM;

    //pM.generateMotionLines(lectPoints, (pD.skipFrameMovementDetection + 1));

    pM.generateMotionLines(lectPoints, 4 * 2);


    pM.checkLogic();

    //pM.printLines();

    // Passing a fixed y-value in for height. This is only used in the culled points, and the original y value is preserved in the motion lines that pass the threshold
    pM.cullMotion(150, y);

    //pM.relinkMotion();

    //pM.repairCulling(20);


    cout << "Debug 1" << endl;


    Size sS(3840, pM.motion.size() * 11);
    Mat lineDrawing = Mat::zeros(sS, CV_8UC3);

    cout << "Debug 2" << endl;

    //Create an image showing lines of motion
    pM.generateMotionImage(lineDrawing);

    cout << "Debug 3" << endl;

    //Get motion lines. Returns a vector of type Movement
    vector<PresenterMotion::Movement> movementLines;
    pM.getMotionLines(movementLines);

    cout << "Debug 4" << endl;


    //imshow("Frame", lineDrawing);
    //waitKey(0);
    //destroyAllWindows();

    imwrite("Line Image.jpg", lineDrawing);

    //Attache board usage data to the motion lines
    //pM.attatchBoardUsage(pD);

    PanLogic pL;

    //Initialise the Pan Class with the size of the input frame and the size of the crop frame size
    //TODO - Set y level

    pL.initialise(pD.videoDimension, pD.panOutputVideoSize, y);

    vector<Rect> cropRectangles;


    pL.doPan(movementLines, cropRectangles);

    cout << "Debug 5" << endl;


    //Just to debug
    int sm = 0;
    for (int i = 0; i < movementLines.size(); i++)
    {
        sm += movementLines.at(i).numFrames;
    }

    if (debugOn && printText)
    {
        cout << "These values [1], [3], & [4] below should all be the same!!!" << endl;
        cout << "[1] Number of original frames: " << pD.totalFrames << endl;
        cout << "[2] Number of Points from tracking stage: " << lectPoints.size() << endl;
        cout << "[3] Number of frames as stored by motion Lines: " << sm << endl;
        cout << "[4] Number of Bounding/Cropping rectangles: " << cropRectangles.size() << endl;
    }


    VideoWriter outputVideo;
    outputVideo.open(pD.panOutputVideoSaveFileName, CV_FOURCC('F', 'L', 'V', '1'), pD.fps, pD.panOutputVideoSize, 1);


    VideoWriter outputVideoAll;
    outputVideoAll.open("finalOverviewDebugView.flv", CV_FOURCC('F', 'L', 'V', '1'), pD.fps, pD.videoDimension, 1);

    VideoWriter outputBoardSegment;
    outputBoardSegment.open("BoardSegment-" + pD.panOutputVideoSaveFileName, CV_FOURCC('F', 'L', 'V', '1'), pD.fps,
                            Size(pD.boardCropRegion.width, pD.boardCropRegion.height), 1);

    FileReader fileReader;
    fileReader.readFile(pD.inputFileName, pD);

    int lineindex = 0;
    int accumulatedLineCount = 0;

    Mat drawing;

    RNG rng(12345);
    Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));

    int secondIteration = 0;
    int sIndex = 0;
    int printInfoCounter = pD.fps * 60;
    int boardCounterWriter = pD.fps;

    CodeTimer timer;

    timer.start();

    int timeSum = 0;
    int remainTime = 0;

    int seconds, hours, minutes;


    for (int i = 0; i < cropRectangles.size(); i++)
    {


        if (i > (movementLines.at(lineindex).numFrames) + accumulatedLineCount)
        {
            accumulatedLineCount += movementLines.at(lineindex).numFrames;
            lineindex++;
        }

        if (printInfoCounter == 0)
        {
            cout << "Completed " << (i + 1) << " of " << cropRectangles.size() << " ("
                 << (((i + 1) / cropRectangles.size()) * 100) << ")" << endl;

            timeSum += timer.stop(1);

            minutes = timeSum / 60;
            seconds = timeSum % 60;
            hours = minutes / 60;
            minutes = minutes % 60;
            cout << "Time taken: " << hours << ":" << minutes
                 << ":" << seconds << "\n";


            remainTime = ((float) timeSum / (i + 1)) * (cropRectangles.size() - i);
            minutes = remainTime / 60;
            seconds = remainTime % 60;
            hours = minutes / 60;
            minutes = minutes % 60;
            cout << "Estimated time remaining: " << hours << ":" << minutes
                 << ":" << seconds << "\n";
            cout << "------------------------------------------\n" << endl;

            timer.start();
            printInfoCounter = pD.fps * 60;
        }
        printInfoCounter--;


        //Basic crop operation

        fileReader.getNextFrame(drawing);



        outputVideo.write(drawing(cropRectangles[i]));

        //Save the board crop region
        if (boardCounterWriter == 0)
        {
            //Write file for the blackboard segmentation
            boardCounterWriter = pD.fps;
            //outputBoardSegment.write(drawing(pD.boardCropRegion));
        }

        boardCounterWriter--;




        //Debug overview
        if (saveDebugVideo)
        {
            rectangle(drawing, cropRectangles[i].tl(), cropRectangles[i].br(), color, 2, 8, 0);

            if (secondIteration > 3)
            {
                secondIteration = 0;
                sIndex++;
            }
            rectangle(drawing, pD.vals.at(sIndex).tl(), pD.vals.at(sIndex).br(), color, 2, 8, 0);


            secondIteration++;

            if (movementLines.at(lineindex).isDropData)
            {
                line(drawing, Point(movementLines.at(lineindex).start.x, 500),
                     Point(movementLines.at(lineindex).end.x, 500), Scalar(0, 0, 0), 10,
                     8);

            } else if (movementLines.at(lineindex).right)
            {
                arrowedLine(drawing, Point(movementLines.at(lineindex).start.x, 500),
                            Point(movementLines.at(lineindex).end.x, 500), Scalar(0, 0, 255), 10,
                            8);
            } else if (!movementLines.at(lineindex).right)
            {
                arrowedLine(drawing, Point(movementLines.at(lineindex).start.x, 500),
                            Point(movementLines.at(lineindex).end.x, 500), Scalar(255, 170, 0), 10,
                            8);
            }


            if (movementLines.at(lineindex).boardUsed)
            {
                if (movementLines.at(lineindex).rightBoardUsed)
                {
                    if (movementLines.at(lineindex).isDropData)
                    {
                        putText(drawing, "Dropped Data & Right Board Flagged as Used",
                                Point(100, 100), FONT_HERSHEY_PLAIN, 6.0,
                                color, 4.0);

                    } else if (movementLines.at(lineindex).right)
                    {
                        putText(drawing, "Moving Right & Right Board Flagged as Used",
                                Point(100, 100), FONT_HERSHEY_PLAIN, 6.0,
                                color, 4.0);

                    } else if (!movementLines.at(lineindex).right)
                    {
                        putText(drawing, "Moving Left & Right Board Flagged as Used",
                                Point(100, 100), FONT_HERSHEY_PLAIN, 6.0,
                                color, 4.0);

                    }

                } else
                {
                    if (movementLines.at(lineindex).isDropData)
                    {
                        putText(drawing, "Dropped Data & Left Board Flagged as Used",
                                Point(100, 100), FONT_HERSHEY_PLAIN, 6.0,
                                color, 4.0);

                    } else if (movementLines.at(lineindex).right)
                    {
                        putText(drawing, "Moving Right & Left Board Flagged as Used",
                                Point(100, 100), FONT_HERSHEY_PLAIN, 6.0,
                                color, 4.0);

                    } else if (!movementLines.at(lineindex).right)
                    {
                        putText(drawing, "Moving Left & Left Board Flagged as Used",
                                Point(100, 100), FONT_HERSHEY_PLAIN, 6.0,
                                color, 4.0);

                    }
                }
            } else
            {
                if (movementLines.at(lineindex).isDropData)
                {
                    putText(drawing, "Dropped Data",
                            Point(100, 100), FONT_HERSHEY_PLAIN, 6.0,
                            color, 4.0);

                } else if (movementLines.at(lineindex).right)
                {
                    putText(drawing, "Moving Right",
                            Point(100, 100), FONT_HERSHEY_PLAIN, 6.0,
                            color, 4.0);

                } else if (!movementLines.at(lineindex).right)
                {
                    putText(drawing, "Moving Left",
                            Point(100, 100), FONT_HERSHEY_PLAIN, 6.0,
                            color, 4.0);

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
    //close file
    outputVideo.release();
    outputVideoAll.release();
    outputBoardSegment.release();
    fileReader.getInputVideo().release();

}