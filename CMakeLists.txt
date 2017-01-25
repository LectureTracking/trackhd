cmake_minimum_required(VERSION 3.5)
project(Track4K)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
#set(CMAKE_CXX_STANDARD 11)

set(OpenCV_DIR "C:\\opencv\\build\\install")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(SOURCE_FILES FileReader.cpp FileReader.h MetaFrame.cpp MetaFrame.h "segmentation/Track4KPreProcess.cpp" CodeTimer.cpp CodeTimer.h "segmentation/IlluminationCorrection.cpp" "segmentation/IlluminationCorrection.h" "segmentation/MotionDetection.cpp" "segmentation/MotionDetection.h" panning/VirtualCinematographer.cpp panning/VirtualCinematographer.h panning/VideoOutput.cpp panning/VideoOutput.h panning/FrameCropper.cpp panning/FrameCropper.h "segmentation/Track4KPreProcess.h" "segmentation/BoardDetection.cpp" "segmentation/BoardDetection.h" panning/PresenterMotion.cpp panning/PresenterMotion.h panning/PanLogic.cpp panning/PanLogic.h mainDriver.cpp PersistentData.h PersistentData.cpp PointPlotter.cpp PointPlotter.h tracking/RecognitionDriver.h tracking/RecognitionDriver.cpp
        tracking/MovementDetection.h tracking/MovementDetection.cpp tracking/Ghost.cpp tracking/Ghost.h "segmentation/HelperMethods.cpp" "segmentation/HelperMethods.h")
add_executable(Track4K ${SOURCE_FILES})


set(OpenCV_LIBS opencv_calib3d opencv_core opencv_features2d opencv_flann opencv_highgui opencv_imgcodecs opencv_imgproc opencv_ml opencv_objdetect opencv_photo opencv_shape opencv_stitching opencv_superres opencv_video opencv_videoio opencv_videostab opencv_xfeatures2d)

target_link_libraries(Track4K ${OpenCV_LIBS})

