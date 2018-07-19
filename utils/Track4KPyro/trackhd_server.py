#!/usr/bin/env python3
# Track4k Script to execute Track4K and cropvid from a
# remote machine. To run in a standalone server (With Track4K and Cropvid installed).
# Author: Maximiliano Lira Del Canto | RRZK University of Cologne, Germany



import Pyro4
import subprocess
import os

@Pyro4.expose
@Pyro4.behavior(instance_mode = 'single')
class trackhd:

    def cropvid(self, input_file, output_file, track_file):
        cmd = ['/usr/local/bin/cropvid', input_file, output_file, track_file]
        while True:
            app = subprocess.run(cmd, universal_newlines=True, stdout=subprocess.PIPE)
            if app.returncode == 0:
                break
        return [app.returncode, app.stdout]

    def track4k(self, input_file, output_file, width, height, mode):
        if mode == 'txt':
            output_track = output_file + '.txt'
        else:
            output_track = 'trackhd.json'
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



# def getNS():
#     """
#     Return a Pyro name server proxy. If there is no name server running,
#     start one on 0.0.0.0 (all interfaces), as a background process.
#
#     """
#     import Pyro4
#     try:
#         return Pyro4.locateNS()
#     except Pyro4.errors.NamingError:
#         print("Pyro name server not found; starting a new one")
#     os.system("python3 -m Pyro4.naming -n 0.0.0.0 -p 15236 &")
#     # TODO: spawn a proper daemon ala http://code.activestate.com/recipes/278731/ ?
#     # like this, if there's an error somewhere, we'll never know... (and the loop
#     # below will block). And it probably doesn't work on windows, either.
#     while True:
#         try:
#             return Pyro4.locateNS()
#         except:
#             pass

def main():

#    getNS()

    Pyro4.Daemon.serveSimple(
        {
            trackhd: "trackhd.prototype"
        },
    host = '0.0.0.0',
    port = 15236,
    ns = False)

if __name__=="__main__":
    main()


#app = trackhd()
#app.track4k(input_file='/mnt/opencast/4k_sample/presenter.mkv',output_file='/mnt/opencast/4k_sample/tracked.mkv',width='1920', height='1080', mode='json')
