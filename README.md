# TRACK4K
Track4K is an open source C++ project that takes a High Definition video of a lecture recording and then produces a smaller cropped output video, which frames the lecturer. This is done using image processing and computer vision algorithms to track the lecturer and uses the lecturer position information to pan the virtual camera.

## Getting Started
These instructions will help get the program and all its dependencies set up on your machine.

### Prerequisites
These instructions are written with the assumption that the project will be installed on a Linux-based system (preferably a Debian version). Track4K has been tested on Ubuntu 16.04

 To be able to run this project, you will need to first install the following dependencies:

* ffmpeg (3.4 or newer)
* OpenCV 3 (3.2.0 or future releases)
* OpenCV Extra Modules (latest version on repository)
* C++ Libraries (6.3 or future releases)
* CMake (3.8.0 or future releases)
* git (2.10.2 or future releases)

### Installation

#### FFmpeg
The standard repositories of your distribution may include FFmpeg 3.4+. If not, FFmpeg 3.4 can be built from source (more on that topic [here](https://github.com/FFmpeg/FFmpeg/blob/master/INSTALL.md)).

For Ubuntu-based distributions, the PPA `ppa:jonathonf/ffmpeg-3` allows for simpler installation without the needing to build from source. The PPA can be added as follows:

```
$ sudo add-apt-repository ppa:jonathonf/ffmpeg-3
[Press enter when prompted]
$ sudo apt-get update
```

#### Downloading and Installing base dependencies
The first on the install list (and most important) is CMake, followed by git, C++ and various multimedia packages.
The following terminal command will get and install the necessary requirements

```
$ sudo apt-get install cmake git build-essential libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev libavfilter-dev libx264-dev libx265-dev libvpx-dev liblzma-dev libbz2-dev libva-dev libvdpau-dev
```

#### Downloading and Installing the OpenCV libraries
The next step is to download and install the OpenCV libraries.
The necessary OpenCV library comes in two components. First download the core OpenCV library. Choose any directory as your download destination directory.
Clone OpenCV from Git as follows:
```
$ cd `your_chosen_working_directory`
$ git clone https://github.com/opencv/opencv

```
Next, repeat the process for the Extra modules. Remain in the same working directory and execute the following terminal command:
```
$ git clone https://github.com/opencv/opencv_contrib
```
You should now have two folders in your working directory.
The next step is to build OpenCV.

#### Building the OpenCV library
Your Chosen directory now contains two folders, opencv and opencv_contrib. The opencv folder contains the main OpenCV libraries and opencv_contib contains the extra modules.

```
$ cd `your_chosen_working_directory`
```
Inside the main OpenCV folder, change directory into the build folder (create one if it does not exist) and remove all files, since it will require rebuilding. To rebuild OpenCV run the following command from within the build folder:

```
$ cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ..
```
This step will generate a MakeFile. Once complete perform the following command to run make faster (the number after the j-flag is the number of processors the job will use). If you are not sure how many processors the machine has use the following instruction to find out:

```
cat/proc/cpuinfo | grep processor | wc -l
```
Use the result from this in the j-flag

```
$ make -j`processor_count`
```
Remain in the build folder and run the following cmake command to make the extra modules.
The path decribed below is an example. Fill in the directory path on your machine which points to the OpenCV Extra modules folder.

```
cmake -DOPENCV_EXTRA_MODULES_PATH=`OpenCV_Extra_Modules_Folder_Path`/modules ../
```
Next step is to make these files:

```
$ make -j8
```

Finally, install these modules by running the following command:
```
$ sudo make install
```
#### Building Track4K

##### Automatic Method

There is a shell script in the trackhd folder called intall_track4k.sh which can be used to install track4k automatically.
To use this script run the following command:

```
sudo ./install_track4k.sh
```

This will run all the steps listed in the manual method mentioned below.

##### Manual Method

This method is for the case where the automatic method does not work. It does everything the shell script does manually.

The trackhd directory should have 2 main folders inside it: source and build. The source folder comntains all the header and source files while the build file contains all object files and executables.
The first step is to navigate into the build folder. Once inside run delete all files (if any) and then type the following command in terminal:

```
cmake ../source
```
Now it is possible to run the build instruction:

```
make -j`number_of_processors`
```
You can now install the project to /usr/local/bin/ by running the following command:

```
sudo make install
```

Then build cropvid:

```
cd cropvid
./build.sh
cp cropvid /usr/local/bin/
```

#### Running Track4K

Track4K runs in two parts: track4k analyzes a video file and produces a cropping data file in text format. cropvid crops the
video file according to the cropping information in the data file, using ffmpeg libraries.

```
$ track4k <inputFileName> <outputFileName> <output-width> <output-height>
$ cropvid <input file> <output file> <cropping file>
```

Example:

```
track4k presenter.mkv presenter-crop.txt 1920 1080
cropvid presenter.mkv tracked.mkv presenter-crop.txt
```

Track4K can also output the cropping information in JSON format, when the output filename has a `.json` extension:

Example:

```
track4k presenter.mkv presenter-crop.json 1920 1080
```

The JSON format includes a timestamp as well as a frame number. The timestamp is only guaranteed to be accurate when
the source video has been recorded with a fixed frame rate. It may be incorrect for variable frame rate source videos.

### Memory Requirements
The program reads a maximum of 29 frames into memory at a time. So a minimum of 4GB RAM should be sufficient.

## Built With

* [OpenCV](http://www.opencv.org) - The computer vision library of choice

## License

  Copyright 2016 Charles Fitzhenry / Mohamed Tanweer Khatieb / Maximilian Hahn
  Licensed under the Educational Community License, Version 2.0
  (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

  http://www.osedu.org/licenses/ECL-2.0

  Unless required by applicable law or agreed to in writing,
  software distributed under the License is distributed on an "AS IS"
  BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
  or implied. See the License for the specific language governing
  permissions and limitations under the License.
