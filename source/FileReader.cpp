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
// Created by Charles Fitzhenry on 2016/07/14.
//

#include "FileReader.h"
#include "opencv2/opencv.hpp"

#include <limits>
#include <iomanip>

using namespace std;
using namespace cv;

bool FileReader::readFile(std::string filename, PersistentData &pD)
{
    //read in video file
    inputVideo = VideoCapture(filename);

    if (!inputVideo.isOpened())
    {
        cout << "Could not open the input video: " << filename << endl;
        return false;
    }

    cout << "Reading video file: " << filename << endl;

    fps = inputVideo.get(CV_CAP_PROP_FPS); // Frame Rate
    numFrames = inputVideo.get(CV_CAP_PROP_FRAME_COUNT); // Number of frames (may not be accurate)

    videoDuration = numFrames / fps; //Duration of video file in seconds

    ex = static_cast<int>(inputVideo.get(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form

    // Transform from int to char via Bitwise operators
    char EXT[] = {(char) (ex & 0XFF), (char) ((ex & 0XFF00) >> 8), (char) ((ex & 0XFF0000) >> 16),
                  (char) ((ex & 0XFF000000) >> 24), 0};

    videoDimension = Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
                          (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

    // Print out progress info
    cout << "Input frame resolution: Width=" << videoDimension.width << "  Height=" << videoDimension.height
         << " Frames=" << numFrames << endl;
    // cout << "Input codec type: " << EXT << endl;
    cout << "Calculated Video Duration (frames / fps): " << std::fixed << std::setprecision(3) << videoDuration << " seconds" << endl;
    cout << "FPS: " <<  std::fixed << std::setprecision(6) << fps << endl;

    //Set video file info
    pD.setVideoInfo(fps, videoDimension, ex);

    return true;
}

//This method returns the next section (where @segSize is in seconds)
void FileReader::getNextSegment(int segSize, std::vector<MetaFrame> &frameVec)
{
    //Clear the vector
    frameVec.clear();

    //Calculate how many frames to read that will amount to segSize
    int numFrames = fps * segSize; //This is the number of frames in segSize seconds

    cout << "Section is " << segSize << " seconds(s) long, which amounts to " << numFrames << " frames." << endl;
    for (int i = 0; i < numFrames; i++)
    {

        //read the current frame
        if (!inputVideo.read(frame))
        {
            cerr << "End of video file in getNextSegment()" << endl;
            endOfFile = true;
            break; //If end of video file
        }

        MetaFrame tmp = MetaFrame(frame.clone());

        //add frame to vector
        frameVec.push_back(tmp);

    }
}

void FileReader::getNextSegment(int segSize, std::vector<cv::Mat> &frameVec)
{
    //Clear the vector
    frameVec.clear();

    //cout << "Section is " << segSize << " frames." << endl;
    for (int i = 0; i < segSize; i++)
    {
        //read the current frame
        if (!inputVideo.read(frame))
        {
            //cerr << "End of reading video file" << endl;
            endOfFile = true;
            break; //If end of video file
        }
        //add frame to vector
        frameVec.push_back(move(frame));
    }
}

void FileReader::getNextFrame(cv::Mat &frame)
{
        //read the current frame
        if (!inputVideo.read(frame))
        {
            cerr << "End of video file in getNextFrame()" << endl;
            endOfFile = true;
        } else {
            // Appears to return a calculated position from frame and framerate rather than actual position
            // long stamp = inputVideo.get( CV_CAP_PROP_POS_MSEC );
            // std::cout << "Timestamp: " << stamp << std::endl;
        }
}


bool FileReader::isEndOfFile()
{
    return endOfFile;
}

