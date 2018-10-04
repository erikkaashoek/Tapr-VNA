This project is a continuation of the TAPR VNA 3.1.
It combines SW and HW designs from various VNA projects to come with a cheap and simple design capable to measure from 1 MHz (or below) to at least 700MHz. It also can do TDR to check cables on short and open

Main changes are (see VNA current pridge.pdf in Doc folder):
- Using SI5351 (on 5$ ebay breakout module) to generate test signal and 5kHz offset mixing signal. Both can be square wave as the mixing removes all unwanted harmonics
- 3 SA612 to do the downmixing to IF at 5khz, LM324 to convert balanced outputs to single ended and buffer the switches
- No USB interface to a complex VNA modules using multiple phase/magnitude detectors but a serial interface to set the SI5351 sweep and PC audio IF (5 kHz) used to compute the phase/magnitude of reflection and transmission. 
- Added DSP on processing on PC (see audioinput.c) to extract the phase/magnitude from the audio. Using 16 bit audio processing delivers over 100dB dynamic range and full 360 degrees phase detection
- Removal of all complex calibration of the phase/magnitude detectors as the audio DSP is completely lineair
- Adding a mockup test device simulating the audio signals generated during a sweep of SOLT to test the SW without connected module

An example implementation uses (see Schematic in doc folder):
 - PC with this SW
 - Auduino module controlled by serial line steering the SI5351 to sweep and the transmission/reflection switch
 - Resistor based reflection bridge and 50 ohm transmission sink
 - 3 SA612 + ompamps to create the reference (left audio channel), reflection and transmission (right audio channel) audio IF signals
 - two FET's and two npn transistors to switch between reflection and transmission for right audio channel.
 
 The PC software builds on  Microsoft Visual C++ 2010 Express (free) and probably also on the newer free versions
 The Arduino SW is normal arduino C++ and does run on any Arduino UNO or NANO or similar


