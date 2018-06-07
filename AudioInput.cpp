#pragma once

#include "stdafx.h"

//#using <Winmm.dll>

#include <Windows.h>
#include <MMSystem.h>
#include <iostream>

#include "AudioInput.h"
/*
typedef struct measurement {
	float magnitude;
	float phase;
	float reference;
} measurementType;
*/
measurementType measured[1024*100];
int measurementCount[1024];
int measurementIndex[1024];
int lastMeasurement;

int sampleRate = 44100;
short int waveIn[3][NUMPTS];   // 'short int' is a 16-bit type; I request 16-bit samples below

float decoded[1024][2];
volatile int nextDecoded = 0;

volatile float magSig;
volatile float phaseSig;
volatile float volSig;

double audio_volume;
double audio_phase;
bool audio_simulation = false;
int audio_delay;

#define DEBUGAUDIO

#ifdef DEBUGAUDIO
short temp_audio[NUMPTS];
#endif
HWAVEIN      hWaveIn;
WAVEHDR      WaveInHdr[2];
MMRESULT result;

 // Specify recording parameters
 WAVEFORMATEX pFormat;


 //const 
	 short sincos_tbl[48][2] = {
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
/* 5khz 44100 s/s */
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
  };

long acc_samp_s;
long acc_samp_c;
long acc_ref_s;
long acc_ref_c;

volatile float gamma[4];


#define PI	3.14159265358979

#include "Constants.h"

#define todb(X) (20.0f * log10 ((float(X)/(float)33000)));


float max_rr = 0;
float min_rr = (float)1e+30;
float factor = 1;
int reference_signal_level=0;
int prev_ref_signal = 0;

/*
void calculate_gamma(volatile float gamma[4])
{
  float rs = (float) acc_ref_s;
  float rc = (float) acc_ref_c;
  float rr = rs * rs + rc * rc;
  double x;
  double y;
  //rr = sqrtf(rr) * 1e8;
  float ss = (float) acc_samp_s;
  float sc = (float) acc_samp_c;
  float sr = ss * ss + sc * sc;
  float rPhase =  atan2(rc,rs);
  float rMag = sqrt((float)rs*rs + rc*rc);
  float sPhase =  atan2(sc,ss);
  float sMag = sqrt((float)ss*ss + sc*sc);
  prev_ref_signal = reference_signal_level;
  if (rr > max_rr) max_rr = rr;
  if (rr < min_rr) min_rr = rr;
  factor = max_rr / min_rr;
  if (factor > 100) { // Signal level found
		reference_signal_level = (int)(100.0 * rr / max_rr);
  }
  gamma[0] = (sc * rc + ss * rs) / rr;
  gamma[1] = (ss * rc - sc * rs) / rr;
  gamma[2] = sqrt(gamma[0]*gamma[0] + gamma[1]*gamma[1]);
//  gamma[0] * gamma[0] + gamma[1] * gamma[1];

		x = PI / 2.0 * gamma[0];
		y = PI / 2.0 * gamma[1];

		//x = sin(x);
		//y = -sin(y);

		gamma[3] = (float) (180.0/PI * atan2(y,x));
		while (gamma[3] > +180.0) gamma[3] -= 360.0;
		while (gamma[3] < -180.0) gamma[3] += 360.0;

		magSig = gamma[0];
		phaseSig = gamma[3];

		decoded[0][nextDecoded] = (short) (gamma[2] * 20000);
		decoded[1][nextDecoded++] = (short) ( gamma[3] * 30000.0 / 180.0);
		if (nextDecoded >= 1024) nextDecoded = 0;

		if (reference_signal_level > 40 && prev_ref_signal <= 40) {
		reference_signal_level = reference_signal_level;
		return;

	}

  return;
}

*/

void dsp_process(short *capture, long length)
{
  short *p = capture;
  int len = length / 2;
  int i;
  long samp_s = 0;
  long samp_c = 0;
  long ref_s = 0;
  long ref_c = 0;
  double ref_mag;
  double ref_phase;
  double samp_mag;
  double samp_phase;

  for (i = 0; i < len; i++) {
    short smp = *p++; //  = *p++;
    short ref = *p++;
	
//    ref_buf[i] = ref;
//    samp_buf[i] = smp;
    short s = sincos_tbl[i][0];
    short c = sincos_tbl[i][1];
    samp_s += smp * s / 16;
    samp_c += smp * c / 16;
    ref_s += ref * s / 16;
    ref_c += ref * c / 16;
#if 0
    uint32_t sc = *(uint32_t)&sincos_tbl[i];
    samp_s = __SMLABB(sr, sc, samp_s);
    samp_c = __SMLABT(sr, sc, samp_c);
    ref_s = __SMLATB(sr, sc, ref_s);
    ref_c = __SMLATT(sr, sc, ref_c);
#endif
  }
  acc_samp_s = samp_s;
  acc_samp_c = samp_c;
  acc_ref_s = ref_s;
  acc_ref_c = ref_c;

  ref_mag = sqrt((ref_s*(float)ref_s)+ref_c*(float)ref_c);
  ref_phase = 360.0f / 2 / PI  * atan2((float)ref_c, (float)ref_s);
  
  samp_mag = sqrt((samp_s*(float)samp_s)+samp_c*(float)samp_c);
  samp_phase = 360.0f / 2 / PI  * atan2((float)samp_c, (float)samp_s);

  gamma[0] = todb((samp_mag / ref_mag)*32000);
  gamma[1] = (float)ref_phase - (float)samp_phase;
  while (gamma[1] >= 180.0f) gamma[1] -= 360.0;
  while (gamma[1] <= -180.0f) gamma[1] += 360.0;

 		magSig = gamma[0];
		phaseSig = gamma[1];
		volSig = todb(ref_mag / 32000); 
  
  //calculate_gamma(gamma);
}


void reset_dsp_accumerator(void)
{
  acc_ref_s = 0;
  acc_ref_c = 0;
  acc_samp_s = 0;
  acc_samp_c = 0;
}

typedef enum audioStates { AS_NOCHANGE, AS_ARMED, AS_STARTED, AS_SIGNAL, AS_SILENCE, AS_STOPPING, AS_FINISHED };
audioStates audioState = AS_ARMED;
audioStates nextState = AS_NOCHANGE;

#define UPCOUNT 0
#define DOWNCOUNT	1
int maxPoints = 0;

void ArmAudio(int mP)
{
	audioState = AS_ARMED;
//	maxPoints = mP;
	lastMeasurement = 0;
	nextDecoded = 0;
}

int lastI=-1;
int lastJ=-1;
bool lastRefl = false;

bool RetreiveData(int i, int d, float& m, float& p, float& tm, float& tp)
{
	if (i < lastMeasurement-1) {
		if (i != lastI ) {
			lastI = i;
			lastJ = 0;
		}
		m = measured[measurementIndex[i] + 1 + lastJ].magnitude;
		p = measured[measurementIndex[i] + 1 + lastJ].phase+180;
		tm = measured[measurementIndex[i] + 1 + d + lastJ].magnitude;
		tp = measured[measurementIndex[i] + 1 + d + lastJ].phase + 180;
//		if (i > 4 && m > -20)
//			m = m;
		lastJ++;
		return true;
	} 
	return (false);
}

#define SIGNAL_THRESHOLD -20
void StoreMeasurement()
{
	int match = 0;
	if (nextDecoded < 1024*100) {
		measured[nextDecoded].magnitude = magSig;
		measured[nextDecoded].phase = phaseSig;
		measured[nextDecoded++].reference = volSig;
	}
	if (nextDecoded > 10  && (lastMeasurement == 0 || (measurementIndex[lastMeasurement-1] < nextDecoded - 20 )) ) {
		if ( measured[nextDecoded-9].reference < SIGNAL_THRESHOLD - 10 ) match++;
		if ( measured[nextDecoded-8].reference < SIGNAL_THRESHOLD - 10 ) match++;
		if ( measured[nextDecoded-7].reference < SIGNAL_THRESHOLD - 10 ) match++;
		if ( measured[nextDecoded-6].reference < SIGNAL_THRESHOLD - 10 ) match++;
		if ( measured[nextDecoded-5].reference < SIGNAL_THRESHOLD - 10 ) match++;
		if ( measured[nextDecoded-4].reference > SIGNAL_THRESHOLD ) match++;
		if ( measured[nextDecoded-3].reference > SIGNAL_THRESHOLD ) match++;
		if ( measured[nextDecoded-2].reference > SIGNAL_THRESHOLD ) match++;
		if ( measured[nextDecoded-1].reference > SIGNAL_THRESHOLD ) match++;
		if (match > 6) {
			 if (measured[nextDecoded-6].reference > SIGNAL_THRESHOLD) measurementIndex[lastMeasurement++] = nextDecoded - 6;
			 else if (measured[nextDecoded-5].reference > SIGNAL_THRESHOLD) measurementIndex[lastMeasurement++] = nextDecoded - 5;
			 else if (measured[nextDecoded-4].reference > SIGNAL_THRESHOLD) measurementIndex[lastMeasurement++] = nextDecoded - 4;
			 else if (measured[nextDecoded-3].reference > SIGNAL_THRESHOLD) measurementIndex[lastMeasurement++] = nextDecoded - 3;
			 else measurementIndex[lastMeasurement++] = nextDecoded - 2;
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
				nextDecoded = nextDecoded;
		}
	}
}


 VOID ProcessHeader(WAVEHDR * pHdr)
{
	MMRESULT mRes=0;
	short *audio;
	static int state = 0;
	int i;

//	TRACE("%d",pHdr->dwUser);
	if(WHDR_DONE==(WHDR_DONE &pHdr->dwFlags))
	{

		audio = waveIn[0];
		
		//for (i = 0; i < 48; i++) {
//			sincos_tbl[i][0] = (short)(32000 * sin(3.1416 * 2  * i * 5000 / 44100));
//			sincos_tbl[i][1] = (short)(32000 * cos(3.1416 * 2  * i * 5000 / 44100));
		//}
		if (audio_simulation) {
			for (i = 0; i < NUMPTS/2; i++) {
				audio [i*2+0] = (short)(32000 * audio_volume * sin(PI * 2 * ((i+1) + audio_phase * 9 / 360)  * 5000 / 44100));
				audio [i*2+1] = (short)(32000 * sin(PI * 2 * (i) * 5000 / 44100));
			}
		} else
			audio = (short *)pHdr->lpData;

		i = NUMPTS / 2 / SAMP;
		while (i-- > 0) {
//float measured[1024][100];
//int measurementCount[1024];
//int lastMeasurement;

			dsp_process(audio, SAMP*2);
			StoreMeasurement();
			audio = & (audio[SAMP*2]);
		}
/*

			switch (audioState) {
			case AS_ARMED:
				if (volSig < -20)
					nextState = AS_STARTED;
				break;
			case AS_STARTED:
				if (volSig > -20) {
					nextState = AS_SIGNAL;
				}
				break;
			case AS_SIGNAL:
				if (volSig < -20) {
					if (decoded[nextDecoded][UPCOUNT] == 1) { // Single peak
						nextState = AS_SILENCE; // back to silence
						nextDecoded--;
					} else
						nextState = AS_STOPPING;
				} else {
					decoded[nextDecoded][UPCOUNT]++;
					StoreMeasurement();
				}
				break;
			case AS_STOPPING:
				if (volSig > -20) {
					decoded[nextDecoded][UPCOUNT]++;
					nextState = AS_SIGNAL;
				} else {
					nextState = AS_SILENCE;
					decoded[nextDecoded][DOWNCOUNT] = 1;
				}
			case AS_SILENCE:
				if (volSig > -20 && decoded[nextDecoded][DOWNCOUNT] > 7) {
					nextDecoded++;
					if (nextDecoded < maxPoints) {
						nextState = AS_SIGNAL;
					} else
 						nextState = AS_FINISHED;
				} else {
					decoded[nextDecoded][DOWNCOUNT]++;
					StoreMeasurement();
				}
				break;
			case AS_FINISHED:
				break;
			}
			if (nextState != AS_NOCHANGE) {
				switch (nextState) {
				case AS_STARTED:
					nextDecoded = 0;
					measurementCount[nextDecoded] = 0;
					decoded[nextDecoded][UPCOUNT] = 0;
					decoded[nextDecoded][DOWNCOUNT] = 0;
					break;
				case AS_SIGNAL:
					measurementCount[nextDecoded] = 0;
					decoded[nextDecoded][UPCOUNT] = 1;
					StoreMeasurement();
					break;
				case AS_STOPPING:
					break;
				case AS_SILENCE:
					decoded[nextDecoded][DOWNCOUNT] = 1;
					StoreMeasurement();
					break;
				case AS_FINISHED:
					break;
				}
				audioState = nextState;
			}

		}
		
		if (state == AS_SIGNAL || state == AS_STOPPING || state == AS_SILENCE)
			StoreMeasurement();
*/

//		mmioWrite(m_hOPFile,pHdr->lpData,pHdr->dwBytesRecorded);
		mRes=waveInAddBuffer(hWaveIn,pHdr,sizeof(WAVEHDR));
//		if(mRes!=0)
//			StoreError(mRes,TRUE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
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
 pFormat.wFormatTag=WAVE_FORMAT_PCM;     // simple, uncompressed format
 pFormat.nChannels=2;                    //  1=mono, 2=stereo
 pFormat.nSamplesPerSec=sampleRate;      // 44100
 pFormat.nAvgBytesPerSec=sampleRate*4;   // = nSamplesPerSec * n.Channels * wBitsPerSample/8
 pFormat.nBlockAlign=4;                  // = n.Channels * wBitsPerSample/8
 pFormat.wBitsPerSample=16;              //  16 for high quality, 8 for telephone-grade
 pFormat.cbSize=0;
 /*
 	WAVEINCAPS stWIC={0};
	MMRESULT mRes;

		ZeroMemory(&stWIC,sizeof(WAVEINCAPS));
		mRes=waveInGetDevCaps(nSel,&stWIC,sizeof(WAVEINCAPS));
*/

 result = waveInOpen(&hWaveIn, WAVE_MAPPER,&pFormat,
			 (DWORD_PTR)waveInProc,0L,CALLBACK_FUNCTION);
//            0L, 0L, WAVE_FORMAT_DIRECT);
 if (result)
 {
  // wchar_t fault[256];
  //waveInGetErrorText(result, fault, 256);
  //Application->MessageBox(fault, "Failed to open waveform input device.",
  //            MB_OK | MB_ICONEXCLAMATION);
  return(result);
 }

 for (i=0; i<2 ; i++) {
 // Set up and prepare header for input
 WaveInHdr[i].lpData = (LPSTR)(waveIn[i]);
 WaveInHdr[i].dwBufferLength = NUMPTS*2;
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
	return(0);
}
