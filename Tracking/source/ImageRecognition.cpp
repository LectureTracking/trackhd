//
// Created by Fudge on 2016/08/09.
//

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "../headers/ImageRecognition.h"

#include <iostream>
#include <stdio.h>


using namespace std;
using namespace cv;

bool ImageRecognition::loadImage(string imageName){

    //load image in colour and display
    Mat image;
    image = imread(imageName, CV_LOAD_IMAGE_GRAYSCALE);

    //if image failed to load
    if(image.empty()){
        std::cout << "\nImage load " << imageName << " failed!" << std::endl;
        return false;
    }

    //namedWindow("window1", 1);
    //imshow("window1", image);

    //load face cascade (.xml file)
    CascadeClassifier face_cascade;
    face_cascade.load("C:\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_default.xml");

    //Detect face
    vector<Rect> faces;
    //
    face_cascade.detectMultiScale(image,
                                  faces, //vector of rectangles on detected objects
                                  1.1, //how much image size is reduced at each scale
                                  5, //how many min neighbours to have to retain
                                  0 | CV_HAAR_SCALE_IMAGE, //search type flag
                                  Size(
                                          20, //min scale of object
                                          25 //max scale of object (max = min = unbounded)
                                  ));

    //Draw circles on the detected faces
    for(int i = 0; i < faces.size(); i++){
        Point center(faces[i].x + faces[i].width * 0.5, faces[i].y + faces[i].height * 0.5);
        ellipse(image, center, Size(faces[i].width * 0.5, faces[i].height * 0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
    }

    imshow("Detected Face", image);

    waitKey(0);

    return true;
}