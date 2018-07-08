# Darshana

Redundant Route Hijacking Detection System

This system has 3 components: client, server andd hijacker.
- **Client/Source:** Machine that monitors the connection between itself and the destination. Runs the main program.
- **Server/Destination:** Machine being monitored by the client.
- **Hijacker:** Machine that intercepts the connection. This component affects the metrics measured by the client. This component can be simulated within this project.

## Technologies

Java v8, python v2.7.9, Maven, Paris-Traceroute, WHOIS tool, PyCrypto, GeoLiteCity.dat from MaxMind.

## How to run

All the codebase is inside darshana folder.

**Note:** Before running Darshana, you need to download the GeoLiteCity.dat file that is used to calculate distance between two remote machines.
To do so run the following script: `sh getGeoLiteCity.sh`

All machines are assumed to be with public IP, to run when all machines are in different networks.

### Build

To build the project run: `sh build.sh`

### Run Server

- This component will run the cryptographic ping server side (`CryptoPingServer.py`), to respond to the client's cryptographic ping requests.

```
sh run-server_args.sh <destination-ip> <destination-port>
```
Short version that uses default values: `sh run-server.sh`

### Run Client

- The following arguments `<lat> <hop> <path> <prop> <path-nones>` are thresholds to be considered by Darshana.
- Accepted values for `<monitoring-mode>` are "lat", "hop", "path", "prop" and "full".
- Probe period `<probe-period>` is given in seconds (s).
- `<client-ip>` is the ip to be used for communication, if their values are either "p" or "publicip", then this ip will be set with the public IP, seen from external networks.

```
sh run-client_args.sh <monitoring-mode> <client-ip> <destination-ip> <destination-port> <lat> <hop> <path> <prop> <path-nones> <probe-period>
```
Short version that uses default values: `sh run-client.sh <monitoring-mode> <destination-ip>`

### Run Hijacker

- `<hijacker.ip>` has the same behavior as the `<client-ip>` argument has in the above instruction.

```
sh run-hijacker_args.sh <hijacker-ip> <client-ip>
```
Short version that uses default values: `sh run-hijacker.sh <client-ip>`

## Example

### Server (10.0.2.28): 
- Customized arguments: `sh run-server_args.sh 10.0.2.28 10008`
- Default arguments: `sh run-server.sh`

### Client (10.0.2.26):
- Customized arguments: `sh run-client_args.sh full 10.0.2.26 10.0.2.28 10008 1.5 1 0.8 1.5 0.5 2`
- Default arguments: `sh run-client.sh full 10.0.2.28`

### Hijacker (10.0.2.29):
- Customized arguments: `sh run-hijacker_args.sh 10.0.2.29 10.0.2.26`
- Default arguments: `sh run-hijacker.sh 10.0.2.26`

## DAR - Darshana Alerts Receiver module

To run Darshana's Alert Receiver, you can run the script `run-dar_args.sh <alerts-receiver-ip> <alerts-receiver-port>`
If you want to receive alerts by darshana you have to run in a remote machine the following commands:

```
cd dar
sh build.sh
sh run-dar.sh
```

To allow the client to send messages to this receiver, you have to run the `run-client-broadcaster.sh`:
```
cd darshana
sh build.sh
sh run-client-broadcaster.sh <monitoring-mode> <destination-ip> <dar-ip> <dar-port>
```
