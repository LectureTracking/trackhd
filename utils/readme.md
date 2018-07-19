# Track4K Utilities

In this folder you can find different utilities to work with Track4K, the utilities availible at the moment are:

* **Track4KPyro** : Python3 script built with Pyro4 to execute remotely Track4K by using remote objects in a Client/Server Scheme.
* **opencast_workflows** : Samples of workflows to work with ssh_track4k.py
* **ssh_track4k.py** (Deprecated): Python3 script built with Paramiko to execute remotely Track4K

## Track4KPyro

The idea behind Track4KPyro is because Track4 and cropvid were built to work in Ubuntu systems and it was need to run this programs over other Linux machines like CentOS or Debian based systems.

Track4KPyro makes possible to work with any other machine without installing dependencies not supported officially for the distribution or making custom builds that might be very troublesome.

One of the uses of Track4KPyro is to be part of an Opencast workflow.

### Installation

Simply you need to install this dependencies in each machine, for opencast users: this includes all the workers and the admin:

* Python 3 or newer
* Argparse and Pyro4 libraries (Can be installed using PiP for Python 3)

**Important:** Before the first execution, you need to check:

* If you use with an NFS share, you need to use the same username, group, uid and gid of the same user that will work with the processed assets.

#### In the machine with track4K and cropvid installed:

* Copy and execute `trackhd_server-py`, you should see this in the terminal:

```
Object <class '__main__.trackhd'>:

    uri = PYRO:trackhd.prototype@0.0.0.0:15236
Pyro daemon running.
```
\* The port and the IP from what accept the clients can be changed in the code.

* Allow  inbound TCP connection throught the port


#### In the client machine:

* Set the *IP*, and *Port* of the machine that has `trackhd_server.py`running inside the `trackhd_client.py` script.
* for Opencast users:
  - Allow the client script to be executed by adding in the Execute bundle configuration file `org.opencastproject.execute.impl.ExecuteServiceImpl.cfg` *In each worker and admin node*.
  - Install the dependencies and the client script *In each worker and admin node*.
  - Install or configure the scripts to use the `Execute-Once` or `Execute-many` WOH

### Usage
The options to use in `trackhd_client.py` are:

```
usage: trackhd_client.py [-h]
                         input_file output_file width_out height_out
                         {txt,json}

Executes track4K and cropvid in a remote machine

positional arguments:
  input_file   Input filename
  output_file  Name of the output file
  width_out    Output width of the video
  height_out   Output height of the video
  {txt,json}   Mode of the tracking, txt mode: Track + Video Crop. json mode:
               Only Track in JSON format for use in applications that can use that info

optional arguments:
  -h, --help   show this help message and exit
```

### Future Work:

* Integrate *trackhd_server.py* as a OS service.
* Exception handling in case of problems.



## Opencast Workflows

This workflows for opencast are a samples they how will work with **Track4KPyro**, you only have to remember to install and allow this script in each admin and worker. node of opencast.
