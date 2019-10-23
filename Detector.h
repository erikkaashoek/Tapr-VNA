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

// Routines to encapsulate the magnitude/phase detector IC
// Refl and Tran detector routines are starting to diverge.
//
//	09-03-2007  TC McDermott
//


#include "stdafx.h"
#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::IO;
using namespace System::Windows::Forms;

	/// Analog Devices AD8302 device-dependent constants
public ref class Detector
{
public:
	int maxMagnitude;					///< maximum magnitude reading
	float unitsPerdB;				///< magnitude ADC counts per dB. (~57.0)
	array<Double>^ pherror;				///< phase error for each degree
	array<Int32,2>^ MagTable;				///< refl magnitude raw data table
	array<Double>^ m;					///< y = mx + b  from linear regression, by frequency
	array<Double>^ b;					///< y = mx + b from linear regression, by frequency
	array<Double>^ r;					///< 'r' is the correlation coefficient for the regression, by frequency
	array<Double>^ flat;					///< ADC count where amp detector bottoms out
	static const int t1 = 110;			// twiddle factors for detector floor correction
	static const int t2 = 110;
	String^ name;						// Name of detector (TxDet or RxDet)

										// New 06-09-2007
	double diffQDelayNs;				///< differential time delay of Q-reference compared to I-reference
	int Iasym, Qasym, IasymLo;					///< Phase detector lead/lag asymmetry

	int Qmin, Qmax, Imin, Imax;			/// Phase Detector mins and maxes - New 06-16-2007
	int Qmid, Imid, Qmag, Imag;			/// derived detector center lines & half-range

	int IminLo, ImaxLo, ImidLo, ImagLo;	/// derived I phase detector low ref level mins and maxes  - New 09-30-2007
	
	int beginIndex, endIndex;			/// Index of 90-degrees and 90+360 degrees of the shorted cal cable

	bool phaseCalibrated;				///< true if phase values have been calibrated
	InstrumentCalDataSet^ Cal;

	Detector(InstrumentCalDataSet^ Cal);// Constructor, allocate default values
	bool PhaseCal(array<UInt16>^ Iphase, array<UInt16>^ Qphase, array<UInt16>^ IphaseLo);	// construct phase points from raw data
	bool AmpCal(array<UInt16,2>^ Mag);		// construct amplitude calibration table from raw data
	double IQtoDegrees(int I, int Q, int Freq, double MagDb, int ILo, int QLo);	// convert I/Q ADC reading to degrees
	double MagTodBRefl(int Freq, int Mag);  // convert detector magnitude count to dB. via piecewise linear fit
	double MagTodBTran(int Freq, int Mag, int LowMag, int MidMag);	// convert detector magnitude count to dB. via table lookup
	double CompensateQDelay(double Degrees, int Freq);  // compensate phase reading for Q-reference delay
	double CompensateAsymmetry(double Degrees);	// compensate phase reading for detector assymmetry
};


