#ifndef SIGNAL_H
#define SIGNAL_H

#include "Complex.h"
#include <malloc.h>
#include <iostream>
#include <math.h>
class Signal
{
    public:
        // Constructors and destructor
        Signal();
        virtual ~Signal();

        // Various signal operations
        static int initSignalFromSocket(unsigned char * in, int inLength, Complex ** out, int * outLength);
        static int decimate(Complex * in, int inLength, Complex ** out, int * outLength, double * filter, int filterLength, int factor);
        static int decimate(double * in, int inLength, double ** out, int * outLength, double * filter, int filterLength, int factor);
        static int fmDemodulate(Complex * in, int inLength, double ** out, int * outLength);
        static int fmDemodulate(Complex * in, int inLength, Complex ** out, int * outLength);
        static int shiftFrequency(Complex * in, int inLength, Complex ** out, int * outLength, double frequency, double sampling_frequency, int direction);
        static int shiftFrequency(double * in, int inLength, double ** out, int * outLength, double frequency, double sampling_frequency, int direction);
        static int downsample(Complex * in, int inLength, Complex ** out, int * outLength, int factor);
        static int downsample(double * in, int inLength, double ** out, int * outLength, int factor);
        static int FmLrGenerate(Complex * lprIn, Complex * lmrIn, int inLength, Complex ** leftOut, int * leftOutLength, Complex ** rightOut, int * rightOutLength, double volume);
        static int FmLrGenerate(double * lprIn, double * lmrIn, int inLength, double ** leftOut, int * leftOutLength, double ** rightOut, int * rightOutLength, double volume);
        static int produceAudio(double * in, int inLength, short ** out, int * outLength, double volume);
        static int produceFmStereoAudio(Complex * leftIn, Complex * rightIn, int inLength, short ** audioOut, int * audioOutLength);
        static int produceFmStereoAudio(double * leftIn, double * rightIn, int inLength, short ** audioOut, int * audioOutLength);
        static double computeSignalPower(Complex * in, int inLength);
        static double computeSignalPower(double * in, int inLength);
        static double toPowerDb(double in);
    protected:
    private:
        static Complex filterSample(Complex * in, int index, double * filter, int filterLength);
        static double filterSample(double * in, int index, double * filter, int filterLength);
};

#endif // SIGNAL_H
