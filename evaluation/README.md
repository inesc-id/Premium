# Evaluation README

This README aims to provide some tips and instructions on how to test the system.
In this folder you can find some scripts that run applications that use MACHETE mechanism.

For evaluation purposes we run this in a real network, so we are counting on external/public IPs. 
Therefore all this scripts have the use-stun-server flag active (it is given as an argument with value 1).

To compile the code for MPD and ONs you can sun the script `clean-pull-make.sh`.

## Running MPM Adapter

This script is needed just for the Sender and the Receiver, because the overlay nodes script already runs the MPM Adapter.
To run the adapter separately you can run the following script, where `<mpm-adapter-id>` is a number higher than 1000. 
Keep in mind that the nodes of the system, both MPD and ONs cannot have an adapter running with the same `<mpm-adapter-id>`:
```
sh run-adapter.sh <mpm-adapter-id>
```

## Running Overlay Nodes
This script runs the Overlay Node code. The only argument, `<node-identifier>`, is a number higher than 0. 
```
sh run-node.sh <node-identifier>
```

## Running Multipath Devices

To run this with the reactive feature, run for the sender this:

Sender:
```
sh run-hi-client.sh <receiver-ip>
```
Receiver:
```
sh run-hi-server.sh
```
### Several iterations run

To test the setup and teardown time, by running multiple times and get around 30 experiments, run these scripts:

Sender:
```
sh run-eval-hi-client.sh <receiver-ip>
```
Receiver:
```
sh run-eval-hi-server.sh
```
For these scripts to work, MACHETE is using an option `DO_NOTHING_AND_IGNORE_ALERTS` to ignore the alerts.
This option can be changed in this file [SCMultipath/machete_sender.c](../SCMultipath/machete_sender.c)
You can find all of the options in [SCMultipath/premium_specs.h](../SCMultipath/premium_specs.h).
Also in the `SCMultipath/path_monitoring.c` module change within the `stop_path_monitoring_module()`, use `phtheard_cancel` instead of `phread_kill` function.
