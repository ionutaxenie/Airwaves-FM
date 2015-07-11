/*
 * SoundPlayer.cpp
 *
 *  Created on: Dec 7, 2014
 *      Author: ionut
 */

#include "SoundPlayer.h"

// Empty constructor
SoundPlayer::SoundPlayer()
{
}

// Destructor that frees memory used by Pulse Audio
SoundPlayer::~SoundPlayer() {
	pa_simple_free(s);
}

// Constructor that initializes Pulse Audio
SoundPlayer::SoundPlayer(int channels, int sampling_rate)
{
	ss.format = PA_SAMPLE_S16NE;
	ss.channels = channels;
	ss.rate = sampling_rate;

	pa_buffer_attr buffer_attr;
	buffer_attr.fragsize = (uint32_t) -1;
	buffer_attr.maxlength = (uint32_t) -1;
	buffer_attr.minreq = (uint32_t) -1;
	buffer_attr.prebuf = (uint32_t) -1;
	buffer_attr.tlength = (uint32_t) -1;

	s = pa_simple_new(NULL, "airwaves_fm", PA_STREAM_PLAYBACK, NULL, "Music", &ss, NULL, &buffer_attr, NULL);
}

// Method used to send data to the sound card
void SoundPlayer::PlaySamples(short * buffer, int number_of_samples)
{
	int error;
	pa_simple_write(s, buffer, number_of_samples, &error);
}

