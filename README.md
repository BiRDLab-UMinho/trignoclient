# Delsys® Trigno Client Library

This library provides a basic C++ API for receiving, parsing and handling handling EMG and kinematic data acquired 
Includes an open-source implementation of the  <a href="https://www.delsys.com/downloads/USERSGUIDE/trigno/sdk.pdf">Trigno Wireless System SDK's</a> network protocol.

It was developed for use with, and extensively tested on, the Delsys Trigno Avanti system, but should technically be extensible to custom applications or other Delsys products (e.g. Trigno Quattro sensors). The code is under constant development, feel free to contribute in any way!


### Build & Install

The library can be easily built with CMake (pre-configured project files included). For user-friendly interface, you can use *ccmake* or *cmake-gui*. Advanced build options include building the associated Doxygen documentation by enabling the **BUILD_DOC** flag (default is OFF), and compiling example code as separate binaries with the **WITH_EXAMPLES** flag. 

```
cd trignoclient
mkdir build
cd build
cmake ..
make
```

Additionally, to install on the system's or project's path directory, edit the **CMAKE_INSTALL_PREFIX** macro (default is /*usr/local*) and call

```
make install
```

to install dynamic libraries and header files for development (super user priviledges may be required).


#### Note 
*Boost* libraries are required for successful compilation. Refer to documentation on these libraries and your OS distribution to properly install them.


### Usage

trignoclient's API is extensively documented (doxygen). This section only attempts to superficially describe the most common workflow when developing on top of trignoclient.

![trignoclient workflow](../structure.png "trignoclient workflow")


#### Network streaming

The Trigno Wireless System SDK's provides a configurable TCP server streaming sensor data in real time. The entry point into *trignoclient* is through a *network::Client* object, which handles both incoming EMG and Auxliary data, as well as the command interface with the server for configuration and status purposes.

#### Data structures

Read sensor data is stored into three different data container types: *Sample*, which holds single or multichannel data for a single sensor, *Frame* with multiple synchronized samples for all active sensors at a specific instant, and *Sequence*, which represents a time-indexed sequence of frames i.e. the whole recorded sensor signal.

These classes were designed with the high sample rate of Trigno systems in mind, and as such it is cheap to copy and move them around as needed. They are also versatile enough to hold both EMG and AUX data, as well as custom processed/filtered data. 

#### Development of experimental protocols

Lastly, at a higher abstraction level, *trignoclient* also provides basic tools for streamlining the design of custom experimental protocols with the Trigno SDK. Basic workflow is provided through the *tools::BasicProtocol* class, which represents an abstract type for each protocol, and provides basic implementation of common operations such as *wait*, *message*, *record*, *save* and *plot*.

Additionally, other tools are provided for data processing e.g. *RMSFilter*, *SequenceMetrics* and *SynergyExtractor* that

When designing a new acquisition protocol, it is strongly recommended to derive from *BasicProtocol* and override *run()* function with the desired experimental steps. 

It is noteworthy to mention that provided tools can be run synchronously or asynchronously (multi-threaded), through the base implementation of *std::basic_executor*, which is particularly helpful in online (while data is being received) applications or with demanding processing operations. When extending *trignoclient* tool set, the same class topology is encouraged.

### NOTE

Delsys® Trigno® Software, namely the *Trigno Control Application*, is copyrighted proprietary software that requires a valid commercial license to operate. A Windows OS running the software, and connected to the sensing hardware is strictly required for use. Refer to the <a href="https://www.delsys.com/downloads/USERSGUIDE/trigno/wireless-biofeedback-system.pdf">Trigno Wireless Biofeedback System</a> and for installation, configuration and additional instructions.

It is also important to note that *EMGWorks Acquisition* and *EMGWorks Analysis* are not compatible with the *Trigno Control Application*, i.e. it is not possible to run them simultaneously. Therefore the main scope of this library is the development of standalone third-party applications, which must reimplement several utilities provided by the EMGWorks tools when required. Additionaly, the output files used to load EMG results are stored in a proprietary format that is not serialized/human-readable, therefore it is not possible to load EMG/AUX data saved by *trignoclient* into *EMGWorks Analysis*

The code in this repository is provided as-is and free-of-charge under the MIT license, see LICENSE for details. 