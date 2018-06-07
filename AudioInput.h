 
#define SAMP	44		// Audio samples per dsp
#define NUMPTS 2*SAMP*10 // stereo  * 10 * SAMP
extern short int waveIn[3][NUMPTS];   // 'short int' is a 16-bit type; I request 16-bit samples below

extern float decoded[1024][2];
extern  int audio_delay;
extern volatile float gamma[4];

extern double audio_volume;
extern double audio_phase;
extern bool audio_simulation;

extern int OpenAudio (void);
extern void ArmAudio(int pM);
extern bool RetreiveData(int i, int duration, float& m, float& p, float& tm, float& tp);


typedef struct measurement {
	float magnitude;
	float phase;
	float reference;
} measurementType;

extern measurementType measured[1024*100];
extern int measurementIndex[1024];
extern int lastMeasurement;
