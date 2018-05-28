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
// Routines to encapsulate the magnitude/phase detector IC AD8302
// Refl and Tran detector routines are starting to diverge.
//
//	09-03-2007  TC McDermott - moved to a separate file
//	01-06-2008  TC McDermott - added additional error checking on 1m cal cable length
//




#include "stdafx.h"
#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::IO;
using namespace System::Text;
using namespace System::Windows::Forms;

#include <math.h>
#include <complex>
#include "DataDisplay.h"
#include "Constants.h"
#include "DisplayRoutines.h"

#include <float.h>


/// Detector Constructor, allocate default values
Detector::Detector(void)							
{
	unitsPerdB = 57.0;				// default for uncalibrated data
	pherror = gcnew array<Double>(360);	// hold phaserror array
	MagTable = gcnew array<Int32,2>(21, 4);	// hold raw refl Mag count data
	m = gcnew array<Double>(21);			// value of 'm' at each frequency
	b = gcnew array<Double>(21);			// value of 'b' at each frequency
	r = gcnew array<Double>(21);			// value of 'r' at each frequency
	flat = gcnew array<Double>(21);		// value of 'flat' at each frequency

	//beginIndex = 0;				// initialized by constructor
	//endIndex = 0;
	//phaseCalibrated = false;		// values are not yet calibrated

	// Initialize uncalibrated values


	diffQDelayNs = 0.34;			// difference of Q-reference from I-reference, in nanoseconds - initial guess
	Qmin = -100, Qmax = 3700;		// Phase Detector mins and maxes. Initialze to extreme values - New 06-16-2007
	Imin = -100, Imax = 3700;
	Qmid = 1850, Qmag = 1800;		// Initial guesses - these are refined by the cal routine
	Imid = 1850, Imag = 1800;
	Iasym = Qasym = 180;

	IminLo = 1100, ImaxLo = 2570;	// Initial guesses - NEW 09-30-2007
	ImidLo = (IminLo + ImaxLo) / 2;
	ImagLo = ImaxLo - ImidLo;
	IasymLo = 180;					// This value remains at default (unchanged by calibration).

	for (int i=0; i<360; i++)
		pherror[i] = 0.0;			// Filled in by cal routine (or from stored table)

}
	/// construct phase points from raw data
bool Detector::PhaseCal(array<UInt16>^ Iphase, array<UInt16>^ Qphase, array<UInt16>^ IphaseLo)
	{

		// Deglitch the raw readings - but they're usually pretty clean, and this doesn't change
		// anything very often.

		DeGlitch(Iphase, PHASECALGRIDSIZE, true);
		DeGlitch(Qphase, PHASECALGRIDSIZE, true);
		DeGlitch(IphaseLo, PHASECALGRIDSIZE, false);

		// This routine gets called from both TxDet and RxDet, so it
		// has to work for both calibration cases.  Shorted 1-meter reflection
		// cable has roughly similar phase response as 3-meter transmission cable.

		// 06-09-2007
		// IPhase and QPhase are measured on a 1-meter shorted cable.
		// The transmit buffer is driven by I-phase, so it is the reference.
		// Buffer phase distortions above about 5 MHz are linear in frequency, and
		// thus can be ignored for purpose of this calibration routine.
		// Q-phase may have a different time delay than I-phase (of either sign),
		// so the differential time delay must be derived. Q-phase could also have
		// a quadrature error, but it's small. 
		// The phase detector slopes are not symmetrical + and -, the phase detector
		// gain is different between the two slopes. On one unit, the
		// symmetry ~ 13/(13+14) = 0.48


		// New code 06-23-2007.  
		// 1. Find Qmax, Imax - the maxes of the Iphase and Qphase magnitudes.
		//    Limit the search range to 700 Khz - 95 MHz.
		//    Note that the ImaxLo, IminLo parameters are only meaningful for Tran
		//    but not for Refl. We caclulate them both, but only use them for Tran.

		phaseCalibrated = false;
		Qmax = Imax = ImaxLo = 0;				// initialize to most negative value
		Qmin = Imin = IminLo = 4000;			// initialize to most positive value
		for (int i=5; i<950; i++)
		{
			if(Qphase[i] > Qmax)
				Qmax = Qphase[i];
			if(Iphase[i] > Imax)
				Imax = Iphase[i];
			if(IphaseLo[i] > ImaxLo)
				ImaxLo = IphaseLo[i];
			
			if(Qphase[i] < Qmin)
				Qmin = Qphase[i];
			if(Iphase[i] < Imin)
				Imin = Iphase[i];
			if(IphaseLo[i] < IminLo)
				IminLo = IphaseLo[i];
		}

		Qmid = (Qmax + Qmin) / 2;	
		Qmag = Qmax - Qmid;
		Imid = (Imax + Imin) / 2;	
		Imag = Imax - Imid;
		ImidLo = (ImaxLo + IminLo) / 2;		// only meaningful for Tran  10-01-2007
		ImagLo = ImaxLo - ImidLo;			// only meaningful for Tran

		// I-phase first minimum is at frequency index ~ 0 Hertz.
		// Find I-phase second minimum.

		// Because the cable length can vary somewhat, need to bound
		// the various searches for max and min.

		// Estimate the period of the cable from the I-phase center crossings

		int firstImid, secondImid, thirdImid;	// 1st, 2nd, and 3rd mid-value crossing of Iphase
		int firstImax, secondImin;				// 1st local maximum and 2nd local minimum of Iphase
		const int firstImin = 0;				// first min is at zero
		int thirdQmid;							// 3rd mid-value crossing of Qphase
		int firstQmin, firstQmax, secondQmin;	// 1st local min, 1st local max, 2nd local min of Qphase
		int epsilon, Isearch, Qsearch;			// hold local minima and deltas during searches
		int cableIPeriod;
		int stopI, startI;						// stop & start indexes for search
		bool ErrorFlag = false;					// is true if the calibration failed

		// find firstImid
		epsilon = 2000;
		for (int i = 50; i<375; i++)
		{
			if (abs(Iphase[i] - Imid) < epsilon) // update if we're closer to Imid value
			{
				firstImid = i;
				epsilon = abs(Iphase[i] - Imid);
			}
			if (abs(Iphase[i] - Imid) > (epsilon + 400))	// gone well past the minimum
				break;
		}

		// find secondImid
		epsilon = 2000;

		stopI = firstImid*4;
		if (stopI > 1024)				// don't search past the end of the array
			stopI = 1024;

		for (int i = firstImid+50; i<stopI; i++)
			if (abs(Iphase[i] - Imid) < epsilon)
			{
				secondImid = i;
				epsilon = abs(Iphase[i] - Imid);
			};

		// find thirdImid
		epsilon = 2000;

		stopI = firstImid*6;
		if (stopI > 1024)				// don't search past the end of the array
			stopI = 1024;

		for (int i = secondImid+50; i<stopI; i++)
			if (abs(Iphase[i] - Imid) < epsilon)
			{
				thirdImid = i;
				epsilon = abs(Iphase[i] - Imid);
			}

		cableIPeriod = thirdImid - firstImid;
		// find secondImin
		Isearch = 4000;
		stopI = cableIPeriod+100;
		if (stopI > 1024)				// don't search past the end of the array
			stopI = 1024;

		startI = cableIPeriod - 100;	// don't search too low in frequency
		if (startI < 25)
			startI = 25;

		for (int i=startI; i<stopI; i++)
			if (Iphase[i] < Isearch)
			{
				Isearch = Iphase[i];
				secondImin = i;
			};

		// find the firstImax
		Isearch = 0;

		stopI = cableIPeriod/2 + 100;
		if (stopI > 1024)				// don't search past the end of the array
			stopI = 1024;

		startI = cableIPeriod/2 - 100;	// don't search too low in frequency
		if (startI < 25)
			startI = 25;

		for(int i = startI; i<stopI; i++)
			if (Iphase[i] > Isearch)
			{
				Isearch = Iphase[i];
				firstImax = i;
			};

		// First mid-crossing of Q-phase (firstQmid) is assumed to be at zero
	
		// Find third zero-crossing of Qphase
		epsilon = 2000;

		stopI = cableIPeriod+100;
		if (stopI > 1024)				// don't search past the end of the array
			stopI = 1024;

		startI = cableIPeriod - 100;
		if (startI < 25)
			startI = 25;

		for (int i=startI; i<stopI; i++)
			if (abs(Qphase[i] - Qmid) < epsilon)
			{
				thirdQmid = i;
				epsilon = abs(Qphase[i] - Qmid);
			};

		// find firstQmin
		Qsearch = 4000;

		startI = thirdQmid/4 - 100;
		if (startI < 25)				// don't search too low in frequency
			startI = 25;

		stopI = thirdQmid/4 + 100;
		if (stopI > 1024)				// don't search past the end of the array
			stopI = 1024;

		for (int i=startI; i<stopI; i++)
			if (Qphase[i] < Qsearch)
			{
				Qsearch = Qphase[i];
				firstQmin = i;
			};


		// find firstQmax
		Qsearch = 0;
		for (int i=thirdQmid/2; i<thirdQmid; i++)
			if (Qphase[i] > Qsearch)
			{
				Qsearch = Qphase[i];
				firstQmax = i;
			};

		// find secondQmin
		Qsearch = 4000;
		for (int i=thirdQmid; i<1024; i++)
			if (Qphase[i] < Qsearch)
			{
				Qsearch = Qphase[i];
				secondQmin = i;
			};


			
		// Check the results for errors that will prevent the algorithm
		// from converging or giving too little range from which to extrapolate results.
		// This can be caused by the test cable being the wrong length.

		if((cableIPeriod > 800) || (secondQmin > 1022))	// The calibration cable is too short
		{
			MessageBox::Show("Cannot converge phase calibration.\n\r"
				"The cable used may be too short.",
				"Phase Calibration Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			ErrorFlag = true;
			return(false);
		}

		if((cableIPeriod < 325) || (thirdQmid < 325))	// The calibration cable is too long
		{
			MessageBox::Show("Cannot converge phase calibration.\n\r"
				"The cable used may be too long.",
				"Phase Calibration Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			ErrorFlag = true;
			return(false);
		}

		// compare the periods of I and Q, derive Q differential delay

		int iPeriod = secondImin - firstImin;		
		int qPeriod = thirdQmid - 0;
		float deltaPhase = 360.0f * (qPeriod-iPeriod)/(float)iPeriod;
		float deltaFreq = (float)GetFreqFromPhaseCalGrid(qPeriod);
		diffQDelayNs = 1e+9 * deltaPhase/(deltaFreq * 360.0f);  // differential time delay Q-phase compared to I-phase

		// Compute phase detector leading / laging asymmetry for I and Q
		Iasym = (firstImax-firstImin)*360/(secondImin-firstImin);
		Qasym = (firstQmax-firstQmin)*360/(secondQmin-firstQmin);					


		if((Iasym < 160) || (Iasym > 200) || (Qasym < 160) || (Qasym > 200))	// The detector asymmetry is out of range
		{
			StringBuilder^ msg = gcnew StringBuilder("Phase detector asymmetry is out of range.");
			msg->AppendLine("    Normal range = 160 to 200 degrees.");
			msg->AppendLine("Cannot converge detector phase calibration.");
			msg->Append("Detector = ");
			msg->AppendLine(this->name);	// Identify which detector we're calibrating
			msg->AppendFormat("Iasym = {0,4} degrees, Qasym = {1,4} degrees.", Iasym, Qasym);
			msg->AppendLine();

			MessageBox::Show(msg->ToString(), "Phase Calibration Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			ErrorFlag = true;
		}


		// Determine error of measured phase by comparison to the cable. It's not completely
		// periodic, so generate the error every closest degree.
		// pherror[degrees] = difference.
		// The cable phase linearity assumption is good above the HighPassPole frequency
		// of the ModAmp. While the ModAmp buffer time delay introduces phase delay, it's linear with
		// frequency and thus does not affect cable calibration (it's the same effect as the cal cable
		// being slightly longer). The Q-phase differential time delay must be removed to make the
		// error compensation independent of frequency.


		// Find 90 degree point before starting.
		double dblEpsilon = 2000.0;

		startI = firstImid - 60;
		if (startI < 25)				// don't search too low in frequency
			startI = 25;

		stopI = firstImid+60;
		if (stopI > 1024)				// don't search past end of array
			stopI = 1024;

		for (int i= startI; i<stopI; i++)
		{
			double phase = IQtoDegrees(Iphase[i], Qphase[i], 0, 0, 0, 0);
			// remove the frequency-dependent error caused by diffQdelay (since not yet phaseCalibrated).
			// Don't need to look at asymmetry effects yet - since we are going to find the same phase
			// point at 90 and 360+90 the impact of asymmetry will cancel out.
			phase += CompensateQDelay(phase, GetFreqFromPhaseCalGrid(i));
			phase = NormalizePhaseDegr(phase);

			if (abs(phase - 90.0) < dblEpsilon)
			{
				beginIndex = i;
				dblEpsilon = abs(phase - 90.0);
			}
		}

		// Find 90 degree + 360 degree point
		dblEpsilon = 2000.0;

		startI = thirdImid - 60;
		if (startI < 25)					// don;t start too low in frequency
			startI = 25;

		stopI = thirdImid + 60;
		if (stopI > 1024)					// don't search past end of the array
			stopI = 1024;

		for (int i=startI; i<stopI; i++)
		{
			double phase = IQtoDegrees(Iphase[i], Qphase[i], 0, 0, 0, 0);
			//  remove the frequency-dependent error caused by diffQdelay
			phase += CompensateQDelay(phase, GetFreqFromPhaseCalGrid(i));

			if (abs(phase - 90.0) < dblEpsilon)
			{
				endIndex = i;
				dblEpsilon = abs(phase - 90.0);
			}
		}


		for (int i= beginIndex; i<endIndex; i++)		// sweep over 360 degrees - from +90 to 0 to -180..+180 to -90.
		{
			// Model cable as a time delay. 

			// Force use of normal-ref-level if this is a TRAN run (via 0 values for ILo and QLo)
			double phase = IQtoDegrees(Iphase[i], Qphase[i], 0, 0, 0, 0);

			//  remove the frequency-dependent error caused by diffQdelay
			phase += CompensateQDelay(phase, GetFreqFromPhaseCalGrid(i));
			phase = NormalizePhaseDegr(phase);

			//  remove the angle-dependent error caused by detector asymmetry
			phase += CompensateAsymmetry(phase);
			phase = NormalizePhaseDegr(phase);

			// derive cable's phase using time delay linearity approximation
			double cphase = 90.0 - 360.0 * (double)(i - beginIndex) / (double)(endIndex-beginIndex);
			if (cphase <= -180.0)
				cphase += 360.0;

			double error = cphase - phase;

			error = NormalizePhaseDegr(error);
			pherror[(int)(cphase + 0.5)+179] = error;	// offset index -->  [-179 .. +180] is the index range
		}


		// Normalize the phase error tables so that 0 degrees has no correction.
		double offset = pherror[179];		// correction at zero degrees
		for (int i=0; i<360; i++)
			pherror[i] -= offset;

		// Sweep through table and clean up the correction glitches.
		// Uusually at 0 or 359 degrees, but also a sometimes near the start/stop point
		// of the sweep (-90, +270 degrees), or where the sweep wraps from 180 to -180.
		SmoothPhaseTable(pherror);


		// Dump the traces to a file for debugging purposes
		FileStream^ fs;
		StreamWriter^ sw;

		fs = gcnew FileStream("VNACalDebug.txt", FileMode::Create, FileAccess::Write);
		sw = gcnew StreamWriter(fs);

		sw->Write("PhaseCal debug - dump of raw traces. Detector type = ");
		sw->WriteLine(this->name);

		StringBuilder^ prt = gcnew StringBuilder("");
		prt->AppendFormat(": Imin = {0,5}, Imid = {1,5}, Imax = {2,5},", Imin, Imid, Imax);
		prt->AppendFormat(" Imag = {0,5}", Imag);
		prt->AppendLine();

		prt->AppendFormat(": IminLo = {0,5}, ImidLo = {1,5}, ImaxLo = {2,5},", IminLo, ImidLo, ImaxLo);
		prt->AppendFormat("  ImagLo = {0,5}", ImagLo);
		prt->AppendLine();

		prt->AppendFormat(": Qmin = {0,5}, Qmid = {1,5}, Qmax = {2,5},", Qmin, Qmid, Qmax);
		prt->AppendFormat("  Qmag = {0,5}", Qmag);
		prt->AppendLine();

		prt->AppendFormat(": firstImid = {0,5}, firstImin = {1,5}, secondImid = {2,5},", firstImid, firstImin.ToString(), secondImid);
		prt->AppendFormat("  thirdImid = {0,5}", thirdImid);
		prt->AppendLine();

		prt->AppendFormat(": firstImax = {0,5}, secondImin = {1,5}", firstImax, secondImin);
		prt->AppendLine();

		prt->AppendFormat(": firstQmin = {0,5}, firstQmax = {1,5}, secondQmin = {2,5},", firstQmin, firstQmax, secondQmin);
		prt->AppendFormat("  thirdQmid = {0,5}", thirdQmid);
		prt->AppendLine();

		sw->Write(prt);
		sw->WriteLine("Start of raw dump");

		sw->WriteLine("Index  Iphase  Qphase  IphaseLo");
		for (int i=0; i<1024; i++)
		{
			sw->Write(i.ToString()); sw->Write(", ");	// comma delimit the table
			sw->Write(Iphase[i].ToString()); sw->Write(", ");
			sw->Write(Qphase[i].ToString()); sw->Write(", ");
			sw->WriteLine(IphaseLo[i].ToString());
		}

		sw->Flush();	
		sw->Close();	// close VNACalDebug.txt file
	
		if(ErrorFlag)					// If something failed
		{
			// generate error message
			StringBuilder^ str;
			str	= gcnew StringBuilder("Cable ranging failed. \n\r"
			"Cannot converge detector phase calibration.  Detector = ");

			str->Append(this->name);	// Identify which detector we're calibrating
			str->Append("\n\r");
			str->Append("Generating dump of raw data in the file  VNACalDebug.txt\n\r");
			
			MessageBox::Show(str->ToString(), "Phase Calibration Error", MessageBoxButtons::OK, MessageBoxIcon::Error);

			phaseCalibrated = false;
			return(false);	// calibration failed

		}	// end if ErrorFlag

		phaseCalibrated = true;
		return(true);
	}
/// Construct amplitude calibration table from raw Magnitude data
bool Detector::AmpCal(array<UInt16,2>^ Mag)
{

	int freq; int level;
	int sumx, sumy, sumxsq, sumysq, sumxy, n;
	int sumf;
	n = 26;											// number of points in regression
    
	//  Use least-squares linear regression to compute  'm' and 'b'  in  "y = mx + b"
	//  'x' is the raw DAC count, 'y' is the value of dbm derived from that raw count. 
	//  Best value for correlation coefficient, 'r' is when the range 0..-25 dbm is used
	//  NB: level in dbm is opposite sign from the array index.
	//  Also compute the level where the curve flatens out (frequency-dependent).

	for (freq = 0; freq<21; freq++)		// compute at each frequency index
	{
		sumx = 0; sumy = 0;
		sumxsq = 0; sumysq = 0; sumxy = 0;
		sumf = 0;

		for (level=0; level<26; level++)			// compute from 0 dbm to -25 dbm 
		{
            sumx += Mag[freq, level];
			sumy += -level;
			sumxsq += Mag[freq, level] * Mag[freq, level];
			sumysq += level * level;
			sumxy += Mag[freq, level] * -level;
		}
		m[freq] = (double)(n * sumxy - sumx * sumy) / (double)( n * sumxsq - sumx * sumx);
		b[freq] = (double)(sumy - m[freq] * sumx) / (double)n;
		r[freq] = (double)(n * sumxy - sumx * sumy) / sqrt((double)(n * sumxsq - sumx * sumx)* (n * sumysq - sumy * sumy));

		for (level=50; level<=59; level++)			// find count where detector curve flatens out
         	sumf += Mag[freq, level];				// Take average of bottom 10 level readings
		
		flat[freq] = (double)sumf/10.0;

		// 10-24-2005  Adjust 'b' (intercept) to align with 0 db.
		// b[freq] -=  m[freq] * Mag[freq, 0] + b[freq] - pow(10.0, ((flat[freq] + t1 - Mag[freq, 0]) / t2));
	}

		// Test whether the AmpCal correlation (r) is good or poor from 500 KHz to 100 MHz

		for (freq=3; freq<19; freq++)
		{
			if(r[freq] < 0.92)		// poor correlation between cal measurememnts and detector model
				return(false);
		}
		return(true);				// good correlation

}
/// Convert VNA I/Q raw count readings to degrees
double Detector::IQtoDegrees(int I, int Q, int Freq, double MagDb, int ILo, int QLo)
{
	// New method - use Max, Min, Sin(), Arctan()   06-16-2007

	double x, y, degrees;

	// If signal is REFL, or if TRANS and less than -20 dB, or ILo or Qlo are not available 
	if ((MagDb <= -20.0) || (String::Compare(this->name, "REFL") == 0) || (ILo == 0) || (QLo == 0))
	{
		x = Math::PI / 2.0 * (double)(I - Imid)/(double)Imag;
		y = Math::PI / 2.0 * (double)(Q - Qmid)/(double)Qmag;

		//x = sin(x);
		//y = -sin(y);

		degrees = RAD2DEGR * atan2(y,x);
		degrees = NormalizePhaseDegr(degrees);

		// During initial detector calibration, or if no detector cal has been run
		// the additional compensators are not used.
		if (phaseCalibrated)
		{
			degrees += CompensateQDelay(degrees, Freq);
			degrees = NormalizePhaseDegr(degrees);

			degrees += CompensateAsymmetry(degrees);
			degrees = NormalizePhaseDegr(degrees);

			degrees += pherror[(int)(degrees+179)];	// index to phase table is  -179 to +180 degrees
			degrees = NormalizePhaseDegr(degrees);
		}

	}
	else		// TRAN receiver low-ref  (hi-magnitude rcv signal) condition
	{
		x = (double)(ILo - ImidLo)/(double)ImagLo;  // Uses new ILow parameters
		y = Math::PI / 2.0 * (double)(QLo - Qmid)/(double)Qmag;		 // Q parameters are same as before

		// x = sin(x);  I-phase detector is a sine in the TRAN-LoRef condition, so no need to convert it.
		//y = -sin(y);

		degrees = RAD2DEGR * atan2(y,x);
		degrees = NormalizePhaseDegr(degrees);

		if (phaseCalibrated)
		{
			degrees += CompensateQDelay(degrees, Freq);
			degrees = NormalizePhaseDegr(degrees);

			// Only compensate the Qasymmetry value. I is forced to perfectly symetrical
			degrees += CompensateAsymmetry(degrees);
			degrees = NormalizePhaseDegr(degrees);

			// Correct phase from table
			degrees += pherror[(int)(degrees+179)];	// index to phase table is  -179 to +180 degrees
			degrees = NormalizePhaseDegr(degrees);
		}
	}

	// If degrees is not_a_number then subsequent array index operations will fail
	if (Double::IsNaN(degrees))
		throw gcnew System::ArithmeticException(" Detector::IQtoDegrees   degrees is not_a_number ");
	return (degrees);


}/// Convert detector magnitude count to dB. via piecewise linear fit
/// Convert magnitude reading to dB of detector response
/// by interpolation of Detector Calibration points
/// Does not model the directional coupler.
double Detector::MagTodBRefl(int Freq, int Mag)
	{
		double result;
		int FreqBase, FreqBaseNext;		// Two adjacent cal frequency points
		double FreqRemainder;			// remainder of freq between cal points
		double dBResultFLow, dBResultFHigh;	// Intermediate interpolation results

		if (phaseCalibrated)
		{
			// Result is frequency-dependent. Interpolate the frequency between two datasets to construct
			// a linear-frequency approximation of the cal data for the desired frequency.
			// Every 200 KHz from 200K to 1MHz, then every 10 MHz up to 120 MHz.
			// Index:  0    1    2    3    4    5    6    7    8   9   10   11   12   13   14   15   16   17    18    19 
			// Freq:  200k 300k 400k 500k 600k 700k 800k 900k 1M  10M  20M  30M  40M  50M  60M  70M  80M  90M  100M  110M 


			// Build interpolated dataset between two frequencies

			if (Freq <= 1000000)	// 1 MHZ and below
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


		// Mag is the S11 raw ADC count measured with +0 dbm Tx Level
		// Compute dbm from y = mx + b.  Coefficients previously derived from linear regression

		dBResultFLow = m[FreqBase] * Mag + b[FreqBase];
		dBResultFHigh = m[FreqBaseNext] * Mag + b[FreqBaseNext];

		// Tweak the bottom end of the reading for detector bottoming out.  Based on 'flat' and
		// emperically derived correction.

		dBResultFLow = dBResultFLow - pow(10.0, ((flat[FreqBase] + t1 - Mag) / t2));
		dBResultFHigh = dBResultFHigh - pow(10.0, ((flat[FreqBaseNext] + t1 - Mag) / t2));

        // interpolate result between FLow and FHigh

		result = dBResultFLow + (dBResultFHigh - dBResultFLow) * FreqRemainder;

		return(result);

		}
		else									// uncalibrated measurement
			return(((Mag-MAXTRANMAGNITUDE)/57.0));			// roughly 57 counts per dB
	}

/// Convert detector magnitude count to dB. via table lookup
double Detector::MagTodBTran(int Freq, int Mag0, int Mag34, int Mag17)
	{
		double result;
		int FreqBase, FreqBaseNext;		// Two adjacent cal frequency points
		double FreqRemainder;			// remainder of freq between cal points
		double dBResultFLow, dBResultFHigh;	// Intermediate interpolation results
		const int t1 = 110;				// twiddle factors for detector floor correction
		const int t2 = 110;

		if (phaseCalibrated)
		{
			// Result is frequency-dependent. Interpolate the frequency between two datasets to construct
			// a linear-frequency approximation of the cal data for the desired frequency.
			// Every 200 KHz from 200K to 1MHz, then every 10 MHz up to 120 MHz.
			// Index:  0    1    2    3    4    5    6    7    8   9   10   11   12   13   14   15   16   17    18    19 
			// Freq:  200k 300k 400k 500k 600k 700k 800k 900k 1M  10M  20M  30M  40M  50M  60M  70M  80M  90M  100M  110M 

			if (Freq <= 1000000)	// 1 MHZ and below
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

// New algorithm for V1.5 - use Mag0 until resultant amplitude is > 0 db,
// then use Mag17 until it's resultant amplitude is > 0 db, then use Mag34
		// Mag0 is the S21 raw ADC count measured with +0 dbm Tx Level
		// Mag17 is the S21 raw ADC count measured with -17 dbm Tx Level
		// Mag34 is the S21 raw ADC count measured with -34 dbm Tx Level

		// copy to local values so we can debug
		int Hi, Mid, Lo;
		Hi = Mag0, Mid = Mag17, Lo = Mag34;

		// Compute dbm from y = mx + b.  Coefficients previously derived from linear regression

		dBResultFLow = m[FreqBase] * Mag0 + b[FreqBase];
		dBResultFHigh = m[FreqBaseNext] * Mag0 + b[FreqBaseNext];

		// Tweak the bottom end of the reading for detector bottoming out.  Based on 'flat' and
		// emperically derived correction.

		dBResultFLow = dBResultFLow - pow(10.0, ((flat[FreqBase] + t1 - Mag0) / t2));
		dBResultFHigh = dBResultFHigh - pow(10.0, ((flat[FreqBaseNext] + t1 - Mag0) / t2));

        // interpolate result between FLow and FHigh

		result = dBResultFLow + (dBResultFHigh - dBResultFLow) * FreqRemainder;

		if(result <= 0.0)
			return(result-(double)TARGETLOMAG-6.0);


		// Compute dbm from y = mx + b.  Coefficients previously derived from linear regression

		dBResultFLow = m[FreqBase] * Mag17 + b[FreqBase];
		dBResultFHigh = m[FreqBaseNext] * Mag17 + b[FreqBaseNext];

		// Tweak the bottom end of the reading for detector bottoming out.  Based on 'flat' and
		// emperically derived correction.

		dBResultFLow = dBResultFLow - pow(10.0, ((flat[FreqBase] + t1 - Mag17) / t2));
		dBResultFHigh = dBResultFHigh - pow(10.0, ((flat[FreqBaseNext] + t1 - Mag17) / t2));

        // interpolate result between FLow and FHigh

		result = dBResultFLow + (dBResultFHigh - dBResultFLow) * FreqRemainder;

		if(result<= 0.0)
			return(result-(double)TARGETMIDMAG-6.0);

		
		// Compute dbm from y = mx + b.  Coefficients previously derived from linear regression

		dBResultFLow = m[FreqBase] * Mag34 + b[FreqBase];
		dBResultFHigh = m[FreqBaseNext] * Mag34 + b[FreqBaseNext];

		// Tweak the bottom end of the reading for detector bottoming out.  Based on 'flat' and
		// emperically derived correction.

		dBResultFLow = dBResultFLow - pow(10.0, ((flat[FreqBase] + t1 - Mag34) / t2));
		dBResultFHigh = dBResultFHigh - pow(10.0, ((flat[FreqBaseNext] + t1 - Mag34) / t2));

        // interpolate result between FLow and FHigh

		result = dBResultFLow + (dBResultFHigh - dBResultFLow) * FreqRemainder;

		return(result-6.0);

		}
		else												// uncalibrated measurement
			return(((Mag34-MAXTRANMAGNITUDE)/57.0) + 8.0);			// roughly 57 counts per dB

	}

/// Compensate for Q-reference time delay compared to I-reference
double Detector::CompensateQDelay(double Degrees, int Freq)  // compensate phase reading for Q-reference delay
{
	// compensate the phase detector reading to account for time delay of Q-reference 
	// compared to the I-reference. Freq=0 causes no correction.

	// Convert Q delay to degrees at Freq.   time = -dPhase/dFreq  => ph = t * fr.
	// Positive Q delay causes phase error to be negative.

	double delayDegr = 360.0 * diffQDelayNs * (double)Freq * 1e-9;
	double indexDegr = Degrees * 2;				// error is periodic at 2-per-cycle
	indexDegr += delayDegr/2;						// advance or retard the correction index
	indexDegr = NormalizePhaseDegr(indexDegr);
	double correction = delayDegr / 2.0 * ( - 1.0 - cos( DEGR2RAD * indexDegr));

	return correction;
}

/// Compensate phase reading for detector assymmetry
double Detector::CompensateAsymmetry(double Degrees)
{
	// Compensate for I and Q detector asymmetry.
	// Correction is double-period effect

	double Imag = (180 - Iasym)/2;
	double Qmag = (Qasym - 180)/2;
	double indexDegr = Degrees * 2;

	double Icomp = Imag/2 * (1.0 -cos(DEGR2RAD * indexDegr));
	double Qcomp = Qmag/2 * (1.0 +cos(DEGR2RAD * indexDegr));

	return (Icomp+Qcomp);

};			
