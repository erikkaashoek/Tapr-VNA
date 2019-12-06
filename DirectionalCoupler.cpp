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
//	Routines to encapsulate directional coupler compensation
//
//  07-04-07  TCM Original
//	09-23-07  TCM Use Gradient descent optimization to fit correction coefficients
//				to coupler mag and phase ripple measurements.



#include "stdafx.h"
#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::ComponentModel;
using namespace System::IO;
using namespace System::Text;
using namespace System::Windows::Forms;
using namespace System::Globalization;

#include <math.h>
#include <complex>
#include "DataDisplay.h"
#include "Constants.h"
#include "DisplayRoutines.h"
#include "DirectionalCoupler.h"

#include <float.h>

	/// Constructor for DirectionalCoupler
DirectionalCoupler::DirectionalCoupler(InstrumentCalDataSet^ Calset)
{
	DirMag = gcnew array<Int32>(PHASECALGRIDSIZE);					///< Directivity Magnitude ADC count at each frequnecy
	DirIphs = gcnew array<Int32>(PHASECALGRIDSIZE);					///< Directivity I phase ADC count at each frequency
	DirQphs = gcnew array<Int32>(PHASECALGRIDSIZE);					///< Directivity Q phase ADC count at each frequency

	openAngle = gcnew array<Double>(PHASECALGRIDSIZE);		///< Array of open angles actually measured
	shortAngle = gcnew array<Double>(PHASECALGRIDSIZE);		///< Array of shorted angles actually measured
	phaseError = gcnew array<Double>(PHASECALGRIDSIZE);		///< Array of open-short angle error for LF phase compensator (error signature)
	magError = gcnew array<Double>(PHASECALGRIDSIZE);		///< Array of open-short magnitude error for magnitude compensator (error signature)

	PhRipCoeff = gcnew array<PerErrModelCoeff^>(6);		/// Set of coefficients to PeriodicErrorModel for phase ripple error

	Cal = Calset;			///< VNA hardware device

	for (int i=0; i<6; i++)							// NEW 09-22-2007
		PhRipCoeff[i] = gcnew PerErrModelCoeff;

	MagRipCoeff = gcnew array<PerErrModelCoeff^>(6);		/// Set of coefficients to PeriodicErrorModel for magnitude ripple error
	for (int i=0; i<6; i++)							// NEW 09-22-2007
		MagRipCoeff[i] = gcnew PerErrModelCoeff;
#if 0
	// Initial guess for coupler phase angle open-short errors 
	PhRipCoeff[0]->freq = 15500000; PhRipCoeff[0]->offset = 134.29; PhRipCoeff[0]->mag = 4.14;
	PhRipCoeff[1]->freq = 34500000; PhRipCoeff[1]->offset = 137.20; PhRipCoeff[1]->mag = 3.43;
	PhRipCoeff[2]->freq = 53500000; PhRipCoeff[2]->offset = 99.71; PhRipCoeff[2]->mag = 4.02;
	PhRipCoeff[3]->freq = 72500000; PhRipCoeff[3]->offset = 112.31; PhRipCoeff[3]->mag = 1.31;
	PhRipCoeff[4]->freq = 91500000; PhRipCoeff[4]->offset = 123.86; PhRipCoeff[4]->mag = 2.73;
	PhRipCoeff[5]->freq = 110500000; PhRipCoeff[5]->offset = 158.88; PhRipCoeff[5]->mag = 1.35;

	// Initial guess for coupler magnitude delta open-short errors 
	MagRipCoeff[0]->freq = 15500000; MagRipCoeff[0]->offset = +26.02; MagRipCoeff[0]->mag = 0.033;
	MagRipCoeff[1]->freq = 34500000; MagRipCoeff[1]->offset = -27.40; MagRipCoeff[1]->mag = 0.009;
	MagRipCoeff[2]->freq = 53500000; MagRipCoeff[2]->offset = -58.02; MagRipCoeff[2]->mag = 0.026;
	MagRipCoeff[3]->freq = 72500000; MagRipCoeff[3]->offset = -53.54; MagRipCoeff[3]->mag = 0.016;
	MagRipCoeff[4]->freq = 91500000; MagRipCoeff[4]->offset = -86.80; MagRipCoeff[4]->mag = 0.013;
	MagRipCoeff[5]->freq = 110500000; MagRipCoeff[5]->offset = -86.80; MagRipCoeff[5]->mag = 0.013;
#else
		// Initial guess for coupler phase angle open-short errors 
	PhRipCoeff[0]->freq = 100000000; PhRipCoeff[0]->offset = 134.29; PhRipCoeff[0]->mag = 4.14;
	PhRipCoeff[1]->freq = 300000000; PhRipCoeff[1]->offset = 137.20; PhRipCoeff[1]->mag = 3.43;
	PhRipCoeff[2]->freq = 600000000; PhRipCoeff[2]->offset = 99.71; PhRipCoeff[2]->mag = 4.02;
	PhRipCoeff[3]->freq = 900000000; PhRipCoeff[3]->offset = 112.31; PhRipCoeff[3]->mag = 1.31;
	PhRipCoeff[4]->freq = 1200000000; PhRipCoeff[4]->offset = 123.86; PhRipCoeff[4]->mag = 2.73;
	PhRipCoeff[5]->freq = 1500000000; PhRipCoeff[5]->offset = 158.88; PhRipCoeff[5]->mag = 1.35;

	// Initial guess for coupler magnitude delta open-short errors 
	MagRipCoeff[0]->freq = 100000000; MagRipCoeff[0]->offset = +26.02; MagRipCoeff[0]->mag = 0.033;
	MagRipCoeff[1]->freq = 300000000; MagRipCoeff[1]->offset = -27.40; MagRipCoeff[1]->mag = 0.009;
	MagRipCoeff[2]->freq = 600000000; MagRipCoeff[2]->offset = -58.02; MagRipCoeff[2]->mag = 0.026;
	MagRipCoeff[3]->freq = 900000000; MagRipCoeff[3]->offset = -53.54; MagRipCoeff[3]->mag = 0.016;
	MagRipCoeff[4]->freq = 1200000000; MagRipCoeff[4]->offset = -86.80; MagRipCoeff[4]->mag = 0.013;
	MagRipCoeff[5]->freq = 1500000000; MagRipCoeff[5]->offset = -86.80; MagRipCoeff[5]->mag = 0.013;
#endif
	// initialized by .NET constructor
	DirCalibrated = false;					// directivity calibration not yet run
	RippleCalibrated = false;					// Mag & Phase ripple calibration not yet run

	PhaseDCOffset = 0.0;						// initialize offset to none
	MagDCOffset = 0.0;						// initialize offset to none

}

	/// Record raw directivity cal data
void DirectionalCoupler::DirectivityCal(array<UInt16,2>^ DirectivityMag)
{
	// Store the raw ADC counts from the Directivity Calibration
	// The consuming routine needs to convert them to mag+phase

	for (int i=0; i<PHASECALGRIDSIZE; i++)
	{
		DirMag[i] = DirectivityMag[i, MagQ];
		DirIphs[i] = DirectivityMag[i, PhaseI];
		DirQphs[i] = DirectivityMag[i, PhaseQ];
	}
}
void DirectionalCoupler::CompensateDirectivity(InstrumentCalDataSet^ cal, double& rmagnitudeLin, double& rphase, int Freq) 
{
	if(!DirCalibrated)				// don't compensate if we're not calibrated
		return;

	int FreqBase, FreqBaseNext;		// Two adjacent cal frequency points
	double FreqRemainder;			// remainder of freq between cal points


	// Result is frequency-dependent. Interpolate the frequency between two datasets to construct
	// a linear-frequency approximation of the cal data for the desired frequency.
	// Every 200 KHz from 200K to 1MHz, then every 10 MHz up to 120 MHz.
	// Index:  0    1    2    3    4    5    6    7    8   9   10   11   12   13   14   15   16   17    18    19 
	// Freq:  200k 300k 400k 500k 600k 700k 800k 900k 1M  10M  20M  30M  40M  50M  60M  70M  80M  90M  100M  110M 


	// Build interpolated dataset between two frequencies

	if (Freq <= cal->VNA->GetMaxFreq()/10)
	{
		FreqBase = (Freq - 200000)/100000;
		FreqRemainder = (double)(Freq - (FreqBase + 2) * 100000)/100000;  // Remainder of frequency, value between  0..1
	}
	else if (Freq < 10000000)	// Between 1 and 10 MHz
	{
		FreqBase = 8;
		FreqRemainder = (double)(Freq - 1000000)/9000000;
	}
	else		// 10 MHz to 120 MHz
	{
		FreqBase = (Freq / 10000000) + 8;						// Base is the integer part of the frequency index
		FreqRemainder = (double)(Freq - (FreqBase - 8) * 10000000)/10000000;  // Remainder of frequency, value between  0..1
	}


	if (FreqRemainder < 0.001)
		FreqBaseNext = FreqBase;					// Prevent index overflow
	else
		FreqBaseNext = FreqBase + 1;


	int I, Q, M;		// Interpolate between Directivity Cal points
	
	I = DirIphs[FreqBase] + (int)((DirIphs[FreqBaseNext] - DirIphs[FreqBase]) * FreqRemainder);
	Q = DirQphs[FreqBase] + (int)((DirQphs[FreqBaseNext] - DirQphs[FreqBase]) * FreqRemainder);
	M = DirMag[FreqBase] + (int)((DirMag[FreqBaseNext] - DirMag[FreqBase]) * FreqRemainder);

	double DirMagnitudeDB = cal->RxDet->MagTodBRefl(Freq, M);
	double DirMagnitudeLin = pow(10.0, (DirMagnitudeDB/20.0));		// translate db_return_loss to voltage ratio

	double DirPhase = cal->RxDet->IQtoDegrees(I, Q, Freq, 0, 0, 0);

	AddPolar(rmagnitudeLin, rphase, -DirMagnitudeLin, DirPhase);	// Subtract Directivity error from Reflection reading
	rphase = NormalizePhaseDegr(rphase);

};
	/// Compensate coupler phase ripple error NEW 09-23-2007
double DirectionalCoupler::PhaseRippleCompensate(double phase, __int64 frequency)
{
	// Correct the coupler phase ripple error.
	// The correction consists of phase error amplitude, and offset angle to the measurement.
	// The error is modeled as a sinusoidal error term vs. measurement angle.
	// The values of the magnitude and offset angle come from the detector calibration 3m cable open and short

	// The correction is half the difference between open angle and inverted short angle.

	if (!RippleCalibrated)
		return phase;

    double correction = PeriodicModelCorrection(frequency, phase, PhRipCoeff, PhaseDCOffset);

	return NormalizePhaseDegr(phase + correction);
};

	// Compensate coupler magnitude ripple error   NEW 09-23-2007
double DirectionalCoupler::MagRippleCorrection(double phase, __int64 frequency)
{
	if (!RippleCalibrated)
		return 0.0;		// no compensation

    double correction = PeriodicModelCorrection(frequency, phase, MagRipCoeff, MagDCOffset);

	return correction;	// add this compensation to the raw value

};
	/// Derive table correction vectors Short+Open - NEW - 09-23-2007
bool DirectionalCoupler::GenerateShortOpenComp(InstrumentCalDataSet^ cal, array<UInt16,2>^ shortedrefl,
		array<UInt16,2>^ openrefl)
{
	// Derive parameters to correct open-short phase error.
	// The magnitude and offset angle are frequency dependent - and are established at 6 points.
	// The offset_angle and magnitude values are linearly interpolated between these 6 points.
	// 
	// This version starts with an initial guess for the parameter values, and refines the parameters to
	//   minimize (error - compensator)^2 over the range 6 - 100 MHz  using a gradient-descent method.	

    array<Double>^ partialDerivative = gcnew array<Double>(12);	// array of 12 partial derivatives in two groups of five:
									// dOffset0, ... dOffset5, dMagnitude0, ... dMagnitude5

	// Decent velocity. 
	array<Double>^ weights = gcnew array<Double>{ 0.05, 0.05, 0.05, 0.05, 0.05, 0.05,
		0.001, 0.001, 0.001, 0.001, 0.001, 0.001 };  

	const int startIndex = 65;		// frequency range of the correction process
	const int stopIndex = 1010;		//     "       "
	const int iterLimit = 1000;		// iteration limit

	RippleCalibrated = false;		// disable the new ripple compensators

#if 0

	// Step 1.  Construct the error vs. frequency and open-angle vs. frequency arrays.
	//          Also construct the magnitude difference between open and short vs. frequency.
	for(int i=0; i<PHASECALGRIDSIZE; i++)
	{
		double mag, phs, shortPhase, openPhase;
		double shortMag, openMag;
		double& rmag = mag;
		double& rphs = phs;
		__int64	Freq;
		MeasurementSet^ calPoint = gcnew MeasurementSet;

		Freq = Cal->GetFreqFromPhaseCalGrid(i);

		calPoint->ReflPI = shortedrefl[i, PhaseI];
		calPoint->ReflPQ = shortedrefl[i, PhaseQ];
		calPoint->ReflMQ = shortedrefl[i, MagQ];

		cal->ResolveReflPolar(calPoint,	Freq, rmag, rphs, true);
		shortPhase = phs;
		shortMag = mag;


		calPoint->ReflPI = openrefl[i, PhaseI];
		calPoint->ReflPQ = openrefl[i, PhaseQ];
		calPoint->ReflMQ = openrefl[i, MagQ];

		cal->ResolveReflPolar(calPoint, Freq, rmag, rphs, true);
		openPhase = phs;
		openMag = mag;

		// invert the shorted-meas angle
		shortPhase = NormalizePhaseDegr(shortPhase - 180.0);

		openAngle[i] = openPhase;
		shortAngle[i] = shortPhase;

		// Open and inverted short should be the same angle, record half the difference as error
		phaseError[i] = NormalizePhaseDegr(openPhase - shortPhase) / 2.0;

		// Linear magnitude difference between open and short on 3 meter cable. half the difference
		// is the error.
		magError[i] = (openMag - shortMag) / 2.0;
	}	
	
	// Clean up any glitches on the error terms
	array<Double>^ TempPhaseError = gcnew array<Double>(PHASECALGRIDSIZE);
	array<Double>^ TempMagError = gcnew array<Double>(PHASECALGRIDSIZE);

	for (int i=0; i<PHASECALGRIDSIZE; i++)	// Make a copy of the data
	{
		TempPhaseError[i] = phaseError[i];
		TempMagError[i] = magError[i];
	}

	for (int i=0; i<PHASECALGRIDSIZE-6; i++)
	{
		phaseError[i+3] = Median7(TempPhaseError,i);
		magError[i+3] = Median7(TempMagError,i);
	}

#endif
	
	//	Step 2.   Determine the bias of all the phase & mag samples.
	//
	//  Add up the phase values, divide by the total number of samples.
	//  This is a DC offset to the compensator. Ignore very low frequencies.
	//

	//  Modification 11-30-2013.  Re-initialize values just in case a previous
	//  instrument Calibration (Detector Calibration) was run but did not converge.
	//  Since the detector.ica file would exist in that case, the values in this
	//  routine would be initialized with whatever was read from the detector.ica file.
	//  That forces us to start convergence from a bad place rather than our nitial guess.

	PhaseDCOffset = 0;
	MagDCOffset = 0;

	// Initial guess for coupler phase angle open-short errors 
	PhRipCoeff[0]->freq = 100000000; PhRipCoeff[0]->offset = 134.29; PhRipCoeff[0]->mag = 4.14;
	PhRipCoeff[1]->freq = 300000000; PhRipCoeff[1]->offset = 137.20; PhRipCoeff[1]->mag = 3.43;
	PhRipCoeff[2]->freq = 600000000; PhRipCoeff[2]->offset = 99.71; PhRipCoeff[2]->mag = 4.02;
	PhRipCoeff[3]->freq = 900000000; PhRipCoeff[3]->offset = 112.31; PhRipCoeff[3]->mag = 1.31;
	PhRipCoeff[4]->freq = 1200000000; PhRipCoeff[4]->offset = 123.86; PhRipCoeff[4]->mag = 2.73;
	PhRipCoeff[5]->freq = 1500000000; PhRipCoeff[5]->offset = 158.88; PhRipCoeff[5]->mag = 1.35;

	// Initial guess for coupler magnitude delta open-short errors 
	MagRipCoeff[0]->freq = 100000000; MagRipCoeff[0]->offset = +26.02; MagRipCoeff[0]->mag = 0.033;
	MagRipCoeff[1]->freq = 300000000; MagRipCoeff[1]->offset = -27.40; MagRipCoeff[1]->mag = 0.009;
	MagRipCoeff[2]->freq = 600000000; MagRipCoeff[2]->offset = -58.02; MagRipCoeff[2]->mag = 0.026;
	MagRipCoeff[3]->freq = 900000000; MagRipCoeff[3]->offset = -53.54; MagRipCoeff[3]->mag = 0.016;
	MagRipCoeff[4]->freq = 1200000000; MagRipCoeff[4]->offset = -86.80; MagRipCoeff[4]->mag = 0.013;
	MagRipCoeff[5]->freq = 1500000000; MagRipCoeff[5]->offset = -86.80; MagRipCoeff[5]->mag = 0.013;


	for (int i=60; i<PHASECALGRIDSIZE; i++)
	{
		PhaseDCOffset += phaseError[i];
		MagDCOffset += magError[i];
	}

	PhaseDCOffset /= (double)(PHASECALGRIDSIZE-60);
	MagDCOffset /= (double)(PHASECALGRIDSIZE-60);

	double InitialPhaseMSE, FinalPhaseMSE, InitialMagMSE, FinalMagMSE;

	InitialPhaseMSE = SumSquareError(startIndex, stopIndex, openAngle, phaseError, PhRipCoeff, PhaseDCOffset);
	InitialMagMSE = SumSquareError(startIndex, stopIndex, openAngle, magError, MagRipCoeff, MagDCOffset);


	// Step 3. Find the coefficients that minimize the squared error of the phase compensator.
	// Compute the gradient, then descend along it by amount specified by weight.
	// Assumes that MSE is well-behaved.  If not, we'll need to update the weights based
	// on the momentum of each partial's behavior.

for(int iterations=0; iterations<iterLimit; iterations++)
	{
		// Just for debugging, remove from final version
		//double MSE = SumSquareError(startIndex, stopIndex, openAngle, phaseError, PhRipCoeff);

		// Compute the gradient of the 12-dimensional parameter set, save in partials
		Gradient(partialDerivative, startIndex, stopIndex, openAngle, phaseError, PhRipCoeff, PhaseDCOffset);

		//using namespace System::Diagnostics;
		//Debug::Write(" Iter:", iterations.ToString());
		//Debug::Write(" MSE:", MSE.ToString());
		//Debug::Write("   Off0:", PhRipCoeff[0]->offset.ToString());
		//Debug::Write(" Mag0:", PhRipCoeff[0]->mag.ToString());
		//Debug::Write(" Off1:", PhRipCoeff[1]->offset.ToString());
		//Debug::Write(" Mag1:", PhRipCoeff[1]->mag.ToString());
		//Debug::Write(" Off2:", PhRipCoeff[2]->offset.ToString());
		//Debug::Write(" Mag2:", PhRipCoeff[2]->mag.ToString());
		//Debug::Write(" Off3:", PhRipCoeff[3]->offset.ToString());
		//Debug::Write(" Mag3:", PhRipCoeff[3]->mag.ToString());
		//Debug::Write(" Off4:", PhRipCoeff[4]->offset.ToString());
		//Debug::Write(" Mag4:", PhRipCoeff[4]->mag.ToString());
		//Debug::Write(" Off5:", PhRipCoeff[5]->offset.ToString());
		//Debug::Write(" Mag5:", PhRipCoeff[5]->mag.ToString());
		//Debug::WriteLine("");

		// Update the coefficient values by stepping in the direction of the decent gradient by weight amount
		Descend(partialDerivative, weights, PhRipCoeff);
	}

	// Step 3. Find the coefficients that minimize the squared error of the magnitude compensator.
	// Compute the gradient, then descend along it by amount specified by weight.
	// Assumes that MSE is well-behaved.  If not, we'll need to update the weights based
	// on the momentum of each partial's behavior.

	for(int i=0; i<6; i++)		// adjust weights for different error signature scale factor of the magnitude error
	{
		weights[i] = 1000.0;
		weights[i+6] = 0.002;
	}

for(int iterations=0; iterations<iterLimit; iterations++)
	{
		// Just for debugging, remove from final version
		//double MSE = SumSquareError(startIndex, stopIndex, openAngle, magError, MagRipCoeff);

		// Compute the gradient of the 10-dimensional parameter set, save in partials
		Gradient(partialDerivative, startIndex, stopIndex, openAngle, magError, MagRipCoeff, MagDCOffset);

		//using namespace System::Diagnostics;
		//Debug::Write(" Iter:", iterations.ToString());
		//Debug::Write(" MSE:", MSE.ToString());
		//Debug::Write("   Off0:", MagRipCoeff[0]->offset.ToString());
		//Debug::Write(" Mag0:", MagRipCoeff[0]->mag.ToString());
		//Debug::Write(" Off1:", MagRipCoeff[1]->offset.ToString());
		//Debug::Write(" Mag1:", MagRipCoeff[1]->mag.ToString());
		//Debug::Write(" Off2:", MagRipCoeff[2]->offset.ToString());
		//Debug::Write(" Mag2:", MagRipCoeff[2]->mag.ToString());
		//Debug::Write(" Off3:", MagRipCoeff[3]->offset.ToString());
		//Debug::Write(" Mag3:", MagRipCoeff[3]->mag.ToString());
		//Debug::Write(" Off4:", MagRipCoeff[4]->offset.ToString());
		//Debug::Write(" Mag4:", MagRipCoeff[4]->mag.ToString());
		//Debug::Write(" Off5:", MagRipCoeff[4]->offset.ToString());
		//Debug::Write(" Mag5:", MagRipCoeff[4]->mag.ToString());
		//Debug::WriteLine("");

		// Update the coefficient values by stepping in the direction of the decent gradient by weight amount
		Descend(partialDerivative, weights, MagRipCoeff);
	}

	FinalPhaseMSE = SumSquareError(startIndex, stopIndex, openAngle, phaseError, PhRipCoeff, PhaseDCOffset);
	FinalMagMSE = SumSquareError(startIndex, stopIndex, openAngle, magError, MagRipCoeff, MagDCOffset);

	// Test the coefficient integrity. Mag should not flip sign between points

	//bool PhSignPlus = true;
	//bool MagSignPlus = true;
	//bool SignFlip = false;

	//if(PhRipCoeff[0]->mag < 0.0)
	//	PhSignPlus = false;

	//if(MagRipCoeff[0]->mag < 0.0)
	//	MagSignPlus = false;

	//for(int i=1; i<6; i++)
	//{
	//	if((MagRipCoeff[i]->mag < -0.002) && (MagSignPlus))	// sign flip within +/- 0.002 length is OK
	//		SignFlip = true;
	//	if((MagRipCoeff[i]->mag > 0.002) && (!MagSignPlus))
	//		SignFlip = true;
	//	if((PhRipCoeff[i]->mag < -0.1) && (PhSignPlus))		// sign flip within +/- 0.1 degree is OK
	//		SignFlip = true;
	//	if((PhRipCoeff[i]->mag > 0.1) && (!PhSignPlus))
	//		SignFlip = true;
	//}

	bool PhaseOK = false;
	bool MagOK = false;

	if (((FinalPhaseMSE - InitialPhaseMSE) > 600) || (FinalPhaseMSE < 800))
		PhaseOK = true;

	if (((FinalMagMSE - InitialMagMSE) > 0.3) || (FinalMagMSE < 0.05))
		MagOK = true;


	if(!(PhaseOK && MagOK))			// Coefficients have a problem
	{
		// Generate error message.
		StringBuilder^ msg = gcnew StringBuilder("Open - short 3m compensator coefficients did not converge properly.");
		msg->AppendLine();

		msg->AppendFormat("Initial PhaseMSE = {0}   Final PhaseMSE = {1}", InitialPhaseMSE.ToString(), FinalPhaseMSE.ToString());
		msg->AppendLine();

		msg->AppendFormat("Initial MagMSE = {0}   Final MagMSE = {1}", InitialMagMSE.ToString(), FinalMagMSE.ToString());
		msg->AppendLine();

		msg->AppendLine("The Raw data will be written to a debug text file: VNADebugOSComp.txt");
		MessageBox::Show(msg->ToString(), "Ripple Calibration Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
	}
	if(true)			// Always write out the model parameters
	{
		// dump the traces to a file
		FileStream^ fs;
		StreamWriter^ sw;

		CultureInfo^ CI = gcnew CultureInfo("en-us");		// Over-ride local numeric formatting culture to generate
															// US English-formatted strings for text debug file.
															// Necessary due to how Excel imports from CSV files

		fs = gcnew FileStream("VNADebugOSComp.txt", FileMode::Create, FileAccess::Write);
		sw = gcnew StreamWriter(fs);

		sw->Write("Ripple Cal debug - dump of raw traces.");

		StringBuilder^ prt = gcnew StringBuilder("");

		prt->AppendFormat("Initial PhaseMSE = {0}   Final PhaseMSE = {1}  ", InitialPhaseMSE.ToString(CI), FinalPhaseMSE.ToString(CI));
		prt->AppendFormat(" PhaseDCOffset = {0} ", PhaseDCOffset.ToString(CI));
		prt->AppendFormat("Initial MagMSE = {0}   Final MagMSE = {1}  ", InitialMagMSE.ToString(CI), FinalMagMSE.ToString(CI));
		prt->AppendFormat(" MagDCOffset = {0} ", MagDCOffset.ToString(CI));

		for(int i=0; i<6; i++)	// print out the Magnitude compensator coefficients
		{
			prt->AppendFormat("MagRipCoeff[{0}]->mag = {1} ", i, MagRipCoeff[i]->mag.ToString(CI));
			prt->AppendFormat("MagRipCoeff[{0}]->freq = {1} ", i, MagRipCoeff[i]->freq.ToString(CI));
			prt->AppendFormat("MagRipCoeff[{0}]->offset = {1} ", i, MagRipCoeff[i]->offset.ToString(CI));
			prt->AppendLine();
		}

		for(int i=0; i<6; i++)	// print out the Phase compensator coefficients
		{
			prt->AppendFormat("PhRipCoeff[{0}]->mag = {1} ", i, PhRipCoeff[i]->mag.ToString(CI));
			prt->AppendFormat("PhRipCoeff[{0}]->freq = {1} ", i, PhRipCoeff[i]->freq.ToString(CI));
			prt->AppendFormat("PhRipCoeff[{0}]->offset = {1} ", i, PhRipCoeff[i]->offset.ToString(CI));
			prt->AppendLine();
		}

		sw->Write(prt->ToString());

		sw->WriteLine(" Start of Raw Dump: Index  openAngle  phaseError  magError");

		for (int i=0; i<PHASECALGRIDSIZE; i++)
		{
			sw->Write(i.ToString(CI)); sw->Write(", ");	// comma delimit the table
			sw->Write(openAngle[i].ToString(CI)); sw->Write(", ");
			sw->Write(phaseError[i].ToString(CI)); sw->Write(", ");
			sw->WriteLine(magError[i].ToString(CI));
		}

		sw->WriteLine(" Start of computed error model signatures. Index  phaseCorrection  magCorrection");

		for (int i=0; i<PHASECALGRIDSIZE; i++)
		{
			__int64 freq = Cal->GetFreqFromPhaseCalGrid(i);
			double phcor = PeriodicModelCorrection(freq, openAngle[i], PhRipCoeff, PhaseDCOffset);
			double magcor = PeriodicModelCorrection(freq, openAngle[i], MagRipCoeff, MagDCOffset);

			sw->Write(i.ToString(CI)); sw->Write(", ");	// comma delimit the table
			sw->Write(phcor.ToString(CI)); sw->Write(", ");
			sw->WriteLine(magcor.ToString(CI)); 
		};

		sw->Flush();	
		sw->Close();	// close VNADebugCompOS.txt file
	}

	// Always allow the calibration to complete, even if we had poor convergence.
	RippleCalibrated = true;		// calibration completed, can now use these compensators
	return true;					// sucessful convergence
}

// Update parameter estimates by descending along the gradient   NEW 09-23-2007
void DirectionalCoupler::Descend(array<Double>^ partialDerivative, array<Double>^ weight,
		array<PerErrModelCoeff^>^ coeff)
{
	// The partialDerivative array is the n-dimensional gradient at the point
	// specified by current value of the coefficients. Subtracting partial*weight
	// from the coefficient array moves the model in the direction opposite the
	// gradient (descent). 

	// This routine gets called by both the phase estimator and mag estimator.
	// The phase error vector has both a length and direction (mag & offset angle),
	// and the magnitude error vector has length and direction components as well.
	
	for(int i=0; i<6; i++)
	{
		coeff[i]->offset -= partialDerivative[i] * weight[i];			// angle of error vec (degrees)
		coeff[i]->mag -= partialDerivative[i+6] * weight[i+6];			// length of error vec (dB)
	}
};

// Compute error predicted by point-wise periodic error model
double DirectionalCoupler::PeriodicModelCorrection(__int64 frequency, double phase, array<PerErrModelCoeff^>^ coeff,
		double DCOffset)
{
	// Derive correction magnitude using periodic model from phase and frequency inputs.
	// The model has an offset angle and magnitude at 6 discrete frequencies, and presumes
	// a cosine shape with frequency-dependent magnitude and offset angle.
	// Linearly interpolate between the 6 frequency points.

	double correctionAngle;
	double correctionPhAmp;
	double correction;

	if(frequency < coeff[0]->freq)	
	{
		correctionAngle = NormalizePhaseDegr(phase - coeff[0]->offset);
		correctionPhAmp = coeff[0]->mag;
	}
	else if (frequency < coeff[1]->freq)
	{
		correctionAngle = NormalizePhaseDegr(phase - Interpolate(frequency, coeff[0]->freq, coeff[0]->offset, coeff[1]->freq, coeff[1]->offset));
		correctionPhAmp = Interpolate(frequency, coeff[0]->freq, coeff[0]->mag, coeff[1]->freq, coeff[1]->mag);
	}
	else if (frequency < coeff[2]->freq)	
	{
		correctionAngle = NormalizePhaseDegr(phase - Interpolate(frequency, coeff[1]->freq, coeff[1]->offset, coeff[2]->freq, coeff[2]->offset));
		correctionPhAmp = Interpolate(frequency, coeff[1]->freq, coeff[1]->mag, coeff[2]->freq, coeff[2]->mag);
	}
	else if (frequency < coeff[3]->freq)
	{
		correctionAngle = NormalizePhaseDegr(phase - Interpolate(frequency, coeff[2]->freq, coeff[2]->offset, coeff[3]->freq, coeff[3]->offset));
		correctionPhAmp = Interpolate(frequency, coeff[2]->freq, coeff[2]->mag, coeff[3]->freq, coeff[3]->mag);
	}
	else if (frequency < coeff[4]->freq)
	{
		correctionAngle = NormalizePhaseDegr(phase - Interpolate(frequency, coeff[3]->freq, coeff[3]->offset, coeff[4]->freq, coeff[4]->offset));
		correctionPhAmp = Interpolate(frequency, coeff[3]->freq, coeff[3]->mag, coeff[4]->freq, coeff[4]->mag);
	}
	else if (frequency < coeff[5]->freq)
	{
		correctionAngle = NormalizePhaseDegr(phase - Interpolate(frequency, coeff[4]->freq, coeff[4]->offset, coeff[5]->freq, coeff[5]->offset));
		correctionPhAmp = Interpolate(frequency, coeff[4]->freq, coeff[4]->mag, coeff[5]->freq, coeff[5]->mag);
	}
	else
	{
		correctionAngle = NormalizePhaseDegr(phase - coeff[5]->offset);
		correctionPhAmp = coeff[5]->mag;
	}

	correction = correctionPhAmp * cos(DEGR2RAD * correctionAngle);

	// Adjust correction by DC Offset of the samples
	correction -= DCOffset;

	return correction;
};

// Compute SumSqErr of model compared to error signature
double DirectionalCoupler::SumSquareError(int startI, int stopI, array<Double>^ angle, array<Double>^ errSignature,
		array<PerErrModelCoeff^>^ modelCoeff, double DCOffset)
{
	// Compute how well the model & coefficients match the error signature.
	// Lower SSE means closer match. Mag and Phase have widely different scale factors.

	double sumSqErr = 0.0;		// accumulated residual squared error

	for(int i=startI; i<stopI; i++)	// add up squared error over the frequency range of interest 
	{
		__int64 frequency = Cal->GetFreqFromPhaseCalGrid(i);
		double phase = angle[i];
		double errorValue = errSignature[i];

		double correction = PeriodicModelCorrection(frequency, phase, modelCoeff, DCOffset);

		double residualError = errorValue + correction; // correction is opposite sign to error

		sumSqErr += residualError * residualError;
	}

	return sumSqErr;	// sum Sq Error
};

// Compute gradient of the model parameter set vs. the SumSqrError of the fit
void DirectionalCoupler::Gradient(array<Double>^ partialDerivative, int startI, int stopI,
		array<Double>^ angle, array<Double>^ errSignature, array<PerErrModelCoeff^>^ modelCoeff, double DCOffset)
{
	// Vary each parameter (dPar) independently and record the change in the MSE (dMSE) as fraction.
	// partialDerivatives is the array of 6 partial derivatives. [0..5] are the offset angle derivatives,
	// and [6..11] are the magnitude derivatives.

	const double dAngle = 0.01;	// small angle increment
	const double dMag = 0.01;	// small magnitude increment
	double dMSE;				// resulting minimum square error change

	double baseMSE = SumSquareError(startI, stopI, angle, errSignature, modelCoeff, DCOffset);

	for (int i=0; i<6; i++)
	{
		modelCoeff[i]->offset += dAngle;
		dMSE = SumSquareError(startI, stopI, angle, errSignature, modelCoeff, DCOffset) - baseMSE;
		partialDerivative[i] = dMSE / dAngle;	// partial derivative
		modelCoeff[i]->offset -= dAngle;

		modelCoeff[i]->mag += dMag;
		dMSE = SumSquareError(startI, stopI, angle, errSignature, modelCoeff, DCOffset) - baseMSE;
		partialDerivative[i+6] = dMSE / dMag;	// partial derivative
		modelCoeff[i]->mag -= dMag;
	}
};

