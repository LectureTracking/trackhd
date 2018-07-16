#!/usr/bin/env python3
# Track4k Script to execute Track4K and cropvid from a
# remote machine.
# Author: Maximiliano Lira Del Canto | RRZK University of Cologne, Germany



#import Pyro4
import subprocess
import shlex

#@Pyro4.expose
#@Pyro4.behavior(instance_mode = 'single')


class trackhd:

    def track4k(self, input_file, output_file, width, height, mode):
        if mode == 'txt':
            output_file = output_file + '.txt'
        else:
            output_file = output_file + '.json'
        cmd = ['/usr/local/bin/track4k', input_file, output_file, width, height]
        while True:
            print('En proceso JSON')
            app = subprocess.run(cmd, universal_newlines=True, stdout=subprocess.PIPE)
            print(app.stdout)
            if app.returncode == 0:
                break
            return [app.returncode, app.stdout]

    def cropvid(self, input_file, output_file, track_file):
        cmd = ['/usr/local/bin/cropvid', input_file, output_file, track_file]
        while True:
            app = subprocess.run(cmd, stdout=subprocess.PIPE )
            if app.returncode == 0:
                break
        return [app.returncode, app.stdout]


app = trackhd()
app.track4k(input_file='/mnt/opencast/4k_sample/presenter.mkv',output_file='/mnt/opencast/4k_sample/tracked.mkv',width='1920', height='1080', mode='json')
