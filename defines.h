/*
 * defines.h
 *
 *  Created on: Jun 13, 2015
 *      Author: ionut
 */

#ifndef DEFINES_H_
#define DEFINES_H_

// Enable freq++ and freq-- commands for testing results

//#define TESTING_RESULTS

#ifdef TESTING_RESULTS
#define FREQUENCY_INCREMENT_STRING					(char *)"++"
#define FREQUENCY_DECREMENT_STRING					(char *)"--"
#define FREQUENCY_INCREMENT_STEP					(int)100000
#define FREQUENCY_DECREMENT_STEP					(int)100000
#endif

// Receiver Mode enumeration
enum Mode
{
	ModeMono,
	ModeStereo,
	ModeUndefined
};

/* DEFAULTS */

#define UNDEFINED 									-1
#define DEFAULT_FREQUENCY 							89000000
#define DEFAULT_FILTER_ORDER 						100
#define DEFAULT_VOLUME								1

/* COMMANDS RELATED */

#define MAX_COMMAND_SIZE							100
#define COMMAND_SEPARATOR							'='
#define COMMAND_QUERRY								'?'
#define QUIT_COMMAND_STRING							(char *)"quit"
#define FREQUENCY_COMMAND_STRING					(char *)"freq"
#define VOLUME_COMMAND_STRING						(char *)"volume"
#define POWER_QUERRY_COMMAND						(char *)"power=?"

/* LIMITS */

#define LOWEST_FREQUENCY							87500000
#define HIGHEST_FREQUENCY							108000000
#define LOWEST_VOLUME								0
#define HIGHEST_VOLUME								1
#define LOWEST_FILTER_ORDER							2
#define HIGHEST_FILTER_ORDER						200

/* GENERAL CONSTANTS */

#define GAIN										(char *)"0.0"
#define RAW_SIGNAL_SAMPLE_RATE						2116800
#define SIGNAL_BLOCK_PERIOD							0.25
#define NUMBER_OF_AUDIO_CHANNELS					2
#define AUDIO_SAMPLE_RATE							44100
#define LOCAL_IP									(char *)"127.0.0.1"
#define PORT										12390
#define STEREO_FIRST_FILTER_FREQ					(char *)"0.1"
#define STEREO_SECOND_FILTER_FREQ					(char *)"0.12"
#define STEREO_FIRST_DECIMATION_FACTOR				8
#define STEREO_SECOND_DECIMATION_FACTOR				6
#define STEREO_DEMODULATED_SIGNAL_SAMPLE_RATE		264600
#define STEREO_SHIFT_FREQUENCY						38000
#define MONO_FIRST_FILTER_FREQ						(char *)"0.1"
#define MONO_SECOND_FILTER_FREQ						(char *)"0.16"
#define MONO_FIRST_DECIMATION_FACTOR				8
#define MONO_SECOND_DECIMATION_FACTOR				6

#endif /* DEFINES_H_ */
