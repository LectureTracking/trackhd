#!/usr/bin/env python3
# Track4k Script to execute Track4K and cropvid from a
# remote machine. For Opencast execution
# Author: Maximiliano Lira Del Canto | RRZK University of Cologne, Germany


import sys
import Pyro4
import Pyro4.util
import argparse

sys.excepthook = Pyro4.util.excepthook

parser = argparse.ArgumentParser(description='Executes track4K and cropvid in a remote machine')

#Argparsers arguments and description

parser.add_argument('input_file', type=str,
                    help ='Input filename')

parser.add_argument('output_file', type=str,
                    help='Name of the output file')

parser.add_argument('width_out', type=str,
                    help ='Output width of the video')

parser.add_argument('height_out', type=str,
                    help ='Output height of the video')

parser.add_argument('track_mode', type=str, choices=['txt', 'json'],
                    help='Mode of the tracking, txt mode: Track + Video Crop. json mode: Only Track in JSON format for use in applications that can use that info')


args = parser.parse_args()


# Configure IP and port of the TrackHD serverself.
uri = 'PYRO:trackhd.prototype@<IP>:<Port>'
trackhd = Pyro4.Proxy(uri)


#Run the application
app = trackhd
app.track4k(args.input_file, args.output_file, args.width_out, args.height_out, args.track_mode)
