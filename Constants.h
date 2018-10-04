//
//    Copyright 2004-2010, Thomas C. McDermott, N5EG
//    This file is part of VNAR - the Vector Network Analyzer program.
//
//    VNAR is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    VNAR is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with VNAR, if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
#pragma	once

//	Constants.h
//
//	07-25-04	TCM	Original
//	05-10-05	TCM Delete VNA_XTAL_CONST and refereces to it. This is now
//				handled by an Internal Crystal Calibration function.
//  03-19-06    TCM Add constants for the S21 measurement level offsets for
//				Mid and Lo levels.
//  02-27-07    TCM Add constants for ExpectedValue mode, redefine glitch
//				constants.
//  05-26-07    TCM Add constants for epsilon (allowed error before applying median
//				filter).
//  06-08-2007	TCM Add constants for phase lead caused by 2 highpass poles in the
//				modamp AC coupling circuit.
//  09-30-2007  TCM Add TargetPhLoMAG constant - to measure S21 phase with reduced generator
//				output level. This avoids rx preamp overload of the phase detector on
//				'hot' rx signals.
//
//	Constants for the VNA instrument + others.
//

#include <math.h>

///\def Converts radians to degrees
#define RAD2DEGR 180.0/Math::PI
///\def Converts degrees to radians
#define DEGR2RAD Math::PI/180.0

//	Constants related to the actual numeric VNA instrument readings
//  These are used until the Detector Calibration is run or loaded from a file.
//#define MAXREFLMAGNITUDE 2100			// temporary until Detector Calibration is run,
										// observed on S/N 1
#define MAXTRANMAGNITUDE 3800			// temporary until Detector Calibration is run,
										// observed on S/N 1

// Constants related to calibration grid (both instrument calibration and fixture calibration)
///\def Maximum Calibration Frequency
#define MAXCALFREQ 249000000
///\def Minimum Calibration Frequency
#define MINCALFREQ   1000000
///\def Number of Calibration Points
#define NUMCALPTS 1024.0
///\def Size of Phase Calibration Grid
#define PHASECALGRIDSIZE 1024
///\def Error size for double-precision rank-order median filter
#define EPSILON 0.005

// def Maximum DDS output level DAC control word
//#define MAX_DDS_LEVEL	0xFFF				// Multiplier for DDS Amplitude, unsigned 12-bit int. Maximum = 0xFFF

// Set the 0 db level to 2.5 dB below the maximum possible level
//#define MAX_DDS_LEVEL	0xC00				// Multiplier for DDS Amplitude, unsigned 12-bit int. Maximum = 0xFFF

///\ DDS Maximum Level Control
//	 Set the ~0 db level to 3 dB below the maximum possible level
#define MAX_DDS_LEVEL	0xB4F				// Multiplier for DDS Amplitude, unsigned 12-bit int. Maximum = 0xFFF

//  this constant is not dependent on MAX_DDS_LEVEL since it references the Q-DAC
#define QDAC_ZERODBM	0xAAA				//0xAAA		// 0 dbm for Q-DAC ref level
											//0xFFF		// about +3.5 dB too hot for Q-DAC

// constants used by Time Domain display
#define	TDRTIMETICK		4.167				// 4.167 nanoseconds per TDR time tick
#define METERSPERNSEC	0.2998				// meters per nanosecond - free space
#define FEETPERNSEC		0.9836				// feet per nanosecond - free space


// constants that set the Mid and Lo levels for the target to measure S21 magnitude
#define TARGETMIDMAG	17
#define TARGETLOMAG		34

// constant - sets Lo output level for the target to measure S21 phase with non-overloading signals
#define TARGETPHLOMAG	20

// constant: sample-to-sample deviation that constitutes a glitch (bad reading)
#define GLITCHSIZE		30

// Calling ExpectedValue with mode==true tests for phase detector saturation
#define	PHASEMODE		true
#define AMPLMODE		false
// Should normally only define one of either FASTGLITCH or SLOWGLITCH as true
//#define	FASTGLITCH							// use FAST glitch correction algorithm
//#define	SLOWGLITCH							// use SLOW glitch correction algorithm

/// Array index enumeration for ReflectionMeasurements (in InstrumentCal and DisplayRoutines)
enum ReflMeas { MagQ, PhaseI, PhaseQ };		///< Array index for storing reflection measurements

#define HIGHPASSPOLE (0)		// Phase lead caused by 2 highpass
											// poles of AC coupling capacitors in the ModAmp.
											// 6.2 degrees at 200 Khz.

// #define SHORT_RANGE 3800
#define SHORT_RANGE 30000

#define MAX_PHASE	181.0
#define MIN_PHASE   -181.0
#define NormalizePhase(X) { while ((X) > 180.0) (X) -= 360.; while ((X) < -180.0) (X) += 360.0; }
#define PHASE2SHORT(X) (short)(((X) - MIN_PHASE) * SHORT_RANGE / (MAX_PHASE - MIN_PHASE))
#define SHORT2PHASE(X) (((double)(X) * (MAX_PHASE - MIN_PHASE) / SHORT_RANGE ) + MIN_PHASE )

#define MAX_DB	+1.0
#define MIN_DB   -101.0
#define DB2SHORT(X) (short)(((X) - MIN_DB) * SHORT_RANGE / (MAX_DB - MIN_DB))
#define SHORT2DB(X) (((double)(X) * (MAX_DB - MIN_DB) / SHORT_RANGE ) + MIN_DB )

#define CALSUM	7 //number of measurements to average for calibration


//#define IFFREQ	5128

