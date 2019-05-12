 
#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H
#define WAVEHDRBUFFER 4

#define MAXNUMPTS	2*192*10 // 192kHz
extern int SAMPPERMS;
// For debugging
//#define NUMPTS 2*SAMP // stereo  * 10 * SAMP

//extern float decoded[1024][2];
extern  int audio_delay;
//extern volatile float gamma[4];

extern int sampleRate;
extern int IFREQ;
extern int audioRefLevel;
extern double audio_volume_reflection;
extern double audio_phase_reflection;
extern double audio_volume_transmission;
extern double audio_phase_transmission;
extern bool audio_simulation;
extern unsigned int selectedAudio;
extern int OpenAudio (void);
extern void ArmAudio(int pM, System::IO::Ports::SerialPort^ port);
void MarkFrequency(unsigned long freq);
void MarkFrequency(void);
extern bool RetreiveData(int i, int duration, float& m, float& p, float& tm, float& tp, float& r, unsigned long& fr,int avSamp);
extern void StartAudioSimulation(int mode, int numPoints, int duration, __int64 startF, __int64 stepF,int cable_before, int cable_after, int direction, int r, int c, int l, float n);
extern void SetAudioPower(int power);
extern void DumpMeasurement(System::String ^);
extern void GetMixerInfo();
extern int MixerGetVolume(void);
extern void MixerSetVolume(int);


#define SUMTYPE	float

typedef struct measurementType {
	float magnitude;
	float phase;
	float reference;
	float delta;
	unsigned long freq;
	float dcr;
	float dcs;
	int read;
	SUMTYPE samp_s;
    SUMTYPE samp_c;
    SUMTYPE ref_s;
    SUMTYPE ref_c;
} measurementType;


// Max gridsize = 1024, 1040 gives some extra data slots. 100 ms measurement means more then 200 samples point 
#define MAX_MEASUREMENTS 1040*300
extern volatile measurementType measured[MAX_MEASUREMENTS];
extern volatile int measurementIndex[1100];
extern volatile int lastMeasurement;
extern volatile measurementType actualMeasurement;
extern volatile int nextDecoded;

#endif