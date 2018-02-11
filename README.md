# Airwaves FM

Airwaves FM is a software defined FM radio receiver that uses the RTL-SDR receiver.
As a bonus you have around 18700 FIR filters generated using Matlab with the window method using a Blackman window. Their orders range from 2 to 200 and their frequencies from 0.01 to 0.99 with an increment of 0.01. The code that generated them is also provided.

## Prerequisites

* Linux distro (was tested on Ubuntu 14.04 ONLY). It should not be hard to modify to make it work on Windows or other OS if you have the experience.
* GNU C++ compiler or another C++ compiler
* the rtl-sdr library librtlsdr with rtl_tcp (which should come with the library)
* Pulse Audio library libpulse

## Compilation

Compile using the given Makefile with the classic "make" command.

## Usage

./airwaves_fm -m "mode" -f "frequency" -o "order"

where "mode" can be mono or stereo, "frequency" can be a frequency in the FM band and "order" can be an order for the used filters in the range 2-200. For some unknown reason the application gives a segfault if the order is very high (somewhere in the range 180-200).

The program accepts the following runtime commands:

* freq=? to print the current frequency
* freq="frequency" where "frequency" is a frequency in the FM band
* volume=? to print the volume
* volume="volume" to change the volume (where "volume" is a value between 0.0 to 1.0)
* power=? to print the mean power of a 250 ms signal block before demodulation in dB relative to the level of 1
* quit to exit the program correctly

## License

The program is released under the three-clause BSD License. You can find the full license text in License.txt.
