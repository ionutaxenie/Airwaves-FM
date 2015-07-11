/*
 * SoundPlayer.h
 *
 *  Created on: Dec 7, 2014
 *      Author: ionut
 */

#ifndef SOUNDPLAYER_H_
#define SOUNDPLAYER_H_

#include <pulse/simple.h>

class SoundPlayer {
public:

	// Constructors and destructor
	SoundPlayer();
	virtual ~SoundPlayer();
	SoundPlayer(int channels, int sampling_rate);

	// Method for playing samples
	void PlaySamples(short * buffer, int number_of_samples);

private:

	// Members needed for Pulse Audio
	pa_simple *s;
	pa_sample_spec ss;
};

#endif /* SOUNDPLAYER_H_ */
