# Deprecated Packages

In this folder are utilities that where deprecated because there is no improvement for the project or there is a new better approach to achieve the same task. 

## ssh_track4k

The idea behind ssh_track4k is because Track4 and cropvid were built to work in Ubuntu systems and it was need to run this programs over other Linux machines like CentOS or Debian based systems.

ssh_track4k makes possible to work with any other machine without installing dependencies not supported officially for the distribution or making custom builds that might be very troublesome.

One of the uses of ssh_track4k is to be part of an Opencast workflow, the next image shows how works:

![ssh_track4K example](/utils/assets/track4k-opencast-example.png)

### Installation

Simply you need to have this dependencies:

* Python 3 or newer
* Argparse and Paramiko libraries (Can be installed using PiP)

**Important:** Before the first execution, you need to:

* Put the *IP*, the *username* and the *password* of the machine that has track4K and cropvid installed inside the script.
* If you use with an NFS share, you need to use the same username, group, uid and gid of the same user that will work with the processed assets.




### Usage
The options to use ssh_track4k are:

```
usage: ssh_track4k.py [-h]
                      input_file output_file width_out height_out {txt,json}

Executes track4K and cropvid in a remote machine

positional arguments:
  input_file   Input filename
  output_file  Name of the output file
  width_out    Output width of the video
  height_out   Output height of the video
  {txt,json}   Mode of the tracking, txt mode: Track + Video Crop. json mode:
               Only Track in JSON format for use in applications that can use
               that info

optional arguments:
  -h, --help   show this help message and exit
```
