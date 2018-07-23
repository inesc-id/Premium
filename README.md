# PREMIUM

**PREMIUM** stands for **P**rivate **RE**active **M**ult**I**path comm**U**nication **M**iddleware

PREMIUM provides a mechanism to split network traffic among multiple paths, and is able to react in near real-time to hijacking attacks. 
The solution uses two components: MACHETE and Darshana. 
The first is a multipath communication component that splits data, with Multipath TCP (MPTCP), among multiple physical paths on top of an overlay network, using when possible multiple Internet Service Providers (ISPs) through multihoming.
The second is a route hijacking monitor, that uses a combination of detection mechanisms to alert the user that its data traffic is likely being intercepted. The end client uses this reactive middleware so that hijack alerts can trigger path changes, to protect the communication.

For more information about PREMIUM and its components please read and cite ;) the following publications ([BibTeX file](doc/PREMIUM.bib)):

Diogo Raposo, Miguel L. Pardal, Luis Rodrigues, Miguel Correia
_MACHETE: Multi-path Communication for Cloud Security_
IEEE International Symposium on Network Computing and Applications (NCA), 2016.

Karan H. Balu, Miguel L. Pardal, Miguel Correia
_DARSHANA: Detecting Route Hijacking For Communication Confidentiality_
IEEE International Symposium on Network Computing and Applications (NCA), 2016.

***

More about PREMIUM and the SafeCloud project at http://www.safecloud-project.eu/

## Getting Started
For this guide we will need 5 virtual machines to simulate 5 distributed components of MACHETE: a sender, a receiver, a multipath manager to store information about the network and two overlay nodes to forward traffic through different routes. The virtual machines will be executed in Virtual Box and they all use the following virtual disk.


### Prerequisites

 * [VirtualBox](https://www.virtualbox.org)

### Instalation and set-up

The following instructions should be followed using the virtual machines previously created. A guide that describes how to set the virtual machines can be found [here](doc/VIRTUALBOX-GUIDE.md).

This virtual image has MACHETE’s dependencies but still misses Darshana dependencies. So we have to install them by running:

```
$ cd MACHETE/install 
$ sh install_darshana_dependencies.sh

```
Now, we need to have DepSpacito, MACHETE and Darshana repositories together in your working directory:

```
_/home
 |-- DepSpacito
 |-- MACHETE
 |-- Darshana 
```


MPM - Multipath Manager
Start the virtual machine MACHETE-MPM. 
The MultiPath Manager is a coordination service called DepSpace. To start DepSpace open a terminal and go to the DepSpacito folder:

```
$ cd DepSpacito
$ su
$ git pull
$ sh build.sh
$ sh run.sh
```

The build.sh will output the IP address of the machine. Take note of this IP address since it will be required by every component of MACHETE.
Overlay Nodes
Now let us start two overlay nodes. To do so repeat the following steps twice, first for MACHETE-ON-1 and second for MACHETE-ON-2.

```
$ su
$ cd MACHETE
$ git pull
$ cd SCNodeCode 
$ sh build.sh <MPM_IP_ADDRESS>
```

Then for MACHETE-ON-1 execute:
```
$ sh run1.sh
```

And for MACHETE-ON-2 execute:
```
$ sh run2.sh
```

Receiver
Start the receiver Virtual Machine. Open the terminal and execute the following commands:

```
$ su
$ cd MACHETE
$ git pull
$ cd SCMultipath
$ sh build.sh <MPM_IP_ADDRESS>
$ sh run-receiver.sh
```

In a different terminal window execute:
```
$ hostname -I
```

This will output the IP address of the receiver which will be required by the sender. Take note of the receiver address.
Sender
Start the sender Virtual Machine. Open the terminal and execute the following commands:

```
$ su
$ cd MACHETE
$ git pull
$ cd SCMultipath
$ sh build.sh <MPM_IP_ADDRESS>
$ sh run-sender.sh <RECEIVER_IP_ADDRESS> <FILE_PATH_TO_SEND>
```

The file size should be greater than 1MB, since below this value it is difficult for MACHETE to successfully break the file into parts. A good file to send is an image file, this way we can see the image in the receiver VM.
If everything was correctly configured, the file should be transmitted from the sender to the receiver, with traffic going through two different overlay nodes. It is possible to use a traffic analysis tool, such as bmon or tcpdump, to visualize data being sent from sender to receiver.
To visualize the traffic execute one of the following commands:

```
$ tcpdump -i eth# -Q out 
```
where # is the number of the network interface

or

```
$ bmon
```




## Built With

* [MP-TCP](https://www.multipath-tcp.org) - The Linux Kernel MultiPath TCP project
* [Maven](https://maven.apache.org/) - Dependency Management
* [DepSpace](https://github.com/bft-smart/depspace) - The fault and intrusion-tolerant secure tuple space 
* [MACHETE](https://github.com/inesc-id/MACHETE) - Multi-Path Communication implementation
* [Darshana](https://github.com/inesc-id/darshana) - Redundant Route Hijacking Detection System

## Authors


* **Isabel Costa** - *Development of the PREMIUM middleware*
* **Diogo Raposo** - *Development of the [MACHETE](https://github.com/inesc-id/MACHETE) middleware*
* **Karan Balu** - *Development of the [Darshana](https://github.com/inesc-id/darshana) middleware*
* **David Matos** - *Development of the PREMIUM middleware*

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* **[Isabel Costa](https://github.com/isabelcosta)**
* **[Prof. Miguel Correia](https://github.com/mpcorreia)** 
* **[Prof. Miguel Pardal](https://github.com/miguelpardal)** 
* **[David Matos](https://github.com/davidmatos)** 
* **[Diogo Raposo](https://github.com/diogoraposo)** 
* **[Karan Balu](https://github.com/karan-balu)** 
