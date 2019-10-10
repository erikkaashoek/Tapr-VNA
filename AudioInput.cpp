#pragma once

#include "stdafx.h"

//#using <Winmm.dll>
using namespace std;  
#include <stdlib.h>

#include <Windows.h>
#include <MMSystem.h>
#include <iostream>
#include <mmreg.h>
#include "Mockup.h"

#include "AudioInput.h"
using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Text;
	using namespace System::IO;
	using namespace System::Data;
	using namespace System::Reflection;

	using namespace System::Windows::Forms;




/*
typedef struct measurement {
	float magnitude;
	float phase;
	float reference;
} measurementType;
*/
volatile measurementType measured[MAX_MEASUREMENTS];
volatile measurementType actualMeasurement;
volatile int measurementCount[1100];
volatile int measurementIndex[1100];
volatile int lastMeasurement;
volatile int maxMeasurement;
volatile unsigned long lastFreq;
volatile double audioRefLevel = 0.0;
int mark = 0;
bool audioStopping = false;

//static System::IO::Ports::SerialPort^ serialPort;

//float decoded[1024][2];
volatile int nextDecoded = 0;

//volatile float magSig;
//volatile float phaseSig;
//volatile float volSig;

double audio_volume_reflection;
double audio_phase_reflection;
double audio_volume_transmission;
double audio_phase_transmission;
bool audio_simulation = false;
int audio_delay;

int simMode = 0;
int	simPoint = 0;
int	simMaxPoint = 0;
int	simDuration = 3;
int	simStep = 0;
__int64 simStartF;
__int64 simStepF;
__int64 simWidth;
int simBefore;
int simAfter;
int simDirection;
long simS = 0;
int simR;
int simC;
int simSC;
int simL;
float simN;

volatile int audioPower = false;

#define DEBUGAUDIO

#ifdef DEBUGAUDIO
//short temp_audio[NUMPTS];
#endif
HWAVEIN      hWaveIn;
unsigned int selectedAudio = WAVE_MAPPER;
//int sampleRate = 44100;
int sampleRate = 192000;
int oldSampleRate = 0;

int SAMPPERMS = 2;
int IFREQ = 4000;  // Invariant : IFREQ == (sampleRate / SAMP) * integer!!!!!!
int SAMP=192;		// Audio samples per dsp, recalculated when samplerate changes, maximum sample rate

// For normal operation
int NUMPTS=2*192*10; // stereo  * 10 * SAMP, maximum value

//#define SAMPLETYPE short int
#define SAMPLETYPE  float // ' int' is a 32-bit type; I request 24-bit samples below
SAMPLETYPE waveIn[WAVEHDRBUFFER][MAXNUMPTS];
#define VALIDSAMPLEBITS 32
#define SAMPLEBITS	32

WAVEHDR      WaveInHdr[WAVEHDRBUFFER];
MMRESULT result;


MMRESULT mRes;
HMIXER  hmx;
MIXERLINE						mixerLine;
MIXERLINECONTROLS mlc = {0};
MIXERCONTROL mc = {0};
MIXERCONTROLDETAILS mcd = {0};
MIXERCONTROLDETAILS_UNSIGNED mcdu = {0};



 // Specify recording parameters
// WAVEFORMATEX pFormat;
WAVEFORMATEXTENSIBLE pFormat;

 //table size 200 > 192kHz
	 SAMPLETYPE sincos_tbl[200][2] = {
/* 5 khz 48000 s/s
  { 10533,  31029 }, { 27246,  18205 }, { 32698,  -2143 }, { 24636, -21605 },
  {  6393, -32138 }, {-14493, -29389 }, {-29389, -14493 }, {-32138,   6393 },
  {-21605,  24636 }, { -2143,  32698 }, { 18205,  27246 }, { 31029,  10533 },
  { 31029, -10533 }, { 18205, -27246 }, { -2143, -32698 }, {-21605, -24636 },
  {-32138,  -6393 }, {-29389,  14493 }, {-14493,  29389 }, {  6393,  32138 },
  { 24636,  21605 }, { 32698,   2143 }, { 27246, -18205 }, { 10533, -31029 },
  {-10533, -31029 }, {-27246, -18205 }, {-32698,   2143 }, {-24636,  21605 },
  { -6393,  32138 }, { 14493,  29389 }, { 29389,  14493 }, { 32138,  -6393 },
  { 21605, -24636 }, { 2143,  -32698 }, {-18205, -27246 }, {-31029, -10533 },
  {-31029,  10533 }, {-18205,  27246 }, {  2143,  32698 }, { 21605,  24636 },
  { 32138,   6393 }, { 29389, -14493 }, { 14493, -29389 }, { -6393, -32138 },
  {-24636, -21605 }, {-32698,  -2143 }, {-27246,  18205 }, {-10533,  31029 }
*/
/*	 // 5128 Hz in 44.1kHz
  { -30605, 11706 } ,
  { -14982, 29141 } ,
  { 8289, 31701 } ,
  { 27328, 18079 } ,
  { 32418, -4773 } ,
  { 20958, -25188 } ,
  { -1200, -32745 } ,
  { -22745, -23587 } ,
  { -32680, -2388 } ,
  { -25932, 20030 } ,
  { -5947, 32223 } ,
  { 17074, 27967 } ,
  { 31379, 9435 } ,
  { 29666, -13913 } ,
  { 12810, -30159 } ,
  { -10586, -31010 } ,
  { -28578, -16031 } ,
  { -31981, 7132 } ,
  { -19059, 26654 } ,
  { 3592, 32570 } ,
  { 24410, 21859 } ,
  { 32767, -9 } ,
  { 24397, -21873 } ,
  { 3574, -32572 } ,
  { -19074, -26643 } ,
  { -31985, -7114 } ,
  { -28569, 16047 } ,
  { -10569, 31016 } ,
  { 12827, 30152 } ,
  { 29674, 13897 } ,
  { 31374, -9453 } ,
  { 17058, -27977 } ,
  { -5966, -32219 } ,
  { -25944, -20015 } ,
  { -32678, 2407 } ,
  { -22732, 23600 } ,
  { -1181, 32746 } ,
  { 20973, 25176 } ,
  { 32420, 4754 } ,
  { 27318, -18094 } ,
  { 8271, -31706 } ,
  { -14998, -29133 } ,
  { -30611, -11688 } ,
  { -30598, 11723 } ,
*/
	 
//5khz 44100 s/s

  { 10529, 31029 } ,   { 28251, 16600 } ,  { 32231, -5904 } ,  { 20534, -25536 } ,
  { -1150, -32749 } ,  { -22276, -24034 } ,  { -32568, -3629 } ,  { -27019, 18540 } ,
  { -8330, 31691 } ,  { 14411, 29428 } ,  { 30142, 12851 } ,  { 31212, -9977 } ,
  { 17100, -27953 } ,  { -5329, -32333 } ,  { -25166, -20988 } ,  { -32764, 565 } ,
  { -24426, 21843 } ,  { -4209, 32496 } ,  { 18056, 27343 } ,  { 31538, 8891 } ,
  { 29680, -13886 } ,  { 13386, -29910 } ,  { -9420, -31386 } ,  { -27644, -17597 } ,
  { -32423, 4750 } ,  { -21433, 24787 } ,  { -18, 32767 } ,  { 21404, 24810 } ,
  { 32416, 4785 } ,  { 27660, -17568 } ,  { 9451, -31376 } ,  { -13356, -29924 } ,
  { -29667, -13918 } ,  { -31549, 8857 } ,  { -18087, 27324 } ,  { 4172, 32500 } ,
  { 24401, 21869 } ,  { 32761, 600 } ,  { 25187, -20961 } ,  { 5361, -32327 } ,
  { -17072, -27971 } ,  { -31203, -10010 } ,  { -30157, 12819 } ,  { -14444, 29412 } ,
  { 8294, 31700 } ,  { 26997, 18569 } ,  { 32569, -3595 } ,  { 22300, -24010 }

/*
  // 5khz 44100 s/s scale by div 16
  { 658, 1939 } ,   { 1766, 1038 } ,  { 2014, -369 } ,  { 1283, -1596 } ,
  { -72, -2047 } ,  { -1392, -1502 } ,  { -2035, -227 } ,  { -1689, 1159 } ,
  { -521, 1981 } ,  { 901, 1839 } ,  { 1884, 803 } ,  { 1951, -623 } ,
  { 1069, -1747 } ,  { -333, -2021 } ,  { -1573, -1312 } ,  { -2048, 35 } ,
  { -1527, 1365 } ,  { -263, 2031 } ,  { 1129, 1709 } ,  { 1971, 556 } ,
  { 1855, -868 } ,  { 837, -1869 } ,  { -589, -1962 } ,  { -1728, -1100 } ,
  { -2026, 297 } ,  { -1339, 1549 } ,  { -1, 2048 } ,  { 1338, 1551 } ,
  { 2026, 299 } ,  { 1729, -1098 } ,  { 591, -1961 } ,  { -835, -1870 } ,
  { -1854, -870 } ,  { -1972, 554 } ,  { -1130, 1708 } ,  { 261, 2031 } ,
  { 1525, 1367 } ,  { 2048, 38 } ,  { 1574, -1310 } ,  { 335, -2020 } ,
  { -1067, -1748 } ,  { -1950, -626 } ,  { -1885, 801 } ,  { -903, 1838 } ,
  { 518, 1981 } ,  { 1687, 1161 } ,  { 2036, -225 } ,  { 1394, -1501 }
*/
};


#define PI	3.14159265358979

#include "Constants.h"

float max_rr = 0;
float min_rr = (float)1e+30;
float factor = 1;
int reference_signal_level=0;
int prev_ref_signal = 0;
long saved_ref_mag;

//#pragma STDC FP_CONTRACT ON

void dsp_process(SAMPLETYPE *capture, long length)
{
  SAMPLETYPE *p = capture;
  int len = length / 2;
  int i;
  SUMTYPE samp_s = 0;
  SUMTYPE samp_c = 0;
  SUMTYPE ref_s = 0;
  SUMTYPE ref_c = 0;
  SUMTYPE samp_dc = 0;
  SUMTYPE ref_dc = 0;
  SUMTYPE ref_mag;
  SUMTYPE samp_mag;

  for (i = 0; i < len; i++) {
    SUMTYPE smp = (*p++ );
    SUMTYPE ref = (*p++ );
    SUMTYPE s = sincos_tbl[i][0];
    SUMTYPE c = sincos_tbl[i][1];

 
	if (audioPower) {
		samp_s += smp*smp;
		samp_c += smp;
		ref_s += ref*ref;
		ref_c += ref;
		samp_dc += smp;
		ref_dc += ref;
	} else {
		samp_s += smp * s;
	    samp_c += smp * c;
		ref_s += ref * s;
		ref_c += ref * c;
		samp_dc += smp;
		ref_dc += ref;
	}

  }

  	actualMeasurement.samp_s = samp_s;
  	actualMeasurement.samp_c = samp_c;
  	actualMeasurement.ref_s = ref_s;
  	actualMeasurement.ref_c = ref_c;
 

#define REFERENCE_LEVEL_REDUCTION	1  // Reduction of reference level to avoid crosstalk, 20dB


	actualMeasurement.dcr = ((float)ref_dc) / len;
	actualMeasurement.dcs = ((float)samp_dc )/ len;
  if (audioPower) {
//	  	ref_s *= REFERENCE_LEVEL_REDUCTION*REFERENCE_LEVEL_REDUCTION;	// Compensate for reduced level
//		ref_c *= REFERENCE_LEVEL_REDUCTION;	
//	samp_mag = samp_s - abs(samp_c);

	  samp_mag = sqrt(((float)samp_s)/len - ((float)samp_c)*samp_c/len/len );
	  ref_mag = sqrt(((float)ref_s)/len - ((float)ref_c)*ref_c/len/len) / toLin(audioRefLevel);
//	  samp_mag = sqrt(((float)samp_s / len) /* - ((float)samp_c)*samp_c */);
//	  ref_mag = sqrt(((float)ref_s / len) /* - ((float)ref_c)*ref_c */) * REFERENCE_LEVEL_REDUCTION;
	  if (nextDecoded == 1)
		 saved_ref_mag = (long) ref_mag;
	  else if (nextDecoded < 4)
		 saved_ref_mag = ( 3 * saved_ref_mag + (long) ref_mag) / 4;
	  samp_mag = samp_mag; // * MAXSAMPLEVALUE / saved_ref_mag; // For now do not scale measurment to reference signal
//	  samp_phase = 0;
	  actualMeasurement.magnitude = (float) todb(samp_mag); // Scale for unknown reason
	  actualMeasurement.phase = (float)0.0;
	  // ref_mag = MAXSAMPLEVALUE*MAXSAMPLEVALUE*10; // No reference measurement during power measurement	  
	  actualMeasurement.reference =  todb(ref_mag);
  } else {

	  	ref_s /= toLin(audioRefLevel);	// Compensate for reduced level
		ref_c /= toLin(audioRefLevel);	

	  ref_mag = sqrt((ref_s*(float)ref_s)+ref_c*(float)ref_c);


#define FAST_DSP
#ifdef FAST_DSP	
	  double fast_mag, v0,v1;
	double fast_phase;
	 v0 = (samp_c*(float)ref_c + samp_s*(float)ref_s) / ((ref_s*(float)ref_s)+ref_c*(float)ref_c);
	v1 = (samp_s*(float)ref_c - samp_c*(float)ref_s) / ((ref_s*(float)ref_s)+ref_c*(float)ref_c);

	fast_mag = sqrt(v0*v0+v1*v1);
	fast_phase =360.0f / 2 / PI  * atan2(v1, v0);
	actualMeasurement.magnitude = todb(fast_mag*MAXSAMPLEVALUE);
	actualMeasurement.phase = (float)fast_phase;
#else
	double ref_phase;
    double samp_phase;

	samp_mag = sqrt((samp_s*(float)samp_s)+samp_c*(float)samp_c)  ;
	ref_phase = 360.0f / 2 / PI  * atan2((float)ref_c, (float)ref_s);
	samp_phase = 360.0f / 2 / PI  * atan2((float)samp_c, (float)samp_s);
	actualMeasurement.magnitude = todb((samp_mag / ref_mag )*MAXSAMPLEVALUE);
	actualMeasurement.phase = (float)ref_phase - (float)samp_phase;
#endif
	actualMeasurement.reference =  todb(ref_mag / 20 );
  }

  while (actualMeasurement.phase >= 180.0f) actualMeasurement.phase-= 360.0;
  while (actualMeasurement.phase <= -180.0f) actualMeasurement.phase += 360.0;
}





typedef enum audioStates { AS_NOCHANGE, AS_ARMED, AS_STARTED, AS_SIGNAL, AS_SILENCE, AS_STOPPING, AS_FINISHED };
audioStates audioState = AS_ARMED;
audioStates nextState = AS_NOCHANGE;

#define UPCOUNT 0
#define DOWNCOUNT	1
int maxPoints = 0;

void ArmAudio(int mP, System::IO::Ports::SerialPort^ port)
{
//	serialPort = port
	audioState = AS_ARMED;
	maxMeasurement = mP;
	lastMeasurement = 0;
	nextDecoded = 0;
}

void Process(int start, int len)
{
	SUMTYPE samp_s = 0;
	SUMTYPE samp_c = 0;
	SUMTYPE ref_s = 0;
	SUMTYPE ref_c = 0;
	SUMTYPE samp_dc = 0;
	SUMTYPE ref_dc = 0;
	SUMTYPE ref_mag;
	double fast_mag, v0,v1;
	double fast_phase;
	int i;

	
	for (i = 0; i < len; i++) {
		samp_s += measured[start+i].samp_s;
		samp_c += measured[start+i].samp_c;
		ref_s += measured[start+i].ref_s;
		ref_c += measured[start+i].ref_c;
	}


	v0 = (samp_c*(float)ref_c + samp_s*(float)ref_s) / ((ref_s*(float)ref_s)+ref_c*(float)ref_c);
	v1 = (samp_s*(float)ref_c - samp_c*(float)ref_s) / ((ref_s*(float)ref_s)+ref_c*(float)ref_c);

	fast_mag = sqrt(v0*v0+v1*v1);
	fast_phase =360.0f / 2 / PI  * atan2(v1, v0);

	measured[start].magnitude = todb(fast_mag*MAXSAMPLEVALUE);
	measured[start].phase = (float)fast_phase;

	ref_mag = sqrt((ref_s*(float)ref_s)+ref_c*(float)ref_c) / len;

	measured[start].reference =  todb(ref_mag/20);
}

int lastI=-1;
int lastJ=-1;
bool lastRefl = false;

bool RetreiveData(int i, int d, float& m, float& p, float& tm, float& tp, float& r, unsigned long& fr, int avSamp)
{
//	int offs;
	if ( (!audioPower && i < lastMeasurement-1 && i < maxMeasurement) ||
		 (audioPower && lastMeasurement > 0  && measurementIndex[0] + i*d < nextDecoded-20 )) {
		if (i != lastI ) {
			lastI = i;
			lastJ = 0;
		}
		if (audioPower) {
			int offs = measurementIndex[0];
			r = measured[offs + i*d + lastJ].reference;
			tm = measured[offs + i*d + lastJ].magnitude;
			tp = measured[offs + i*d + lastJ].phase;
			measured[offs + i*d + lastJ].read = i*100 + lastJ+1;
			m = 0.0;
			p = 0.0;
		} else {
#if 0
			if (lastJ == 0) {
				Process(measurementIndex[i], avSamp);
				Process(measurementIndex[i]+ (d + 2) * SAMPPERMS, avSamp);
			}
#endif
			measured[measurementIndex[i] + 0 + lastJ].freq = fr;
			if (fr < 300000000L)
				r = measured[measurementIndex[i] + 0 + lastJ].reference;
			else if (fr < 900000000L)
				r = measured[measurementIndex[i] + 0 + lastJ].reference + (float)16.0; // Harmonic mixing;
			else
				r = measured[measurementIndex[i] + 0 + lastJ].reference + (float)26.0; // Harmonic mixing

			m = measured[measurementIndex[i] + 0 + lastJ].magnitude;
			p = measured[measurementIndex[i] + 0 + lastJ].phase;
			measured[measurementIndex[i] + 0 + lastJ].read = lastJ+1;
			tm = measured[measurementIndex[i] + 0 + (d + 2) * SAMPPERMS + lastJ].magnitude;
			tp = measured[measurementIndex[i] + 0 + (d + 2) * SAMPPERMS + lastJ].phase;
			measured[measurementIndex[i] + 0 + (d + 2) * SAMPPERMS + lastJ].read = -lastJ-1;
		}
//		if (i > 4 && m > -20)
//			m = m;
		lastJ++;
		return true;
	} 
	return (false);
}

#define SIGNAL_THRESHOLD -30	
#define SILENCE_THRESHOLD	-45

void MarkFrequency(unsigned long freq)
{
	measured[mark++].freq = freq;
}

void MarkFrequency(void)
{
	mark = 0;
}

double average_step=0;
static int force_count = 0;

void StoreMeasurement()
{
	int lowmatch = 0;
	int highmatch = 0;
	float delta4 = 0;
	int found = false;
	if(lastMeasurement>maxMeasurement)
		audioState = AS_FINISHED;
	if(audioState != AS_ARMED) {
		return;
	}
	if(lastMeasurement>maxMeasurement)
		audioState = AS_FINISHED;

	if (nextDecoded ==0) {
		average_step = 0.0;
		force_count = 0;
	}
//	if (lastMeasurement >= maxMeasurement)
//		return;
	if (nextDecoded < MAX_MEASUREMENTS) {
	  	measured[nextDecoded].samp_s = actualMeasurement.samp_s;
		measured[nextDecoded].samp_c = actualMeasurement.samp_c;
  		measured[nextDecoded].ref_s = actualMeasurement.ref_s;
  		measured[nextDecoded].ref_c = actualMeasurement.ref_c;
		measured[nextDecoded].magnitude = actualMeasurement.magnitude;
		measured[nextDecoded].phase = actualMeasurement.phase;
		measured[nextDecoded].dcs = actualMeasurement.dcs;
		measured[nextDecoded].dcr = actualMeasurement.dcr;
		measured[nextDecoded].freq = lastFreq;
		measured[nextDecoded].read = 0;
		measured[nextDecoded].force = force_count;
		measured[nextDecoded].average = (int)average_step;
//		if (actualMeasurement.reference < -110)
//			actualMeasurement.reference = -100;
		if (nextDecoded > 0)
			measured[nextDecoded].delta = actualMeasurement.reference - measured[nextDecoded-1].reference;
		else
			measured[nextDecoded].delta = actualMeasurement.reference;
		measured[nextDecoded++].reference = actualMeasurement.reference;
	}
	if (audioPower) { // No measurment indirection when measuring audioPower
		if (lastMeasurement == 0 && actualMeasurement.reference < SIGNAL_THRESHOLD && lastMeasurement < 1100  ) {
			measurementIndex[lastMeasurement++] = nextDecoded;
		}
		return;
	}
//	if (lastMeasurement > maxMeasurement)
//		return; // Don't bother to store
	if (nextDecoded > 20 && (lastMeasurement == 0 || (measurementIndex[lastMeasurement-1] + 6 < nextDecoded )) ) {
#ifdef SUPERSIMPLE
		if(measured[nextDecoded-2].reference < SILENCE_THRESHOLD && measured[nextDecoded-1].reference > SILENCE_THRESHOLD) {
			measurementIndex[lastMeasurement++] = nextDecoded+1;
			found = true;
		}
#endif


#if 1 // new algorith for start of signal detection
#if 0
		float d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,ds1,ds2,ds3;
		d10 = measured[nextDecoded-10].delta;
		d9 = measured[nextDecoded-9].delta;
		d8 = measured[nextDecoded-8].delta;
		d7 = measured[nextDecoded-7].delta;
		d6 = measured[nextDecoded-6].delta;
		d5 = measured[nextDecoded-5].delta;
		d4 = measured[nextDecoded-4].delta;
		d3 = measured[nextDecoded-3].delta;
		d2 = measured[nextDecoded-2].delta;
		d1 = measured[nextDecoded-1].delta;
#define S_NOISE	3.0
#define S_STEP	5.0
#define S_DISTANCE 3
		ds1 = -d10-d9-d8-d7+d6+d5;
		ds2 = -d10-d9-d8+d7+d6+d5;
		ds3 = -d10-d9+d8+d7+d6+d5;
		if ((lastMeasurement == 0 || measurementIndex[lastMeasurement-1] <  nextDecoded - 5 - (average_step == 0.0? S_DISTANCE : average_step - 3)) && //Minimum gap with previous signal start
			(ds1>S_STEP || ds2>S_STEP || ds3>S_STEP) &&																		// Minimum off/on signal
			abs(d5)>S_NOISE && abs(d4) < S_NOISE && abs(d3)<S_NOISE && abs(d2)<S_NOISE && abs(d1)<S_NOISE) { // Stable signal
			measurementIndex[lastMeasurement++] = nextDecoded - 5; 
			found = true;
//			if (lastMeasurement > 10 && (measurementIndex[lastMeasurement-1]- measurementIndex[lastMeasurement-2]) > average_step + 2) {
//				return;
//			}

		}

		if (lastMeasurement > 10 && average_step > 0.0 && (nextDecoded - 5 > measurementIndex[lastMeasurement-1] + 5*average_step)) { // Give up
			return;
		}

#define B_NOISE	12.0
		if (average_step > 0 && lastMeasurement > 10 && (nextDecoded - 7 > measurementIndex[lastMeasurement-1] + average_step 
			&& abs(d5)>B_NOISE && abs(d4) < B_NOISE && abs(d3)<B_NOISE && abs(d2)<B_NOISE && abs(d1)<B_NOISE)
			) {
			measurementIndex[lastMeasurement++] = nextDecoded - 7; 
			found = true;
			return;
		} else { 
			if (!found && lastMeasurement > 10 && (nextDecoded - 5 > measurementIndex[lastMeasurement-1] + average_step + 3)) {
				return;
			}
		}


#else
		float d1,d2,d3,d4,d5,ds5,d6,d7,d8,d9,d10;
		float r1,r2,r3,r4,r5,r6,r7,r8,r9,r10;
		d10 = measured[nextDecoded-10].delta;
		d9 = measured[nextDecoded-9].delta;
		d8 = measured[nextDecoded-8].delta;
		d7 = measured[nextDecoded-7].delta;
		d6 = measured[nextDecoded-6].delta;
		d5 = measured[nextDecoded-5].delta;
		d4 = measured[nextDecoded-4].delta;
		d3 = measured[nextDecoded-3].delta;
		d2 = measured[nextDecoded-2].delta;
		d1 = measured[nextDecoded-1].delta;
		r10 = measured[nextDecoded-10].reference;
		r9 = measured[nextDecoded-9].reference;
		r8 = measured[nextDecoded-8].reference;
		r7 = measured[nextDecoded-7].reference;
		r6 = measured[nextDecoded-6].reference;
		r5 = measured[nextDecoded-5].reference;
		r4 = measured[nextDecoded-4].reference;
		r3 = measured[nextDecoded-3].reference;
		r2 = measured[nextDecoded-2].reference;
		r1 = measured[nextDecoded-1].reference;
		float minbefore = (r5<r6?r5:r6);
		float minafter = (r2<r3?r2:r3);
		ds5 = d5 + d4 + d3 + d2 + d1;
		if (minbefore < minafter && d4 > 10 && /* d2 > -2 && d1 > -2 && */ d4 >= d3 && d4 > d2 && d4 > d5 && d4 > d6  && d2 < 10 && d1 < 4 && r1 > -70.0 && lastMeasurement < 1100 ) {
			float index = ((-5 * d5) + (-4 * d4) + (-3 * d3) + (-2 * d2) + (-1 * d1)) / ds5;
			index = - 4;
			measurementIndex[lastMeasurement++] = nextDecoded + (int)index + 1 + (abs(d2)>2?1:0) ; // + 1 after biggest step, +2 is first reliable signal
//			if ((int)fabs(minafter - minbefore) < -10.0) // lowest acceptable reference signal delta
//				lastMeasurement--; // small peak after reference disappeared.
//			else
				found = true;
		}

		if (lastMeasurement > 10 && average_step > 0.0 && (nextDecoded - 5 > measurementIndex[lastMeasurement-1] + 5*average_step)) { // Give up
			return;
		}
#if BAD_RECOVERY
#define B_NOISE	4.0
		if (average_step > 0 && lastMeasurement > 10 && (nextDecoded - 5 > measurementIndex[lastMeasurement-1] + average_step -2 )
			&& abs(d5)>B_NOISE && abs(d4) < B_NOISE && abs(d3)<B_NOISE && abs(d2)<B_NOISE && abs(d1)<B_NOISE
			) {
			measurementIndex[lastMeasurement++] = nextDecoded - 5; 
			found = true;
			return;
		} else
		if (average_step > 0 && lastMeasurement > 10 && (nextDecoded - 7 >= measurementIndex[lastMeasurement-1] + average_step)
//			&& abs(d5)>B_NOISE 
			&& abs(d6) < B_NOISE && abs(d5) < B_NOISE && abs(d4) < B_NOISE && abs(d3)<B_NOISE // && abs(d2)<B_NOISE && abs(d1)<B_NOISE
			) {
			measurementIndex[lastMeasurement++] = nextDecoded - 7; 
			found = true;
			return;
		} else { 
			if (!found && lastMeasurement > 10 && (nextDecoded - 8 >= measurementIndex[lastMeasurement-1] + average_step)) {
				measurementIndex[lastMeasurement++] = nextDecoded - 8 - 1; 
				return;
			}
		}
#endif

#endif
#else
//		if ( measured[nextDecoded-10].reference < SIGNAL_THRESHOLD ) lowmatch++;
//		if ( measured[nextDecoded-9].reference < SIGNAL_THRESHOLD ) lowmatch++;
//		if ( measured[nextDecoded-8].reference < SIGNAL_THRESHOLD ) lowmatch++;
		if ( measured[nextDecoded-7].reference < SIGNAL_THRESHOLD ) lowmatch++;
		if ( measured[nextDecoded-6].reference < SIGNAL_THRESHOLD ) lowmatch++;
		if ( measured[nextDecoded-5].reference > SIGNAL_THRESHOLD ) highmatch++;
		if ( measured[nextDecoded-4].reference > SIGNAL_THRESHOLD ) highmatch++;
		if ( measured[nextDecoded-3].reference > SIGNAL_THRESHOLD ) highmatch++;
		if ( measured[nextDecoded-2].reference > SIGNAL_THRESHOLD ) highmatch++;
		if ( measured[nextDecoded-1].reference > SIGNAL_THRESHOLD ) highmatch++;
		if (lowmatch >= 1 && highmatch >= 2) {
			 if (measured[nextDecoded-6].reference > SIGNAL_THRESHOLD) 
				 measurementIndex[lastMeasurement++] = nextDecoded - 6;
			 else if (measured[nextDecoded-5].reference > SIGNAL_THRESHOLD) 
				 measurementIndex[lastMeasurement++] = nextDecoded - 5;
			 else if (measured[nextDecoded-4].reference > SIGNAL_THRESHOLD) 
				 measurementIndex[lastMeasurement++] = nextDecoded - 4;
			 else if (measured[nextDecoded-3].reference > SIGNAL_THRESHOLD) 
				 measurementIndex[lastMeasurement++] = nextDecoded - 3;
			 else 
				 measurementIndex[lastMeasurement++] = nextDecoded - 2;
			if (lastMeasurement > 5 &&
				(measurementIndex[lastMeasurement-1] - measurementIndex[lastMeasurement - 2]) * 2 > 
					(measurementIndex[lastMeasurement-2] - measurementIndex[lastMeasurement - 3]) * 3
				) {
					 lowmatch = lastMeasurement; // probably skipped one measurement, just for having a breakpoint location for debugging
			}
		}
		if ( measured[nextDecoded-9].reference > SIGNAL_THRESHOLD &&
			 measured[nextDecoded-8].reference > SIGNAL_THRESHOLD &&
			 measured[nextDecoded-7].reference > SIGNAL_THRESHOLD &&
			 measured[nextDecoded-6].reference > SIGNAL_THRESHOLD &&
			 measured[nextDecoded-4].reference < SIGNAL_THRESHOLD &&
			 measured[nextDecoded-3].reference < SIGNAL_THRESHOLD &&
			 measured[nextDecoded-2].reference < SIGNAL_THRESHOLD &&
			 measured[nextDecoded-1].reference < SIGNAL_THRESHOLD   ) {
			 if (measurementIndex[lastMeasurement-1] + 15 > nextDecoded - 5 )
				nextDecoded = nextDecoded; // probably skipped one measurement, just for having a breakpoint location for debugging
		}
#endif

	}
	if (found) {
		if (lastMeasurement > 2) {
			if (average_step == 0) {
				average_step = measurementIndex[lastMeasurement-1]- measurementIndex[lastMeasurement-2];
			} else
				average_step = (average_step * 3 + measurementIndex[lastMeasurement-1]- measurementIndex[lastMeasurement-2])/4;
		}
		force_count = 0;
	}
	if (force_count < 8 && lastMeasurement > 3 && average_step > 0.0 && (nextDecoded >= measurementIndex[lastMeasurement-1] + 1.7*average_step)) { // force
		measurementIndex[lastMeasurement++] = measurementIndex[lastMeasurement-1] + (int)average_step + 1;
		force_count++;
	}
}


#include <string>

void DumpMeasurement(String^ outfile)
{
	FileStream^ fs;
	StreamWriter^ sw;
	try
	{
		fs = gcnew FileStream(outfile, FileMode::Create, FileAccess::Write);
		sw = gcnew StreamWriter(fs);
		sw->WriteLine("sep=;");
		sw->WriteLine("index; freq; ref; delta; read; mag; phase; dcr; dcs ; samp_s; samp_c; ref_s;ref_c ; measurement; force; average");
		int decoded=0;
		int measure = 0;
		string s;
		while (decoded < nextDecoded) {
			sw->Write(measure.ToString("N"));
			sw->Write("; ");
			sw->Write(measured[decoded].freq.ToString("N"));
			sw->Write("; ");
			sw->Write(measured[decoded].reference.ToString("N3"));
			sw->Write("; ");
			sw->Write(measured[decoded].delta.ToString("N1"));
			sw->Write("; ");
			sw->Write(measured[decoded].read.ToString("N1"));
			sw->Write("; ");
			sw->Write(measured[decoded].magnitude.ToString("N3"));
			sw->Write("; ");
			sw->Write(measured[decoded].phase.ToString("N3"));
			sw->Write("; ");
			sw->Write(measured[decoded].dcr.ToString("N1"));
			sw->Write("; ");
			sw->Write(measured[decoded].dcs.ToString("N1"));
			sw->Write("; ");
			sw->Write(measured[decoded].samp_s.ToString("N6"));
			sw->Write("; ");
			sw->Write(measured[decoded].samp_c.ToString("N6"));
			sw->Write("; ");
			sw->Write(measured[decoded].ref_s.ToString("N6"));
			sw->Write("; ");
			sw->Write(measured[decoded].ref_c.ToString("N6"));
			decoded++;
			sw->Write("; ");
			if (measure < lastMeasurement+1) {
				if (measurementIndex[measure+1] == decoded) {
					sw->Write(measure.ToString("N0"));
					measure++;
				} else
					sw->Write(" ");
			} else
				sw->Write(" ");
			sw->Write("; ");
			if (measured[decoded].force >0)
				sw->Write(measured[decoded].force.ToString("N6"));
			else
				sw->Write(" ");
			sw->Write("; ");
			sw->Write(measured[decoded].average.ToString("N6"));
			sw->WriteLine();
		}
		sw->Flush();	
		sw->Close();	// close VNAdecodig.csv file
	}
	catch(System::IO::IOException^ pe)
	{
		MessageBox::Show(pe->Message,"Error");
	}


}



#include <complex>
using namespace std;  


#define Z0	50.0

#define INDUCTANCE	2.0*PI*freq*pow(10,simL/20.0)/1e10
#define CAPACITANCE	1/(2.0*PI*freq*pow(10,simC/20.0)/1e13)
#define SOURCECAPACITANCE	1/(2.0*PI*freq*pow(10,simSC/20.0)/1e13)
#define RESISTANCE pow(10, (simR - 50.0)/10.0)*Z0
#define NOISE (float)((0.5 - (rand() % 1000)/1000.0) * simN )


complex <double> modelLoadRefl(__int64 freq)
{
//	long f = freq;
//	double ind = INDUCTANCE;
//	double cap = CAPACITANCE;
//	res = pow(10, (res - 50.0)/20.0)*Z0;
	complex <double> Zl (0.0, -INDUCTANCE), Zr(RESISTANCE, 0.0) , Zc(0.0, + CAPACITANCE) , Zsc(0.0, + SOURCECAPACITANCE) , Zs ( Z0, 0.0);
	Zl = Zl + (Zr * Zc)/(Zr + Zc); // L series with R/C
	Zl = (Zl * Zsc)/(Zl + Zsc); // Source capacitance parallel to total load
	return( (Zl - Zs)/(Zl +Zs) );	// Reflection due to mismatch
}

complex <double> modelLoadTran(__int64 freq, double res)
{
	if (res < 50.0) {
		return ( polar(1 - (50.0-res)/50.0, 0.0) );
	}
	if (res == 50.0) {
		return ( polar(1 - (50.0-res)/50.0, 0.0) );
	}
	return( polar(1 -(res-50.0) / (res+50.0), 0.0) ); 
}

	complex <double> debugC;
complex <double> modelRefl(__int64 freq, double bef, double res, double aft)
{
	bef = pow(10,2*(bef/25.0-2.0));
	aft = pow(10,2*(aft/25.0-2.0));
	double del1 = bef/160e6, del2 = aft/160e6;
	complex <double> r = polar(1.,del1*freq*2*PI) * modelLoadRefl(freq) * polar(1.,del1*freq*2*PI) ;
	debugC = r;
	return(r);
}

complex <double> modelTran(__int64 freq, double bef, double res, double aft)
{
	bef = pow(10,2*(bef/25.0-2.0));
	aft = pow(10,2*(aft/25.0-2.0));
	double del1 = bef/10./160e7, del2 = aft/10./160e7;
	complex <double> r = polar(1.,del1*freq*2*PI) * modelLoadTran(freq,res) * polar(1.,del2*freq*2*PI) ;
	return(r);
}

VOID ProcessHeader(WAVEHDR * pHdr)
{
	MMRESULT mRes=0;
	SAMPLETYPE *audio;
	static int state = 0;
	int i;
	int remaining;
	double temp;
	float sigLevel = (float)1;
	complex <double> tran;
	complex <double> refl;
//	double v,a;
	double pi = 3.14159265359;
	if (pHdr->dwBytesRecorded == 0)
		return;

	//	TRACE("%d",pHdr->dwUser);
	if(WHDR_DONE==(WHDR_DONE &pHdr->dwFlags))
	{

		//if (pHdr->dwBytesRecorded != (SAMPLEBITS/8) * NUMPTS )
		//	MessageBox::Show("Incorrect number of audio bytes to process.", "Error");//, MB_OK | MB_ICONEXCLAMATION);

		audio = (SAMPLETYPE *)pHdr->lpData;


		remaining = NUMPTS / 2 / SAMP;
		while (remaining-- > 0) {
//#define DEBUGSIM
#ifdef DEBUGSIM
			if (simMode == 0) {
	simMode = M_SHORT;
	simPoint = 0;
	simMaxPoint = 100;
	simDuration = 3;
	simStep = 0;
	simStartF = 20000000;
	simStepF = 1000000;
	simDirection = 0;
	simS = 0;
	simBefore = 5;
	simAfter = 3;
			}
#endif
			if (simMode) {
				audio_simulation = true;
				audio = waveIn[0];

				if (simStartF > MAXCALFREQ * 3)
					sigLevel = sigLevel / 16;
				else if (simStartF > MAXCALFREQ)
					sigLevel = sigLevel / 4;

				//if (simPoint < simMaxPoint ) {

				audio_simulation = false;
				if (simMode == M_SHORT) {
					tran = modelTran(simStartF, simBefore, 1.0, simAfter);
					refl = modelRefl(simStartF, simBefore, 1.0, simAfter);
				}
				if (simMode == M_OPEN) {
					tran = modelTran(simStartF, simBefore, 20000.0, simAfter);
					refl = modelRefl(simStartF, simBefore, 20000.0, simAfter);
				}
				if (simMode == M_LOAD) {
					tran = modelTran(simStartF, simBefore, 0.01, simAfter);
					refl = modelRefl(simStartF, simBefore, 50.0, simAfter);
				}
				if (simMode == M_THROUGH) {
					tran = modelTran(simStartF, simBefore, 50.0, simAfter);
					refl = modelRefl(simStartF, simBefore, 50.0, simAfter);
				}
				if (simMode == M_ATTN) {
					tran = modelTran(simStartF, simBefore, 0.5, simAfter);
					refl = modelRefl(simStartF, simBefore, 50.0, simAfter);
				}


#define SILENCE_GAP	2
				if (simDirection) {
					temp = audio_volume_reflection;
					audio_volume_reflection = audio_volume_transmission;
					audio_volume_reflection = temp;
					temp = audio_phase_reflection;
					audio_phase_reflection = audio_phase_transmission;
					audio_phase_transmission = temp;
				}
				// simS = 0;
#ifdef DEBUGSIM
				//simS = 0;
				for (i = 0; i < SAMP; i++) {
					a = abs(refl);
					v = arg(refl);
					audio [i*2+0] = 0+(SAMPLETYPE)(MAXSAMPLEVALUE * abs(real(refl)) * cos(PI *2 * simS * IFREQ / sampleRate + v ));
					audio [i*2+1] = 0+(SAMPLETYPE)(MAXSAMPLEVALUE * 0.1 * cos(PI *2 * (simS) * IFREQ / sampleRate)); // Reference
					simS++;
					simStartF += simStepF;
				}
#else
				if (audioPower) {
					// Audio power simulation
					if (simPoint < simMaxPoint && simStep/SAMPPERMS >= simDuration ) {
						simPoint += 1;
						if (simPoint < simMaxPoint) {
							simStep = 0;
							simStartF += simStepF;
						}
					}
					for (i = 0; i < SAMP; i++) {
						double a = abs(tran);
						// v = arg(tran);
						double freq = (simPoint -1 - simMaxPoint/2) * (double) simStepF;
						if (abs(freq) > sampleRate / 2)
							a = a* 0.0000000000001;
						audio [i*2+0] = 0+(SAMPLETYPE)(MAXSAMPLEVALUE * a * (1 - (rand() % 1000)/1000.0*0.00001) * cos(PI *2 * simS * freq / sampleRate + arg(tran)));
						if (simPoint >= simMaxPoint || simPoint < 5)
							audio [i*2+1] = 0+(SAMPLETYPE)(MAXSAMPLEVALUE * 1.0 * (1 - (rand() % 1000)/1000.0*0.00001) * cos(PI *2 * (simS) * freq / sampleRate)) * toLin(audioRefLevel); // Reference
						else 
							audio [i*2+1] = (SAMPLETYPE) (0.00001 * MAXSAMPLEVALUE);
						simS++;
					}

			
				} else {
				// Regular simulation
				if (simPoint < simMaxPoint && simStep/SAMPPERMS >= SILENCE_GAP + (simDuration+2)*2 ) {
					simPoint += 1;
					if (simPoint < simMaxPoint) {
						simStep = 0;
						simStartF += simStepF;
					}
				}
#if 0			// Test IF convolution filter
				double a = abs(tran);
				// v = arg(tran);
				double freq = (simPoint -1 - simMaxPoint/2) * simStepF;
				if (abs(freq) > sampleRate / 2)
					a = a* 0.0000000000001;
#else
				double freq = IFREQ;
				double a = abs(tran);

#endif


				if (simStep/SAMPPERMS < SILENCE_GAP ) { // Initial silence
					for (i = 0; i < SAMP; i++) {
						audio [i*2+0] = NOISE + (SAMPLETYPE)(MAXSAMPLEVALUE * 0.0000000000000000000001 * sin(PI * 2 * ((simS+0))  * IFREQ / sampleRate));
						audio [i*2+1] = NOISE + (SAMPLETYPE)(MAXSAMPLEVALUE * 0.0000000000000000000001 * sin(PI * 2 * (simS) * IFREQ / sampleRate))*toLin(audioRefLevel); // Reference
						simS++;
					}
				} else if (simStep/SAMPPERMS < SILENCE_GAP + (simDuration+2) ) { // Reflection
					for (i = 0; i < SAMP; i++) {
//						a = abs(refl);
//						v = arg(refl);
						audio [i*2+0] = NOISE + (SAMPLETYPE)(MAXSAMPLEVALUE * abs(refl) * cos(PI * 2 * simS * IFREQ / sampleRate + arg(refl) ));
						audio [i*2+1] = NOISE + (SAMPLETYPE)(MAXSAMPLEVALUE * 1.0 * cos(PI * 2 * simS * IFREQ / sampleRate))*toLin(audioRefLevel); // Reference
						simS++;
					}
				} else { // Transmission
					for (i = 0; i < SAMP; i++) {
//						a = abs(tran);
//						v = arg(tran);
						audio [i*2+0] = NOISE + (SAMPLETYPE)(MAXSAMPLEVALUE * a * cos(PI *2 * simS * freq / sampleRate + arg(tran)));
						audio [i*2+1] = NOISE + (SAMPLETYPE)(MAXSAMPLEVALUE * 1.0 * cos(PI *2 * (simS) * IFREQ / sampleRate))*toLin(audioRefLevel); // Reference
						simS++;
					}
				}
				}
#endif
				//				if ((rand() % 20) < 19) // add uncertainty on timing
				simStep += 1; 
				//}
			}
			//float measured[1024][100];
			//int measurementCount[1024];
			//int lastMeasurement;

			//if (!simMode || simPoint < simMaxPoint ) {
			dsp_process(audio, SAMP*2);
			StoreMeasurement();
			audio = & (audio[SAMP*2]);
			//}
		}
		if (!audioStopping) mRes=waveInAddBuffer(hWaveIn,pHdr,sizeof(WAVEHDR));
	}
}

void CALLBACK waveInProc(HWAVEIN hwi,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)
{
	WAVEHDR *pHdr=NULL;
	switch(uMsg)
	{
	case WIM_CLOSE:
		break;

	case WIM_DATA:
		{
			ProcessHeader((WAVEHDR *)dwParam1);
		}
		break;

	case WIM_OPEN:
		break;

	default:
		break;
	}
}



int OpenAudio (void) {
	int i;
	bool again = true;
	double a;
	int wholeIF;

	if (hWaveIn != NULL) {
		audioStopping = true;
		Sleep(20);
		waveInReset(hWaveIn);
		waveInClose(hWaveIn);
		audioStopping = false;
	}
	while (again) {
#if 0
		pFormat.wFormatTag=WAVE_FORMAT_PCM; // WAVE_FORMAT_PCM;     // simple, uncompressed format
		pFormat.nChannels=2;                    //  1=mono, 2=stereo
		pFormat.nSamplesPerSec=sampleRate;      // 44100
		pFormat.nAvgBytesPerSec=sampleRate*4;   // = nSamplesPerSec * n.Channels * wBitsPerSample/8
		pFormat.nBlockAlign=4;                  // = n.Channels * wBitsPerSample/8
		pFormat.wBitsPerSample=16;              //  16 for high quality, 8 for telephone-grade
		pFormat.cbSize=0;
#else
		pFormat.Format.wFormatTag=WAVE_FORMAT_EXTENSIBLE; // WAVE_FORMAT_PCM;     // simple, uncompressed format
		pFormat.Format.nChannels=2;                    //  1=mono, 2=stereo
		pFormat.Format.nSamplesPerSec=sampleRate;      // 44100
		pFormat.Format.wBitsPerSample=SAMPLEBITS;              //  16 for high quality, 8 for telephone-grade
		pFormat.Format.nBlockAlign= (pFormat.Format.wBitsPerSample / 8)  * pFormat.Format.nChannels;                  // = n.Channels * wBitsPerSample/8
		pFormat.Format.nAvgBytesPerSec=sampleRate * pFormat.Format.nBlockAlign;   // = nSamplesPerSec * n.Channels * wBitsPerSample/8
		pFormat.Format.cbSize=22;
		pFormat.Samples.wValidBitsPerSample = VALIDSAMPLEBITS;
		pFormat.dwChannelMask = SPEAKER_FRONT_LEFT || SPEAKER_FRONT_RIGHT;
		pFormat.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT; KSDATAFORMAT_SUBTYPE_PCM;
#endif
		WAVEINCAPS stWIC={0};
		MMRESULT mRes;

		ZeroMemory(&stWIC,sizeof(WAVEINCAPS));
		mRes=waveInGetDevCaps(selectedAudio,&stWIC,sizeof(WAVEINCAPS));


		result = waveInOpen(&hWaveIn, selectedAudio,(LPCWAVEFORMATEX) &pFormat, (DWORD_PTR)waveInProc,0L,CALLBACK_FUNCTION);
		//            0L, 0L, WAVE_FORMAT_DIRECT);

		if (result != 0) {
			if (sampleRate == 192000)
				sampleRate = 96000;
			else if (sampleRate == 96000)
				sampleRate = 48000;
			else if (sampleRate == 48000)
				sampleRate = 44100;
			else if (sampleRate == 44100)
				sampleRate = 24000;
			else if (sampleRate == 24000)
				sampleRate = 12000;
			else
				again = false;
		} else
			again = false;
	}
	if (result)
	{
		//char fault[256];
		//waveInGetErrorText(result, (LPWSTR)&fault[0], 256);
		MessageBox::Show("Failed to open waveform input device.", "Error");//, MB_OK | MB_ICONEXCLAMATION);
		return(result);
	} else {
/*		if (sampleRate == 192000)
			MessageBox::Show("Input samplerate = 192 kHz", "Success");
		if (sampleRate == 96000)
			MessageBox::Show("Input samplerate = 96 kHz", "Success");
		if (sampleRate == 48000)
			MessageBox::Show("Input samplerate = 48 kHz", "Success");
		if (sampleRate == 44100)
			MessageBox::Show("Input samplerate = 44.1kHz", "Success");
		if (sampleRate == 24000)
			MessageBox::Show("Input samplerate = 24 kHz", "Success");
		if (sampleRate == 12000)
			MessageBox::Show("Input samplerate = 12 kHz", "Success");
*/
	}

//	SAMPPERMS = 1;
	SAMP = sampleRate / 1000 / SAMPPERMS ;		// Audio samples per dsp
	wholeIF = sampleRate / SAMP;
	IFREQ = ((int)(IFREQ / wholeIF)) * wholeIF; // Ensure one SAMP contains complete sin/cos

	NUMPTS=2*SAMP*10; // samples per buffer from audio input
	a = 0.16;

//#define WINDOW(n) ((1.0-a)/2 - 0.5 * cos((2.0*PI*n)/(SAMP-1)) + (a/2.0)*cos((4*PI*n)/(SAMP-1)))
#define WINDOW(n) 1

	for (i = 0; i < SAMP; i++) {
		sincos_tbl[i][0] = (SAMPLETYPE)(MAXSAMPLEVALUE * sin( PI * 2  * i * (IFREQ)/ sampleRate) * WINDOW(i));
		sincos_tbl[i][1] = (SAMPLETYPE)(MAXSAMPLEVALUE * cos( PI * 2  * i * (IFREQ) / sampleRate) * WINDOW(i));
	}


	for (i=0; i<WAVEHDRBUFFER ; i++) {
		// Set up and prepare header for input
		WaveInHdr[i].lpData = (LPSTR)(waveIn[i]);
		WaveInHdr[i].dwBufferLength = NUMPTS*SAMPLEBITS/8;
		WaveInHdr[i].dwBytesRecorded=0;
		WaveInHdr[i].dwUser = 0L;
		WaveInHdr[i].dwFlags = 0L;
		WaveInHdr[i].dwLoops = 0L;
		waveInPrepareHeader(hWaveIn, &WaveInHdr[i], sizeof(WAVEHDR));
		// Insert a wave input buffer
		result = waveInAddBuffer(hWaveIn, &WaveInHdr[i], sizeof(WAVEHDR));
		if (result)
		{
			//  MessageBox(Application->Handle, "Failed to read block from device",
			//                   NULL, MB_OK | MB_ICONEXCLAMATION);
			return(result);
		}
	}

	// Commence sampling input
	result = waveInStart(hWaveIn);
	if (result)
	{
		//  MessageBox(Application->Handle, "Failed to start recording",
		//                   NULL, MB_OK | MB_ICONEXCLAMATION);
		return(result);

	}
	audio_delay = -1;
//------------------------------------ get mixer data --------------------------------
	GetMixerInfo();
// ------------------------------------------------------------------------------------

	return(0);
}

void StartAudioSimulation(int mode, int numPoints, int duration, __int64 startF, __int64 stepF, int cable_before, int cable_after, int direction, int r, int c, int sc, int l, float n)
{
	simMode = mode;
	simPoint = 0;
	simMaxPoint = numPoints;
	simDuration = duration;
	simStep = 0;
	simStartF = startF;
	simStepF = stepF;
	simWidth = stepF*numPoints;
	simBefore = cable_before;
	simAfter = cable_after;
	simDirection = direction;
	simS = 0;
	simR = r;
	simC = c;
	simSC = sc;
	simL = l;
	simN = n;
}

void SetAudioPower(int power)
{
	audioPower = power;
}

void GetMixerInfo()
{
				if (selectedAudio >= 0 && selectedAudio < waveInGetNumDevs())
				{
					mRes = mixerOpen((LPHMIXER)&hmx, (UINT) hWaveIn, NULL, NULL, MIXER_OBJECTF_HWAVEIN);
					if (mRes == MMSYSERR_NOERROR)
					{
						mixerLine.cbStruct = sizeof(MIXERLINE);
						mixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
						mRes = mixerGetLineInfo((HMIXEROBJ)hmx, &mixerLine, MIXER_GETLINEINFOF_COMPONENTTYPE);
						if (mRes == MMSYSERR_NOERROR)
						{
							int numSrc = mixerLine.cConnections;
							if (numSrc)
							{
								for (int n = 0; n < numSrc; n++)
								{
									mixerLine.cbStruct = sizeof(MIXERLINE);
									mixerLine.dwSource = n;

									if (!(mRes = mixerGetLineInfo((HMIXEROBJ)hmx, &mixerLine, MIXER_GETLINEINFOF_SOURCE)))
									{
										//					if (mixerLine.dwComponentType != MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER)
										//						printf("\t#%lu: %s\n", n, mixerLine.szName);
										mixerLine.szName;

										mlc.cbStruct = sizeof(MIXERLINECONTROLS);
										mlc.dwLineID = mixerLine.dwLineID;
										mlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
										mlc.cControls = 1;
										mlc.pamxctrl = &mc;
										mlc.cbmxctrl = sizeof(MIXERCONTROL);
										mRes = mixerGetLineControls((HMIXEROBJ) hmx, &mlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);



//										mcdu.dwValue = 0; // the volume is a number between 0 and 65535
										mcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
										mcd.hwndOwner = 0;
										mcd.dwControlID = mc.dwControlID;
										mcd.paDetails = &mcdu;
										mcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
										mcd.cChannels = 2;
//										mRes = mixerGetControlDetails((HMIXEROBJ) hmx, &mcd, MIXER_SETCONTROLDETAILSF_VALUE);

//										mcdu.dwValue = 10000; // the volume is a number between 0 and 65535
//										mRes = mixerSetControlDetails((HMIXEROBJ) hmx, &mcd, MIXER_SETCONTROLDETAILSF_VALUE);



									}
								}

								//						printf("ERROR: There are no WAVE inputs to adjust!\n");
								//goto record;
							}

							mRes = mRes;


						}
					}
				}


}

System::Void MixerSetVolume(int volume) // 0 - 100
		 {
			 	mcdu.dwValue = volume*655; // the volume is a number between 0 and 65535
				mRes = mixerSetControlDetails((HMIXEROBJ) hmx, &mcd, MIXER_SETCONTROLDETAILSF_VALUE);
		 }
int MixerGetVolume() // 0 - 100
		 {
			 	//mcdu.dwValue = volume*655; // the volume is a number between 0 and 65535
				mRes = mixerGetControlDetails((HMIXEROBJ) hmx, &mcd, MIXER_SETCONTROLDETAILSF_VALUE);
				return(mcdu.dwValue / 655);
		 }
