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
### Demo with out running Darshanas:

Run the following script to run the demonstration of DAR API without using sockets and running Darshanas:
```
sh compile.sh
```

Run the following script to run and see the DAR API working, according to mocked alerts given as arguments:
```
sh script.sh
```