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

//	Routines to encapsulate directional coupler compensation


#include "stdafx.h"
#include "DisplayRoutines.h"
#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::ComponentModel;
using namespace System::IO;
using namespace System::Windows::Forms;

ref class InstrumentCalDataSet;			//so DirectionalCoupler class can forward-reference InstrumentCalDataSet

/// Set of coefficients for 6-point periodic error model
public ref struct PerErrModelCoeff {
	int freq;							///< frequency where set applies
	double offset;						///< offset angle of the correction in degrees
	double mag;							///< peak magnitude of the correction
};

/// Encapsulate directional coupler error model
public ref class DirectionalCoupler
{
public:

	//	double calCableSurgeRealOhms, calCableSurgeImagOhms;	// real and imaginary parts of cal cable surge impedance

	array<Int32>^ DirMag;					// Directivity cal raw data
	array<Int32>^ DirIphs;					// Directivity cal raw data
	array<Int32>^ DirQphs;					// Directivity cal raw data

	array<PerErrModelCoeff^>^ PhRipCoeff;	// Array of phase ripple coefficients
	array<PerErrModelCoeff^>^ MagRipCoeff;	// Array of mag ripple coefficients

	array<Double>^ phaseError;			// Array of open-short angle error for LF phase computation
	array<Double>^ magError;				// Array of open-short magnitude for magnitude compensation
	array<Double>^ openAngle;			// Array of open angle for LF phase computation
	array<Double>^ shortAngle;			// Array of short angle for LF phase computation

	bool DirCalibrated;					///< true if Directivity Calibration has been run or values loaded from cal file
	bool RippleCalibrated;				///< true if Mag & Phase ripple calibration has been run or values loaded from cal file

	double PhaseDCOffset;				///< Value of phase error offset (sum of all  phaseError/numPoints)
	double MagDCOffset;					///< Value of mag error offset (sum of magError/numPoints)
	InstrumentCalDataSet^ Cal;			///< Cal data 


DirectionalCoupler(InstrumentCalDataSet^ Cal);	// constructor
	// store coupler directivity calibration raw readings
void DirectivityCal(array<UInt16,2>^ readings);
	// Compensate S11 reading for Coupler Directivity
void CompensateDirectivity(InstrumentCalDataSet^ cal, double& rmagnitudeLin, double& rphase, int Freq); 
	// Generate table of unit-length ripple compensation vectors
bool GenerateShortOpenComp(InstrumentCalDataSet^ cal, array<UInt16,2>^ shortedrefl , array<UInt16,2>^ openrefl);
double PhaseRippleCompensate(double phase, int frequency);
	// Compensate coupler magnitude ripple error   NEW 09-23-2007
double MagRippleCorrection(double phase, int frequency);
	// Update parameter estimates by descending along the gradient   NEW 09-23-2007
void DirectionalCoupler::Descend(array<Double>^ partialDerivative, array<Double>^ weight, array<PerErrModelCoeff^>^ coeff);
	// Compute error predicted by point-wise periodic error model
double DirectionalCoupler::PeriodicModelCorrection(__int64 frequency, double phase, array<PerErrModelCoeff^>^ PerErrModelCoeff,
		double DCOffset);
	// Compute SumSqErr for model compared to error signature
double DirectionalCoupler::SumSquareError(int startI, int stopI, array<Double>^ angle,
		array<Double>^ errSignature, array<PerErrModelCoeff^>^ coeff, double DCOffset);
	// Compute gradient of the model parameter set vs. the SumSqError of the fit
void DirectionalCoupler::Gradient(array<Double>^ partialDerivative, int startI, int stopI,
		array<Double>^ angle, array<Double>^ errSignature, array<PerErrModelCoeff^>^  modelCoeff,
		double DCOffset);

};

