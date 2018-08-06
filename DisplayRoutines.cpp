//
//    Copyright 2004-2010 Thomas C. McDermott, N5EG
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

#pragma once
//
//
//	Routines to manipulate display points in rectangular
//  and polar format for the Vector Network Analyzer
//
//	7-4-03  TCMcDermott
//  7-10-03 Add Frequency_Grid Class.
//  7-11-03 Add InstrumentCalDataSet.
//  7-12-03 Add routines to compute S-parameter error terms
//			  and convert measured S11 to actual S11.
//  7-13-03 Add routines to save CalDat to file, and load
//			  CalData from file.
//  7-27-03 Add routines to compute rectangular display
//			  coordinates.
//  8-17-03 Add Detector Class to support separate detector
//			  calibration.
//  9-16-03 Phase correction algorithm implemented for
//			  detector calibration.  ResolveTranPolar and
//			  ResolveReflPolar changed to use detector cal values.
//  3-28-04 Add TxLevel adjust to IDAC on the VNA board
//  4-15-04 Revise amplitude interpretation algorithm
// 11-08-04 Revise Detector amplitude calibration algorithm to account for
//			two sets of S21 readings - with and without 40 dB pad.
//			Also improve to one reading every dB of range for S21.
//			REFL detector does not have this done, but TRAN detector does,
//			so there are two different calibration algorithms. Also affects
//			MagTodB routine.  May have to zero out the LowMag routine for
//			REFL, but still take data every 1 dB like TRAN.
// 12-29-05 Add Coupler Ripple and Directivity calibration compensation
//			independent of fixture calibration. These are both controlled
//			by #define statements within this file. If they are changed, all
//			detector and fixture calibrations will need to be re-run.
// 03-20-06 Change MagTodBTran to use 3 measurements: Hi, Mid, and Lo
// 02-25-07 Improve glitch detection for long cables (mostly phase).
// 04-10-07 Add 30 msec and 100 msec delays for very narrow filter sweeps.
//			Also correct values for sweep delay (e.g. 10 msec was really about 6 msec).
// 07-17-07 Add DirectionalCoupler Class to encapsulate improved error model.
//			Disable old Coupler Ripple compensation (via #define).
// 10-04-07 Added deglitching and smoothing routines for phase calibrations.
// 10-14-07 Refactored some code, adding MeasurementSet structure.



#include "stdafx.h"
#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::IO;
using namespace System::Windows::Forms;

#include <math.h>
#include <complex>
#include "DataDisplay.h"
#include "Constants.h"
#include "DisplayRoutines.h"

#include <float.h>

// #define DIRECTCAL		// enable compensation of coupler directivity to magnitude

	// Convert Mag and Phase to X,Y screen display coordinates in polar display mode
void ToDisplayPolar(double magnitude, double phase, int polarRad, int xoffset, int yoffset, int& X, int& Y)
{

	double fx, fy, phase_radians;
	int	x, y;

	phase_radians = phase * DEGR2RAD;

	fx = magnitude * cos(phase_radians);
	fy = magnitude * sin(phase_radians);

	// Scale to the size of the chart (polar radius)

	fx = fx * static_cast<double>(polarRad);
	fy = fy * static_cast<double>(polarRad);

	// Convert to integer

	x = (int)fx;
	y = (int)fy;

	// Center on the chart

	x = x + polarRad + xoffset;
	y = polarRad + yoffset - y;

	X = x;		// return X and Y display points
	Y = y;

}



// Convert Magnitude to rectangular (X, Y) display coordinates
int ToDisplayRectMag(double magnitude, int height, float dbScaleFactor, int refLevel)
{
	/// magnitude has a normalized value.
	/// Zero dB is the top of the screen - magnitude value = 1.000
	/// -100 dB is the bottom of the screen (@ 10 db/div) - magnitude value 1e-5
	/// Convert magnitude to vertical display accounting for the scale factor in dB/division

	double dbmag;

	if(magnitude < 0.0000001)
		dbmag = 140.0;
	else
		dbmag = refLevel - 20.0 * log10(magnitude);	// 0 to 100 for zero dB to -100 dB.

	dbmag /= ((double)dbScaleFactor * 10.0);			// 0 to +1 for 10db/div

	// scale to the vertical screen display area
	int vertical = (int)(dbmag * (double)height);	// 0 to scopeDisp.height for zero dB to height for max dB

	if (vertical > height)					// crop display - probably a better way to do this
		vertical = height;
	if (vertical < 0)
		vertical = 0;

	return(vertical);						// return Y as positive number

}
// Convert Phase to rectangular (X, Y) display coordinates
int ToDisplayRectPhs(double phase, int height)
{
	// phase in degrees ranges from -180 to +180

	phase += 180.0;								///< offset the phase display so zero is centerline
	double range = phase * (double)height / 360.0; ///< top to bottom of display is 360 degrees

	return((int)range);
}
// Convert value of groupdelay to Y-display coordinate
int ToDisplayRectGD(double groupdelay, int height, int scaleFactor)
{

	// scale factor = 1  implies 100 picoseconds
	// convert to nanoseconds
	groupdelay *= 1000000000.0;

	/// Set full screen display to 10 units of the selected scale.
	// 100 picoseconds * 10 = 1 nanosecond full-scale at max resolution.
	groupdelay /= scaleFactor;

	// scale it to display region
	groupdelay *= (double)height;

	/// clip display value to screen height
	if (groupdelay > height)
		groupdelay = height;

	return((int)groupdelay);
}
// Convert Resistance portion of S11 to Y display coordinate
int ToDisplayRectR(float resistance, int scale, int height)
{
	int vertical;

	vertical = (int)((resistance * (float)height) / ((float)scale * 10.0));

	if (vertical > height)
		vertical = height;
	if (vertical < 0)
		vertical =  0;

	return(vertical);
};

// Convert Reactance portion of S11 to Y display coordinate
int ToDisplayRectjX(float reactance, int scale, int height)
{
	int vertical;

	vertical = (int)((reactance * (float)height) / ((float)scale * 10.0));

	vertical += height/2;		// offset so that middle of the screen is zero reactance

	if (vertical > height)
		vertical = height;
	if (vertical < 0)
		vertical =  0;

	return(vertical);

};

// Compensate for ModAmp buffer amplifer phase distortions
double BufferAmpPhaseComp(double phaseDegrees, int Frequency)
{
	double phase = phaseDegrees;
	phase -= HIGHPASSPOLE / (double)Frequency; // compensate for AC coupling caps

	return (phase);
}

// Construct a new frequency grid of size numPoints
FrequencyGrid::FrequencyGrid(int numPoints)
{
	if (numPoints > 1024)
		throw gcnew System::IndexOutOfRangeException("FrequencyGrid constructor: numpoints out of range");

	FrequencyIndex = gcnew array<Int32>(numPoints);
	startFreq = 200000;
	stopFreq = 120000000;
	points = numPoints;
	Build();
};
// Set start frequency of grid
void FrequencyGrid::SetStartF(int start)
	{
		startFreq = start;
		Build();
	}
// Set stop frequency of grid
void FrequencyGrid::SetStopF(int stop)
{
	stopFreq = stop;
	Build();
}
// Convert gridpoint to it's frequency
int FrequencyGrid::Frequency(int gridpoint)
{
	/// if gridpoint is out of range, clip to actual frequency range

	if (gridpoint < 0)
		return (FrequencyIndex[0]);
	if (gridpoint >= points)
		return (FrequencyIndex[points-1]);

	return(FrequencyIndex[gridpoint]);
	
}
// Derive DDS divisor value (as 64-bit integer) from Frequency
long long int FrequencyGrid::DDS(int Frequency)
{

/// Calculate the 48-bit synthesizer accumulator value from Frequency
/// The VNA crystal is ~12.000 MHz. It gets multiplied by 24 going to the DDS accumulator 
/// The clock rate is thus approximately 24 * ~12.000 MHz. = ~288.000 MHz.
/// The synthesizer accumulator value needed is:   N = Fdesired * 2^48 / Fclk

	// Division done in two steps to prevent numeric overflow.

	// Calibration routine can determine a better value for Fclk than
	// our initial guess.
	// For calibration, emit F= 100.000000 Mhz.
	// Then new guess for Fclk = 2.88 * Fmeasured
	// Ferror is the Frequency calibration error at 100 MHz.
	// It has to be scaled up to 288 MHz.

	long long int N;
	N = Frequency;
	//N = Convert::ToInt64(Frequency) * 4294967296;  // Freq * 2^32
	//N /= (288000000 + (ferror * 288) / 100 );	   // Ferror * 2.88
	//N *= 65536;								       // Freq * 2^16
	return(N);
}
/// Find nearest gridpoint to Frequency
/// \result is GridPoint Number or -1 if error
int FrequencyGrid::GridPoint(int Frequency)	
{

	if (((double)Frequency > stopFreq) || ((double)Frequency < startFreq))
		return(-1);				// Frequency is outside the grid range

	double result = ((double)Frequency - startFreq) / delta;
	int iresult = (int)(result + 0.5);

	return(iresult);
}
int FrequencyGrid::Points() { return points; }		/// get number of points in grid
int FrequencyGrid::StartF() { return startFreq; }	/// get start frequency of grid
int FrequencyGrid::StopF() { return stopFreq; }		/// get stop frequency of grid
int FrequencyGrid::Ferror() { return ferror; }		/// get Frequency error of the reference clock
void FrequencyGrid::set_Ferror(int f) { ferror = f; }	/// set Frequency error of the reference clock

// Construct the FrequencyGrid once the stop, start, and numpoints are known
void FrequencyGrid::Build(void)
{
	delta = ((double)stopFreq-(double)startFreq)/(points-1);

	// build the frequency grid
	for (int i = 0; i<points; i++)
		FrequencyIndex[i] = startFreq + (int)((double)i * delta);
}


// InstrumentCalDataSet Constructor - allocate memory for Calibration Data
//   Also holds specific fixture calibration data - if any. 
InstrumentCalDataSet::InstrumentCalDataSet(String^ StartUpDir)	
{
//	RxDet = gcnew Detector();		// construct AD8702 objects
//	RxDet->name = "REFL";

//	TxDet = gcnew Detector();
//	TxDet->name = "TRAN";

//	DirCoupler = gcnew DirectionalCoupler();		/// Holds Directional coupler error model

	EdReal = gcnew array<Double>(1024); EdImag = gcnew array<Double>(1024);
	EsReal = gcnew array<Double>(1024); EsImag = gcnew array<Double>(1024);
	EtReal = gcnew array<Double>(1024); EtImag = gcnew array<Double>(1024);
	ThReal = gcnew array<Double>(1024); ThImag = gcnew array<Double>(1024);
	S11shortReal = gcnew array<Double>(1024); S11shortImag = gcnew array<Double>(1024);
	S11openReal = gcnew array<Double>(1024); S11openImag = gcnew array<Double>(1024);
	S11termReal = gcnew array<Double>(1024); S11termImag = gcnew array<Double>(1024);
	FixtureCalLogFreqMode = false;					// Default to Linear Frequency mode for Fixture Cal
	maxCalFreq = 224000000;

// Try to read in the detector.ica file to pre-load the Detector calibration constants
// If the file does not exist, warn user to run detector cal first

	FileStream^ fs;
	BinaryReader^ br;

	String^ filename = String::Concat(StartUpDir, "\\detector.ica");
	try
	{
		// Create a filestream & binary reader

		fs = gcnew FileStream(filename, FileMode::Open, FileAccess::Read);
		br = gcnew BinaryReader(fs);
	}
	catch(System::IO::IOException^ pe)
	{
		(void) pe;
/*
		MessageBox::Show("Detector Calibration File Not Found.\n\r"
			"Be sure to run   Calibration->Detector Calibration...   first", pe->Message,
			MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
*/		if (br)
			br->Close();
		if (fs)
			fs->Close();
		return;
	}

	// Define header to match file identifying type and version

	String^ recognized = gcnew String("VNA Detector Calibration Data Set Version 2.1.1");
	String^ header;

	header = br->ReadString();		// get string header on infile
	if (String::Compare(recognized, header) != 0)
	{
		MessageBox::Show(
			"detector.ica file is incompatible type or version.\n\rExpecting Detector Version 2.1.1"
			"\n\rNote: Detector Calibration must be re-run because the file \n\rformat has been changed in software release 2.1",
			"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
		br->Close();
		fs->Close();
		return;
	}

	// read in the AD8302 Phase Detector constants, error tables, and DirCoupler tables
	try
	{
#if 0
		RxDet->Qmid = br->ReadInt32();
		RxDet->Qmag = br->ReadInt32();
		RxDet->Imid = br->ReadInt32();
		RxDet->Imag = br->ReadInt32();
		RxDet->diffQDelayNs = br->ReadDouble();
		RxDet->Iasym = br->ReadInt32();
		RxDet->Qasym = br->ReadInt32();
		for (int degree = 0; degree<360; degree++)
			RxDet->pherror[degree] = br->ReadDouble();

		TxDet->Qmid = br->ReadInt32();
		TxDet->Qmag = br->ReadInt32();
		TxDet->Imid = br->ReadInt32();
		TxDet->Imag = br->ReadInt32();
		TxDet->diffQDelayNs = br->ReadDouble();
		TxDet->Iasym = br->ReadInt32();
		TxDet->Qasym = br->ReadInt32();
		for (int degree = 0; degree<360; degree++)
			TxDet->pherror[degree] = br->ReadDouble();

		// Read in the AD8302 Amplitude Detector coefficient tables

		for (int FreqIdx=0; FreqIdx<21; FreqIdx++)
		{
			RxDet->m[FreqIdx] = br->ReadDouble();
			RxDet->b[FreqIdx] = br->ReadDouble();
			RxDet->r[FreqIdx] = br->ReadDouble();
			RxDet->flat[FreqIdx] = br->ReadDouble();
		}

		for (int FreqIdx=0; FreqIdx<21; FreqIdx++)
		{
			TxDet->m[FreqIdx] = br->ReadDouble();
			TxDet->b[FreqIdx] = br->ReadDouble();
			TxDet->r[FreqIdx] = br->ReadDouble();
			TxDet->flat[FreqIdx] = br->ReadDouble();
		}
#endif
		// Read in the Internal Crystal Frequency Error
		// Test the value to make sure it's within reasonable range.
		FreqError = br->ReadInt32();
		if (Math::Abs(FreqError) > 3000000)
			FreqError = 0;
#if 0
		// Read in the directivity calibration raw values   10-18-2005
		// Moved from RxDet to DirCoupler					05-27-2007
		for (int i=0; i<21; i++)
		{
			DirCoupler->DirIphs[i] = br->ReadInt32();
			DirCoupler->DirQphs[i] = br->ReadInt32();
			DirCoupler->DirMag[i] = br->ReadInt32();
		}

		// 09-25-2007 - read the Ripple periodic error model parameters
		for (int i=0; i<6; i++)
		{
			DirCoupler->MagRipCoeff[i]->freq = br->ReadInt32();
			DirCoupler->MagRipCoeff[i]->offset = br->ReadDouble();
			DirCoupler->MagRipCoeff[i]->mag = br->ReadDouble();
			DirCoupler->PhRipCoeff[i]->freq = br->ReadInt32();
			DirCoupler->PhRipCoeff[i]->offset = br->ReadDouble();
			DirCoupler->PhRipCoeff[i]->mag = br->ReadDouble();
		}

		// 11-17-2007 - Read in the DC Offset of the Phase and Mag Open-Short calibrations

		DirCoupler->PhaseDCOffset = br->ReadDouble();
		DirCoupler->MagDCOffset = br->ReadDouble();

		// 10-01-2007 - read the Iphase-low-ref-level values for TRAN
		TxDet->ImidLo = br->ReadInt32();
		TxDet->ImagLo = br->ReadInt32();
#endif
		//RxDet->ImidLo = 0;		// these are not meaningful for REFL
		//RxDet->ImagLo = 0;		// and should be initialized to zero by constructor

		fs->Flush();
		fs->Close();
	}
	catch(System::IO::IOException^ pe)
	{
		MessageBox::Show("Error trying to load Detector Calibration File.\n\r"
			"Delete the file:   detector.ica   and re-run Detector Calibration", pe->Message,
			MessageBoxButtons::OK, MessageBoxIcon::Error);

		return;
	}

	__finally
	{
		if (br)
			br->Close();
		if (fs)
			fs->Close();
	}

//	RxDet->phaseCalibrated = true;		// loaded a valid detector calibration
//	TxDet->phaseCalibrated = true;
//	DirCoupler->DirCalibrated = true;
//	DirCoupler->RippleCalibrated = true;
};


/// Compute Frequency of grid point for linear(f) and log(f) fixture cal points
int InstrumentCalDataSet::GetFreqFromFixtureCalGrid(long index, bool LogMode)	/// Convert Phase Calibration Grid index to Frequency.
{

	// Note: linear mode is the default, and provides backwards-compatibility with older Fixture Cal files

	if(LogMode)								// Compute fixture calibration log frequency point
		{											/// Grid is 1024 points.
			double FreqIncrement;
			FreqIncrement = Math::Pow(10,Math::Log10(maxCalFreq/MINCALFREQ)/(NUMCALPTS-1));

			if (index < PHASECALGRIDSIZE && index >= 0)
				return (Convert::ToInt32(MINCALFREQ * Math::Pow(FreqIncrement, index)));
			else
				throw gcnew System::ArgumentOutOfRangeException(
				  "GetFreqFromFixtureCalGrid: index is invalid ");	// bad index value
		}
	else									// Compute fixture calibration linear frequency point
		{											/// Grid is 1024 points.
			if (index < PHASECALGRIDSIZE && index >= 0)
				return (Convert::ToInt32(MINCALFREQ + (index * (maxCalFreq - MINCALFREQ)/(NUMCALPTS-1))));
			else
				throw gcnew System::ArgumentOutOfRangeException(
				  "GetFreqFromFixtureCalGrid: index is invalid ");	// bad index value
		}												
};


// Resolve reflected measured data set to Linear Magnitude and Phase in Degrees
void InstrumentCalDataSet::ResolveReflPolar(MeasurementSet^ dataPoint, int Frequency, double& rmagLin, double& rphsDegr,
			bool CouplerComp)
{
	double magnitudeDB, phase, magnitudeLin;
	double& rphase =  phase;
	double& rmagnitudeLin = magnitudeLin;

	// translate raw readings to phaseDegr, magLin and magDB values from RxDet

	magnitudeDB = SHORT2DB(dataPoint->ReflMQ); // RxDet->MagTodBRefl(Frequency, dataPoint->ReflMQ);
    magnitudeLin = pow(10.0, (magnitudeDB/20.0));	

	phase = SHORT2PHASE(dataPoint->ReflPQ); // RxDet->IQtoDegrees(dataPoint->ReflPI, dataPoint->ReflPQ, Frequency, 0, 0, 0);

	// Adjust the reflected Magnitude and Phase by the Directivity Cal

#ifdef DIRECTCAL
	DirCoupler->CompensateDirectivity(this, rmagnitudeLin, rphase, Frequency);
#endif
#if 0
	if(CouplerComp)
	{

		if(RxDet->phaseCalibrated)
		{
			// Compensate for buffer amp phase delays due to highpass filter formed by
			// coupling caps, and time delay of the amplifier.
			phase = BufferAmpPhaseComp(phase, Frequency);

			// Compensate for coupler phase & magnitude ripple  09-23-2007
			phase = DirCoupler->PhaseRippleCompensate(phase, Frequency);
			double magCorr = DirCoupler->MagRippleCorrection(phase, Frequency);
			magnitudeLin += magnitudeLin * magCorr;
		}
	}
#endif

	rphsDegr = NormalizePhaseDegr(phase);	// return phase in degrees (-180..+180)
	rmagLin = magnitudeLin;				// return linear magnitude (0..1)

	return;
}

// Resolve transmitted measured data set to Magnitude and Phase
void InstrumentCalDataSet::ResolveTranPolar(MeasurementSet^ dataPoint, int Frequency, double& rmag, double& rphs)
{
	double magnitudeDB, phase, magnitudeLin;


	magnitudeDB = SHORT2DB(dataPoint->TranMQLo);
	phase = SHORT2PHASE(dataPoint->TranPQ);
	
	//magnitudeDB = TxDet->MagTodBTran(Frequency, dataPoint->TranMQHi, dataPoint->TranMQLo, dataPoint->TranMQMid);

	//phase = TxDet->IQtoDegrees(dataPoint->TranPI, dataPoint->TranPQ, Frequency, magnitudeDB, dataPoint->TranPILow, dataPoint->TranPQLow);

	// Compensate phase at low frequencies 
	//phase -= HIGHPASSPOLE/Frequency;	

	// offset phase by 180 degrees for Trans measurements
	phase += 180.0;
	phase = NormalizePhaseDegr(phase);

	magnitudeLin = pow(10.0, (magnitudeDB/20.0));		// translate db to voltage ratio

	rmag = magnitudeLin;		// return linear magnitude (0..1)
	rphs = phase;			// return phase in degrees (-180..+180)
}


// Construct Cal error terms from Cal raw data at each frequency point
void CalToErrorTerms(InstrumentCalDataSet^ Cal)
{
    for (int i=0; i<1024; i++)
	{
		std::complex<double> two(2.0, 0.0);
		std::complex<double> rslt(0.0, 0.0);
		
		double realpart, imagpart;

		realpart = Cal->S11shortReal[i];
		imagpart = Cal->S11shortImag[i];
        std::complex<double>	Sshort(realpart, imagpart);

		realpart = Cal->S11openReal[i];
		imagpart = Cal->S11openImag[i];
		std::complex<double> Sopen(realpart, imagpart);

		realpart = Cal->S11termReal[i];
		imagpart = Cal->S11termImag[i];
		std::complex<double> Sterm(realpart, imagpart);

		// Directivity error term
		Cal->EdReal[i] = Cal->S11termReal[i];
		Cal->EdImag[i] = Cal->S11termImag[i];

		// Source mismatch error term
		rslt = ((two * Sterm) - (Sshort + Sopen)) / (Sshort - Sopen);
		Cal->EsReal[i] = real(rslt);
		Cal->EsImag[i] = imag(rslt);

		// Tracking error term
		rslt = ((two * (Sopen - Sterm) * (Sshort - Sterm)) / (Sshort - Sopen));
		Cal->EtReal[i] = real(rslt);
		Cal->EtImag[i] = imag(rslt);
	}

};

// Convert measured S11 into actual S11 via fixture calibration
void CorrectS11(InstrumentCalDataSet^ Cal, int Frequency, bool ReflExtn, double measmag, double measphs, double& rsltmag, double& rsltphs)
{
	// Modified 02-07-2010 to select Linear(f) or Log(f) fixture calibration 

	int i, j;
	double delta, position;
	double realpart, imagpart;

	if (Cal->FixtureCalLogFreqMode == false)					// Linear(f) interpolation
	{
		// There are 1024 fixture calibration frequencies linearly spaced. Find the two adjacent to the
		// measured frequency. Linearly interpolate (real, imag) between the two cal points.
		
		delta = (Cal->maxCalFreq - MINCALFREQ)/ (NUMCALPTS - 1.0);	// frequency separation of cal points

		i = (int)(((double)Frequency - MINCALFREQ) / delta);	// Cal frequency directly below ours
		if (i >= (int)NUMCALPTS)
			i = (int)NUMCALPTS - 1;
		j = i+1;												// Cal frequency directly above ours
		if(j >= (int)NUMCALPTS)
			j = (int)NUMCALPTS - 1;								// In case we are close to MAX cal frequnecy

		position = (((double)Frequency - MINCALFREQ) / delta) - i ;	// fractional position between i and j
	}
	else														// Log(f) interpolation
	{
		// There are 1024 fixture calibration frequencies, logrithmically spaced. Find the two adjacent to the
		// measured frequency. Linearly interpolate (real, imag) between the two cal points.

		double LogFreqIncrement = Math::Log10(Cal->maxCalFreq/MINCALFREQ)/(NUMCALPTS-1);
		double FreqIncrement = Math::Pow(10, LogFreqIncrement);

		// compute index equivalent to the frequency
		double exactIndex = Math::Log10(Frequency/MINCALFREQ)/LogFreqIncrement;
		i = (int)exactIndex;									// Cal frequency index directly below ours
		j=i+1;													// Cal frequency index directly above ours
		
		if(j >= (int)NUMCALPTS)
			j = (int)NUMCALPTS - 1;								// In case we are close to MAX cal frequency

		double Flow = MINCALFREQ * Math::Pow(FreqIncrement, i);
		double Fhigh =  MINCALFREQ * Math::Pow(FreqIncrement, j);

		if (abs(Fhigh - Flow) < 1e-6)
			position = 0.0;										// Supress interpolation at the end points
		else
			position = ((double)Frequency - Flow) / (Fhigh - Flow);

	}

	// transform measured S11 into actual S11

	std::complex<double> rslt;

	realpart = Cal->EdReal[i] + ((Cal->EdReal[j] - Cal->EdReal[i]) * position); // interpolate
	imagpart = Cal->EdImag[i] + ((Cal->EdImag[j] - Cal->EdImag[i]) * position);
	std::complex<double>	Ed(realpart, imagpart);



	realpart = Cal->EsReal[i] + ((Cal->EsReal[j] - Cal->EsReal[i]) * position); // interpolate
	imagpart = Cal->EsImag[i] + ((Cal->EsImag[j] - Cal->EsImag[i]) * position);
	std::complex<double>	Es(realpart, imagpart);

	realpart = Cal->EtReal[i] + ((Cal->EtReal[j] - Cal->EtReal[i]) * position); // interpolate
	imagpart = Cal->EtImag[i] + ((Cal->EtImag[j] - Cal->EtImag[i]) * position);
	std::complex<double>	Et(realpart, imagpart);
	
	// Convert measured data to rectangular coordinates
    double measreal, measimag;
	double phase_radians = measphs * DEGR2RAD;
	measreal = measmag * cos(phase_radians);
	measimag = measmag * sin(phase_radians);
    std::complex<double>	S11meas(measreal, measimag);

	if (ReflExtn == false)		// perform full SOLT compensation
		rslt = ((S11meas - Ed) / (Es * (S11meas - Ed) + Et));

	// Currently the limited compensation can not be enabled by the user
	// because the checkbox is not displayed. This is a remnant from some experiments.
	else				// perform limited Et compensation
	{
		// Linearize the phase component of Et by use of the equivalent cable time delay.
		// The control for selecting this option is in Form1, but is not currently enabled.
		// We should probably replace it with a better method to compensate for the
		// interconnecting cable not have a surge impedance of exactly 50+j0 ohms.

		double oldRealLin = real(Et);
		double oldImagLin = imag(Et);
		double oldAngleRadians = atan2(oldImagLin, oldRealLin);

		double linmag = sqrt(real(Et) * real(Et) + imag(Et) * imag(Et));
		double newAngleRadians = -(Cal->reflTimeDelayEquivalent * 2 * Math::PI * Frequency);	

		double realLin = linmag * cos(newAngleRadians);
		double imagLin = linmag * sin(newAngleRadians);
		std::complex<double> ELin(realLin, imagLin);

		rslt = S11meas / ELin;
	}

	// Convert results to polar coordinates
	double x = real(rslt);
	double y = imag(rslt);

	double fphase = atan2(y, x) * RAD2DEGR;
	double fmagnitude = sqrt(x*x + y*y);

	rsltmag = fmagnitude;
	rsltphs = fphase;
}
// Convert measured S21 into actual S21 via fixture calibration
void CorrectS21(InstrumentCalDataSet^ Cal, int Frequency, double measmag, double measphs, double& rsltmag, double& rsltphs)
{
	// Modified 02-07-2010 to select Linear(f) or Log(f) fixture calibration 

	int i, j;
	double delta, position;
	double realpart, imagpart;

	if (Cal->FixtureCalLogFreqMode == false)					// Linear(f) interpolation
	{
		// There are 1024 fixture calibration frequencies linearly spaced. Find the two adjacent to the
		// measured frequency. Linearly interpolate (real, imag) between the two cal points.

		delta = (Cal->maxCalFreq - MINCALFREQ)/ (NUMCALPTS - 1);		// frequency separation of cal points

		i = (int)(((double)Frequency - MINCALFREQ) / delta);	// Cal frequency directly below ours
		if (i >= (int)NUMCALPTS)
			i = (int)NUMCALPTS - 1;
		j = i+1;												// Cal frequency directly above ours
		if(j >= (int)NUMCALPTS)
			j = (int)NUMCALPTS - 1;								// In case we are close to MAX cal frequnecy

		position = (((double)Frequency - MINCALFREQ) / delta) - i ;	// fractional position between i and j
	}
	else														// Log(f) interpolation
	{
		// There are 1024 fixture calibration frequencies, logrithmically spaced. Find the two adjacent to the
		// measured frequency. Linearly interpolate (real, imag) between the two cal points.

		double LogFreqIncrement = Math::Log10(Cal->maxCalFreq/MINCALFREQ)/(NUMCALPTS-1);
		double FreqIncrement = Math::Pow(10, LogFreqIncrement);

		// compute index equivalent to the frequency
		double exactIndex = Math::Log10(Frequency/MINCALFREQ)/LogFreqIncrement;
		i = (int)exactIndex;									// Cal frequency index directly below ours
		j=i+1;													// Cal frequency index directly above ours
		
		if(j >= (int)NUMCALPTS)
			j = (int)NUMCALPTS - 1;								// In case we are close to MAX cal frequency

		double Flow = MINCALFREQ * Math::Pow(FreqIncrement, i);
		double Fhigh =  MINCALFREQ * Math::Pow(FreqIncrement, j);

		if (abs(Fhigh - Flow) < 1e-6)
			position = 0.0;										// supress interpolation at the end points
		else
			position = ((double)Frequency - Flow) / (Fhigh - Flow);
	}

	realpart = Cal->ThReal[i] + ((Cal->ThReal[j] - Cal->ThReal[i]) * position);		// interpolate between calibration points
	imagpart = Cal->ThImag[i] + ((Cal->ThImag[j] - Cal->ThImag[i]) * position);

	// transform measured S21 into actual S21

	std::complex<double> rslt;
    std::complex<double>	Th(realpart, imagpart);

	// Convert measured data to rectangular coordinates
    double measreal, measimag;
	double phase_radians = measphs * DEGR2RAD;
	measreal = measmag * cos(phase_radians);
	measimag = measmag * sin(phase_radians);
    std::complex<double>	S21meas(measreal, measimag);

	rslt = S21meas / Th;

	// Convert results to polar coordinates
	double x = real(rslt);
	double y = imag(rslt);

	double fphase = atan2(y, x) * RAD2DEGR;
	double fmagnitude = sqrt(x*x + y*y);

	rsltmag = fmagnitude;
	rsltphs = fphase;
}
	// Load Fixture Calibration Data Set. Modified 02-07-2010 to recognize LOG(f) type Fixture Cal file
bool LoadCalDataSet(OpenFileDialog^ infile, InstrumentCalDataSet^ Cal)
{
	FileStream^ fs;
	BinaryReader^ br;

	try
	{
		// Create a filestream & binary reader

		fs = gcnew FileStream(infile->FileName, FileMode::Open, FileAccess::Read);
		br = gcnew BinaryReader(fs);

			// Define header to match file identifying type and version

		String^ recognizedLinear = gcnew String("VNA Calibration Data Set Version 1.0.0");
		String^ recognizedLog = gcnew String("VNA Calibration Data Set Version 1.0.0.LOG");

		String^ header;
		header = br->ReadString();		// get string header on infile


		if (String::Compare(recognizedLinear, header) == 0)
			Cal->FixtureCalLogFreqMode = false;	// linear(f) cal file
		else if (String::Compare(recognizedLog, header) == 0)
			Cal->FixtureCalLogFreqMode = true;	// log(f) mode cal file
		else									// not a valide cal file
		{
			MessageBox::Show(
				"InstrumentCalDataSet file is incompatible type or version.\n\rExpecting Version 1.0.0",
				"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			br->Close();
			fs->Close();
			return false;
		}

		for (int i=0; i<1024; i++)		// read in the data set
		{
			Cal->EdReal[i] = br->ReadDouble();
			Cal->EdImag[i] = br->ReadDouble();
			Cal->EsReal[i] = br->ReadDouble();
			Cal->EsImag[i] = br->ReadDouble();
			Cal->EtReal[i] = br->ReadDouble();
			Cal->EtImag[i] = br->ReadDouble();
			Cal->ThReal[i] = br->ReadDouble();
			Cal->ThImag[i] = br->ReadDouble();
			// debug - load in the generating dataset
			Cal->S11openReal[i] = br->ReadDouble();
			Cal->S11openImag[i] = br->ReadDouble();
			Cal->S11shortReal[i] = br->ReadDouble();
			Cal->S11shortImag[i] = br->ReadDouble();
			Cal->S11termReal[i] = br->ReadDouble();
			Cal->S11termImag[i] = br->ReadDouble();
		}

		try 
		{
			Cal->maxCalFreq = (long) br->ReadDouble();
		}
		catch( Exception^ /* e */ )	// Don't bother warning the user ...
		{											// They probably don't care anyway
//					MessageBox::Show("Can not open stored serial port.", serialPort1->PortName,
//									 MessageBoxButtons::OK, MessageBoxIcon::Information);
		}

		// Compute equivalent time delay of the cal fixture (as though it were linear)
		// This is used for Reference Plane Extension compensation mode.
		// The phase component of Et is ~ the phase of the fixture.

		double sumDeltaPhase = 0.0;
		for (int i=20; i<1000; i++)
		{
			double phase1 = RAD2DEGR * atan2(Cal->EtReal[i], Cal->EtImag[i]);
			double phase2 = RAD2DEGR * atan2(Cal->EtReal[i+1], Cal->EtImag[i+1]);

			sumDeltaPhase += NormalizePhaseDegr(phase1 - phase2);
		}

		// time delay is the total running deltaphase over the frequency range.

		int freq2 = Cal->GetFreqFromFixtureCalGrid(1000, false);
		int freq1 = Cal->GetFreqFromFixtureCalGrid(20,  false);
		int freqd = freq2-freq1;
		Cal->reflTimeDelayEquivalent = -sumDeltaPhase / (360.0 * (double)freqd);

		return true;
	}
	catch(System::IO::FileNotFoundException^ pe)
	{
		MessageBox::Show(String::Concat(pe->Message,
			"\n\n\rError trying to read Cal Data Set. The file cannot be found.\n\r"
			"Load a valid Fixture Calibration file in order to enable Fixture Calibration."),
			"Fixture Cal File Not Found", MessageBoxButtons::OK, MessageBoxIcon::Warning);
		return false;
	}
	catch(System::IO::IOException^ pe)
	{
		MessageBox::Show(String::Concat(pe->Message,
			"\n\n\rError trying to read Cal Data Set:\n\r", infile->FileName,
			"\n\n\rEither the file is corrupted or the file version is incompatible.\n\r"
			"Try creating a new Fixture Cal file, or Load a valid Fixture Calibration file\n\r"
			"in order to enable Fixture Calibration."),
			"Fixture Cal Data Error", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
		return false;
	}
	__finally
	{
		if (br)
            br->Close();
        if (fs)
			fs->Close();
	}
}

	// Save Fixture Calibration Data Set - Modified 02-07-2010 to append Lin/Log mode to file header
void SaveCalDataSet(SaveFileDialog^ outfile, InstrumentCalDataSet^ Cal)
{
	FileStream^ fs;
	BinaryWriter^ bw;

	try
	{
		// Create a filestream & binary writer

		fs = gcnew FileStream(outfile->FileName, FileMode::Create, FileAccess::Write);
		bw = gcnew BinaryWriter(fs);

		// Define header to match file identifying type and version

		String^ recognized;

		if (Cal->FixtureCalLogFreqMode)	//Log mode - use new identifier string
			recognized = gcnew String("VNA Calibration Data Set Version 1.0.0.LOG");
		else							// Linear mode - use backwards compatible identifier string
			recognized = gcnew String("VNA Calibration Data Set Version 1.0.0");

		bw->Write(recognized);		// put string header on outfile

		for (int i=0; i<1024; i++)		// write out the data set
		{
			bw->Write(Cal->EdReal[i]);
			bw->Write(Cal->EdImag[i]);
			bw->Write(Cal->EsReal[i]);
			bw->Write(Cal->EsImag[i]);
			bw->Write(Cal->EtReal[i]);
			bw->Write(Cal->EtImag[i]);
			bw->Write(Cal->ThReal[i]);
			bw->Write(Cal->ThImag[i]);

			// debug - also write out the measurements
			bw->Write(Cal->S11openReal[i]);
			bw->Write(Cal->S11openImag[i]);
			bw->Write(Cal->S11shortReal[i]);
			bw->Write(Cal->S11shortImag[i]);
			bw->Write(Cal->S11termReal[i]);
			bw->Write(Cal->S11termImag[i]);
		}
		bw->Write((double)(Cal->maxCalFreq));
    }
	catch(System::IO::IOException^ pe)
	{
		MessageBox::Show("Error", pe->Message);
	}

	__finally
	{
		if (fs)
            fs->Flush();
		if (bw)
			bw->Close();
		if (fs)
			fs->Close();
	}

}




// Convert Log Tx Level to linear value needed by DAC register
unsigned short TxLevLinear(int tranLevel)
{
	// 0 dbm is the max level. Corresponds to 4095 decimal for IDAC control register.
	// 0 dbm -> 4095     -20 dbm -> 410    -40 dbm -> 41

	unsigned short linlevel = (unsigned short)((double)MAX_DDS_LEVEL * pow(10.0,(((double)tranLevel)/20.0)) + 0.5);
	return (linlevel);
};
// Convert linear DAC level to dBm.  4095 = 0 dBm.
float dBFromLinear(int DAClevel)
{
	float dB = 20.0f * log10 ((float(DAClevel)/(float)MAX_DDS_LEVEL));
	return (dB);
};


// Convert Return Loss (S11 mag) to SWR, then to Y-coordinate.
int ToDisplayAsSWR(double mag, int height, int scaledB)
{

	/// Derive vertical plot value using dB scale factor as SWR range.
	/// scaledB has value 10, 5, 2 or 1 dB/division.
	/// Interpret it as 10 SWR units full scale - i.e. 1 to 11
	/// or as 5 units (1 to 6), 2 units (1..3), or as 1 unit (1..2)

	if (mag > 1.0)		// clip display if S11 mag is greater than unity
		mag = 1.0;		// indicates something wrong in compensation

	int vertical;
	double swr = ( 1.0 + mag ) / ( 1.0 - mag);

	vertical = (int)(((swr - 1.0) / (double)scaledB) * (double)height);

	vertical = height - vertical;			// flip display -- SWR = 1 is at bottom line of display

	if (vertical > height)					// crop display - probably a better way to do this
		vertical = height;
	if (vertical < 0)
		vertical = 0;

	return(vertical);						// return Y as positive number

}
	// Export S-parameters to file.
void ExportSParams(int mode,  FrequencyGrid^ FG, 
				array<Double>^ S11Mag,  array<Double>^ S11Phs,
				array<Double>^ S21Mag,  array<Double>^ S21Phs,
				array<Double>^ S22Mag,  array<Double>^ S22Phs,
				array<Double>^ S12Mag,  array<Double>^ S12Phs)
{
	/// Uses Mag/Phase Forward and Reverse held in internal storage variables.
	/// mode: 0 -> S2P rectangular mode, 1 -> S2P polar mode, 2-> CSV rectangular mode

	double FwdTranMagdB, FwdTranPhsDeg, FwdReflMagdB, FwdReflPhsDeg;
	double RevTranMagdB, RevTranPhsDeg, RevReflMagdB, RevReflPhsDeg;
	double FwdTranRe, FwdTranIm, FwdReflRe, FwdReflIm;
	double RevTranRe, RevTranIm, RevReflRe, RevReflIm;

	FileStream^ fs;
	StreamWriter^ sw;

	SaveFileDialog^ outfile = gcnew SaveFileDialog();
	if(mode<2)	// S2P format
		outfile->Filter = "s-parameter 2-port files (*.s2p)|*.s2p";
	else		// CSV format
		outfile->Filter = "CSV format files (*.csv)|*.csv";

	outfile->AddExtension = true;
	if (outfile->ShowDialog() != DialogResult::OK)
		return;

	Globalization::CultureInfo^ NAFmt = gcnew Globalization::CultureInfo("en-US");
	Globalization::NumberFormatInfo^ NAnfi = NAFmt->NumberFormat;

	Globalization::CultureInfo^ EurFmt = gcnew Globalization::CultureInfo("de-DE");
	Globalization::NumberFormatInfo^ Eurnfi = EurFmt->NumberFormat;

	try
	{
		// Create a filestream & stream writer

		fs = gcnew FileStream(outfile->FileName, FileMode::Create, FileAccess::Write);
		sw = gcnew StreamWriter(fs);

			// Write Header for S-parameter file

			sw->WriteLine("! Vector Network Analyzer VNA R2");		// put string header on outfile
			sw->WriteLine("! Tucson Amateur Packet Radio");		// put string header on outfile
			DateTime today = DateTime::Now;
			sw->Write("! ");
			sw->Write(today.ToLongDateString());
			sw->Write("   ");
			sw->WriteLine(today.ToLongTimeString());

		// Header line defines type of file
		if (mode==1)	// S2P polar
		{
			sw->WriteLine("! Frequency         S11                 S21                  S12                  S22");
			sw->WriteLine("# HZ  S  DB R 50");	// polar is in DB amplitude and degrees WRT 50 ohms
		}
		if (mode==0)	// S2P rectangular
		{
			sw->WriteLine("! Frequency              S11                           S21                            S12                            S22");
			sw->WriteLine("# HZ  S  RI R 50");	// rectangular is in Real and Imaginary WRT 50 ohms
		}
		if (mode==2)	// CSV North America rectangular
		{
			sw->WriteLine("50+j0,\"=COMPLEX(50,0)\",,Unity,\"=COMPLEX(1,0)\"");
			sw->WriteLine(",");
			sw->WriteLine("Frequency (Hz.),S11,S21,S12,S22,,Zin,Zout");
		}
		if (mode==3)	// CSV Europe rectangular
		{
			sw->WriteLine("50+j0;\"=COMPLEX(50;0)\";;Unity;\"=COMPLEX(1;0)\"");
			sw->WriteLine(";");
			sw->WriteLine("Frequency (Hz.);S11;S21;S12;S22;;Zin;Zout");
		}

		// write out the parameters

		if (mode==1)											// S2P polar format
			for(int i=0; i<FG->points; i++)
			{
				FwdReflMagdB = 20.0 * log10(S11Mag[i]);
				FwdReflPhsDeg = S11Phs[i];
				FwdTranMagdB = 20.0 * log10(S21Mag[i]);
				FwdTranPhsDeg = S21Phs[i];
				RevTranMagdB = 20.0 * log10(S12Mag[i]);
				RevTranPhsDeg = S12Phs[i];
				RevReflMagdB = 20.0 * log10(S22Mag[i]);
				RevReflPhsDeg = S22Phs[i];

				sw->Write(FG->Frequency(i).ToString("D9"));			sw->Write("    ");
				sw->Write(FwdReflMagdB.ToString("F3"));			sw->Write("  ");
				sw->Write(FwdReflPhsDeg.ToString("F3"));			sw->Write("    ");
				sw->Write(FwdTranMagdB.ToString("F3"));			sw->Write("  ");
				sw->Write(FwdTranPhsDeg.ToString("F3"));			sw->Write("    ");
				sw->Write(RevTranMagdB.ToString("F3"));			sw->Write("  ");
				sw->Write(RevTranPhsDeg.ToString("F3"));			sw->Write("    ");
				sw->Write(RevReflMagdB.ToString("F3"));			sw->Write("  ");
				sw->WriteLine(RevReflPhsDeg.ToString("F3"));
			}

		if (mode==0)												// S2P rectangular format
			for(int i=0; i<FG->points; i++)
			{
				FwdReflRe = S11Mag[i] * cos(S11Phs[i] * DEGR2RAD);
				FwdReflIm = S11Mag[i] * sin(S11Phs[i] * DEGR2RAD);
				FwdTranRe = S21Mag[i] * cos(S21Phs[i] * DEGR2RAD);
				FwdTranIm = S21Mag[i] * sin(S21Phs[i] * DEGR2RAD);
				RevTranRe = S12Mag[i] * cos(S12Phs[i] * DEGR2RAD);
				RevTranIm = S12Mag[i] * sin(S12Phs[i] * DEGR2RAD);
				RevReflRe = S22Mag[i] * cos(S22Phs[i] * DEGR2RAD);
				RevReflIm = S22Mag[i] * sin(S22Phs[i] * DEGR2RAD);

				sw->Write(FG->Frequency(i).ToString("D9"));		sw->Write("    ");
				sw->Write(FwdReflRe.ToString("E5"));				sw->Write("  ");
				sw->Write(FwdReflIm.ToString("E5"));				sw->Write("    ");
				sw->Write(FwdTranRe.ToString("E5"));				sw->Write("  ");
				sw->Write(FwdTranIm.ToString("E5"));				sw->Write("    ");
				sw->Write(RevTranRe.ToString("E5"));				sw->Write("  ");
				sw->Write(RevTranIm.ToString("E5"));				sw->Write("    ");
				sw->Write(RevReflRe.ToString("E5"));				sw->Write("  ");
				sw->WriteLine(RevReflIm.ToString("E5"));
			}

		if (mode==2)												// CSV North America rectangular format
			for(int i=0; i<FG->points; i++)
			{
				FwdReflRe = S11Mag[i] * cos(S11Phs[i] * DEGR2RAD);
				FwdReflIm = S11Mag[i] * sin(S11Phs[i] * DEGR2RAD);
				FwdTranRe = S21Mag[i] * cos(S21Phs[i] * DEGR2RAD);
				FwdTranIm = S21Mag[i] * sin(S21Phs[i] * DEGR2RAD);
				RevTranRe = S12Mag[i] * cos(S12Phs[i] * DEGR2RAD);
				RevTranIm = S12Mag[i] * sin(S12Phs[i] * DEGR2RAD);
				RevReflRe = S22Mag[i] * cos(S22Phs[i] * DEGR2RAD);
				RevReflIm = S22Mag[i] * sin(S22Phs[i] * DEGR2RAD);

				sw->Write(FG->Frequency(i).ToString("D9", NAnfi));		sw->Write(",");
				sw->Write("\"=COMPLEX({0},{1})\",",FwdReflRe.ToString("E5", NAnfi),FwdReflIm.ToString("E5", NAnfi));
				sw->Write("\"=COMPLEX({0},{1})\",",FwdTranRe.ToString("E5", NAnfi),FwdTranIm.ToString("E5", NAnfi));
				sw->Write("\"=COMPLEX({0},{1})\",",RevTranRe.ToString("E5", NAnfi),RevTranIm.ToString("E5", NAnfi));
				sw->Write("\"=COMPLEX({0},{1})\",,",RevReflRe.ToString("E5", NAnfi),RevReflIm.ToString("E5", NAnfi));

				// build row number - data starts in the 7th row of the spreadsheet
				int rownum = i+7;
				String^ Row = rownum.ToString(NAnfi);

				// Zin (S11 converted to impedance)
				//=IMPRODUCT(50ohms,(IMDIV(IMSUM(unity,S11),IMSUB(unity,S11)))
				sw->Write("\"=IMPRODUCT($B$4,(IMDIV(IMSUM($E$4,$B{0}),IMSUB($E$4,$B{0}))))\",",Row);
				//Zout (S22 converted to impedance)
				sw->WriteLine("\"=IMPRODUCT($B$4,(IMDIV(IMSUM($E$4,$E{0}),IMSUB($E$4,$E{0}))))\",",Row);
			}


		if (mode==3)												// CSV Europe rectangular format
			for(int i=0; i<FG->points; i++)
			{
				FwdReflRe = S11Mag[i] * cos(S11Phs[i] * DEGR2RAD);
				FwdReflIm = S11Mag[i] * sin(S11Phs[i] * DEGR2RAD);
				FwdTranRe = S21Mag[i] * cos(S21Phs[i] * DEGR2RAD);
				FwdTranIm = S21Mag[i] * sin(S21Phs[i] * DEGR2RAD);
				RevTranRe = S12Mag[i] * cos(S12Phs[i] * DEGR2RAD);
				RevTranIm = S12Mag[i] * sin(S12Phs[i] * DEGR2RAD);
				RevReflRe = S22Mag[i] * cos(S22Phs[i] * DEGR2RAD);
				RevReflIm = S22Mag[i] * sin(S22Phs[i] * DEGR2RAD);

				sw->Write(FG->Frequency(i).ToString("D9", Eurnfi));		sw->Write(";");
				sw->Write("\"=COMPLEX({0};{1})\";",FwdReflRe.ToString("E5", Eurnfi),FwdReflIm.ToString("E5", Eurnfi));
				sw->Write("\"=COMPLEX({0};{1})\";",FwdTranRe.ToString("E5", Eurnfi),FwdTranIm.ToString("E5", Eurnfi));
				sw->Write("\"=COMPLEX({0};{1})\";",RevTranRe.ToString("E5", Eurnfi),RevTranIm.ToString("E5", Eurnfi));
				sw->Write("\"=COMPLEX({0};{1})\";;",RevReflRe.ToString("E5", Eurnfi),RevReflIm.ToString("E5", Eurnfi));

				// build row number - data starts in the 7th row of the spreadsheet
				int rownum = i+7;
				String^ Row = rownum.ToString(Eurnfi);

				// Zin (S11 converted to impedance)
				//=IMPRODUCT(50ohms,(IMDIV(IMSUM(unity,S11),IMSUB(unity,S11)))
				sw->Write("\"=IMPRODUCT($B$4;(IMDIV(IMSUM($E$4;$B{0});IMSUB($E$4;$B{0}))))\";",Row);
				//Zout (S22 converted to impedance)
				sw->WriteLine("\"=IMPRODUCT($B$4;(IMDIV(IMSUM($E$4;$E{0});IMSUB($E$4;$E{0}))))\";",Row);
			}
	
			
		sw->WriteLine();
    }
	catch(System::IO::IOException^ pe)
	{
		MessageBox::Show("File Output Error", pe->Message);
	}
	__finally
	{
		if (sw)
            sw->Flush();
		if (sw)
			sw->Close();
		if (fs)
			fs->Close();
	}

}

// Store S-parameters to internal temporary storage.
void StoreSParams(bool calmode, bool ReflExtn, FrequencyGrid^ FG, InstrumentCalDataSet^ CalData,
				  array<MeasurementSet^>^ dataSet, array<Double>^ reflMag,
				  array<Double>^ reflPhs, array<Double>^ tranMag,
				  array<Double>^ tranPhs)
{
	/// Store Forward caller gives us pointers to Forward storage,
	///   store Reverse caller gives us pointers to Reverse storage
	/// calmode: 1 -> use fixture calibration

	double fmagnitude;			// 0 to 1
	double fphase;				// -180 to +180	
	double& rmag = fmagnitude;
	double& rphs = fphase;

	for (int i=0; i<FG->points; i++)	// Display measurements on the frequency grid
	{
		// Compute S11 or S22
		CalData->ResolveReflPolar(dataSet[i], FG->Frequency(i), rmag, rphs, true);
		if (calmode)
			CorrectS11(CalData, FG->Frequency(i), ReflExtn, fmagnitude, fphase, rmag, rphs);

		reflMag[i] = fmagnitude;	// store reflected component in polar form
		reflPhs[i] = fphase;
		
		// Compute S21 or S12
		CalData->ResolveTranPolar(dataSet[i], FG->Frequency(i), rmag, rphs);
		if (calmode)
			CorrectS21(CalData, FG->Frequency(i), fmagnitude, fphase, rmag, rphs);

		tranMag[i] = fmagnitude;	// store transmitted component in polar form
		tranPhs[i] = fphase;

	}
}

// Linear interpolation of Xval between Xlow and Xhigh yielding Y result between Ylow and Yhi
double Interpolate(int Xval, int Xlow, double Ylow, int Xhi, double Yhi)
{
    double slope = (Yhi - Ylow) / ((double)Xhi-(double)Xlow);
	double result = Ylow + (((double)Xval - (double)Xlow) * slope);
	return(result);
}
//double Interpolate(float Xval, float Xlow, float Ylow, float Xhi, float Yhi)
//{
//    float slope = (Yhi - Ylow) / (Xhi-Xlow);
//	double result = Ylow + ((Xval - Xlow) * slope);
//	return(result);
//}

// Median filtering routine.  Input is 7 measurements [0..6]. Returns the median value.
// Data can be a large array, index points to the beginning of the 7 elements to examine. 
int Median7(array<UInt16>^ data, int index)
{
	int i, j;
	int swap;
	int numbers[7];

	for (i=0; i<7; i++)			// make a local copy of the data
		numbers[i] = (int)data[index + i];		// don't alter what was given to us

	// bubble sort the elements
	for (i=6; i>=0; i--)
        for (j=1; j<=i; j++)
            if (numbers[j-1] > numbers[j])
			{
				swap = numbers[j-1];
				numbers[j-1] = numbers[j];
				numbers[j] = swap;
			}

	return(numbers[3]);	// the median value
}
int Median7(array<UInt16>^ data)
{
	int i, j;
	int swap;
	int numbers[7];

	for (i=0; i<7; i++)			// make a local copy of the data
		numbers[i] = data[i];	// don't alter what was given to us

	// bubble sort the elements
	for (i=6; i>=0; i--)
        for (j=1; j<=i; j++)
            if (numbers[j-1] > numbers[j])
			{
				swap = numbers[j-1];
				numbers[j-1] = numbers[j];
				numbers[j] = swap;
			}

	return(numbers[3]);	// the median value
}
int Median7(array<Int32>^ data)
{
	int i, j;
	int swap;
	int numbers[7];

	for (i=0; i<7; i++)			// make a local copy of the data
		numbers[i] = data[i];	// don't alter what was given to us

	// bubble sort the elements
	for (i=6; i>=0; i--)
        for (j=1; j<=i; j++)
            if (numbers[j-1] > numbers[j])
			{
				swap = numbers[j-1];
				numbers[j-1] = numbers[j];
				numbers[j] = swap;
			}

	return(numbers[3]);	// the median value
}

double Median7(array<Double>^ data, int index, double epsilon )
{
	int i, j;
	double swap;
	array<Double>^ numbers = gcnew array<Double>(7);

	for (i=0; i<7; i++)			// make a local copy of the data
		numbers[i] = data[index+i];	// don't alter what was given to us

	// bubble sort the elements
	for (i=6; i>=0; i--)
        for (j=1; j<=i; j++)
            if (numbers[j-1] > numbers[j])
			{
				swap = numbers[j-1];
				numbers[j-1] = numbers[j];
				numbers[j] = swap;
			}

	if (abs(data[index+3] - numbers[3]) > epsilon)
		return(numbers[3]);	// the median value if deviation is larger than epsilon
	else
		return(data[index+3]); // the original value if deviation is smaller than epsilon
}




// Range routine. Determine range of middle 5 samples in a group of 7 samples.
// If the range is small, then the samples look like one another even if one
// of them is a glitch.
int Range(array<UInt16>^ data, int index)
{
	int i, j;
	unsigned int temp;
	unsigned int numbers[7];

	for (i=0; i<7; i++)			// make a local copy of the data
		numbers[i] = data[index + i];		// don't alter what was given to us

	// bubble sort the elements
	for (i=6; i>=0; i--)
        for (j=1; j<=i; j++)
            if (numbers[j-1] > numbers[j])
			{
				temp = numbers[j-1];
				numbers[j-1] = numbers[j];
				numbers[j] = temp;
			}

	return(numbers[5]-numbers[1]);	// the range of central samples
}
// Determined expected value of middle element based on all 7 samples in the array
int ExpectedValue(array<UInt16>^ data, int index, bool mode)
{
	// Filter raw reading from the VNA and smooth obvious reading glitch.
	//
	// Return actual raw reading if it's close to the expected reading,
	// Else return the median of the 7 samples that include the reading.

	// Mode==false means amplitude filtering. Mode==true means Phase filtering

	int LinearPredictor;			// predicted value from previous or trailing samples
	int MedianValue, RangeSpread;	// Median and Range of the 7 values
	int Deviation;					// Difference between actual and expected value
	int Delta;						// how far apart adjacent samples are

	// determine Expected value as the median
	MedianValue = Median7(data, index);
	RangeSpread = Range(data, index); 
	Deviation = abs(data[index+3] - MedianValue);
	if (Deviation < GLITCHSIZE || Deviation < RangeSpread)
		return(data[index+3]);			// return measurement if it is close

	// if that failed, determine Expected value from previous 2 samples by linear extrapolation
	Delta = data[index+2] - data[index+1];
	LinearPredictor = data[index+2] + Delta;
	Deviation = abs(data[index+3] - LinearPredictor);
	if (Deviation < (GLITCHSIZE + abs(Delta/5)))
		return(data[index+3]);			// return measurement if it is close
/*
	if (mode == true)	// phase measurement saturates about ~120 low and ~3500 high
	{
		if (LinearPredictor < 120 && data[index + 3] < 120)
			return(data[index+3]);		// return phase data if it's saturated low
		if (LinearPredictor > 3480 && data[index + 3] > 3480)
			return(data[index+3]);		// return phase data if it's saturated high
	}
*/
	// If that failed, determine Expected value from trailing 2 samples by linear extrapolation
	Delta = data[index+4] - data[index+5];
	LinearPredictor = data[index+4] + Delta;
	Deviation = abs(data[index+3] - LinearPredictor);
	if (Deviation < (GLITCHSIZE + abs(Delta/5)))
		return(data[index+3]);			// return measurement if it is close
/*
	if (mode == true)	// phase measurement saturates about ~120 low and ~3500 high
	{
		if (LinearPredictor < 120 && data[index + 3] < 120)
			return(data[index+3]);		// return phase data if it's saturated low
		if (LinearPredictor > 3480 && data[index + 3] > 3480)
			return(data[index+3]);		// return phase data if it's saturated high
	}
*/
	// All reasonableness tests failed, so return the median value (filter it)
	return(MedianValue);
}

// Deglitch an entire array of raw VNA readings using ExpectedValue
void DeGlitch(array<UInt16>^ raw, int count, bool phasemode)
{
	array<UInt16>^ copy;
	copy = gcnew array<UInt16>(PHASECALGRIDSIZE);

	for(int i=0; i<count; i++)
		copy[i] = (unsigned short)raw[i];

	for(int i=0; i<(count-7); i++)
		raw[i+3] = ExpectedValue(copy, i, phasemode);

};

void DeGlitch(array<MeasurementSet^>^ dataSet, int count)
{
	array<UInt16>^ copy;
	copy = gcnew array<UInt16>(PHASECALGRIDSIZE);

	for(int i=0; i<count; i++)
		copy[i] = dataSet[i]->ReflMI;
	for(int i=0; i<(count-7); i++)
		dataSet[i+3]->ReflMI = ExpectedValue(copy, i, AMPLMODE);

	for(int i=0; i<count; i++)
		copy[i] = dataSet[i]->ReflMQ;
	for(int i=0; i<(count-7); i++)
		dataSet[i+3]->ReflMQ = ExpectedValue(copy, i, AMPLMODE);

	for(int i=0; i<count; i++)
		copy[i] = dataSet[i]->ReflPI;
	for(int i=0; i<(count-7); i++)
		dataSet[i+3]->ReflPI = ExpectedValue(copy, i, PHASEMODE);

	for(int i=0; i<count; i++)
		copy[i] = dataSet[i]->ReflPQ;
	for(int i=0; i<(count-7); i++)
		dataSet[i+3]->ReflPQ = ExpectedValue(copy, i, PHASEMODE);

	for(int i=0; i<count; i++)
		copy[i] = dataSet[i]->TranMI;
	for(int i=0; i<(count-7); i++)
		dataSet[i+3]->TranMI = ExpectedValue(copy, i, AMPLMODE);

	for(int i=0; i<count; i++)
		copy[i] = dataSet[i]->TranMQHi;
	for(int i=0; i<(count-7); i++)
		dataSet[i+3]->TranMQHi = ExpectedValue(copy, i, AMPLMODE);

	for(int i=0; i<count; i++)
		copy[i] = dataSet[i]->TranPI;
	for(int i=0; i<(count-7); i++)
		dataSet[i+3]->TranPI = ExpectedValue(copy, i, PHASEMODE);

	for(int i=0; i<count; i++)
		copy[i] = dataSet[i]->TranPQ;
	for(int i=0; i<(count-7); i++)
		dataSet[i+3]->TranPQ = ExpectedValue(copy, i, PHASEMODE);

	for(int i=0; i<count; i++)
		copy[i] = dataSet[i]->TranMQLo;
	for(int i=0; i<(count-7); i++)
		dataSet[i+3]->TranMQLo = ExpectedValue(copy, i, AMPLMODE);

	for(int i=0; i<count; i++)
		copy[i] = dataSet[i]->TranMQMid;
	for(int i=0; i<(count-7); i++)
		dataSet[i+3]->TranMQMid = ExpectedValue(copy, i, AMPLMODE);

	for(int i=0; i<count; i++)
		copy[i] = dataSet[i]->TranPILow;
	for(int i=0; i<(count-7); i++)
		dataSet[i+3]->TranPILow = ExpectedValue(copy, i, PHASEMODE);

	for(int i=0; i<count; i++)
		copy[i] = dataSet[i]->TranPQLow;
	for(int i=0; i<(count-7); i++)
		dataSet[i+3]->TranPQLow = ExpectedValue(copy, i, PHASEMODE);


}

// Smooth a phase error table
void SmoothPhaseTable(array<Double>^ table)
{
	// The phase table has 360 entries, adjacent samples should be
	// smooth, and the table should wrap smoothly (359 approx equal to 0)

	array<Double>^ copy;
	copy = gcnew array<Double>(366);		// copy[0..365] holds circularly wrapped table

	const double eps = 0.2;				// glitch size in degrees before we start correcting

	for(int i=0; i<360; i++)
		copy[i] = table[i];

	for(int i=360; i<366; i++)
		copy[i] = table[i-360];			// duplicate part of table that wraps around zero index

	for(int i=0; i<354; i++)
		table[i+3] = Median7(copy, i, eps);		// filter table[3] to table[356]

	table[357] = Median7(copy, 354, eps);		// clean up wrap-around
	table[358] = Median7(copy, 355, eps);		
	table[359] = Median7(copy, 356, eps);		
	table[0] = Median7(copy, 357, eps);		
	table[1] = Median7(copy, 358, eps);		
	table[2] = Median7(copy, 359, eps);		

};

//
//
// DrawLine routines that checks and bounds points to within display rectangle.
// The gr->SetClip() function should do this, but certain values of arguments to DrawLine outside the
// clipped region cause it to throw an exception rather than clip. These routines fix that problem.
void DrawLineBound(System::Drawing::Graphics^ gr, System::Drawing::Rectangle r,
				   System::Drawing::Pen^ pen, int x1, int y1, int x2, int y2)
{
	if(x1 < r.X)			// bound x1, x2, y1, y2 within the rectangle r
		x1 = r.X;
	if(x1 > r.Right)
		x1 = r.Right;
	if(x2 < r.X)
		x2 = r.X;
	if(x2 > r.Right)
		x2 = r.Right;
	if(y1 < r.Y)
		y1 = r.Y;
	if(y1 > r.Bottom)
		y1 = r.Bottom;
	if(y2 < r.Y)
		y2 = r.Y;
	if(y2 > r.Bottom)
		y2 = r.Bottom;

	gr->DrawLine(pen, x1, y1, x2, y2);	// draw the bounded line
}
void DrawLineBound(System::Drawing::Graphics^ gr, System::Drawing::Rectangle r,
				   System::Drawing::Pen^ pen, System::Drawing::Point p1, System::Drawing::Point p2)
{
	DrawLineBound(gr, r, pen, p1.X, p1.Y, p2.X, p2.Y);	// draw the bounded line
}
void AddPolar(double& linearMag, double& phaseDegr, double linearMagAdd, double phaseDegrAdd)
{
	double real, imag;

	real = linearMag * cos(phaseDegr*DEGR2RAD);
	imag = linearMag * sin(phaseDegr*DEGR2RAD);

	real += linearMagAdd * cos(phaseDegrAdd*DEGR2RAD);
	imag += linearMagAdd * sin(phaseDegrAdd*DEGR2RAD);

	linearMag = sqrt(real*real+imag*imag);
	phaseDegr = atan2(imag,real) * RAD2DEGR;
}

// Convert MeasurementDelay string to target's loop count
int MeasureDelayStringToCount(String^ value)
{

//	time delay is very roughly 30 usec + loopcount * 40 microseconds
//  Values other than "FAST" use the "SLOW" mode of the buffer

	if(String::Compare(value, "Fast") == 0)
		return 0;
	if(String::Compare(value, "30 us") == 0)
		return 0;
	if(String::Compare(value, "100 us") == 0)
		return 2;
	if(String::Compare(value, "300 us") == 0)
		return 7;
	if(String::Compare(value, "1 ms") == 0)
		return 24;
	if(String::Compare(value, "3 ms") == 0)
		return 74;
	if(String::Compare(value, "10 ms") == 0)
		return 249;
	if(String::Compare(value, "30 ms") == 0)
		return 749;
	if(String::Compare(value, "100 ms") == 0)
		return 2499;

	return 0;	// no delay if unexpected string 
};

#if 0
int GetFreqFromDetMagCalGrid(long index)	/// Convert Detector Magnitude Calibration Grid index to Frequency.
		{											/// Grid is 21 points.
//			if (index < 9 && index >= 0)
//				return (Convert::ToInt32((index + 2) * 100000));	// Every 100 KHz from 200 kHz to 1 Mhz.
//			if (index < 21 && index >= 0)
//				return (Convert::ToInt32((index - 8) * 10000000));	// Then every ten MHz starting at 10 MHz.
			if (index < PHASECALGRIDSIZE && index >= 0)
				return (Convert::ToInt32(MINCALFREQ + (index * (Cal->maxCalFreq - MINCALFREQ)/(21-1))));
			else
				throw gcnew System::ArgumentOutOfRangeException(
				  "GetFreqFromDetMagCalGrid: index is invalid ");	// bad index value
		};

int GetFreqFromPhaseCalGrid(long index)	/// Convert Phase Calibration Grid index to Frequency.
{
			if (index < PHASECALGRIDSIZE && index >= 0)
				return (Convert::ToInt32(MINCALFREQ + (index * (Cal->maxCalFreq - MINCALFREQ)/(NUMCALPTS-1))));
			else
				throw gcnew System::ArgumentOutOfRangeException(
				  "GetFreqFromPhaseCalGrid: index is invalid ");	// bad index value
};

#endif

// Wrap phase to be in the range -180 to +180.  Three overloads.
int NormalizePhaseDegr(int phase)
	{
		if (phase > 180) return(phase - 360);
		if (phase <= -180) return(phase + 360);
		return phase;
	};
double NormalizePhaseDegr(double phase)
	{
		if (phase > 180.0) return(phase - 360.0);
		if (phase <= -180.0) return(phase + 360.0);
		return phase;
	};
float NormalizePhaseDegr(float phase)
	{
		if (phase > 180.0) return(phase - 360.0f);
		if (phase <= -180.0) return(phase + 360.0f);
		return phase;
	};

bool ExcessRxNoiseIngress(array<MeasurementSet^>^ trace, int points)
{
	// check for cases where noise is flooding the receive detector
	// due to poor shielding.  09-27-2008

	int faultCount = 0;
	for(int i=15; i<(points-15); i++)
	{
		UInt16 Mid = trace[i]->TranMQMid;
		UInt16 Lo = trace[i]->TranMQLo;
		UInt16 Hi = trace[i]->TranMQHi;

		if(Mid >= 2300 && Mid < 3300)
			if(Lo > (Mid - 750))
				faultCount+= 100;

		if(Mid > 1300 && Mid < 2300)
			if(Hi < (Mid + 750))
				faultCount+= 100;
	};

	faultCount /= (points-30);		// convert to percent of points faulty

	if(faultCount > 3)	// ~3 percent threshold
		return(true);	// noise ingress detected

	return(false);		// no noise ingress detected
};

/*
/// Compute Frequency of grid point for linear(f) and log(f) fixture cal points
int GetFreqFromFixtureCalGrid(long index, bool LogMode)	/// Convert Phase Calibration Grid index to Frequency.
{

	// Note: linear mode is the default, and provides backwards-compatibility with older Fixture Cal files

	if(LogMode)								// Compute fixture calibration log frequency point
		{											/// Grid is 1024 points.
			double FreqIncrement;
			FreqIncrement = Math::Pow(10,Math::Log10(Cal->maxCalFreq/MINCALFREQ)/(NUMCALPTS-1));

			if (index < PHASECALGRIDSIZE && index >= 0)
				return (Convert::ToInt32(MINCALFREQ * Math::Pow(FreqIncrement, index)));
			else
				throw gcnew System::ArgumentOutOfRangeException(
				  "GetFreqFromFixtureCalGrid: index is invalid ");	// bad index value
		}
	else									// Compute fixture calibration linear frequency point
		{											/// Grid is 1024 points.
			if (index < PHASECALGRIDSIZE && index >= 0)
				return (Convert::ToInt32(MINCALFREQ + (index * (Cal->maxCalFreq - MINCALFREQ)/(NUMCALPTS-1))));
			else
				throw gcnew System::ArgumentOutOfRangeException(
				  "GetFreqFromFixtureCalGrid: index is invalid ");	// bad index value
		}												
};
*/
