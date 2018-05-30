#define NUMPTS 2*96 // 44100 * 10   // 10 seconds
extern short int waveIn[3][NUMPTS];   // 'short int' is a 16-bit type; I request 16-bit samples below
 
extern short int decoded[2][1024];
extern  int audio_delay;
extern volatile float gamma[4];

extern double audio_volume;
extern double audio_phase;
extern bool audio_simulation;

extern int OpenAudio (void);