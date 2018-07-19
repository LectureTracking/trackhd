#!/usr/bin/env python3
# Track4k Script to execute Track4K and cropvid from a
# remote machine.
# Author: Maximiliano Lira Del Canto | RRZK University of Cologne, Germany

import argparse
import paramiko
import os
import sys
import re

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

class ShellHandler:

    def __init__(self, host, user, psw):
        self.ssh = paramiko.SSHClient()
        self.ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        self.ssh.connect(host, username=user, password=psw, port=22)

        channel = self.ssh.invoke_shell()
        self.stdin = channel.makefile('wb')
        self.stdout = channel.makefile('r')

    def __del__(self):
        self.ssh.close()

    def execute(self, cmd):
        """

        :param cmd: the command to be executed on the remote computer
        :examples:  execute('ls')
                    execute('finger')
                    execute('cd folder_name')
        """
        cmd = cmd.strip('\n')
        self.stdin.write(cmd + '\n')
        finish = 'end of stdOUT buffer. finished with exit status'
        echo_cmd = 'echo {} $?'.format(finish)
        self.stdin.write(echo_cmd + '\n')
        shin = self.stdin
        self.stdin.flush()

        shout = []
        sherr = []
        exit_status = 0
        for line in self.stdout:
            if str(line).startswith(cmd) or str(line).startswith(echo_cmd):
                # up for now filled with shell junk from stdin
                shout = []
            elif str(line).startswith(finish):
                # our finish command ends with the exit status
                exit_status = int(str(line).rsplit(maxsplit=1)[1])
                if exit_status:
                    # stderr is combined with stdout.
                    # thus, swap sherr with shout in a case of failure.
                    sherr = shout
                    shout = []
                break
            else:
                # get rid of 'coloring and formatting' special characters
                shout.append(re.compile(r'(\x9B|\x1B\[)[0-?]*[ -/]*[@-~]').sub('', line).
                             replace('\b', '').replace('\r', ''))

        # first and last lines of shout/sherr contain a prompt
        if shout and echo_cmd in shout[-1]:
            shout.pop()
        if shout and cmd in shout[0]:
            shout.pop(0)
        if sherr and echo_cmd in sherr[-1]:
            sherr.pop()
        if sherr and cmd in sherr[0]:
            sherr.pop(0)

        return shin, shout, sherr


# Set the IP, Username and Password  
shell = ShellHandler(<IP>, <User>, <Passwd>)

if args.track_mode == 'txt':
    cmd = '/usr/local/bin/track4k ' + args.input_file + ' ' + args.output_file + '.txt' + ' ' + args.width_out + ' ' + args.height_out
    track = shell.execute(cmd)
    print(type(track))

    cmd = '/usr/local/bin/cropvid ' + args.input_file + ' ' + args.output_file + ' ' + args.output_file + '.txt'
    track = shell.execute(cmd)


# Track4K in virtual cropping mode (Creates a JSON file with the tracking position)
if args.track_mode == 'json':
    cmd = '/usr/local/bin/track4k ' + args.input_file + ' ' + args.output_file + '.json' + ' ' + args.width_out + ' ' + args.height_out
    track = shell.execute(cmd)
    print(type(track))
# Close the SSH pipe after finishing
print('Cropped video ready')
exit()
