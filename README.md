Airwaves FM is a software defined FM radio receiver that uses the RTL-SDR receiver.

In order to get this thing up and running you need:

* Linux distro (was tested on Ubuntu 14.04 ONLY). It should not be hard to modify to make it work on Windows or other OS if you have the experience.
* GNU C++ compiler or another C++ compiler
* the rtl-sdr library librtlsdr with rtl_tcp (which should come with the library)
* Pulse Audio library libpulse

Compile using the given Makefile with the classic "make" command.

As a bonus you have around 17k FIR filters generated using Matlab with the window method using a Blackman window. Their orders range from 2 to 200 and their frequencies from 0.01 to 0.99 with an increment of 0.01. The code that generated them is also provided.

The program is started using:

./airwaves_fm -m <mode> -f <frequency> -o <order>

where <mode> can be mono or stereo, <frequency> can be a frequency in the FM band and <order> can be an order for the used filters in the range 2-200. For some unknown reason the application gives a segfault if the order is very high (somewhere in the range 180-200).

The program accepts the following runtime commands:

* freq=? to print the current frequency
* freq=<frequency> to change the frequency
* volume=? to print the volume
* volume=<volume> to change the volume (0.0 to 1.0 double value)
* power=? to print the mean power of a 250 ms signal block before demodulation in dB relative to the level of 1
* quit to exit the program correctly

This project was developed as my final project for telecommunications engineering degree. It was very challenging to make as I did not have too much experience with discrete signals in C++ and I am aware it could have been much better. Looking forward for some other people to make this more amazing then it currently is.

If you have any issues or questions email me at ionut.axenie@yahoo.com