#!/usr/bin/env python3
# Track4k Script to execute Track4K and cropvid from a
# remote machine. To run in a standalone server (With Track4K and Cropvid installed).
# Author: Maximiliano Lira Del Canto | RRZK University of Cologne, Germany



import Pyro4
import subprocess
import os

#Please Change this value before the first run
# trackhd_port is an int.
trackhd_port = 'CHANGE_ME'



# Expose the trackhd class throught Pyro4 interface
@Pyro4.expose
@Pyro4.behavior(instance_mode = 'single')
class trackhd:
    # Method to crop files
    def cropvid(self, input_file, output_file, track_file):
        cmd = ['/usr/local/bin/cropvid', input_file, output_file, track_file]
        print('Cropping file, please wait...')
        print('Input details:')
        print('Input Filename: ' + input_file)
        print('Output Filename: ' + output_file)
        print('Track File: ' + track_file)
        while True:
            app = subprocess.run(cmd, universal_newlines=True, stdout=subprocess.PIPE)
            if app.returncode == 0:
                break
        return [app.returncode, app.stdout]

    # Method to create the track file for crop or for auto zoom in paella player
    def track4k(self, input_file, output_file, width, height, mode):
        print('Tracking File, please wait...')
        print('Input details:')
        print('Input Filename: ' + input_file)
        print('Output Filename: ' + output_file)
        print('Desired tracking resolution: ' + width + 'x' + height)
        print('Track output mode: ' + mode)
        if mode == 'txt':
            output_track = output_file + '.txt'
        else:
            output_track = output_file
        cmd = ['/usr/local/bin/track4k', input_file, output_track, width, height]
        while True:
            print('Processing video')
            app = subprocess.run(cmd, universal_newlines=True, stdout=subprocess.PIPE)
            print(app.stdout)
            if app.returncode == 0:
                break
        if mode == 'txt':
            print('Executing Cropvid')
            self.cropvid(input_file, output_file, output_track)
            print('Crop succesfull, output file: ' + output_file)
        return [app.returncode, app.stdout]



def main():
    # Start of Pyro4 Server
    Pyro4.Daemon.serveSimple(
        {
            trackhd: "trackhd.prototype"
        },

    # Allow connection from any IP of the server
    host = '0.0.0.0',
    port = trackhd_port,
    ns = False)

if __name__=="__main__":
    main()


# Test lines, uncomment to Test the class without Pyro 4
#app = trackhd()
#app.track4k(input_file='/mnt/opencast/4k_sample/presenter.mkv',output_file='/mnt/opencast/4k_sample/tracked.mkv',width='1920', height='1080', mode='json')
