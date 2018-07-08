# README for installing dependencies and building code 

This folder has installing scripts to set up environment for all the involved projects:
- MACHETE
- Darshana
- DepSpacito

These scripts must run in this folder.

-----

This script builds all componets, runs `build.sh` on DepSpacito, Darshana and MACHETE (MPD, MPM Adapter and ONs):
```
build_everything.sh
```
-----

Cloning DepSpacito and Darshana in parent folder, at the same level as MACHETE:
```
sh clone_missing_projects.sh  
```
-----

This script installs Darshana dependencies. These includes maven, paris-traceroute, java v8, python, pycrypto and whois. 
```
sh install_darshana_dependencies.sh  
```

----

This script installs linux kernel with MPTCP installed. 
This script may not work at first depending on the OS image. 
However for this to work, it will need a reboot.
```
sh install_machete_dependencies.sh
```
