//
// Created by Charles on 2016/10/25.
//

#ifndef TRACK4K_POINTPLOTTER_H
#define TRACK4K_POINTPLOTTER_H
#include "Panning/headers/VideoOutput.h"
#include "Panning/headers/FrameCropper.h"
#include "opencv2/opencv.hpp"
#include "Panning/headers/PresenterMotion.h"
#include "Panning/headers/PanLogic.h"
#include <fstream>
#include "FileReader.h"

class PointPlotter
{
public:
    void plotPoints(PersistentData &pD);
};


#endif //TRACK4K_POINTPLOTTER_H
