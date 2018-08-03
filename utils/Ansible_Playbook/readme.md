# Ansible TrackHD Playbook

This folder has the files you need to install TrackHD server and the comunication with Opencast. To use it first you need to have installed Ansible in the computer that will execute the orders to the remote hosts.


* In the `groups_vars` folder, modify the files according your existing setup.
* Add the URLs or IPs addresses for the clients and the server.


Enter to the folder and run the command in the terminal:

```
ansible-playbook -vv -i hosts trackhd_srv.yml -u <Remote_User> --ask-sudo-pass

ansible-playbook -vv -i hosts trackhd_clnt.yml -u <Remote_User> --ask-sudo-pass
```


Ansible will install TrackHD without hassle.
