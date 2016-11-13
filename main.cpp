#include <iostream>
#include "Signal.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <fcntl.h>
#include <string.h>
#include "SocketManager.h"
#include "SoundPlayer.h"
#include <pthread.h>
#include "Signal.h"
#include "defines.h"

// Initialization of global variables
static bool running = true;
int filterLength1 = UNDEFINED;
int filterLength2 = UNDEFINED;
int frequency = UNDEFINED;
Mode mode = ModeUndefined;
double signalPower = 0;
double volume = (double)DEFAULT_VOLUME;

// Prints the usage of the program to the screen
void print_usage()
{
	printf("%s%s%s%s%s%s%s", "\nUsage: airwaves_fm -m [mono/stereo] -f [87500000 - 108000000] -o [2-200]\n",
			"\t-m is the MODE which can be mono FM radio or stereo FM radio, default mono\n",
			"\t\tStereo FM radio has a greater audio quality, but it's much more CPU intensive than mono.\n",
			"\t-f is the central FREQUENCY which can be in the interval [87500000 - 108000000], default 89000000\n",
			"\t-o is the ORDER of the filters used which can be in the interval [2-200], default 50\n",
			"\t\tBeware that high order filters are VERY intensive for the CPU. An order around 50 is a good compromise between speed and quality.\n",
			"To correctly quit press \"q\" and then ENTER\n\n");
}

// The thread function that listens to the keyboard commands
void *io_thread(void * args)
{
	// Initialize variables
	SocketManager * socketManager = (SocketManager *)args;
	unsigned long int max_command_size = MAX_COMMAND_SIZE;
	int command_length;
	char * runtime_command = (char *) malloc(max_command_size);

	// Infinite keyboard input listening loop
	while (1)
	{
		// Clear command buffer
		memset(runtime_command, 0, max_command_size);

		// Get the new command
		command_length = getline(&runtime_command, &max_command_size, stdin);

		// Skip if unable to read command
		if (command_length < 0)
		{
			printf("Unable to read command!\n");
			continue;
		}

		if (strncmp(runtime_command, QUIT_COMMAND_STRING, strlen(QUIT_COMMAND_STRING)) == 0)
		{
			// If "quit" command was received, quit program
			socketManager->DisconnectFromDevice();
			exit(0);
		}
		else
		{
			// Get the possition of the command separator '='
			char * ptr = strchr(runtime_command, COMMAND_SEPARATOR);

			// If '=' was not found, command is invalid => skip
			if (ptr == NULL)
			{
				printf("Invalid command!\n");
				continue;
			}

			// Go to the first char after '='
			ptr++;

			// Check if the command is related to frequency
			if (strncmp(runtime_command, FREQUENCY_COMMAND_STRING, strlen(FREQUENCY_COMMAND_STRING)) == 0)
			{
				// If command is a querry, print the frequency and continue
				if (*ptr == COMMAND_QUERRY)
				{
					printf("Frequency is %d Hz\n", frequency);
					continue;
				}

#ifdef TESTING_RESULTS

				if (strncmp(ptr, FREQUENCY_INCREMENT_STRING, strlen(FREQUENCY_INCREMENT_STRING)) == 0)
				{
					frequency += FREQUENCY_INCREMENT_STEP;
					if (frequency > HIGHEST_FREQUENCY)
					{
						frequency -= FREQUENCY_INCREMENT_STEP;
					}
					else
					{
						SocketCommand frequencyCommand(Command::SET_FREQUENCY, frequency);
						socketManager->SendCommand(frequencyCommand);
						printf("Frequency set to %d Hz\n", frequency);
						sleep(1);
						printf("Signal power is %f dB\n", signalPower);
					}

					continue;
				}

				if (strncmp(ptr, FREQUENCY_DECREMENT_STRING, strlen(FREQUENCY_DECREMENT_STRING)) == 0)
				{
					frequency -= FREQUENCY_DECREMENT_STEP;
					if (frequency < LOWEST_FREQUENCY)
					{
						frequency += FREQUENCY_DECREMENT_STEP;
					}
					else
					{
						SocketCommand frequencyCommand(Command::SET_FREQUENCY, frequency);
						socketManager->SendCommand(frequencyCommand);
						printf("Frequency set to %d Hz\n", frequency);
						sleep(1);
						printf("Signal power is %f dB\n", signalPower);
					}

					continue;
				}

#endif

				// Get the new frequency because the user wants to set it
				int new_frequency = atoi(ptr);

				// If the frequency is valid, set the new frequency,
				// print it, print signal power and continue
				if (new_frequency >= LOWEST_FREQUENCY && new_frequency <= HIGHEST_FREQUENCY)
				{
					frequency = new_frequency;
					SocketCommand frequencyCommand(Command::SET_FREQUENCY, frequency);
					socketManager->SendCommand(frequencyCommand);
					printf("Frequency set to %d Hz\n", frequency);
					sleep(1);
					printf("Signal power is %f dB\n", signalPower);
					continue;
				}
				else
				{
					// If frequency is invalid, skip
					printf("Invalid frequency! Frequency must be between %d and %d Hz!\n", (int)LOWEST_FREQUENCY, (int)HIGHEST_FREQUENCY);
					continue;
				}
			}
			// If the command is "power=?", print the power and continue
			else if(strncmp(runtime_command, POWER_QUERRY_COMMAND, strlen(POWER_QUERRY_COMMAND)) == 0)
			{
				printf("Signal power is %f dB\n", signalPower);
				continue;
			}
			// Check to see if the command is related to volume
			else if (strncmp(runtime_command, VOLUME_COMMAND_STRING, strlen(VOLUME_COMMAND_STRING)) == 0)
			{
				// If the command is querry, print the volume and continue
				if (*ptr == COMMAND_QUERRY)
				{
					printf("Volume is %f\n", volume);
					continue;
				}

				// Get the new volume because the user wants to set it
				double new_volume = atof(ptr);

				// If the given volume is valid, set it, print it and continue
				if (new_volume >= LOWEST_VOLUME && new_volume <= HIGHEST_VOLUME)
				{
					volume = new_volume;
					printf("Volume set to %f\n", volume);
					continue;
				}
				// If the volume is not valid, skip
				else
				{
					printf("Invalid volume! Volume must be a real number between %d and %d!\n", (int)LOWEST_VOLUME, (int)HIGHEST_VOLUME);
					continue;
				}
			}
		}

	}

	return NULL;
}

int main(int argc, char *argv[])
{
	// The socket manager responsible for communication
	// with the rtl_tcp program
	SocketManager socketManager;

	// The command listening thread
	pthread_t IO_thread;

	// Start the command listening thread
	pthread_create(&IO_thread, NULL, io_thread, &socketManager);

	// Declare and initialize some variables
	int option;
	int signalLength = (int) ( (float) RAW_SIGNAL_SAMPLE_RATE * (float) SIGNAL_BLOCK_PERIOD * 2);
	double * filter1;
	double * filter2;
	double * coeff = (double *) malloc(sizeof(double));
	char filter_location[300];

	// The input buffer in which the samples are received
	unsigned char * inBuffer = (unsigned char *) malloc(signalLength);
	memset(inBuffer, 0, signalLength);

	// Initialize the sound player that uses Pulse Audio Library
	SoundPlayer soundPlayer(NUMBER_OF_AUDIO_CHANNELS, AUDIO_SAMPLE_RATE);

	// Parse the initial frequency, mode and filter orders
	while ((option = getopt(argc, argv, "m:f:o:")) != -1)
	{
		switch (option){
			case 'f' :
				frequency = atoi(optarg);
				if (frequency < LOWEST_FREQUENCY || frequency > HIGHEST_FREQUENCY)
				{
					frequency = UNDEFINED;
				}
				break;

			case 'o' :
				filterLength1 = filterLength2 = atoi(optarg);
				if (filterLength1 < LOWEST_FILTER_ORDER || filterLength1 > HIGHEST_FILTER_ORDER)
				{
					filterLength1 = filterLength2 = UNDEFINED;
				}
				else
				{
					filterLength1++;
					filterLength2++;
				}
				break;

			case 'm' :
				if (strcmp(optarg, "mono") == 0)
				{
					mode = ModeMono;
				}
				else if (strcmp(optarg, "stereo") == 0)
				{
					mode = ModeStereo;
				}
				else
				{
					mode = ModeUndefined;
				}
				break;

			default:
				print_usage();
				return -1;
		}
	}

	// If no initial parameters was given correctly, print the usage
	if (frequency == UNDEFINED && filterLength1 == UNDEFINED && mode == ModeUndefined)
	{
		print_usage();
		exit(0);
	}

	// Set default frequency if a good one was not given
	if (frequency == UNDEFINED)
	{
		frequency = DEFAULT_FREQUENCY;
	}

	// Set default filter order if a good one was not given
	if (filterLength1 == -1)
	{
		filterLength1 = filterLength2 = (int)DEFAULT_FILTER_ORDER + 1;
	}

	// Set default mode if a good one was not specified
	if (mode == ModeUndefined)
	{
		mode = ModeStereo;
	}

	// Allocate the filters
	filter1 = (double *) malloc(filterLength1);
	filter2 = (double *) malloc(filterLength2);

	// Initialize the Socket Manager
	socketManager.Init(LOCAL_IP, PORT);

	// Disconnect from the device in case a previous connection
	// is still active
	socketManager.DisconnectFromDevice();

	// Connect to the rtl_tcp program with initial params via socket
    socketManager.ConnectToDevice(frequency, GAIN, RAW_SIGNAL_SAMPLE_RATE, true);

    // Check if the mode is stereo
    if (mode == ModeStereo)
    {
    	// Declare the signal blocks buffers
    	Complex * initializedInputSignal = NULL;
    	int initializedInputSignalLength;
		Complex * decimatedFmSignal = NULL;
		int decimatedFmSignalLength;
		double * demodSignal = NULL;
		int demodSignalLength;
		double * shiftedDemodSignal = NULL;
		int shiftedDemodSignalLength;
		double * lprSignal = NULL;
		int lprSignalLength;
		double * lmrSignal = NULL;
		int lmrSignalLength;
		double * leftSignal = NULL;
		int leftSignalLength;
		double * rightSignal = NULL;
		int rightSignalLength;
		double * leftFinalSignal = NULL;
		int leftFinalSignalLength;
		double * rightFinalSignal = NULL;
		int rightFinalSignalLength;
		short * audioSamples = NULL;
		int audioSamplesLength;

		// Read the first filter from file
		sprintf(filter_location, "%s%d%s%s%s", "filters/filter_order", (int)(filterLength1 - 1), "_freq", STEREO_FIRST_FILTER_FREQ ,".dat");
		int filter1File = open(filter_location, O_RDONLY);
		if (filter1File < 0)
		{
			printf("Could not open filter 1 file!\n");
			return -1;
		}
		for (int i = 0; i < filterLength1; i++)
		{
			if (read(filter1File, coeff, sizeof(double)) < 0)
			{
				printf("Could not read filter 1 coeff %d\n", i);
				return -1;
			}

			filter1[i] = *coeff;
		}
		close(filter1File);

		// Read the second filter from file
		sprintf(filter_location, "%s%d%s%s%s", "filters/filter_order", (int)(filterLength1 - 1), "_freq", STEREO_SECOND_FILTER_FREQ ,".dat");
		int filter2File = open(filter_location, O_RDONLY);
		if (filter2File < 0)
		{
			printf("Could not open filter 2 file!\n");
			return -1;
		}
		for (int i = 0; i < filterLength2; i++)
		{
			if (read(filter2File, coeff, sizeof(double)) < 0)
			{
				printf("Could not read filter 2 coeff %d\n", i);
				return -1;
			}
			filter2[i] = *coeff;
		}

		// Start the main signal acquisition, processing and playing loop
		while (running)
		{
			// Read a 250 ms buffer from rtl_tcp via socket
			if (socketManager.ReadFromSocket(inBuffer, signalLength) < 0)
			{
				printf ("Could not read samples from socket!\n");
				return -1;
			}

			// Instantiate a signal object from the received block
			Signal::initSignalFromSocket(inBuffer, signalLength, &initializedInputSignal, &initializedInputSignalLength);

			// Decimate and filter the initial signal
			if (Signal::decimate(initializedInputSignal, initializedInputSignalLength, &decimatedFmSignal, &decimatedFmSignalLength, filter1, filterLength1, STEREO_FIRST_DECIMATION_FACTOR) < 0)
			{
				return -1;
			}

			// Compute the signal power before demodulation
			signalPower = Signal::computeSignalPower(decimatedFmSignal, decimatedFmSignalLength);
			signalPower = Signal::toPowerDb(signalPower);

			// FM demodulate the signal block
			if (Signal::fmDemodulate(decimatedFmSignal, decimatedFmSignalLength, &demodSignal, &demodSignalLength) < 0)
			{
				return -1;
			}

			// Filter the demodulated signal
			if (Signal::decimate(demodSignal, demodSignalLength, &lprSignal, &lprSignalLength, filter2, filterLength2, 1) < 0)
			{
				return -1;
			}

			// Shift the spectrum of the signal
			if (Signal::shiftFrequency(demodSignal, demodSignalLength, &shiftedDemodSignal, &shiftedDemodSignalLength, STEREO_SHIFT_FREQUENCY, STEREO_DEMODULATED_SIGNAL_SAMPLE_RATE, -1) < 0)
			{
				return -1;
			}

			// Filter the shifted signal
			if (Signal::decimate(shiftedDemodSignal, shiftedDemodSignalLength, &lmrSignal, &lmrSignalLength, filter2, filterLength2, 1) < 0)
			{
				return -1;
			}

			// Generate the left and right signals
			if (Signal::FmLrGenerate(lprSignal, lmrSignal, lprSignalLength, &leftSignal, &leftSignalLength, &rightSignal, &rightSignalLength, volume) < 0)
			{
				return -1;
			}

			// Downsample the left signal
			if (Signal::downsample(leftSignal, leftSignalLength, &leftFinalSignal, &leftFinalSignalLength, STEREO_SECOND_DECIMATION_FACTOR) < 0)
			{
				return -1;
			}

			// Downsample the right signal
			if (Signal::downsample(rightSignal, rightSignalLength, &rightFinalSignal, &rightFinalSignalLength, STEREO_SECOND_DECIMATION_FACTOR) < 0)
			{
				return -1;
			}

			// Produce the final interleaved audio signal
			if (Signal::produceFmStereoAudio(leftFinalSignal, rightFinalSignal, leftFinalSignalLength, &audioSamples, &audioSamplesLength) < 0)
			{
				return -1;
			}

			// Send the audio signal block to the sound card
			soundPlayer.PlaySamples(audioSamples, audioSamplesLength * 4);
		}

    }
    // Check to see if the mode is mono
    else if (mode == ModeMono)
    {
    	// Declare the signal blocks buffers
    	Complex * initializedInputSignal = NULL;
    	int initializedInputSignalLength;
    	Complex * decimatedSamples = NULL;
    	int decimatedSamplesLength;
    	double * fmDemodulatedSamples = NULL;
    	int fmDemodulatedSaplesLength;
    	double * decimatedFmDemodulatedSamples = NULL;
    	int decimatedFmDemodulatedSamplesLength;
    	short * audioSamples = NULL;
    	int audioSamplesLength;

    	// Read the first filter
		sprintf(filter_location, "%s%d%s%s%s", "filters/filter_order", (int)(filterLength1 - 1), "_freq", MONO_FIRST_FILTER_FREQ ,".dat");
        int filter1File = open(filter_location, O_RDONLY);
        if (filter1File < 0)
        {
        	printf("Could not open filter 1 file!\n");
        	return -1;
        }
    	for (int i = 0; i < filterLength1; i++)
    	{
    		if (read(filter1File, coeff, sizeof(double)) < 0)
    		{
    			printf("Could not read filter 1 coeff %d\n", i);
    			return -1;
    		}

    		filter1[i] = *coeff;
    	}
    	close(filter1File);

    	// Read the second filter
		sprintf(filter_location, "%s%d%s%s%s", "filters/filter_order", (int)(filterLength1 - 1), "_freq", MONO_SECOND_FILTER_FREQ ,".dat");
    	int filter2File = open(filter_location, O_RDONLY);
    	if (filter2File < 0)
    	{
    		printf("Could not open filter 2 file!\n");
    		return -1;
    	}
    	for (int i = 0; i < filterLength2; i++)
    	{
    		if (read(filter2File, coeff, sizeof(double)) < 0)
    		{
    			printf("Could not read filter 2 coeff %d\n", i);
    			return -1;
    		}
    		filter2[i] = *coeff;
    	}

    	close(filter2File);

    	// Start the main acquisition, signal processing and sound playing loop
    	while (running)
    	{

    		// Read a 250 ms samples block from the rtl_tcp program via socket
    		if (socketManager.ReadFromSocket(inBuffer, signalLength) < 0)
    		{
    			printf ("Could not read samples from socket!\n");
    			return -1;
    		}

    		// Instantiate a signal object based on the raw signal block
    		Signal::initSignalFromSocket(inBuffer, signalLength, &initializedInputSignal, &initializedInputSignalLength);

    		// Filter and decimate the signal block
    		if (Signal::decimate(initializedInputSignal, initializedInputSignalLength, &decimatedSamples, &decimatedSamplesLength, filter1, filterLength1, MONO_FIRST_DECIMATION_FACTOR) < 0)
    		{
    			return -1;
    		}

    		// Compute mean signal power before FM demodulation
			signalPower = Signal::computeSignalPower(decimatedSamples, decimatedSamplesLength);
			signalPower = Signal::toPowerDb(signalPower);

			// FM demodulate the signal block
    		if (Signal::fmDemodulate(decimatedSamples, decimatedSamplesLength, &fmDemodulatedSamples, &fmDemodulatedSaplesLength) < 0)
    		{
    			return -1;
    		}

    		// Filter and decimate the demodulated signal
    		if (Signal::decimate(fmDemodulatedSamples, fmDemodulatedSaplesLength, &decimatedFmDemodulatedSamples, &decimatedFmDemodulatedSamplesLength, filter2, filterLength2, MONO_SECOND_DECIMATION_FACTOR) < 0)
    		{
    			return -1;
    		}

    		// Generate the final audio signal
    		if (Signal::produceAudio(decimatedFmDemodulatedSamples, decimatedFmDemodulatedSamplesLength, &audioSamples, &audioSamplesLength, volume) < 0)
    		{
    			return -1;
    		}

    		// Send the audio signal block to the sound card
    		soundPlayer.PlaySamples(audioSamples, audioSamplesLength * 4);
    	}
    }

    return 0;

}
