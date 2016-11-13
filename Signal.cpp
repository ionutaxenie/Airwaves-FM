#include "Signal.h"

// Empty constructor
Signal::Signal()
{
}

int Signal::initSignalFromSocket(unsigned char * in, int inLength, Complex ** out, int * outLength)
{
    int i;

    if (inLength <= 0)
    {
    	printf("Signal from socket length is not valid\n");
    	return -1;
    }

    if (in == NULL)
    {
    	printf("Invalid signal buffer received when initializing signal from socket\n");
    	return -1;
    }

    *outLength = inLength / 2;
    unsigned long int size = *outLength * sizeof(Complex);

    // Allocate the block of complex samples
    if (*out == NULL)
    {
    	*out = (Complex *) malloc(size);
    }

    if (*out == NULL)
    {
    	printf("Could not allocate samples buffer when initializing signal from socket!\n");
    	return -1;
    }

    // Construct the block of complex samples centered around 0
    // from the raw signal encoded as unsigned char values
    for (i = 0; i < *outLength; i++)
    {
        (*out)[i].re = (unsigned char)in[2 * i] - 127;
        (*out)[i].im = (unsigned char)in[2 * i + 1] - 127;
    }

    return 0;
}

// Decimate a complex signal and filter or not before decimation
int Signal::decimate(Complex * in, int inLength, Complex ** out, int * outLength, double * filter, int filterLength, int factor)
{
    int i, j;

    // Return error if filter length is not valid
    if (filterLength <= 0)
    {
    	printf("Filter length is smaller than 1! Aborting decimation...\n");
    	return -1;
    }

    // Return error if filter is empty
    if (filter == NULL)
    {
    	printf("Received NULL filter in decimation! Aborting...\n");
    	return -1;
    }

    // Return error if input signal is empty
    if (in == NULL)
    {
    	printf("Samples buffer is NULL! Aborting decimation...\n");
    	return -1;
    }

    // Compute the length of the final signal
    *outLength = inLength / factor;

    // If the output signal is not allocated, allocate it
    if (*out == NULL)
    {
    	*out = (Complex *) malloc(*outLength * sizeof(Complex));
    }

    // Filter the signal. The filter or the signal are NOT reversed
    // because the function assumes a symmetric FIR filter
    // In case of a non-symmetric filter you have to reverse it yourself
    // before using this function
    for (i = 0; i < inLength; i+=factor)
    {
    	Complex c(0, 0);
	    for (j = 0; j < filterLength; j++)
		{
			if (i - j >= 0)
			{
				c += in[i - j] * filter[filterLength - j - 1];
			}
		}
    	(*out)[ i / factor ] = c;
    }

    return 0;
}

// Decimate a real signal and filter or not before decimation
int Signal::decimate(double * in, int inLength, double ** out, int * outLength, double * filter, int filterLength, int factor)
{
    int i, j;
    double c;

    // Return error if filter length is not valid
    if (filterLength <= 0)
    {
    	printf("Filter length is smaller than 1! Aborting decimation...\n");
    	return -1;
    }

    // Return error if filter is empty
    if (filter == NULL)
    {
    	printf("Received NULL filter in decimation! Aborting...\n");
    	return -1;
    }

    // Return error if signal is empty
    if (in == NULL)
    {
    	printf("Samples buffer is NULL! Aborting decimation...\n");
    	return -1;
    }

    // Compute the length of the output signal
    *outLength = inLength / factor;

    // If the output signal is not allocated, allocate it
    if (*out == NULL)
    {
    	*out = (double *) malloc(*outLength * sizeof(double));
    }

    // Filter the signal. The filter or the signal are NOT reversed
    // because the function assumes a symmetric FIR filter
    // In case of a non-symmetric filter you have to reverse it yourself
    // before using this function
    for (i = 0; i < inLength; i+=factor)
    {
    	c = 0;
        for (j = 0; j < filterLength; j++)
        {
            if (i - j >= 0)
            {
                c += in[i - j] * filter[filterLength - j - 1];
            }
        }

    	(*out)[ i / factor ] = c;
    }

    return 0;
}

// FM demodulate a complex signal. Resulting signal will be a real signal
// in the [-1;1] range
int Signal::fmDemodulate(Complex * in, int inLength, double ** out, int * outLength)
{
	int i;

	// Return error if input signal is empty
    if (in == NULL)
    {
    	printf("Input buffer is NULL! Aborting FM demodulation!");
    	return -1;
    }

    // Output signal has the same length
    *outLength = inLength;

    // If output signal is not allocated, allocate it
    if (*out == NULL)
    {
    	*out = (double *)malloc(*outLength * sizeof(double));
    }

    // Perform the FM demodulation
    for (i = 1; i < *outLength; i++)
    {
    	(*out)[i - 1] = (in[i] * in[i - 1].conj()).angle() / M_PI;
    }

    // Interpolation of the last sample using the last derivative
    // of the signal(first order interpolation);
    // This is used to minimize the nonlinearity distortion
    // (which sounds like a click if you replicate the last sample)
    // What this actually does is predict the last sample i based
    // on the value of the previous sample (i-1) to which the
    // instant derivative between the previous 2 samples (i-1 and i-2) is added.
    (*out)[i] = (*out)[i - 1] * 2.0 - (*out)[i - 2];

    return 0;
}

int Signal::fmDemodulate(Complex * in, int inLength, Complex ** out, int * outLength)
{
	int i;

	// Return error if input signal is empty
    if (in == NULL)
    {
    	printf("Input buffer is NULL! Aborting FM demodulation!");
    	return -1;
    }

    // Output signal has the same length
    *outLength = inLength;

    // If output signal is not allocated, allocate it
    if (*out == NULL)
    {
    	*out = (Complex *)malloc(*outLength * sizeof(Complex));
    }

    // Perform the FM demodulation
    for (i = 1; i < *outLength; i++)
    {
    	(*out)[i - 1] = Complex((in[i] * in[i - 1].conj()).angle() / M_PI, 0);
    }

    // Interpolation of the last sample using the last derivative
    // of the signal(first order interpolation);
    // This is used to minimize the nonlinearity distortion
    // (which sounds like a click if you replicate the last sample)
    // What this actually does is predict the last sample i based
    // on the value of the previous sample (i-1) to which the
    // instant derivative between the previous 2 samples (i-1 and i-2) is added.
    (*out)[i] = (*out)[i - 1] * 2.0 - (*out)[i - 2];

    return 0;
}

// Shift the frequency spectrum of a complex signal
int Signal::shiftFrequency(Complex * in, int inLength, Complex ** out, int * outLength, double frequency, double sampling_frequency, int direction)
{
	int i;

	// Return error if input signal is empty
	if (in == NULL)
	{
		printf("Input buffer is NULL! Aborting frequency shift");
		return -1;
	}

	// Output length is the same
	*outLength = inLength;

	// If output signal is not allocated, allocate it
	if (*out == NULL)
	{
		*out = (Complex *)malloc(*outLength * sizeof(Complex));
	}

	// Perform the frequency spectrum shift
	for (i = 0; i < *outLength; i++)
	{
		Complex c(0, direction * 2 * M_PI * frequency * i / sampling_frequency);
		(*out)[i] = in[i] * c.Exp();
	}

	return 0;
}

// Shift the frequency spectrum of a real signal
int Signal::shiftFrequency(double * in, int inLength, double ** out, int * outLength, double frequency, double sampling_frequency, int direction)
{
	int i;

	// Return error if input signal in empty
	if (in == NULL)
	{
		printf("Input buffer is NULL! Aborting frequency shift");
		return -1;
	}

	// Output signal has the same length
	*outLength = inLength;

	// If output signal in not allocated, allocate it
	if (*out == NULL)
	{
		*out = (double *)malloc(*outLength * sizeof(double));
	}

	// Perform the frequency spectrum shift
	for (i = 0; i < *outLength; i++)
	{
		Complex c(0, (double)direction * 2.0 * double(M_PI) * frequency * (double)i / sampling_frequency);
		Complex inSample(in[i], 0);
		(*out)[i] = (inSample * c.Exp()).re;
	}

	return 0;
}

// Downsample a complex signal
int Signal::downsample(Complex * in, int inLength, Complex ** out, int * outLength, int factor)
{
	int i;

	// Return error if input signal is empty
	if (in == NULL)
	{
		printf("Input buffer is NULL! Aborting downsampling");
		return -1;
	}

	// Compute the output signal length
	*outLength = inLength / factor;

	// If output signal is not allocated, allocate it
	if (*out == NULL)
	{
		*out = (Complex *)malloc(*outLength * sizeof(Complex));
	}

	// Perform the downsampling
	for (i = 0; i < inLength; i += factor)
	{
		(*out)[i / factor] = in[i];
	}

	return 0;
}

// Downsample a real signal
int Signal::downsample(double * in, int inLength, double ** out, int * outLength, int factor)
{
	int i;

	// Return error if the input signal is empty
	if (in == NULL)
	{
		printf("Input buffer is NULL! Aborting downsampling");
		return -1;
	}

	// Compute the output signal length
	*outLength = inLength / factor;

	// If the output signal is not allocated, allocate it
	if (*out == NULL)
	{
		*out = (double *)malloc(*outLength * sizeof(double));
	}

	// Perform the downsampling
	for (i = 0; i < inLength; i += factor)
	{
		(*out)[i / factor] = in[i];
	}

	return 0;
}

// Generate the left and the right channels from
// the complex L+R and L-R audio channels
int Signal::FmLrGenerate(Complex * lprIn, Complex * lmrIn, int inLength, Complex ** leftOut, int * leftOutLength, Complex ** rightOut, int * rightOutLength, double volume)
{
	int i;

	// Return error if the L+R signal is empty
	if (lprIn == NULL)
	{
		printf("LPR input buffer is NULL! Aborting FM LR Generation!");
		return -1;
	}

	// Return error if the L-R signal is empty
	if (lmrIn == NULL)
	{
		printf("LMR input buffer is NULL! Aborting FM LR Generation!");
		return -1;
	}

	// Left and Right channels have the same length as
	// any of the two input signals
	*leftOutLength = inLength;
	*rightOutLength = inLength;

	// If the left output signal is not allocated, allocate it
	if (*leftOut == NULL)
	{
		*leftOut = (Complex *)malloc(*leftOutLength * sizeof(Complex));
	}

	// If the right output signal is not allocated, allocate it
	if (*rightOut == NULL)
	{
		*rightOut = (Complex *)malloc(*rightOutLength * sizeof(Complex));
	}

	// Generate the left and the right signals
	for (i = 0; i < inLength; i++)
	{
		(*leftOut)[i] = (lprIn[i] + lmrIn[i]) * volume;
		(*rightOut)[i] = (lprIn[i] - lmrIn[i] * volume);
	}

	return 0;
}

// Generate the left and the right channels from
// the real L+R and L-R audio channels
int Signal::FmLrGenerate(double * lprIn, double * lmrIn, int inLength, double ** leftOut, int * leftOutLength, double ** rightOut, int * rightOutLength, double volume)
{
	int i;

	// Return error if the L+R signal is empty
	if (lprIn == NULL)
	{
		printf("LPR input buffer is NULL! Aborting FM LR Generation!");
		return -1;
	}

	// Return error if the L-R signal is empty
	if (lmrIn == NULL)
	{
		printf("LMR input buffer is NULL! Aborting FM LR Generation!");
		return -1;
	}

	// Left and Right channels have the same length as
	// any of the two input signals
	*leftOutLength = inLength;
	*rightOutLength = inLength;

	// If the left output signal is not allocated, allocate it
	if (*leftOut == NULL)
	{
		*leftOut = (double *)malloc(*leftOutLength * sizeof(double));
	}

	// If the right output signal is not allocated, allocate it
	if (*rightOut == NULL)
	{
		*rightOut = (double *)malloc(*rightOutLength * sizeof(double));
	}

	// Generate the left and the right signals
	for (i = 0; i < inLength; i++)
	{

		(*leftOut)[i] = (lprIn[i] + lmrIn[i]) * volume;
		(*rightOut)[i] = (lprIn[i] - lmrIn[i]) * volume;
	}

	return 0;
}

// Generates the final audio signal for the mono mode
int Signal::produceAudio(double * in, int inLength, short ** out, int * outLength, double volume)
{
    int i;

    // Return error if input signal is empty
    if (in == NULL)
    {
    	printf("Input buffer is NULL! Aborting audio samples production...\n");
    	return -1;
    }

    // Output length is the same as input length
    *outLength = inLength;

    // If the output signal is not allocated, allocate it
    if (*out == NULL)
    {
    	*out = (short *) malloc(2 * (*outLength) * sizeof(short));
    }

    // Produce the final audio signal
    for (i = 0; i < *outLength; i++)
    {
        (*out)[2 * i] = (*out)[2 * i + 1] = (short) (in[i] * volume * 32767);
    }

    return 0;
}

// Produce the final audio signal for stereo mode from complex signals
int Signal::produceFmStereoAudio(Complex * leftIn, Complex * rightIn, int inLength, short ** audioOut, int * audioOutLength)
{
	int i;

	// Return error if left signal is empty
	if (leftIn == NULL)
	{
		printf("Left input buffer is NULL! Aborting FM stereo audio samples production...");
		return -1;
	}

	// Return error if right signal is empty
	if (rightIn == NULL)
	{
		printf("Right input buffer is NULL! Aborting FM stereo audio samples production...");
		return -1;
	}

	// Output signal length is the same
	*audioOutLength = inLength;

	// If output signal is not allocated, allocate it
	if (*audioOut == NULL)
	{
		*audioOut = (short *)malloc(*audioOutLength * 2 * sizeof(short));
	}

	// Produce final audio signal
	for (i = 0; i < *audioOutLength; i++)
	{
		(*audioOut)[2 * i] = (short) (leftIn[i].re * 32767);
		(*audioOut)[2 * i + 1] = (short) (rightIn[i].re * 32767);
	}

	return 0;
}

// Produce the final audio signal for stereo mode from real signals
int Signal::produceFmStereoAudio(double * leftIn, double * rightIn, int inLength, short ** audioOut, int * audioOutLength)
{
	int i;

	// Return error if left signal is empty
	if (leftIn == NULL)
	{
		printf("Left input buffer is NULL! Aborting FM stereo audio samples production...");
		return -1;
	}

	// Return error if right signal is empty
	if (rightIn == NULL)
	{
		printf("Right input buffer is NULL! Aborting FM stereo audio samples production...");
		return -1;
	}

	// Output signal length is the same
	*audioOutLength = inLength;

	// If output signal is not allocated, allocate it
	if (*audioOut == NULL)
	{
		*audioOut = (short *)malloc(*audioOutLength * 2 * sizeof(short));
	}

	// Produce final audio signal
	for (i = 0; i < *audioOutLength; i++)
	{
		(*audioOut)[2 * i] = (short) (leftIn[i] * 32767);
		(*audioOut)[2 * i + 1] = (short) (rightIn[i] * 32767);
	}

	return 0;
}

// Compute mean power of real signal
double Signal::computeSignalPower(double * in, int inLength)
{
	double squaredSum = 0;

	for (int i = 0; i < inLength; i++)
	{
		squaredSum += ( fabs(in[i] / 127) * fabs(in[i] / 127) );
	}

	return squaredSum / inLength;
}

// Compute mean power of complex signal
double Signal::computeSignalPower(Complex * in, int inLength)
{
	double squaredSum = 0;
	double normedSampleMagnitude = 0;

	for (int i = 0; i < inLength; i++)
	{
		normedSampleMagnitude = (in[i] / 127).abs();
		squaredSum += (normedSampleMagnitude * normedSampleMagnitude);
	}

	return squaredSum / inLength;
}

// Convert absolute power to dB
double Signal::toPowerDb(double in)
{
	return 10 * log10(in);
}

// Destructor that frees the initial signal block
Signal::~Signal()
{
}
