#define NUMPTS 2*1024 // 44100 * 10   // 10 seconds
extern short int waveIn[3][NUMPTS];   // 'short int' is a 16-bit type; I request 16-bit samples below
 
extern short int decoded[2][1024];


extern int OpenAudio (void);