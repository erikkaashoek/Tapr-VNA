#pragma once

#include "stdafx.h"

//#using <Winmm.dll>

#include <Windows.h>
#include <MMSystem.h>
#include <iostream>

#include "AudioInput.h"

int sampleRate = 44100;
short int waveIn[3][NUMPTS];   // 'short int' is a 16-bit type; I request 16-bit samples below

short int decoded[2][1024];
int nextDecoded = 0;

int audio_delay=0;

//#define DEBUGAUDIO

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
};

long acc_samp_s;
long acc_samp_c;
long acc_ref_s;
long acc_ref_c;


float gamma[4];

#define PI	3.1415926535897932384626433832795

#include "Constants.h"

float max_rr = 0;
float min_rr = (float)1e+30;
float factor = 1;
int reference_signal_level=0;
int prev_ref_signal = 0;

void calculate_gamma(float gamma[4])
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

  prev_ref_signal = reference_signal_level;
  if (rr > max_rr) max_rr = rr;
  if (rr < min_rr) min_rr = rr;
  factor = max_rr / min_rr;
  if (factor > 100) { // Signal level found
		reference_signal_level = (int)(100.0 * rr / max_rr);
  }
  gamma[0] =  (sc * rc + ss * rs) / rr;
  gamma[1] =  (ss * rc - sc * rs) / rr;
  gamma[3] = sqrt(sr)/sqrt(rr);
//  gamma[0] * gamma[0] + gamma[1] * gamma[1];

		x = PI / 2.0 * gamma[0];
		y = PI / 2.0 * gamma[1];

		//x = sin(x);
		//y = -sin(y);

		gamma[4] = (float) (180.0/PI * atan2(y,x));
		while (gamma[4] > +180.0) gamma[4] -= 360.0;
		while (gamma[4] < -180.0) gamma[4] += 360.0;

		decoded[0][nextDecoded] = (short) (gamma[3] * 20000);
		decoded[1][nextDecoded++] = (short) ( gamma[4] * 30000.0 / 180.0);
		if (nextDecoded >= 1024) nextDecoded = 0;

		if (reference_signal_level > 40 && prev_ref_signal <= 40) {
		reference_signal_level = reference_signal_level;
		return;

	}

  return;
}

void dsp_process(short *capture, long length)
{
  short *p = capture;
  int len = length / 2;
  int i;
  long samp_s = 0;
  long samp_c = 0;
  long ref_s = 0;
  long ref_c = 0;

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
	calculate_gamma(gamma);
}


void reset_dsp_accumerator(void)
{
  acc_ref_s = 0;
  acc_ref_c = 0;
  acc_samp_s = 0;
  acc_samp_c = 0;
}




 VOID ProcessHeader(WAVEHDR * pHdr)
{
	MMRESULT mRes=0;
	short *audio;
	int i;

//	TRACE("%d",pHdr->dwUser);
	if(WHDR_DONE==(WHDR_DONE &pHdr->dwFlags))
	{

#ifdef DEBUGAUDIO
		audio = waveIn[0];
		for (i = 0; i < 48; i++) {
//			sincos_tbl[i][0] = (short)(32000 * sin(3.1416 * 2  * i * 4410 / 44100));
//			sincos_tbl[i][1] = (short)(32000 * cos(3.1416 * 2  * i * 4410 / 44100));
		}
		for (i = 0; i < 1024; i++) {
			audio [i*2+0] = (short)(32000 * sin(3.1416 * 2 * i * 8820 / 44100));
			audio [i*2+1] = (short)(16000 * sin(3.1416 * 2 * (i+2.5) * 8820 / 44100));
		}
#else
		audio = (short *)pHdr->lpData;

#endif

		i = NUMPTS / 2 / 48;
		while (i-- > 0) {
			//if (audio_delay == 1) {
				dsp_process(audio, 48);
			//}
			audio = & (audio[48*2]);
			if (audio_delay > 0) audio_delay -= 1;
		}

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
  wchar_t fault[256];
  waveInGetErrorText(result, fault, 256);
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
	return(0);
}
