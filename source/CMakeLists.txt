cmake_minimum_required(VERSION 3.5)
project(track4k)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
#set(CMAKE_CXX_STANDARD 11)

set(OpenCV_DIR "C:\\opencv\\build\\install")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})


set(SOURCE_FILES FileReader.cpp FileReader.h MetaFrame.cpp MetaFrame.h "segmentation/Track4KPreProcess.cpp" "segmentation/IlluminationCorrection.cpp" "segmentation/IlluminationCorrection.h" "segmentation/MotionDetection.cpp" "segmentation/MotionDetection.h" panning/VirtualCinematographer.cpp panning/VirtualCinematographer.h "segmentation/Track4KPreProcess.h" "segmentation/BoardDetection.cpp" "segmentation/BoardDetection.h" panning/PresenterMotion.cpp panning/PresenterMotion.h panning/PanLogic.cpp panning/PanLogic.h mainDriver.cpp PersistentData.h PersistentData.cpp tracking/RecognitionDriver.h tracking/RecognitionDriver.cpp
        tracking/MovementDetection.h tracking/MovementDetection.cpp tracking/Ghost.cpp tracking/Ghost.h)
add_executable(track4k ${SOURCE_FILES})


set(OpenCV_LIBS opencv_calib3d opencv_core opencv_features2d opencv_flann opencv_highgui opencv_imgcodecs opencv_imgproc opencv_ml opencv_objdetect opencv_photo opencv_shape opencv_stitching opencv_superres opencv_video opencv_videoio opencv_videostab opencv_xfeatures2d)

target_link_libraries(track4k ${OpenCV_LIBS})


install(TARGETS track4k DESTINATION bin)
