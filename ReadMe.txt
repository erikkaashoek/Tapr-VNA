This project is a continuation of the TAPR VNA 3.1.
It combines SW and HW designs from various VNA projects to come with to cheap and simple design.

Main changes are (see Schematic.jpg in Doc folder):
- Using SI5351 to generate both test signal and 5kHz offset mixing signal. Both can be square wave as the mixing removes all harmonics
- No USB interface to a complex VNA modules using multiple phase/magnitude detectors but a serial interface to set the SI5351 sweep and PC audio IF (5 kHz) used to compute the phase/magnitude of reflection and transmission. 
- Added DSP processing to extract the phase/magnitude from the audio. Using 16 bit audio processing delivers over 100dB dynamic range and full 360 degrees phase detection
- Removal of all complex calibration of the phase/magnitude detectors as the audio DSP is completely lineair
- Adding a mockup test device simulating the audio signals generated during a sweep of SOLT to test the SW without connected module

An example implementation uses:
 - PC with this SW
 - Auduino module controlled by serial line steering the SI5351 to sweep
 - Resistor based SW bridge and 50 transmission sink
 - 3 SA602 + ompamps to create the reference (left audio channel), reflection and transmission (right audio channel) audio IF signals
 - two FET's to switch between reflection and transmission for right audio channel.

Todo:
- Test if 3 harmonic can be used to extend VNA till above 500 MHz. Initial test shows 30dB reduction in SNR but that still may leave above 50dB dynamic range
- Add an programmable attanuator to control the 
