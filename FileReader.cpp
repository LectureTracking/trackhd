//
// Created by Charles on 2016/07/14.
//

#include "FileReader.h"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

bool FileReader::readFile(std::string filename, PersistentData &pD)
{
    //read in video file
    inputVideo = VideoCapture(filename);
    if (!inputVideo.isOpened())
    {
        cout << "Could not open the input video: " << filename << endl;
        return -1;
    }

    fps = inputVideo.get(CV_CAP_PROP_FPS); //Frame Rate
    numFrames = inputVideo.get(CV_CAP_PROP_FRAME_COUNT); //Number of frames

    videoDuration = numFrames / fps; //Duration of video file in seconds

    ex = static_cast<int>(inputVideo.get(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form

    // Transform from int to char via Bitwise operators
    char EXT[] = {(char) (ex & 0XFF), (char) ((ex & 0XFF00) >> 8), (char) ((ex & 0XFF0000) >> 16),
                  (char) ((ex & 0XFF000000) >> 24), 0};

    videoDimension = Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
                          (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

    //Print out progress info

    cout << "Input frame resolution: Width=" << videoDimension.width << "  Height=" << videoDimension.height
         << " of nr#: " << numFrames << endl;
    cout << "Input codec type: " << EXT << endl;
    cout << "Video Duration (Seconds): " << videoDuration << endl;
    cout << "FPS: " << fps << endl;

    //Set video file info
    pD.setVideoInfo(fps,numFrames,videoDimension);

    return 0;

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
            cerr << "End of video file" << endl;
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

    cout << "Section is " << segSize << " frames." << endl;
    for (int i = 0; i < segSize; i++)
    {
        //read the current frame
        if (!inputVideo.read(frame))
        {
            cerr << "End of video file" << endl;
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
            cerr << "End of video file" << endl;
            endOfFile = true;
        }

}


bool FileReader::isEndOfFile()
{
    return endOfFile;
}

