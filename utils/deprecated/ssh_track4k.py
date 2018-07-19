#!/usr/bin/env python3
# Track4k Script to execute Track4K and cropvid from a
# remote machine.
# Author: Maximiliano Lira Del Canto | RRZK University of Cologne, Germany

import argparse
import paramiko
import os
import sys


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

ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())


# Connection to the remote machine
ssh.connect({{IP_Address}}, port=22, username = {{Machine_Username}}, password ={{Machine_Password}})


# Track4K in cropping mode
if args.track_mode == 'txt':
    cmd = '/usr/local/bin/track4k ' + args.input_file + ' ' + args.output_file + '.txt' + ' ' + args.width_out + ' ' + args.height_out
    print (cmd)
    stdin, stdout, stderr = ssh.exec_command(cmd)
    for line in stdout:
        print('... ' + line.strip('\n'))
    for line in stderr:
        print('... ' + line.strip('\n'))

    cmd = '/usr/local/bin/cropvid ' + args.input_file + ' ' + args.output_file + ' ' + args.output_file + '.txt'
    stdin, stdout, stderr = ssh.exec_command(cmd)
    for line in stdout:
        print('... ' + line.strip('\n'))
    for line in stderr:
        print('... ' + line.strip('\n'))


# Track4K in virtual cropping mode (Creates a JSON file with the tracking position)
if args.track_mode == 'json':
    cmd = '/usr/local/bin/track4k ' + args.input_file + ' ' + args.output_file + '.json' + ' ' + args.width_out + ' ' + args.height_out
    stdin, stdout, stderr = ssh.exec_command(cmd)
    for line in stdout:
        print('... ' + line.strip('\n'))
    for line in stderr:
        print('... ' + line.strip('\n'))


# Close the SSH pipe after finishing
ssh.close()
print('Cropped video ready')
exit()
