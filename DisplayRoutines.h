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

/// Utility routines to convert VNA measurements into other
/// useful types and coordinates.
// 


#include "stdafx.h"
//#include "DirectionalCoupler.h"
#include "USB_EZ_interface.h"

//#include "Detector.h"
#include "DataDisplay.h"
#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::IO;
using namespace System::Windows::Forms;


/// Convert Magnitude to rectangular Y display coordinate
int ToDisplayRectMag(double magnitude, System::Drawing::Rectangle scopeDisp, float dbScaleFactor, int refLevel);

/// Convert Phase to rectangular Y display coordinate
int ToDisplayRectPhs(double phase, System::Drawing::Rectangle scopeDisp);

/// Convert Group Delay to rectangular Y display coordinate
int ToDisplayRectGD(double groupdelay, System::Drawing::Rectangle scopeDisp, int scaleFactor);

/// Convert Magnitude and Phase to polar (X,Y) display coordinates
void ToDisplayPolar(double magnitude, double phase, int polarRad, int xoffset, int yoffset, int& X, int& Y);

/// Convert Resistance portion of S11 to Y display coordinate
int ToDisplayRectR(float resistance, int scale, System::Drawing::Rectangle scopeDisp);

/// Convert Reactance portion of S11 to Y display coordinate
int ToDisplayRectjX(float reactance, int scale, System::Drawing::Rectangle scopeDisp);

int ToDisplayRectScaled(double value, System::Drawing::Rectangle scopeDisp, int scaleFactor);

/// Compensate for ModAmpbuffer amplifer phase distortions
double BufferAmpPhaseComp(double phaseDegrees, int Frequency);

/// Frequency Grid Class
public ref class FrequencyGrid
{
private:
	array<__int64>^ FrequencyIndex;		///< Grid of frequencies to measure
	__int64 startFreq;						///< Starting (lowest) Frequency of grid
	__int64 stopFreq;						///< Stopping (highest) Frequency of grid
	int indexer;						///< internal variable to index through grid
	double delta;						///< Frequency separation between points in the grid
public:
	int points;							///< Number of points in the frequency grid
	int ferror;							///< Frequency error of internal crystal at 100 Mhz (from calibration)

public:
	FrequencyGrid(int numPoints);		///< Constructor, allocate array
	void SetStartF(__int64 start);			///< Set start frequency of grid
	void SetStopF(__int64 stop);			///< Set stop frequency of grid
	__int64 Frequency(int gridpoint);		///< convert gridpoint to it's frequency
	long long int DDS(__int64 Frequency);	///< Derive DDS divisor value from Frequency
	int GridPoint(__int64 Frequency);		///< convert Frequency to gridpoint
	int Points();						///< get number of points in grid
	__int64 StartF();						///< get start frequency of grid
	__int64 StopF();						///< get stop frequency of grid
	int Ferror();						///< get Frequency error
	void set_Ferror(int FreqError);		///< set Frequency error

private:
	void Build(void);
};

/// Object to hold Calibration data, raw and computed. Holds both Instrument and Fixture Cal Data
public ref class InstrumentCalDataSet
{
public:

//	Detector^ RxDet, ^TxDet;		///< Holds detector constants
//	DirectionalCoupler^ DirCoupler;		///< Holds Directional coupler error model

	// Fixture Cal data (may not be any)
	array<Double>^ EdReal, ^EdImag, ^EsReal, ^EsImag;
	array<Double>^ EtReal, ^EtImag, ^ThReal, ^ThImag;
	array<Double>^ S11shortReal, ^S11shortImag;
	array<Double>^ S11openReal, ^S11openImag;
	array<Double>^ S11termReal, ^S11termImag;
	bool FixtureCalLogFreqMode;
	__int64 minCalFreq, maxCalFreq;
	VNADevice^ VNA;

	int FreqError;					// Internal Crystal Frequency Error
	double reflTimeDelayEquivalent; // Equivalent time delay of reflection cable go and return (for RefExtn - not enabled)

	InstrumentCalDataSet(String^ StartUpDir, 
					 VNADevice^ VNADev			///< VNA Hardware Device
						);
/// resolve reflected measured data set to Magnitude and Phase
void ResolveReflPolar(MeasurementSet^ dataPoint, __int64 Freq, double& rmag, double& rphs, bool CouplerComp);
/// resolve transmitted measured data set to Magnitude and Phase
void ResolveTranPolar(MeasurementSet^ dataPoint, __int64 Freq, double& rmag, double& rphs);
/// get frequency of calibration grid point
__int64 GetFreqFromFixtureCalGrid(long index, bool Freqmode);
};


/// Derive error terms from cal measurements
void CalToErrorTerms(InstrumentCalDataSet^ Cal);

/// Convert measured S11 into actual S11 via Fixture calibration
void CorrectS11(InstrumentCalDataSet^ Cal, __int64 Frequency, bool ReflExtn, bool calMode, double measmag, double measphs, double& rsltmag, double& rsltphs);

/// Convert measured S21 into actual S21 via Foixture calibration
void CorrectS21(InstrumentCalDataSet^ Cal, __int64 Frequency, bool calMode, double measmag, double measphs, double& rsltmag, double& rsltphs);

/// Load calibration data from a previously saved file
bool LoadCalDataSet(OpenFileDialog^ infile, InstrumentCalDataSet^ Cal);

/// Save newly acquired calibration data to a file
void SaveCalDataSet(SaveFileDialog^ outfile, InstrumentCalDataSet^ Cal);

/// Convert Log Tx Level to linear value needed by DAC register
unsigned short TxLevLinear(int tranLevel);

/// Convert linear DAC level to dBm.  4095 = 0.0 dBm.
float dBFromLinear(int DAClevel);

/// Convert Return Loss (S11 mag) to SWR, then to vertical display coordinate
int ToDisplayAsSWR(double mag, System::Drawing::Rectangle scopeDisp, int scaledB);

/// Export S Parameters to a file
void ExportSParams(int format,  FrequencyGrid^ FG,	array<Double>^ S11Mag, array<Double>^ S11Phs,
				array<Double> ^S21Mag, array<Double>^ S21Phs, array<Double>^ S22Mag,
				array<Double>^ S22Phs, array<Double>^S12Mag, array<Double>^ S12Phs);

/// Store S Parameters to temporary storage
void StoreSParams(bool calmode, bool ReflExtn, FrequencyGrid^ FG, InstrumentCalDataSet^ CalData,
					array<MeasurementSet^>^ Meas, array<Double>^ reflMag, array<Double>^ reflPhs,
					array<Double>^ tranMag, array<Double>^ tranPhs);

/// Linear interpolation of Xval between Xlow and Xhigh yielding Y result between Ylow and Yhi
double Interpolate(int Xval, int Xlow, double Ylow, int Xhi, double Yhi);
//double Interpolate(int Xval, int Xlow, int Ylow, int Xhi, int Yhi);
//double Interpolate(float Xval, float Xlow, float Ylow, float Xhi, float Yhi);

/// Median filtering routine.  Input is 7 measurements, returns the median value.
int Median7(array<UInt16>^ data,	///< Raw data array from VNA
			int index);					///< Position in the array
///
int Median7(array<Int32>^ data);			///< Integer array
///
int Median7(array<UInt16>^ data);			///< Unsigned Short array
///
double Median7(array<double>^ data,		///< double array
			   int,						///< Position in the array to filter
			   double epsilon = 0.0);	///< Don't filter if error is smaller than epsilon

/// Determine range of middle 5 samples in a group of 7 samples.
int Range										/// \return Max-min range of the readings spanning 5 closest samples
				(array<UInt16>^ data,	///< Array of all measurement samples, one per frequency
				int index						///< Look at the 7 samples surrounding (and including) this index
				);

/// Return actual raw reading if it's close to the expected reading, else the median of the
/// 7 adjacent samples that include the reading.
int ExpectedValue
				 (array<UInt16>^ data,	///< Array of all measurement samples, one per frequency
				  int index,					///< The sample we wish to find the expected value of
				  bool mode = false			///< Switch that allows for phase detector saturation.. true=phase, false=amplitude
				  );

/// Deglitch an entire array of raw VNA readings using ExpectedValue - raw overload
void DeGlitch(array<UInt16>^ raw,			///< Raw readings from VNA
			  int count,				///< Number of points in the set
			  bool mode);				///< phasemode = 1 means phase detector reading, =0 means amplitude reading
/// Deglitch an entire array of raw VNA readings using ExpectedValue - dataset overload
void DeGlitch(array<MeasurementSet^>^ dataSet,	///< Entire MeasurementSet
			  int count);						///< number of points in the MeasurementSet
/// Smooth a phase error table
void SmoothPhaseTable(array<Double>^ table);	///< Table of phase error values (360 degrees table)
/// DrawLine routines that checks and bounds points to display rectangle - rectangle overload
void DrawLineBound(System::Drawing::Graphics^, System::Drawing::Rectangle, System::Drawing::Pen^, int, int, int, int);
/// DrawLine routines that checks and bounds points to display rectangle -  2 points overload
void DrawLineBound(System::Drawing::Graphics^, System::Drawing::Rectangle, System::Drawing::Pen^, System::Drawing::Point, System::Drawing::Point);
/// Add 2 polar numbers
void AddPolar(double& linearMag, double& phaseDegr, double linearMagadd, double phaseDegrAdd);
/// Convert MeasurementDelay string to target's loop count
int MeasureDelayStringToCount(String^  value);
/// Convert Detector Magnitude Calibration Grid index to Frequency.
int GetFreqFromDetMagCalGrid(long index);
/// Convert Phase Calibration Grid index to Frequency.
int GetFreqFromPhaseCalGrid(long index);	
/// Wrap phase to +/- 180 degree range.
int NormalizePhaseDegr(int phase);	///< phase as an int.
/// Wrap phase to +/- 180 degree range.
double NormalizePhaseDegr(double phase);	///< phase as a double.
/// Wrap phase to +/- 180 degree range.
float NormalizePhaseDegr(float phase);		///< phase as a float.

/// Check S21 receive measurements for evidence of broadband noise ingress
/// due to unshielded fixture and local strong signal source.
bool ExcessRxNoiseIngress(array<MeasurementSet^>^ trace,	///< Entire raw measurementset.
						  int points					///< Number of points in the dataset.
						  );		
/// Compute Frequency of grid point for linear(f) and log(f) fixture cal points
//int GetFreqFromFixtureCalGrid(long index, bool Freqmode);
