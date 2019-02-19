//
//    Copyright 2004-2011 Thomas C. McDermott, N5EG
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
/// \author Thomas C. McDermott, N5EG

/** \mainpage VNAR3
!!!Vector Network Analyzer Software Documentation \n
---
\n This document package describes the Vector Network Analyzer host (PC) development
software.\n The entry point is Form1.cpp, which creates an object Form1,
and then runs it.\n Form1 creates the main menu and hooks the windows message loop
using the .NET style of Event Delegates.
*/

#pragma once

#include "NumEntBox.h"
#include "USB_EZ_interface.h"
#include "DisplayRoutines.h"
#include "Calibration.h"
#include "DataDisplay.h"
//#include "InstrumentCal.h"
#include "MarkerEntry.h"
#include "About.h"
#include "CursorStatus.h"
#include "TextEntry.h"
#include "Fourier.h"
#include "AudioInput.h"
#include "TDRSetupDialog.h"
#include "Constants.h"
#include "Mockup.h"
#include "SerialPort.h"
#include "SignalGenerator.h"


//#include "AudioInput.h"
//#include "Serial.h"
//#include "AudioDevice.h"
#include <complex>
using namespace std;  

#include <stdlib.h>

//#define DEBUGAUDIOLEVELS		// Display pareto of reference signal measured during a sweep
//#define DEBUGAUDIODISTANCE
//#define DEBUGAUDIO
//#define DEBUGRAWTRANHIPHASE	// display raw transmission phase ADC counts on rectangular screen
//#define DEBUGRAWTRANLOPHASE	// display raw transmission low level phase ADC counts on rectangular screen
//#define DEBUGRAWREFL			// display raw reflection detector ADC counts on rectangular screen
//#define DEBUGRAWREFV			// display raw reference voltage ADC counts on rectangular screen
//#define DUMPRAWTRACES			// dump all ADC counts to a file 'VNAdump.txt'
//#define DUMPRAWDECODING		// dump all raw decoded data

extern  int audio_delay;

#define SOFTWARE_VERSION		"TAPR VNA 4.0"

namespace VNAR3
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Drawing::Drawing2D;
	using namespace System::Drawing::Printing;
	using namespace System::IO;
	using namespace System::Reflection;
	using namespace System::Threading;
	using namespace System::Text;

	/// <summary> 
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{	
	public:
		Form1(array<String ^>^ args)
		{
			if(args->Length > 0)
				CommandLine = Convert::ToString(args[0]);	// Save 1st argument we were invoked with,
			else CommandLine = Convert::ToString("");		// or empty string if none.

			InitializeComponent();
			VNA_Initialize();
//			this->MockupBox = (gcnew Mockup (this->VNA));
      	}
 
	protected:
		~Form1()
		{
			if(components)
				delete(components);
		}

	private:
		Point p1,p2;					///< saved mouse coordinates
		int MarkerMousePick;			///< marker number picked by mouse (-1 = not picked)
		int txLevel;					///< transmit level
		int refLevel;					///< display reference level

		 System::Drawing::Rectangle rect;					///< client size for render (given to us by Paint & Redraw)
		 System::Drawing::Rectangle scopeDisp;			///< rectangular scope display area
		 System::Drawing::Rectangle polarBox;				///< box holding the polar display
		float RectVertScaledB;			///< Rectangular Vertical Scale in db/division
		int polarRadius;				///< radius of the polar display
		int scalefactorGD;				///< Group delay scale factor.   1 means 100 psec/division
		int RectSWRScale;				///< SWR selection scale
		float PolarZoom;				///< Zoom factor for Polar display (1.0 to 3.0)

		float PixelsPerGrid;			///< display pixels per grid point (freq or time)
		int	StartIndex, StopIndex;		///< Start and stop times converted to array index

		array<__int64>^ Marker;			///< Frequency Markers
		array<Single>^ MarkerT;			///< Time Markers
		CursorStatus^ cs;				///< cursor text display
		bool ShowMarkerNumbers;			///< Marker Number Display control
		bool ShowMarkerParameters;		///< Marker Parameter Display control
		float TDRStartTime, TDRStopTime; ///< TDR Display start and stop times, in nanoseconds
		float TDRvf;					///< TDR display estimated cable velocity factor
		bool TDRMetricMode;				///< TDR Display distance mode

		String^	plotTitle;				///< Plot Title (allowed to be empty)
		String^ FixtureCalFileName;		///< Name of Fixture Calibration File loaded (empty if none)

		volatile bool WorkerCollect;				///< Work for VNA Thread to do

		bool SerialCollect;				///< Work for VNA Thread to do

		bool VNAConnected;				///< True if VNA is connected

		bool PreCharge;					///< True when Integration factor changed. Worker thread should
										//	 set false after first sweep.

		array<MeasurementSet^>^ trace;	///< raw data set for one complete measurement set
		array<MeasurementSet^>^ traceSto;  ///< raw data set for storage

		array<Double>^ stS11Mag; array<Double>^ stS11Phs;				///< storage for S11
		array<Double>^ stS21Mag; array<Double>^ stS21Phs;				///< storage for S21
		array<Double>^ stS12Mag; array<Double>^ stS12Phs;				///< storage for S12
		array<Double>^ stS22Mag; array<Double>^ stS22Phs;				///< storage for S22

		PrintDocument^ pdoc;            ///< Printer document
		VNADevice^ VNA;					///< Vector Network Analyzer hardware object
		FrequencyGrid^ FG;				///< Frequency Grid for display
		InstrumentCalDataSet^ CalData;	///< Calibration Data Set, also can hold Fixture Calibration
		String^ StartUpPath;			///< Directory path we started execution in
		String^ AllUsersDataPath;		///< Directory path for application data common to all users
		String^ CurrentUserDataPath;	///< Directory Path for application data specific to current user

		Thread^ VNAWorkerThread;				///< Thread for VNA data gathering
		Thread^ SerialThread;				    ///< Thread for audio data gathering
		ThreadStart^ VNAWorkerThreadDelegate;	///< Threadstart points to method
		ThreadStart^ SerialThreadDelegate;	    ///< Threadstart points to method

		// Holds string that underlines the digit at position[FrequencyDigitIndex]
		static array<String^>^ FrequencyDigitText = gcnew array<String^>{"-", "-_", "-__", "-____", "-_____",
			"-______", "-________", "-_________", "-__________" };
		int FrequencyDigitIndex;		///< Which Digit is being used for increment/decrement Frequency value

		String^ CommandLine;

		// hold colors for the traces - and populate with default colors
		// BackColor is populated by default as a Form1 property

		static Color s21MagColor = Color::Green;
		static Color s21PhsColor = Color::Blue;
		static Color s11MagColor = Color::Red;
		static Color s11PhsColor = Color::Purple;
		static Color s21GroupDelayColor = Color::Aqua;
		static Color s11VSWRColor = Color::DeepPink;
		static Color s11RColor = Color::DeepSkyBlue;
		static Color s11jXColor = Color::BlueViolet;

	private: System::Windows::Forms::Label^  startF;
	private: System::Windows::Forms::Label^  stopF;
	private: System::Windows::Forms::Label^  txL;
	private: System::Windows::Forms::Label^  refL;
	private: System::Windows::Forms::Button^  stopFdown;
	private: System::Windows::Forms::Button^  stopFup;
	private: System::Windows::Forms::Button^  startFdown;
	private: System::Windows::Forms::Button^  startFup;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Button^  txLup;
	private: System::Windows::Forms::Button^  txLdown;
	private: System::Windows::Forms::Button^  refLup;
	private: System::Windows::Forms::Button^  refLdown;
	private: System::Windows::Forms::ToolTip^  toolTip1;
	private: System::Windows::Forms::CheckBox^  calCheckBox;
	private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::Button^  SingleSweep;
private: System::Windows::Forms::Button^  RecurrentSweep;
private: System::Windows::Forms::Button^  SweepSpd;
private: System::Windows::Forms::Label^  StartFrequencyDigitIndicator;
private: System::Windows::Forms::Label^  StopFrequencyDigitIndicator;
private: System::Windows::Forms::Button^  FrequencyDigitIncrease;
private: System::Windows::Forms::Button^  FrequencyDigitDecrease;
private: System::Windows::Forms::ProgressBar^  SweepProgressBar;
private: System::Windows::Forms::MenuStrip^  menuStrip1;
private: System::Windows::Forms::ToolStripMenuItem^  fileMenu;
private: System::Windows::Forms::ToolStripMenuItem^  viewMenu;
private: System::Windows::Forms::ToolStripMenuItem^  calibrateMenu;
private: System::Windows::Forms::ToolStripMenuItem^  traceMenu;
private: System::Windows::Forms::ToolStripMenuItem^  VertScaleItem;
private: System::Windows::Forms::ToolStripMenuItem^  markerMenu;
private: System::Windows::Forms::ToolStripMenuItem^  menuItem5;
private: System::Windows::Forms::ToolStripMenuItem^  StorageMenu;
private: System::Windows::Forms::ToolStripMenuItem^  IntegrationMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  HelpItem;
private: System::Windows::Forms::ToolStripMenuItem^  printItem;
private: System::Windows::Forms::ToolStripMenuItem^  printSetupItem;
private: System::Windows::Forms::ToolStripMenuItem^  printPreviewItem;
private: System::Windows::Forms::ToolStripSeparator^  menuItem14;
private: System::Windows::Forms::ToolStripMenuItem^  SetTitleMenuItem;
private: System::Windows::Forms::ToolStripSeparator^  menuItem7;
private: System::Windows::Forms::ToolStripMenuItem^  storeForward;
private: System::Windows::Forms::ToolStripMenuItem^  storeReverse;
private: System::Windows::Forms::ToolStripMenuItem^  menuItem8;
private: System::Windows::Forms::ToolStripMenuItem^  ExportRectFmt;
private: System::Windows::Forms::ToolStripMenuItem^  ExportPolarFmt;
private: System::Windows::Forms::ToolStripMenuItem^  ExportCSVRectFormat;
private: System::Windows::Forms::ToolStripSeparator^  menuItem11;
private: System::Windows::Forms::ToolStripMenuItem^  SaveConfigurationMenu;
private: System::Windows::Forms::ToolStripMenuItem^  LoadConfigurationMenu;
private: System::Windows::Forms::ToolStripSeparator^  menuItem19;
private: System::Windows::Forms::ToolStripMenuItem^  exitItem;
private: System::Windows::Forms::ToolStripMenuItem^  rectItem;
private: System::Windows::Forms::ToolStripMenuItem^  polarItem;
private: System::Windows::Forms::ToolStripMenuItem^  TDRItem;

private: System::Windows::Forms::ToolStripMenuItem^  runItem;
private: System::Windows::Forms::ToolStripMenuItem^  loadItem;
private: System::Windows::Forms::ToolStripMenuItem^  s11magItem;
private: System::Windows::Forms::ToolStripMenuItem^  s11phsItem;
private: System::Windows::Forms::ToolStripMenuItem^  s21magItem;
private: System::Windows::Forms::ToolStripMenuItem^  s21phsItem;
private: System::Windows::Forms::ToolStripMenuItem^  s21groupdelayItem;
private: System::Windows::Forms::ToolStripMenuItem^  VSWRdisplay;
private: System::Windows::Forms::ToolStripSeparator^  menuItem6;
private: System::Windows::Forms::ToolStripMenuItem^  calSthru;
private: System::Windows::Forms::ToolStripSeparator^  menuItem1;
private: System::Windows::Forms::ToolStripMenuItem^  EtTrace;
private: System::Windows::Forms::ToolStripMenuItem^  EsTrace;
private: System::Windows::Forms::ToolStripMenuItem^  EdTrace;
private: System::Windows::Forms::ToolStripSeparator^  menuItem2;
private: System::Windows::Forms::ToolStripMenuItem^  calSshort;
private: System::Windows::Forms::ToolStripMenuItem^  calSopen;
private: System::Windows::Forms::ToolStripMenuItem^  calSterm;
private: System::Windows::Forms::ToolStripMenuItem^  Scale10dB;
private: System::Windows::Forms::ToolStripMenuItem^  Scale5dB;
private: System::Windows::Forms::ToolStripMenuItem^  Scale2dB;
private: System::Windows::Forms::ToolStripMenuItem^  Scale1dB;
private: System::Windows::Forms::ToolStripSeparator^  menuItem4;
private: System::Windows::Forms::ToolStripSeparator^  menuItem20;
private: System::Windows::Forms::ToolStripSeparator^  menuItem10;
private: System::Windows::Forms::ToolStripSeparator^  menuItem13;
private: System::Windows::Forms::ToolStripMenuItem^  menuItem15;
private: System::Windows::Forms::ToolStripMenuItem^  RightScalePhase;
private: System::Windows::Forms::ToolStripMenuItem^  RightScaleSWR;
private: System::Windows::Forms::ToolStripMenuItem^  RightScaleDelay;
private: System::Windows::Forms::ToolStripSeparator^  menuItem16;
private: System::Windows::Forms::ToolStripMenuItem^  menuItem17;
private: System::Windows::Forms::ToolStripMenuItem^  PolarZoom10Menu;
private: System::Windows::Forms::ToolStripMenuItem^  PolarZoom15Menu;
private: System::Windows::Forms::ToolStripMenuItem^  PolarZoom20Menu;
private: System::Windows::Forms::ToolStripMenuItem^  PolarZoom25Menu;
private: System::Windows::Forms::ToolStripSeparator^  menuItem21;
private: System::Windows::Forms::ToolStripMenuItem^  TDRSetupmenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  ChangeMarker;
private: System::Windows::Forms::ToolStripMenuItem^  ClearAllMarkers;
private: System::Windows::Forms::ToolStripMenuItem^  menuItem9;
private: System::Windows::Forms::ToolStripMenuItem^  MarkerNumEnable;
private: System::Windows::Forms::ToolStripMenuItem^  MarkerNumDisable;
private: System::Windows::Forms::ToolStripMenuItem^  menuItem12;
private: System::Windows::Forms::ToolStripMenuItem^  MarkerParamEnable;
private: System::Windows::Forms::ToolStripMenuItem^  MarkerParamDisable;
private: System::Windows::Forms::ToolStripMenuItem^  grid101menu;
private: System::Windows::Forms::ToolStripMenuItem^  grid201menu;
private: System::Windows::Forms::ToolStripMenuItem^  grid401menu;
private: System::Windows::Forms::ToolStripMenuItem^  grid1024menu;
private: System::Windows::Forms::ToolStripMenuItem^  StoreMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  RecallMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  DisplayMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  IntegrationMenu1x;
private: System::Windows::Forms::ToolStripMenuItem^  IntegrationMenu2x;
private: System::Windows::Forms::ToolStripMenuItem^  IntegrationMenu4x;
private: System::Windows::Forms::ToolStripMenuItem^  IntegrationMenu8x;
private: System::Windows::Forms::ToolStripMenuItem^  IntegrationMenu16x;
private: System::Windows::Forms::ToolStripMenuItem^  HelpMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  AboutMenuItem;
private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
private: System::Windows::Forms::ToolStripMenuItem^  pickTraceColorToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  s11MagnitudeColorToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  s11PhaseColorToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  s21MagnitudeColorToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  s21PhaseColorToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  s21GroupDelayColorToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  sWRColorToolStripMenuItem;
private: System::Windows::Forms::ColorDialog^  colorDialog1;

private: System::Windows::Forms::CheckBox^  RefExtnCheckBox;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::ToolStripMenuItem^  RightScaleOhms;
private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator2;
private: System::Windows::Forms::ToolStripMenuItem^  RDisplay;
private: System::Windows::Forms::ToolStripMenuItem^  jXDisplay;
private: System::Windows::Forms::ToolStripMenuItem^  s11AsRColorToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  s11AsJXColorToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  Scale05dB;
private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem1;
private: System::Windows::Forms::ToolStripMenuItem^  ScaleSWR10;
private: System::Windows::Forms::ToolStripMenuItem^  ScaleSWR5;
private: System::Windows::Forms::ToolStripMenuItem^  ScaleSWR2;
private: System::Windows::Forms::ToolStripMenuItem^  ScaleSWR1;
private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem3;
private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem2;
private: System::Windows::Forms::ToolStripMenuItem^  GDAperture1;
private: System::Windows::Forms::ToolStripMenuItem^  GDAperture4;
private: System::Windows::Forms::ToolStripMenuItem^  GDAperture16;
private: System::Windows::Forms::ToolStripMenuItem^  GDAperture64;
private: System::Windows::Forms::ToolStripMenuItem^  scale1kOhms;
private: System::Windows::Forms::ToolStripMenuItem^  scale100Ohms;
private: System::Windows::Forms::ToolStripMenuItem^  scale10Ohms;
private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem4;
private: System::Windows::Forms::ToolStripMenuItem^  scale1millisec;
private: System::Windows::Forms::ToolStripMenuItem^  scale100microsec;
private: System::Windows::Forms::ToolStripMenuItem^  scale10microsec;
private: System::Windows::Forms::ToolStripMenuItem^  scalemicroSec;
private: System::Windows::Forms::ToolStripMenuItem^  scale100nsec;
private: System::Windows::Forms::ToolStripMenuItem^  scale10nsec;
private: System::Windows::Forms::ToolStripMenuItem^  scalenanoSec;
private: System::Windows::Forms::ToolStripMenuItem^  scale100psec;
private: System::Windows::Forms::ToolStripMenuItem^  ExportCSVRectFormatEurope;
private: System::Windows::Forms::ToolStripMenuItem^  backgroundColorToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  displayMeasuredMinusStored;
private: System::Windows::Forms::ToolStripMenuItem^  DisplayMeasured;
private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator3;
private: System::Windows::Forms::ToolStripMenuItem^  mockupDeviceToolStripMenuItem;
private: Mockup^ MockupBox;
private: SignalGenerator^ SignalGeneratorBox;
private: SerialPort^ SerialPortBox;
		 //private: AudioDevice^ AudioDeviceBox;
private: System::IO::Ports::SerialPort^  serialPort1;
private: System::Windows::Forms::ToolStripMenuItem^  audioDevicesToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  settingsToolStripMenuItem;






private: System::Windows::Forms::ToolStripMenuItem^  serialPortToolStripMenuItem;












private: System::Windows::Forms::ToolStripMenuItem^  signalGeneratorToolStripMenuItem;
private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator4;
private: System::Windows::Forms::ToolStripMenuItem^  rawDetectorDataToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  audioDistanceToolStripMenuItem;
private: System::Windows::Forms::CheckBox^  Spectrum;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::ToolStripMenuItem^  mockupDeviceToolStripMenuItem1;
private: System::Windows::Forms::ToolStripMenuItem^  dumpMeasurementsToolStripMenuItem;
private: System::Windows::Forms::TrackBar^  MIndex;
private: System::Windows::Forms::Button^  AddMarkerButton;









	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->stopFdown = (gcnew System::Windows::Forms::Button());
			this->stopFup = (gcnew System::Windows::Forms::Button());
			this->startFdown = (gcnew System::Windows::Forms::Button());
			this->startFup = (gcnew System::Windows::Forms::Button());
			this->startF = (gcnew System::Windows::Forms::Label());
			this->stopF = (gcnew System::Windows::Forms::Label());
			this->txL = (gcnew System::Windows::Forms::Label());
			this->refL = (gcnew System::Windows::Forms::Label());
			this->txLup = (gcnew System::Windows::Forms::Button());
			this->txLdown = (gcnew System::Windows::Forms::Button());
			this->refLup = (gcnew System::Windows::Forms::Button());
			this->refLdown = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->SweepSpd = (gcnew System::Windows::Forms::Button());
			this->SingleSweep = (gcnew System::Windows::Forms::Button());
			this->RecurrentSweep = (gcnew System::Windows::Forms::Button());
			this->SweepProgressBar = (gcnew System::Windows::Forms::ProgressBar());
			this->FrequencyDigitIncrease = (gcnew System::Windows::Forms::Button());
			this->FrequencyDigitDecrease = (gcnew System::Windows::Forms::Button());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->calCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->StartFrequencyDigitIndicator = (gcnew System::Windows::Forms::Label());
			this->StopFrequencyDigitIndicator = (gcnew System::Windows::Forms::Label());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileMenu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->printItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->printSetupItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->printPreviewItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuItem14 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->SetTitleMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuItem7 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->storeForward = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->storeReverse = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuItem8 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ExportRectFmt = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ExportPolarFmt = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ExportCSVRectFormat = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ExportCSVRectFormatEurope = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuItem11 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->SaveConfigurationMenu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LoadConfigurationMenu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuItem19 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->exitItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->viewMenu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->rectItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->polarItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->TDRItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->audioDevicesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->signalGeneratorToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mockupDeviceToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->calibrateMenu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->runItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->loadItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mockupDeviceToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->traceMenu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->s11magItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->s11phsItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->s21magItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->s21phsItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->s21groupdelayItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->VSWRdisplay = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->RDisplay = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->jXDisplay = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuItem6 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->calSthru = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuItem1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->EtTrace = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->EsTrace = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->EdTrace = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuItem2 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->calSshort = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->calSopen = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->calSterm = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->pickTraceColorToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->s11MagnitudeColorToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->s11PhaseColorToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->s21MagnitudeColorToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->s21PhaseColorToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->s21GroupDelayColorToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->sWRColorToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->s11AsRColorToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->s11AsJXColorToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->backgroundColorToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator4 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->rawDetectorDataToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->audioDistanceToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->VertScaleItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->Scale10dB = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->Scale5dB = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->Scale2dB = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->Scale1dB = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->Scale05dB = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuItem4 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->toolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ScaleSWR10 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ScaleSWR5 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ScaleSWR2 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ScaleSWR1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuItem20 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->toolStripMenuItem3 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->scale1kOhms = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->scale100Ohms = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->scale10Ohms = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuItem13 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->menuItem15 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->RightScalePhase = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->RightScaleSWR = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->RightScaleDelay = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->RightScaleOhms = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuItem10 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->toolStripMenuItem4 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->scale1millisec = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->scale100microsec = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->scale10microsec = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->scalemicroSec = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->scale100nsec = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->scale10nsec = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->scalenanoSec = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->scale100psec = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->toolStripMenuItem2 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->GDAperture1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->GDAperture4 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->GDAperture16 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->GDAperture64 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuItem16 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->menuItem17 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->PolarZoom10Menu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->PolarZoom15Menu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->PolarZoom20Menu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->PolarZoom25Menu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuItem21 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->TDRSetupmenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->markerMenu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ChangeMarker = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ClearAllMarkers = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuItem9 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->MarkerNumEnable = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->MarkerNumDisable = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuItem12 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->MarkerParamEnable = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->MarkerParamDisable = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuItem5 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->grid101menu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->grid201menu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->grid401menu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->grid1024menu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->StorageMenu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->StoreMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->RecallMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator3 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->DisplayMeasured = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->DisplayMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->displayMeasuredMinusStored = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->IntegrationMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->IntegrationMenu1x = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->IntegrationMenu2x = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->IntegrationMenu4x = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->IntegrationMenu8x = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->IntegrationMenu16x = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->HelpItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->HelpMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->AboutMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->settingsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->serialPortToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->dumpMeasurementsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->colorDialog1 = (gcnew System::Windows::Forms::ColorDialog());
			this->RefExtnCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->serialPort1 = (gcnew System::IO::Ports::SerialPort(this->components));
			this->Spectrum = (gcnew System::Windows::Forms::CheckBox());
			this->MIndex = (gcnew System::Windows::Forms::TrackBar());
			this->AddMarkerButton = (gcnew System::Windows::Forms::Button());
			this->menuStrip1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MIndex))->BeginInit();
			this->SuspendLayout();
			// 
			// stopFdown
			// 
			this->stopFdown->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->stopFdown->BackColor = System::Drawing::Color::WhiteSmoke;
			this->stopFdown->Location = System::Drawing::Point(384, 401);
			this->stopFdown->Name = L"stopFdown";
			this->stopFdown->Size = System::Drawing::Size(24, 19);
			this->stopFdown->TabIndex = 0;
			this->stopFdown->Text = L"-";
			this->toolTip1->SetToolTip(this->stopFdown, L"Decrement Stop Frequency");
			this->stopFdown->UseVisualStyleBackColor = false;
			this->stopFdown->Click += gcnew System::EventHandler(this, &Form1::stopFdown_Click);
			// 
			// stopFup
			// 
			this->stopFup->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->stopFup->BackColor = System::Drawing::Color::WhiteSmoke;
			this->stopFup->Location = System::Drawing::Point(384, 383);
			this->stopFup->Name = L"stopFup";
			this->stopFup->Size = System::Drawing::Size(24, 19);
			this->stopFup->TabIndex = 1;
			this->stopFup->Text = L"+";
			this->toolTip1->SetToolTip(this->stopFup, L"Increment Stop Frequency");
			this->stopFup->UseVisualStyleBackColor = false;
			this->stopFup->Click += gcnew System::EventHandler(this, &Form1::stopFup_Click);
			// 
			// startFdown
			// 
			this->startFdown->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->startFdown->BackColor = System::Drawing::Color::WhiteSmoke;
			this->startFdown->Location = System::Drawing::Point(384, 351);
			this->startFdown->Name = L"startFdown";
			this->startFdown->Size = System::Drawing::Size(24, 19);
			this->startFdown->TabIndex = 2;
			this->startFdown->Text = L"-";
			this->toolTip1->SetToolTip(this->startFdown, L"Decrement Start Frequency");
			this->startFdown->UseVisualStyleBackColor = false;
			this->startFdown->Click += gcnew System::EventHandler(this, &Form1::startFdown_Click);
			// 
			// startFup
			// 
			this->startFup->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->startFup->BackColor = System::Drawing::Color::WhiteSmoke;
			this->startFup->Location = System::Drawing::Point(384, 333);
			this->startFup->Name = L"startFup";
			this->startFup->Size = System::Drawing::Size(24, 19);
			this->startFup->TabIndex = 3;
			this->startFup->Text = L"+";
			this->toolTip1->SetToolTip(this->startFup, L"Increment Start Frequency");
			this->startFup->UseVisualStyleBackColor = false;
			this->startFup->Click += gcnew System::EventHandler(this, &Form1::startFup_Click);
			// 
			// startF
			// 
			this->startF->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->startF->BackColor = System::Drawing::Color::Black;
			this->startF->Font = (gcnew System::Drawing::Font(L"Courier New", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->startF->ForeColor = System::Drawing::Color::Aquamarine;
			this->startF->Location = System::Drawing::Point(208, 335);
			this->startF->Name = L"startF";
			this->startF->Size = System::Drawing::Size(176, 25);
			this->startF->TabIndex = 4;
			this->startF->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->toolTip1->SetToolTip(this->startF, L"Double-click to directly enter value");
			this->startF->DoubleClick += gcnew System::EventHandler(this, &Form1::startF_DoubleClick);
			this->startF->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::startF_MouseDown);
			this->startF->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::startF_MouseDown);
			// 
			// stopF
			// 
			this->stopF->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->stopF->BackColor = System::Drawing::Color::Black;
			this->stopF->Font = (gcnew System::Drawing::Font(L"Courier New", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->stopF->ForeColor = System::Drawing::Color::Aquamarine;
			this->stopF->Location = System::Drawing::Point(208, 386);
			this->stopF->Name = L"stopF";
			this->stopF->Size = System::Drawing::Size(176, 27);
			this->stopF->TabIndex = 5;
			this->stopF->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->toolTip1->SetToolTip(this->stopF, L"Double-click to directly enter value");
			this->stopF->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::stopF_MouseDown);
			this->stopF->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::stopF_MouseDown);
			// 
			// txL
			// 
			this->txL->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->txL->BackColor = System::Drawing::Color::Black;
			this->txL->Font = (gcnew System::Drawing::Font(L"Courier New", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txL->ForeColor = System::Drawing::Color::Aquamarine;
			this->txL->Location = System::Drawing::Point(632, 337);
			this->txL->Name = L"txL";
			this->txL->Size = System::Drawing::Size(104, 32);
			this->txL->TabIndex = 6;
			this->txL->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->toolTip1->SetToolTip(this->txL, L"Double-click to directly enter value");
			// 
			// refL
			// 
			this->refL->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->refL->BackColor = System::Drawing::Color::Black;
			this->refL->Font = (gcnew System::Drawing::Font(L"Courier New", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->refL->ForeColor = System::Drawing::Color::Aquamarine;
			this->refL->Location = System::Drawing::Point(632, 386);
			this->refL->Name = L"refL";
			this->refL->Size = System::Drawing::Size(104, 32);
			this->refL->TabIndex = 7;
			this->refL->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->toolTip1->SetToolTip(this->refL, L"Double-click to directly enter value");
			// 
			// txLup
			// 
			this->txLup->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->txLup->BackColor = System::Drawing::Color::WhiteSmoke;
			this->txLup->Location = System::Drawing::Point(736, 335);
			this->txLup->Name = L"txLup";
			this->txLup->Size = System::Drawing::Size(24, 19);
			this->txLup->TabIndex = 8;
			this->txLup->Text = L"+";
			this->toolTip1->SetToolTip(this->txLup, L"Increase Transmit Level");
			this->txLup->UseVisualStyleBackColor = false;
			this->txLup->Click += gcnew System::EventHandler(this, &Form1::txLup_Click);
			// 
			// txLdown
			// 
			this->txLdown->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->txLdown->BackColor = System::Drawing::Color::WhiteSmoke;
			this->txLdown->Location = System::Drawing::Point(736, 354);
			this->txLdown->Name = L"txLdown";
			this->txLdown->Size = System::Drawing::Size(24, 19);
			this->txLdown->TabIndex = 9;
			this->txLdown->Text = L"-";
			this->toolTip1->SetToolTip(this->txLdown, L"Decrease Transmit Level");
			this->txLdown->UseVisualStyleBackColor = false;
			this->txLdown->Click += gcnew System::EventHandler(this, &Form1::txLdown_Click);
			// 
			// refLup
			// 
			this->refLup->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->refLup->BackColor = System::Drawing::Color::WhiteSmoke;
			this->refLup->Location = System::Drawing::Point(736, 383);
			this->refLup->Name = L"refLup";
			this->refLup->Size = System::Drawing::Size(24, 19);
			this->refLup->TabIndex = 10;
			this->refLup->Text = L"+";
			this->toolTip1->SetToolTip(this->refLup, L"Increase Reference Level");
			this->refLup->UseVisualStyleBackColor = false;
			this->refLup->Click += gcnew System::EventHandler(this, &Form1::refLup_Click);
			// 
			// refLdown
			// 
			this->refLdown->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->refLdown->BackColor = System::Drawing::Color::WhiteSmoke;
			this->refLdown->Location = System::Drawing::Point(736, 401);
			this->refLdown->Name = L"refLdown";
			this->refLdown->Size = System::Drawing::Size(24, 19);
			this->refLdown->TabIndex = 11;
			this->refLdown->Text = L"-";
			this->toolTip1->SetToolTip(this->refLdown, L"Decrease Reference Level");
			this->refLdown->UseVisualStyleBackColor = false;
			this->refLdown->Click += gcnew System::EventHandler(this, &Form1::refLdown_Click);
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Font = (gcnew System::Drawing::Font(L"Verdana", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->ForeColor = System::Drawing::SystemColors::InfoText;
			this->label1->Location = System::Drawing::Point(7, 335);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(213, 32);
			this->label1->TabIndex = 12;
			this->label1->Text = L"Start Frequency";
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label2->BackColor = System::Drawing::Color::Transparent;
			this->label2->Font = (gcnew System::Drawing::Font(L"Verdana", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->ForeColor = System::Drawing::SystemColors::InfoText;
			this->label2->Location = System::Drawing::Point(8, 386);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(205, 32);
			this->label2->TabIndex = 13;
			this->label2->Text = L"Stop Frequency";
			// 
			// label3
			// 
			this->label3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label3->BackColor = System::Drawing::Color::Transparent;
			this->label3->Font = (gcnew System::Drawing::Font(L"Verdana", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label3->ForeColor = System::Drawing::SystemColors::InfoText;
			this->label3->Location = System::Drawing::Point(440, 337);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(184, 24);
			this->label3->TabIndex = 14;
			this->label3->Text = L"Tx Level, dB.";
			// 
			// label4
			// 
			this->label4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label4->BackColor = System::Drawing::Color::Transparent;
			this->label4->Font = (gcnew System::Drawing::Font(L"Verdana", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label4->ForeColor = System::Drawing::SystemColors::InfoText;
			this->label4->Location = System::Drawing::Point(440, 386);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(192, 24);
			this->label4->TabIndex = 15;
			this->label4->Text = L"Ref. Level, dB.";
			// 
			// label5
			// 
			this->label5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label5->BackColor = System::Drawing::Color::Transparent;
			this->label5->ForeColor = System::Drawing::SystemColors::InfoText;
			this->label5->Location = System::Drawing::Point(840, 361);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(72, 35);
			this->label5->TabIndex = 18;
			this->label5->Text = L"Apply Fixture Calibration";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->toolTip1->SetToolTip(this->label5, L"Must have Cal File Loaded or must Run Cal file in order to enable.");
			// 
			// SweepSpd
			// 
			this->SweepSpd->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->SweepSpd->BackColor = System::Drawing::Color::WhiteSmoke;
			this->SweepSpd->Location = System::Drawing::Point(784, 390);
			this->SweepSpd->Name = L"SweepSpd";
			this->SweepSpd->Size = System::Drawing::Size(50, 23);
			this->SweepSpd->TabIndex = 21;
			this->SweepSpd->Text = L"1 ms";
			this->toolTip1->SetToolTip(this->SweepSpd, L"Toggle Sweep Speed");
			this->SweepSpd->UseVisualStyleBackColor = false;
			this->SweepSpd->Click += gcnew System::EventHandler(this, &Form1::SweepSpd_Click);
			// 
			// SingleSweep
			// 
			this->SingleSweep->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->SingleSweep->BackColor = System::Drawing::Color::WhiteSmoke;
			this->SingleSweep->Enabled = false;
			this->SingleSweep->Location = System::Drawing::Point(784, 339);
			this->SingleSweep->Name = L"SingleSweep";
			this->SingleSweep->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->SingleSweep->Size = System::Drawing::Size(64, 24);
			this->SingleSweep->TabIndex = 19;
			this->SingleSweep->Text = L"SglSwp";
			this->toolTip1->SetToolTip(this->SingleSweep, L"Trigger a Single Sweep");
			this->SingleSweep->UseVisualStyleBackColor = false;
			this->SingleSweep->Click += gcnew System::EventHandler(this, &Form1::SingleSweep_Click);
			// 
			// RecurrentSweep
			// 
			this->RecurrentSweep->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->RecurrentSweep->BackColor = System::Drawing::Color::WhiteSmoke;
			this->RecurrentSweep->Location = System::Drawing::Point(864, 339);
			this->RecurrentSweep->Name = L"RecurrentSweep";
			this->RecurrentSweep->Size = System::Drawing::Size(64, 24);
			this->RecurrentSweep->TabIndex = 20;
			this->RecurrentSweep->Text = L"Free Run";
			this->toolTip1->SetToolTip(this->RecurrentSweep, L"Toggle on or off the free-running sweep");
			this->RecurrentSweep->UseVisualStyleBackColor = false;
			this->RecurrentSweep->Click += gcnew System::EventHandler(this, &Form1::RecurrentSweep_Click);
			// 
			// SweepProgressBar
			// 
			this->SweepProgressBar->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->SweepProgressBar->Location = System::Drawing::Point(16, 419);
			this->SweepProgressBar->Name = L"SweepProgressBar";
			this->SweepProgressBar->Size = System::Drawing::Size(912, 5);
			this->SweepProgressBar->TabIndex = 26;
			this->toolTip1->SetToolTip(this->SweepProgressBar, L"Sweep Progress");
			// 
			// FrequencyDigitIncrease
			// 
			this->FrequencyDigitIncrease->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->FrequencyDigitIncrease->BackColor = System::Drawing::Color::WhiteSmoke;
			this->FrequencyDigitIncrease->Location = System::Drawing::Point(264, 367);
			this->FrequencyDigitIncrease->Name = L"FrequencyDigitIncrease";
			this->FrequencyDigitIncrease->Size = System::Drawing::Size(24, 19);
			this->FrequencyDigitIncrease->TabIndex = 24;
			this->FrequencyDigitIncrease->Text = L"<";
			this->toolTip1->SetToolTip(this->FrequencyDigitIncrease, L"Move digit selector left");
			this->FrequencyDigitIncrease->UseVisualStyleBackColor = false;
			this->FrequencyDigitIncrease->Click += gcnew System::EventHandler(this, &Form1::FrequencyDigitIncrease_Click);
			// 
			// FrequencyDigitDecrease
			// 
			this->FrequencyDigitDecrease->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->FrequencyDigitDecrease->BackColor = System::Drawing::Color::WhiteSmoke;
			this->FrequencyDigitDecrease->Location = System::Drawing::Point(312, 367);
			this->FrequencyDigitDecrease->Name = L"FrequencyDigitDecrease";
			this->FrequencyDigitDecrease->Size = System::Drawing::Size(24, 19);
			this->FrequencyDigitDecrease->TabIndex = 25;
			this->FrequencyDigitDecrease->Text = L">";
			this->toolTip1->SetToolTip(this->FrequencyDigitDecrease, L"Move digit selector right");
			this->FrequencyDigitDecrease->UseVisualStyleBackColor = false;
			this->FrequencyDigitDecrease->Click += gcnew System::EventHandler(this, &Form1::FrequencyDigitDecrease_Click);
			// 
			// label7
			// 
			this->label7->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label7->BackColor = System::Drawing::Color::Transparent;
			this->label7->ForeColor = System::Drawing::SystemColors::InfoText;
			this->label7->Location = System::Drawing::Point(755, 366);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(58, 22);
			this->label7->TabIndex = 31;
			this->label7->Text = L"Spectrum";
			this->label7->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->toolTip1->SetToolTip(this->label7, L"Must have Cal File Loaded or must Run Cal file in order to enable.");
			// 
			// calCheckBox
			// 
			this->calCheckBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->calCheckBox->BackColor = System::Drawing::Color::Transparent;
			this->calCheckBox->CheckAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->calCheckBox->Enabled = false;
			this->calCheckBox->Location = System::Drawing::Point(912, 367);
			this->calCheckBox->Name = L"calCheckBox";
			this->calCheckBox->Size = System::Drawing::Size(16, 24);
			this->calCheckBox->TabIndex = 17;
			this->calCheckBox->UseVisualStyleBackColor = false;
			this->calCheckBox->CheckedChanged += gcnew System::EventHandler(this, &Form1::calCheckBox_CheckedChanged);
			// 
			// StartFrequencyDigitIndicator
			// 
			this->StartFrequencyDigitIndicator->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->StartFrequencyDigitIndicator->BackColor = System::Drawing::Color::Black;
			this->StartFrequencyDigitIndicator->Font = (gcnew System::Drawing::Font(L"Courier New", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->StartFrequencyDigitIndicator->ForeColor = System::Drawing::Color::Aquamarine;
			this->StartFrequencyDigitIndicator->Location = System::Drawing::Point(208, 351);
			this->StartFrequencyDigitIndicator->Name = L"StartFrequencyDigitIndicator";
			this->StartFrequencyDigitIndicator->Size = System::Drawing::Size(176, 16);
			this->StartFrequencyDigitIndicator->TabIndex = 22;
			this->StartFrequencyDigitIndicator->Text = L"-";
			this->StartFrequencyDigitIndicator->TextAlign = System::Drawing::ContentAlignment::TopRight;
			// 
			// StopFrequencyDigitIndicator
			// 
			this->StopFrequencyDigitIndicator->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->StopFrequencyDigitIndicator->BackColor = System::Drawing::Color::Black;
			this->StopFrequencyDigitIndicator->Font = (gcnew System::Drawing::Font(L"Courier New", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->StopFrequencyDigitIndicator->ForeColor = System::Drawing::Color::Aquamarine;
			this->StopFrequencyDigitIndicator->Location = System::Drawing::Point(208, 402);
			this->StopFrequencyDigitIndicator->Name = L"StopFrequencyDigitIndicator";
			this->StopFrequencyDigitIndicator->Size = System::Drawing::Size(176, 16);
			this->StopFrequencyDigitIndicator->TabIndex = 23;
			this->StopFrequencyDigitIndicator->Text = L"-";
			this->StopFrequencyDigitIndicator->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(11) {this->fileMenu, this->viewMenu, 
				this->calibrateMenu, this->traceMenu, this->VertScaleItem, this->markerMenu, this->menuItem5, this->StorageMenu, this->IntegrationMenuItem, 
				this->HelpItem, this->settingsToolStripMenuItem});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(935, 24);
			this->menuStrip1->TabIndex = 27;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// fileMenu
			// 
			this->fileMenu->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(14) {this->printItem, 
				this->printSetupItem, this->printPreviewItem, this->menuItem14, this->SetTitleMenuItem, this->menuItem7, this->storeForward, 
				this->storeReverse, this->menuItem8, this->menuItem11, this->SaveConfigurationMenu, this->LoadConfigurationMenu, this->menuItem19, 
				this->exitItem});
			this->fileMenu->Name = L"fileMenu";
			this->fileMenu->Size = System::Drawing::Size(37, 20);
			this->fileMenu->Text = L"&File";
			// 
			// printItem
			// 
			this->printItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"printItem.Image")));
			this->printItem->Name = L"printItem";
			this->printItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::P));
			this->printItem->Size = System::Drawing::Size(313, 22);
			this->printItem->Text = L"&Print";
			this->printItem->Click += gcnew System::EventHandler(this, &Form1::printItem_Click);
			// 
			// printSetupItem
			// 
			this->printSetupItem->Name = L"printSetupItem";
			this->printSetupItem->Size = System::Drawing::Size(313, 22);
			this->printSetupItem->Text = L"Print &Setup";
			this->printSetupItem->Click += gcnew System::EventHandler(this, &Form1::printSetupItem_Click);
			// 
			// printPreviewItem
			// 
			this->printPreviewItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"printPreviewItem.Image")));
			this->printPreviewItem->Name = L"printPreviewItem";
			this->printPreviewItem->Size = System::Drawing::Size(313, 22);
			this->printPreviewItem->Text = L"Print Pre&view";
			this->printPreviewItem->Click += gcnew System::EventHandler(this, &Form1::printPreviewItem_Click);
			// 
			// menuItem14
			// 
			this->menuItem14->Name = L"menuItem14";
			this->menuItem14->Size = System::Drawing::Size(310, 6);
			// 
			// SetTitleMenuItem
			// 
			this->SetTitleMenuItem->Name = L"SetTitleMenuItem";
			this->SetTitleMenuItem->Size = System::Drawing::Size(313, 22);
			this->SetTitleMenuItem->Text = L"&Title...";
			this->SetTitleMenuItem->Click += gcnew System::EventHandler(this, &Form1::SetTitleMenuItem_Click);
			// 
			// menuItem7
			// 
			this->menuItem7->Name = L"menuItem7";
			this->menuItem7->Size = System::Drawing::Size(310, 6);
			// 
			// storeForward
			// 
			this->storeForward->Name = L"storeForward";
			this->storeForward->Size = System::Drawing::Size(313, 22);
			this->storeForward->Text = L"Store &Forward Parameters";
			this->storeForward->Click += gcnew System::EventHandler(this, &Form1::storeForward_Click);
			// 
			// storeReverse
			// 
			this->storeReverse->Name = L"storeReverse";
			this->storeReverse->Size = System::Drawing::Size(313, 22);
			this->storeReverse->Text = L"Store &Reverse Parameters";
			this->storeReverse->Click += gcnew System::EventHandler(this, &Form1::storeReverse_Click);
			// 
			// menuItem8
			// 
			this->menuItem8->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->ExportRectFmt, 
				this->ExportPolarFmt, this->ExportCSVRectFormat, this->ExportCSVRectFormatEurope});
			this->menuItem8->Name = L"menuItem8";
			this->menuItem8->Size = System::Drawing::Size(313, 22);
			this->menuItem8->Text = L"E&xport S-parameter File";
			// 
			// ExportRectFmt
			// 
			this->ExportRectFmt->Name = L"ExportRectFmt";
			this->ExportRectFmt->Size = System::Drawing::Size(337, 22);
			this->ExportRectFmt->Text = L"S2P &Rectangular Format";
			this->ExportRectFmt->Click += gcnew System::EventHandler(this, &Form1::ExportRectFmt_Click);
			// 
			// ExportPolarFmt
			// 
			this->ExportPolarFmt->Name = L"ExportPolarFmt";
			this->ExportPolarFmt->Size = System::Drawing::Size(337, 22);
			this->ExportPolarFmt->Text = L"S2P &Polar Format";
			this->ExportPolarFmt->Click += gcnew System::EventHandler(this, &Form1::ExportPolarFmt_Click);
			// 
			// ExportCSVRectFormat
			// 
			this->ExportCSVRectFormat->Name = L"ExportCSVRectFormat";
			this->ExportCSVRectFormat->Size = System::Drawing::Size(337, 22);
			this->ExportCSVRectFormat->Text = L"CSV Rectangular Format (North American format)";
			this->ExportCSVRectFormat->Click += gcnew System::EventHandler(this, &Form1::ExportCSVRectFormat_Click);
			// 
			// ExportCSVRectFormatEurope
			// 
			this->ExportCSVRectFormatEurope->Name = L"ExportCSVRectFormatEurope";
			this->ExportCSVRectFormatEurope->Size = System::Drawing::Size(337, 22);
			this->ExportCSVRectFormatEurope->Text = L"CSV Rectangular Format (European format)";
			this->ExportCSVRectFormatEurope->Click += gcnew System::EventHandler(this, &Form1::ExportCSVRectFormatEurope_Click);
			// 
			// menuItem11
			// 
			this->menuItem11->Name = L"menuItem11";
			this->menuItem11->Size = System::Drawing::Size(310, 6);
			// 
			// SaveConfigurationMenu
			// 
			this->SaveConfigurationMenu->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"SaveConfigurationMenu.Image")));
			this->SaveConfigurationMenu->Name = L"SaveConfigurationMenu";
			this->SaveConfigurationMenu->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::S));
			this->SaveConfigurationMenu->Size = System::Drawing::Size(313, 22);
			this->SaveConfigurationMenu->Text = L"Save &Configuration + Measurement...";
			this->SaveConfigurationMenu->Click += gcnew System::EventHandler(this, &Form1::SaveConfigurationMenu_Click);
			// 
			// LoadConfigurationMenu
			// 
			this->LoadConfigurationMenu->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"LoadConfigurationMenu.Image")));
			this->LoadConfigurationMenu->Name = L"LoadConfigurationMenu";
			this->LoadConfigurationMenu->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::L));
			this->LoadConfigurationMenu->Size = System::Drawing::Size(313, 22);
			this->LoadConfigurationMenu->Text = L"&Load Configuration + Measurement...";
			this->LoadConfigurationMenu->Click += gcnew System::EventHandler(this, &Form1::LoadConfigurationMenu_Click);
			// 
			// menuItem19
			// 
			this->menuItem19->Name = L"menuItem19";
			this->menuItem19->Size = System::Drawing::Size(310, 6);
			// 
			// exitItem
			// 
			this->exitItem->Name = L"exitItem";
			this->exitItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Alt | System::Windows::Forms::Keys::X));
			this->exitItem->Size = System::Drawing::Size(313, 22);
			this->exitItem->Text = L"&Exit";
			this->exitItem->Click += gcnew System::EventHandler(this, &Form1::exitItem_Click);
			// 
			// viewMenu
			// 
			this->viewMenu->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->rectItem, this->polarItem, 
				this->TDRItem, this->audioDevicesToolStripMenuItem, this->signalGeneratorToolStripMenuItem, this->mockupDeviceToolStripMenuItem1});
			this->viewMenu->Name = L"viewMenu";
			this->viewMenu->Size = System::Drawing::Size(44, 20);
			this->viewMenu->Text = L"&View";
			// 
			// rectItem
			// 
			this->rectItem->Checked = true;
			this->rectItem->CheckState = System::Windows::Forms::CheckState::Checked;
			this->rectItem->Name = L"rectItem";
			this->rectItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Alt | System::Windows::Forms::Keys::R));
			this->rectItem->Size = System::Drawing::Size(239, 22);
			this->rectItem->Text = L"&Rectangular";
			this->rectItem->Click += gcnew System::EventHandler(this, &Form1::rectItem_Click);
			// 
			// polarItem
			// 
			this->polarItem->Name = L"polarItem";
			this->polarItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Alt | System::Windows::Forms::Keys::P));
			this->polarItem->Size = System::Drawing::Size(239, 22);
			this->polarItem->Text = L"&Polar";
			this->polarItem->Click += gcnew System::EventHandler(this, &Form1::polarItem_Click);
			// 
			// TDRItem
			// 
			this->TDRItem->Name = L"TDRItem";
			this->TDRItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Alt | System::Windows::Forms::Keys::T));
			this->TDRItem->Size = System::Drawing::Size(239, 22);
			this->TDRItem->Text = L"&Time Domain Reflection";
			this->TDRItem->Click += gcnew System::EventHandler(this, &Form1::TDRItem_Click);
			// 
			// audioDevicesToolStripMenuItem
			// 
			this->audioDevicesToolStripMenuItem->Name = L"audioDevicesToolStripMenuItem";
			this->audioDevicesToolStripMenuItem->Size = System::Drawing::Size(239, 22);
			this->audioDevicesToolStripMenuItem->Text = L"Audio Devices";
			this->audioDevicesToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::audioDevicesToolStripMenuItem_Click);
			// 
			// signalGeneratorToolStripMenuItem
			// 
			this->signalGeneratorToolStripMenuItem->Name = L"signalGeneratorToolStripMenuItem";
			this->signalGeneratorToolStripMenuItem->Size = System::Drawing::Size(239, 22);
			this->signalGeneratorToolStripMenuItem->Text = L"Signal generator";
			this->signalGeneratorToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::signalGeneratorToolStripMenuItem_Click);
			// 
			// mockupDeviceToolStripMenuItem1
			// 
			this->mockupDeviceToolStripMenuItem1->Name = L"mockupDeviceToolStripMenuItem1";
			this->mockupDeviceToolStripMenuItem1->Size = System::Drawing::Size(239, 22);
			this->mockupDeviceToolStripMenuItem1->Text = L"Mockup Device";
			this->mockupDeviceToolStripMenuItem1->Click += gcnew System::EventHandler(this, &Form1::mockupDeviceToolStripMenuItem_Click);
			// 
			// calibrateMenu
			// 
			this->calibrateMenu->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->runItem, 
				this->loadItem, this->mockupDeviceToolStripMenuItem});
			this->calibrateMenu->Name = L"calibrateMenu";
			this->calibrateMenu->Size = System::Drawing::Size(77, 20);
			this->calibrateMenu->Text = L"&Calibration";
			// 
			// runItem
			// 
			this->runItem->Name = L"runItem";
			this->runItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::C));
			this->runItem->Size = System::Drawing::Size(229, 22);
			this->runItem->Text = L"&Fixture Calibration";
			this->runItem->Click += gcnew System::EventHandler(this, &Form1::runItem_Click);
			// 
			// loadItem
			// 
			this->loadItem->Name = L"loadItem";
			this->loadItem->Size = System::Drawing::Size(229, 22);
			this->loadItem->Text = L"&Load Fixture Calibration File...";
			this->loadItem->Click += gcnew System::EventHandler(this, &Form1::loadItem_Click);
			// 
			// mockupDeviceToolStripMenuItem
			// 
			this->mockupDeviceToolStripMenuItem->Name = L"mockupDeviceToolStripMenuItem";
			this->mockupDeviceToolStripMenuItem->Size = System::Drawing::Size(229, 22);
			this->mockupDeviceToolStripMenuItem->Text = L"&Mockup Device";
			this->mockupDeviceToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::mockupDeviceToolStripMenuItem_Click);
			// 
			// traceMenu
			// 
			this->traceMenu->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(23) {this->s11magItem, 
				this->s11phsItem, this->s21magItem, this->s21phsItem, this->s21groupdelayItem, this->VSWRdisplay, this->RDisplay, this->jXDisplay, 
				this->menuItem6, this->calSthru, this->menuItem1, this->EtTrace, this->EsTrace, this->EdTrace, this->menuItem2, this->calSshort, 
				this->calSopen, this->calSterm, this->toolStripSeparator1, this->pickTraceColorToolStripMenuItem, this->toolStripSeparator4, 
				this->rawDetectorDataToolStripMenuItem, this->audioDistanceToolStripMenuItem});
			this->traceMenu->Name = L"traceMenu";
			this->traceMenu->Size = System::Drawing::Size(47, 20);
			this->traceMenu->Text = L"&Trace";
			// 
			// s11magItem
			// 
			this->s11magItem->Checked = true;
			this->s11magItem->CheckState = System::Windows::Forms::CheckState::Checked;
			this->s11magItem->Name = L"s11magItem";
			this->s11magItem->ShortcutKeys = System::Windows::Forms::Keys::F2;
			this->s11magItem->Size = System::Drawing::Size(306, 22);
			this->s11magItem->Text = L"S11 Magnitude (rectangular)";
			this->s11magItem->Click += gcnew System::EventHandler(this, &Form1::s11magItem_Click);
			// 
			// s11phsItem
			// 
			this->s11phsItem->Name = L"s11phsItem";
			this->s11phsItem->ShortcutKeys = System::Windows::Forms::Keys::F3;
			this->s11phsItem->Size = System::Drawing::Size(306, 22);
			this->s11phsItem->Text = L"S11 Phase (rectangular)";
			this->s11phsItem->Click += gcnew System::EventHandler(this, &Form1::s11phsItem_Click);
			// 
			// s21magItem
			// 
			this->s21magItem->Checked = true;
			this->s21magItem->CheckState = System::Windows::Forms::CheckState::Checked;
			this->s21magItem->Name = L"s21magItem";
			this->s21magItem->ShortcutKeys = System::Windows::Forms::Keys::F4;
			this->s21magItem->Size = System::Drawing::Size(306, 22);
			this->s21magItem->Text = L"S21 Magnitude (rectangular)";
			this->s21magItem->Click += gcnew System::EventHandler(this, &Form1::s21magItem_Click);
			// 
			// s21phsItem
			// 
			this->s21phsItem->Name = L"s21phsItem";
			this->s21phsItem->ShortcutKeys = System::Windows::Forms::Keys::F5;
			this->s21phsItem->Size = System::Drawing::Size(306, 22);
			this->s21phsItem->Text = L"S21 Phase (rectangular)";
			this->s21phsItem->Click += gcnew System::EventHandler(this, &Form1::s21phsItem_Click);
			// 
			// s21groupdelayItem
			// 
			this->s21groupdelayItem->Name = L"s21groupdelayItem";
			this->s21groupdelayItem->ShortcutKeys = System::Windows::Forms::Keys::F6;
			this->s21groupdelayItem->Size = System::Drawing::Size(306, 22);
			this->s21groupdelayItem->Text = L"S21 Group Delay (rectangular)";
			this->s21groupdelayItem->Click += gcnew System::EventHandler(this, &Form1::s21groupdelayItem_Click);
			// 
			// VSWRdisplay
			// 
			this->VSWRdisplay->Name = L"VSWRdisplay";
			this->VSWRdisplay->ShortcutKeys = System::Windows::Forms::Keys::F7;
			this->VSWRdisplay->Size = System::Drawing::Size(306, 22);
			this->VSWRdisplay->Text = L"S11 Magnitude as SWR (rectangular)";
			this->VSWRdisplay->Click += gcnew System::EventHandler(this, &Form1::VSWRdisplay_Click);
			// 
			// RDisplay
			// 
			this->RDisplay->Name = L"RDisplay";
			this->RDisplay->ShortcutKeys = System::Windows::Forms::Keys::F8;
			this->RDisplay->Size = System::Drawing::Size(306, 22);
			this->RDisplay->Text = L"S11 as R ohms (rectangular)";
			this->RDisplay->Click += gcnew System::EventHandler(this, &Form1::RDisplay_Click);
			// 
			// jXDisplay
			// 
			this->jXDisplay->Name = L"jXDisplay";
			this->jXDisplay->ShortcutKeys = System::Windows::Forms::Keys::F9;
			this->jXDisplay->Size = System::Drawing::Size(306, 22);
			this->jXDisplay->Text = L"S11 as jX ohms (rectangular)";
			this->jXDisplay->Click += gcnew System::EventHandler(this, &Form1::jXDisplay_Click);
			// 
			// menuItem6
			// 
			this->menuItem6->Name = L"menuItem6";
			this->menuItem6->Size = System::Drawing::Size(303, 6);
			// 
			// calSthru
			// 
			this->calSthru->Name = L"calSthru";
			this->calSthru->Size = System::Drawing::Size(306, 22);
			this->calSthru->Text = L"Raw Calibration Data - S21thru (rectangular)";
			this->calSthru->Click += gcnew System::EventHandler(this, &Form1::calSthru_Click);
			// 
			// menuItem1
			// 
			this->menuItem1->Name = L"menuItem1";
			this->menuItem1->Size = System::Drawing::Size(303, 6);
			// 
			// EtTrace
			// 
			this->EtTrace->Name = L"EtTrace";
			this->EtTrace->Size = System::Drawing::Size(306, 22);
			this->EtTrace->Text = L"Et - Tracking Error Compensation";
			this->EtTrace->Click += gcnew System::EventHandler(this, &Form1::EtTrace_Click);
			// 
			// EsTrace
			// 
			this->EsTrace->Name = L"EsTrace";
			this->EsTrace->Size = System::Drawing::Size(306, 22);
			this->EsTrace->Text = L"Es - Source Mismatch Error Compensation";
			this->EsTrace->Click += gcnew System::EventHandler(this, &Form1::EsTrace_Click);
			// 
			// EdTrace
			// 
			this->EdTrace->Name = L"EdTrace";
			this->EdTrace->Size = System::Drawing::Size(306, 22);
			this->EdTrace->Text = L"Ed - Directivity Error Compensation";
			this->EdTrace->Click += gcnew System::EventHandler(this, &Form1::EdTrace_Click);
			// 
			// menuItem2
			// 
			this->menuItem2->Name = L"menuItem2";
			this->menuItem2->Size = System::Drawing::Size(303, 6);
			// 
			// calSshort
			// 
			this->calSshort->Name = L"calSshort";
			this->calSshort->Size = System::Drawing::Size(306, 22);
			this->calSshort->Text = L"Raw Calibration Data - S11short";
			this->calSshort->Click += gcnew System::EventHandler(this, &Form1::calSshort_Click);
			// 
			// calSopen
			// 
			this->calSopen->Name = L"calSopen";
			this->calSopen->Size = System::Drawing::Size(306, 22);
			this->calSopen->Text = L"Raw Calibration Data - S11open";
			this->calSopen->Click += gcnew System::EventHandler(this, &Form1::calSopen_Click);
			// 
			// calSterm
			// 
			this->calSterm->Name = L"calSterm";
			this->calSterm->Size = System::Drawing::Size(306, 22);
			this->calSterm->Text = L"Raw Calibration Data - S11term";
			this->calSterm->Click += gcnew System::EventHandler(this, &Form1::calSterm_Click);
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(303, 6);
			// 
			// pickTraceColorToolStripMenuItem
			// 
			this->pickTraceColorToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(9) {this->s11MagnitudeColorToolStripMenuItem, 
				this->s11PhaseColorToolStripMenuItem, this->s21MagnitudeColorToolStripMenuItem, this->s21PhaseColorToolStripMenuItem, this->s21GroupDelayColorToolStripMenuItem, 
				this->sWRColorToolStripMenuItem, this->s11AsRColorToolStripMenuItem, this->s11AsJXColorToolStripMenuItem, this->backgroundColorToolStripMenuItem});
			this->pickTraceColorToolStripMenuItem->Name = L"pickTraceColorToolStripMenuItem";
			this->pickTraceColorToolStripMenuItem->Size = System::Drawing::Size(306, 22);
			this->pickTraceColorToolStripMenuItem->Text = L"Choose Trace Colors";
			// 
			// s11MagnitudeColorToolStripMenuItem
			// 
			this->s11MagnitudeColorToolStripMenuItem->Name = L"s11MagnitudeColorToolStripMenuItem";
			this->s11MagnitudeColorToolStripMenuItem->Size = System::Drawing::Size(192, 22);
			this->s11MagnitudeColorToolStripMenuItem->Text = L"S11 Magnitude Color";
			this->s11MagnitudeColorToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::s11MagnitudeColorToolStripMenuItem_Click);
			// 
			// s11PhaseColorToolStripMenuItem
			// 
			this->s11PhaseColorToolStripMenuItem->Name = L"s11PhaseColorToolStripMenuItem";
			this->s11PhaseColorToolStripMenuItem->Size = System::Drawing::Size(192, 22);
			this->s11PhaseColorToolStripMenuItem->Text = L"S11 Phase Color";
			this->s11PhaseColorToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::s11PhaseColorToolStripMenuItem_Click);
			// 
			// s21MagnitudeColorToolStripMenuItem
			// 
			this->s21MagnitudeColorToolStripMenuItem->Name = L"s21MagnitudeColorToolStripMenuItem";
			this->s21MagnitudeColorToolStripMenuItem->Size = System::Drawing::Size(192, 22);
			this->s21MagnitudeColorToolStripMenuItem->Text = L"S21 Magnitude Color";
			this->s21MagnitudeColorToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::s21MagnitudeColorToolStripMenuItem_Click);
			// 
			// s21PhaseColorToolStripMenuItem
			// 
			this->s21PhaseColorToolStripMenuItem->Name = L"s21PhaseColorToolStripMenuItem";
			this->s21PhaseColorToolStripMenuItem->Size = System::Drawing::Size(192, 22);
			this->s21PhaseColorToolStripMenuItem->Text = L"S21 Phase Color";
			this->s21PhaseColorToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::s21PhaseColorToolStripMenuItem_Click);
			// 
			// s21GroupDelayColorToolStripMenuItem
			// 
			this->s21GroupDelayColorToolStripMenuItem->Name = L"s21GroupDelayColorToolStripMenuItem";
			this->s21GroupDelayColorToolStripMenuItem->Size = System::Drawing::Size(192, 22);
			this->s21GroupDelayColorToolStripMenuItem->Text = L"S21 Group Delay Color";
			this->s21GroupDelayColorToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::s21GroupDelayColorToolStripMenuItem_Click);
			// 
			// sWRColorToolStripMenuItem
			// 
			this->sWRColorToolStripMenuItem->Name = L"sWRColorToolStripMenuItem";
			this->sWRColorToolStripMenuItem->Size = System::Drawing::Size(192, 22);
			this->sWRColorToolStripMenuItem->Text = L"SWR Color";
			this->sWRColorToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::sWRColorToolStripMenuItem_Click);
			// 
			// s11AsRColorToolStripMenuItem
			// 
			this->s11AsRColorToolStripMenuItem->Name = L"s11AsRColorToolStripMenuItem";
			this->s11AsRColorToolStripMenuItem->Size = System::Drawing::Size(192, 22);
			this->s11AsRColorToolStripMenuItem->Text = L"S11 as R Color";
			this->s11AsRColorToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::s11AsRColorToolStripMenuItem_Click);
			// 
			// s11AsJXColorToolStripMenuItem
			// 
			this->s11AsJXColorToolStripMenuItem->Name = L"s11AsJXColorToolStripMenuItem";
			this->s11AsJXColorToolStripMenuItem->Size = System::Drawing::Size(192, 22);
			this->s11AsJXColorToolStripMenuItem->Text = L"S11 as jX color";
			this->s11AsJXColorToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::s11AsJXColorToolStripMenuItem_Click);
			// 
			// backgroundColorToolStripMenuItem
			// 
			this->backgroundColorToolStripMenuItem->Name = L"backgroundColorToolStripMenuItem";
			this->backgroundColorToolStripMenuItem->Size = System::Drawing::Size(192, 22);
			this->backgroundColorToolStripMenuItem->Text = L"Background Color";
			this->backgroundColorToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::backgroundColorToolStripMenuItem_Click);
			// 
			// toolStripSeparator4
			// 
			this->toolStripSeparator4->Name = L"toolStripSeparator4";
			this->toolStripSeparator4->Size = System::Drawing::Size(303, 6);
			// 
			// rawDetectorDataToolStripMenuItem
			// 
			this->rawDetectorDataToolStripMenuItem->Name = L"rawDetectorDataToolStripMenuItem";
			this->rawDetectorDataToolStripMenuItem->Size = System::Drawing::Size(306, 22);
			this->rawDetectorDataToolStripMenuItem->Text = L"Raw Detector Data";
			this->rawDetectorDataToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::rawDetectorDataToolStripMenuItem_Click);
			// 
			// audioDistanceToolStripMenuItem
			// 
			this->audioDistanceToolStripMenuItem->Name = L"audioDistanceToolStripMenuItem";
			this->audioDistanceToolStripMenuItem->Size = System::Drawing::Size(306, 22);
			this->audioDistanceToolStripMenuItem->Text = L"Audio Distance";
			this->audioDistanceToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::audioDistanceToolStripMenuItem_Click);
			// 
			// VertScaleItem
			// 
			this->VertScaleItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(19) {this->Scale10dB, 
				this->Scale5dB, this->Scale2dB, this->Scale1dB, this->Scale05dB, this->menuItem4, this->toolStripMenuItem1, this->menuItem20, 
				this->toolStripMenuItem3, this->menuItem13, this->menuItem15, this->menuItem10, this->toolStripMenuItem4, this->toolStripSeparator2, 
				this->toolStripMenuItem2, this->menuItem16, this->menuItem17, this->menuItem21, this->TDRSetupmenuItem});
			this->VertScaleItem->Name = L"VertScaleItem";
			this->VertScaleItem->Size = System::Drawing::Size(66, 20);
			this->VertScaleItem->Text = L"Vert&Scale";
			// 
			// Scale10dB
			// 
			this->Scale10dB->Checked = true;
			this->Scale10dB->CheckState = System::Windows::Forms::CheckState::Checked;
			this->Scale10dB->Name = L"Scale10dB";
			this->Scale10dB->Size = System::Drawing::Size(199, 22);
			this->Scale10dB->Text = L"10 dB / div";
			this->Scale10dB->Click += gcnew System::EventHandler(this, &Form1::Scale10dB_Click);
			// 
			// Scale5dB
			// 
			this->Scale5dB->Name = L"Scale5dB";
			this->Scale5dB->Size = System::Drawing::Size(199, 22);
			this->Scale5dB->Text = L"5 dB / div";
			this->Scale5dB->Click += gcnew System::EventHandler(this, &Form1::Scale5dB_Click);
			// 
			// Scale2dB
			// 
			this->Scale2dB->Name = L"Scale2dB";
			this->Scale2dB->Size = System::Drawing::Size(199, 22);
			this->Scale2dB->Text = L"2 dB / div";
			this->Scale2dB->Click += gcnew System::EventHandler(this, &Form1::Scale2dB_Click);
			// 
			// Scale1dB
			// 
			this->Scale1dB->Name = L"Scale1dB";
			this->Scale1dB->Size = System::Drawing::Size(199, 22);
			this->Scale1dB->Text = L"1 dB / div";
			this->Scale1dB->Click += gcnew System::EventHandler(this, &Form1::Scale1dB_Click);
			// 
			// Scale05dB
			// 
			this->Scale05dB->Name = L"Scale05dB";
			this->Scale05dB->Size = System::Drawing::Size(199, 22);
			this->Scale05dB->Text = L"0.5 dB / div";
			this->Scale05dB->Click += gcnew System::EventHandler(this, &Form1::Scale05dB_Click);
			// 
			// menuItem4
			// 
			this->menuItem4->Name = L"menuItem4";
			this->menuItem4->Size = System::Drawing::Size(196, 6);
			// 
			// toolStripMenuItem1
			// 
			this->toolStripMenuItem1->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->ScaleSWR10, 
				this->ScaleSWR5, this->ScaleSWR2, this->ScaleSWR1});
			this->toolStripMenuItem1->Name = L"toolStripMenuItem1";
			this->toolStripMenuItem1->Size = System::Drawing::Size(199, 22);
			this->toolStripMenuItem1->Text = L"SWR Scale";
			// 
			// ScaleSWR10
			// 
			this->ScaleSWR10->Checked = true;
			this->ScaleSWR10->CheckState = System::Windows::Forms::CheckState::Checked;
			this->ScaleSWR10->Name = L"ScaleSWR10";
			this->ScaleSWR10->Size = System::Drawing::Size(154, 22);
			this->ScaleSWR10->Text = L"SWR 1.0 to 11.0";
			this->ScaleSWR10->Click += gcnew System::EventHandler(this, &Form1::ScaleSWR10_Click);
			// 
			// ScaleSWR5
			// 
			this->ScaleSWR5->Name = L"ScaleSWR5";
			this->ScaleSWR5->Size = System::Drawing::Size(154, 22);
			this->ScaleSWR5->Text = L"SWR 1.0 to 6.0";
			this->ScaleSWR5->Click += gcnew System::EventHandler(this, &Form1::ScaleSWR5_Click);
			// 
			// ScaleSWR2
			// 
			this->ScaleSWR2->Name = L"ScaleSWR2";
			this->ScaleSWR2->Size = System::Drawing::Size(154, 22);
			this->ScaleSWR2->Text = L"SWR 1.0 to 3.0";
			this->ScaleSWR2->Click += gcnew System::EventHandler(this, &Form1::ScaleSWR2_Click);
			// 
			// ScaleSWR1
			// 
			this->ScaleSWR1->Name = L"ScaleSWR1";
			this->ScaleSWR1->Size = System::Drawing::Size(154, 22);
			this->ScaleSWR1->Text = L"SWR 1.0 to 2.0";
			this->ScaleSWR1->Click += gcnew System::EventHandler(this, &Form1::ScaleSWR1_Click);
			// 
			// menuItem20
			// 
			this->menuItem20->Name = L"menuItem20";
			this->menuItem20->Size = System::Drawing::Size(196, 6);
			// 
			// toolStripMenuItem3
			// 
			this->toolStripMenuItem3->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->scale1kOhms, 
				this->scale100Ohms, this->scale10Ohms});
			this->toolStripMenuItem3->Name = L"toolStripMenuItem3";
			this->toolStripMenuItem3->Size = System::Drawing::Size(199, 22);
			this->toolStripMenuItem3->Text = L"Impedance Scale";
			// 
			// scale1kOhms
			// 
			this->scale1kOhms->Name = L"scale1kOhms";
			this->scale1kOhms->Size = System::Drawing::Size(152, 22);
			this->scale1kOhms->Text = L"1k ohms / div";
			this->scale1kOhms->Click += gcnew System::EventHandler(this, &Form1::scale1kOhms_Click);
			// 
			// scale100Ohms
			// 
			this->scale100Ohms->Checked = true;
			this->scale100Ohms->CheckState = System::Windows::Forms::CheckState::Checked;
			this->scale100Ohms->Name = L"scale100Ohms";
			this->scale100Ohms->Size = System::Drawing::Size(152, 22);
			this->scale100Ohms->Text = L"100 ohms / div";
			this->scale100Ohms->Click += gcnew System::EventHandler(this, &Form1::scale100Ohms_Click);
			// 
			// scale10Ohms
			// 
			this->scale10Ohms->Name = L"scale10Ohms";
			this->scale10Ohms->Size = System::Drawing::Size(152, 22);
			this->scale10Ohms->Text = L"10 ohms / div";
			this->scale10Ohms->Click += gcnew System::EventHandler(this, &Form1::scale10Ohms_Click);
			// 
			// menuItem13
			// 
			this->menuItem13->Name = L"menuItem13";
			this->menuItem13->Size = System::Drawing::Size(196, 6);
			// 
			// menuItem15
			// 
			this->menuItem15->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->RightScalePhase, 
				this->RightScaleSWR, this->RightScaleDelay, this->RightScaleOhms});
			this->menuItem15->Name = L"menuItem15";
			this->menuItem15->Size = System::Drawing::Size(199, 22);
			this->menuItem15->Text = L"Right Scale Display";
			// 
			// RightScalePhase
			// 
			this->RightScalePhase->Checked = true;
			this->RightScalePhase->CheckState = System::Windows::Forms::CheckState::Checked;
			this->RightScalePhase->Name = L"RightScalePhase";
			this->RightScalePhase->Size = System::Drawing::Size(106, 22);
			this->RightScalePhase->Text = L"Phase";
			this->RightScalePhase->Click += gcnew System::EventHandler(this, &Form1::RightScalePhase_Click);
			// 
			// RightScaleSWR
			// 
			this->RightScaleSWR->Name = L"RightScaleSWR";
			this->RightScaleSWR->Size = System::Drawing::Size(106, 22);
			this->RightScaleSWR->Text = L"SWR";
			this->RightScaleSWR->Click += gcnew System::EventHandler(this, &Form1::RightScaleSWR_Click);
			// 
			// RightScaleDelay
			// 
			this->RightScaleDelay->Name = L"RightScaleDelay";
			this->RightScaleDelay->Size = System::Drawing::Size(106, 22);
			this->RightScaleDelay->Text = L"Delay";
			this->RightScaleDelay->Click += gcnew System::EventHandler(this, &Form1::RightScaleDelay_Click);
			// 
			// RightScaleOhms
			// 
			this->RightScaleOhms->Name = L"RightScaleOhms";
			this->RightScaleOhms->Size = System::Drawing::Size(106, 22);
			this->RightScaleOhms->Text = L"Ohms";
			this->RightScaleOhms->Click += gcnew System::EventHandler(this, &Form1::RightScaleOhms_Click);
			// 
			// menuItem10
			// 
			this->menuItem10->Name = L"menuItem10";
			this->menuItem10->Size = System::Drawing::Size(196, 6);
			// 
			// toolStripMenuItem4
			// 
			this->toolStripMenuItem4->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(8) {this->scale1millisec, 
				this->scale100microsec, this->scale10microsec, this->scalemicroSec, this->scale100nsec, this->scale10nsec, this->scalenanoSec, 
				this->scale100psec});
			this->toolStripMenuItem4->Name = L"toolStripMenuItem4";
			this->toolStripMenuItem4->Size = System::Drawing::Size(199, 22);
			this->toolStripMenuItem4->Text = L"Group Delay Time Scale";
			// 
			// scale1millisec
			// 
			this->scale1millisec->Name = L"scale1millisec";
			this->scale1millisec->Size = System::Drawing::Size(171, 22);
			this->scale1millisec->Text = L"1 millisecond / div";
			this->scale1millisec->Click += gcnew System::EventHandler(this, &Form1::scale1millisec_Click);
			// 
			// scale100microsec
			// 
			this->scale100microsec->Name = L"scale100microsec";
			this->scale100microsec->Size = System::Drawing::Size(171, 22);
			this->scale100microsec->Text = L"100 microsec / div";
			this->scale100microsec->Click += gcnew System::EventHandler(this, &Form1::scale100microsec_Click);
			// 
			// scale10microsec
			// 
			this->scale10microsec->Name = L"scale10microsec";
			this->scale10microsec->Size = System::Drawing::Size(171, 22);
			this->scale10microsec->Text = L"10 microsec / div";
			this->scale10microsec->Click += gcnew System::EventHandler(this, &Form1::scale10microsec_Click);
			// 
			// scalemicroSec
			// 
			this->scalemicroSec->Checked = true;
			this->scalemicroSec->CheckState = System::Windows::Forms::CheckState::Checked;
			this->scalemicroSec->Name = L"scalemicroSec";
			this->scalemicroSec->Size = System::Drawing::Size(171, 22);
			this->scalemicroSec->Text = L"1 microsec / div";
			this->scalemicroSec->Click += gcnew System::EventHandler(this, &Form1::scalemicroSec_Click);
			// 
			// scale100nsec
			// 
			this->scale100nsec->Name = L"scale100nsec";
			this->scale100nsec->Size = System::Drawing::Size(171, 22);
			this->scale100nsec->Text = L"100 nanosec / div";
			this->scale100nsec->Click += gcnew System::EventHandler(this, &Form1::scale100nsec_Click);
			// 
			// scale10nsec
			// 
			this->scale10nsec->Name = L"scale10nsec";
			this->scale10nsec->Size = System::Drawing::Size(171, 22);
			this->scale10nsec->Text = L"10 nanosec / div";
			this->scale10nsec->Click += gcnew System::EventHandler(this, &Form1::scale10nsec_Click);
			// 
			// scalenanoSec
			// 
			this->scalenanoSec->Name = L"scalenanoSec";
			this->scalenanoSec->Size = System::Drawing::Size(171, 22);
			this->scalenanoSec->Text = L"1 nanosec / div";
			this->scalenanoSec->Click += gcnew System::EventHandler(this, &Form1::scalenanoSec_Click);
			// 
			// scale100psec
			// 
			this->scale100psec->Name = L"scale100psec";
			this->scale100psec->Size = System::Drawing::Size(171, 22);
			this->scale100psec->Text = L"100 picosec / div";
			this->scale100psec->Click += gcnew System::EventHandler(this, &Form1::scale100psec_Click);
			// 
			// toolStripSeparator2
			// 
			this->toolStripSeparator2->Name = L"toolStripSeparator2";
			this->toolStripSeparator2->Size = System::Drawing::Size(196, 6);
			// 
			// toolStripMenuItem2
			// 
			this->toolStripMenuItem2->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->GDAperture1, 
				this->GDAperture4, this->GDAperture16, this->GDAperture64});
			this->toolStripMenuItem2->Name = L"toolStripMenuItem2";
			this->toolStripMenuItem2->Size = System::Drawing::Size(199, 22);
			this->toolStripMenuItem2->Text = L"Group Delay Aperture";
			// 
			// GDAperture1
			// 
			this->GDAperture1->Checked = true;
			this->GDAperture1->CheckState = System::Windows::Forms::CheckState::Checked;
			this->GDAperture1->Name = L"GDAperture1";
			this->GDAperture1->Size = System::Drawing::Size(214, 22);
			this->GDAperture1->Text = L"Group Delay Aperture = 1";
			this->GDAperture1->Click += gcnew System::EventHandler(this, &Form1::GDAperture1_Click);
			// 
			// GDAperture4
			// 
			this->GDAperture4->Name = L"GDAperture4";
			this->GDAperture4->Size = System::Drawing::Size(214, 22);
			this->GDAperture4->Text = L"Group Delay Aperture = 4";
			this->GDAperture4->Click += gcnew System::EventHandler(this, &Form1::GDAperture4_Click);
			// 
			// GDAperture16
			// 
			this->GDAperture16->Name = L"GDAperture16";
			this->GDAperture16->Size = System::Drawing::Size(214, 22);
			this->GDAperture16->Text = L"Group Delay Aperture = 16";
			this->GDAperture16->Click += gcnew System::EventHandler(this, &Form1::GDAperture16_Click);
			// 
			// GDAperture64
			// 
			this->GDAperture64->Name = L"GDAperture64";
			this->GDAperture64->Size = System::Drawing::Size(214, 22);
			this->GDAperture64->Text = L"Group Delay Aperture = 64";
			this->GDAperture64->Click += gcnew System::EventHandler(this, &Form1::GDAperture64_Click);
			// 
			// menuItem16
			// 
			this->menuItem16->Name = L"menuItem16";
			this->menuItem16->Size = System::Drawing::Size(196, 6);
			// 
			// menuItem17
			// 
			this->menuItem17->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->PolarZoom10Menu, 
				this->PolarZoom15Menu, this->PolarZoom20Menu, this->PolarZoom25Menu});
			this->menuItem17->Name = L"menuItem17";
			this->menuItem17->Size = System::Drawing::Size(199, 22);
			this->menuItem17->Text = L"Polar Zoom";
			// 
			// PolarZoom10Menu
			// 
			this->PolarZoom10Menu->Checked = true;
			this->PolarZoom10Menu->CheckState = System::Windows::Forms::CheckState::Checked;
			this->PolarZoom10Menu->Name = L"PolarZoom10Menu";
			this->PolarZoom10Menu->Size = System::Drawing::Size(135, 22);
			this->PolarZoom10Menu->Text = L"Zoom = &1.0";
			this->PolarZoom10Menu->Click += gcnew System::EventHandler(this, &Form1::PolarZoom10Menu_Click);
			// 
			// PolarZoom15Menu
			// 
			this->PolarZoom15Menu->Name = L"PolarZoom15Menu";
			this->PolarZoom15Menu->Size = System::Drawing::Size(135, 22);
			this->PolarZoom15Menu->Text = L"Zoom = 1.&5";
			this->PolarZoom15Menu->Click += gcnew System::EventHandler(this, &Form1::PolarZoom15Menu_Click);
			// 
			// PolarZoom20Menu
			// 
			this->PolarZoom20Menu->Name = L"PolarZoom20Menu";
			this->PolarZoom20Menu->Size = System::Drawing::Size(135, 22);
			this->PolarZoom20Menu->Text = L"Zoom = &2.0";
			this->PolarZoom20Menu->Click += gcnew System::EventHandler(this, &Form1::PolarZoom20Menu_Click);
			// 
			// PolarZoom25Menu
			// 
			this->PolarZoom25Menu->Name = L"PolarZoom25Menu";
			this->PolarZoom25Menu->Size = System::Drawing::Size(135, 22);
			this->PolarZoom25Menu->Text = L"&Zoom = 2.5";
			this->PolarZoom25Menu->Click += gcnew System::EventHandler(this, &Form1::PolarZoom25Menu_Click);
			// 
			// menuItem21
			// 
			this->menuItem21->Name = L"menuItem21";
			this->menuItem21->Size = System::Drawing::Size(196, 6);
			// 
			// TDRSetupmenuItem
			// 
			this->TDRSetupmenuItem->Name = L"TDRSetupmenuItem";
			this->TDRSetupmenuItem->Size = System::Drawing::Size(199, 22);
			this->TDRSetupmenuItem->Text = L"TDR Setup...";
			this->TDRSetupmenuItem->Click += gcnew System::EventHandler(this, &Form1::TDRSetupmenuItem_Click);
			// 
			// markerMenu
			// 
			this->markerMenu->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->ChangeMarker, 
				this->ClearAllMarkers, this->menuItem9, this->menuItem12});
			this->markerMenu->Name = L"markerMenu";
			this->markerMenu->Size = System::Drawing::Size(56, 20);
			this->markerMenu->Text = L"&Marker";
			// 
			// ChangeMarker
			// 
			this->ChangeMarker->Name = L"ChangeMarker";
			this->ChangeMarker->Size = System::Drawing::Size(208, 22);
			this->ChangeMarker->Text = L"&Change Marker Points";
			this->ChangeMarker->Click += gcnew System::EventHandler(this, &Form1::ChangeMarker_Click);
			// 
			// ClearAllMarkers
			// 
			this->ClearAllMarkers->Name = L"ClearAllMarkers";
			this->ClearAllMarkers->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::M));
			this->ClearAllMarkers->Size = System::Drawing::Size(208, 22);
			this->ClearAllMarkers->Text = L"Clear &All Markers";
			this->ClearAllMarkers->Click += gcnew System::EventHandler(this, &Form1::ClearAllMarkers_Click);
			// 
			// menuItem9
			// 
			this->menuItem9->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->MarkerNumEnable, 
				this->MarkerNumDisable});
			this->menuItem9->Name = L"menuItem9";
			this->menuItem9->Size = System::Drawing::Size(208, 22);
			this->menuItem9->Text = L"Marker &Numbers";
			// 
			// MarkerNumEnable
			// 
			this->MarkerNumEnable->Name = L"MarkerNumEnable";
			this->MarkerNumEnable->Size = System::Drawing::Size(112, 22);
			this->MarkerNumEnable->Text = L"&Enable";
			this->MarkerNumEnable->Click += gcnew System::EventHandler(this, &Form1::MarkerNumEnable_Click);
			// 
			// MarkerNumDisable
			// 
			this->MarkerNumDisable->Name = L"MarkerNumDisable";
			this->MarkerNumDisable->Size = System::Drawing::Size(112, 22);
			this->MarkerNumDisable->Text = L"&Disable";
			this->MarkerNumDisable->Click += gcnew System::EventHandler(this, &Form1::MarkerNumDisable_Click);
			// 
			// menuItem12
			// 
			this->menuItem12->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->MarkerParamEnable, 
				this->MarkerParamDisable});
			this->menuItem12->Name = L"menuItem12";
			this->menuItem12->Size = System::Drawing::Size(208, 22);
			this->menuItem12->Text = L"Marker Parameters";
			// 
			// MarkerParamEnable
			// 
			this->MarkerParamEnable->Name = L"MarkerParamEnable";
			this->MarkerParamEnable->Size = System::Drawing::Size(112, 22);
			this->MarkerParamEnable->Text = L"&Enable";
			this->MarkerParamEnable->Click += gcnew System::EventHandler(this, &Form1::MarkerParamEnable_Click);
			// 
			// MarkerParamDisable
			// 
			this->MarkerParamDisable->Name = L"MarkerParamDisable";
			this->MarkerParamDisable->Size = System::Drawing::Size(112, 22);
			this->MarkerParamDisable->Text = L"&Disable";
			this->MarkerParamDisable->Click += gcnew System::EventHandler(this, &Form1::MarkerParamDisable_Click);
			// 
			// menuItem5
			// 
			this->menuItem5->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->grid101menu, 
				this->grid201menu, this->grid401menu, this->grid1024menu});
			this->menuItem5->Name = L"menuItem5";
			this->menuItem5->Size = System::Drawing::Size(64, 20);
			this->menuItem5->Text = L"Freq&Grid";
			// 
			// grid101menu
			// 
			this->grid101menu->Name = L"grid101menu";
			this->grid101menu->Size = System::Drawing::Size(134, 22);
			this->grid101menu->Text = L"100 points";
			this->grid101menu->Click += gcnew System::EventHandler(this, &Form1::grid101menu_Click);
			// 
			// grid201menu
			// 
			this->grid201menu->Checked = true;
			this->grid201menu->CheckState = System::Windows::Forms::CheckState::Checked;
			this->grid201menu->Name = L"grid201menu";
			this->grid201menu->Size = System::Drawing::Size(134, 22);
			this->grid201menu->Text = L"200 points";
			this->grid201menu->Click += gcnew System::EventHandler(this, &Form1::grid201menu_Click);
			// 
			// grid401menu
			// 
			this->grid401menu->Name = L"grid401menu";
			this->grid401menu->Size = System::Drawing::Size(134, 22);
			this->grid401menu->Text = L"400 points";
			this->grid401menu->Click += gcnew System::EventHandler(this, &Form1::grid401menu_Click);
			// 
			// grid1024menu
			// 
			this->grid1024menu->Name = L"grid1024menu";
			this->grid1024menu->Size = System::Drawing::Size(134, 22);
			this->grid1024menu->Text = L"1020 points";
			this->grid1024menu->Click += gcnew System::EventHandler(this, &Form1::grid1024menu_Click);
			// 
			// StorageMenu
			// 
			this->StorageMenu->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->StoreMenuItem, 
				this->RecallMenuItem, this->toolStripSeparator3, this->DisplayMeasured, this->DisplayMenuItem, this->displayMeasuredMinusStored});
			this->StorageMenu->Name = L"StorageMenu";
			this->StorageMenu->Size = System::Drawing::Size(62, 20);
			this->StorageMenu->Text = L" St&orage";
			// 
			// StoreMenuItem
			// 
			this->StoreMenuItem->Name = L"StoreMenuItem";
			this->StoreMenuItem->Size = System::Drawing::Size(246, 22);
			this->StoreMenuItem->Text = L"&Store";
			this->StoreMenuItem->Click += gcnew System::EventHandler(this, &Form1::StoreMenuItem_Click);
			// 
			// RecallMenuItem
			// 
			this->RecallMenuItem->Name = L"RecallMenuItem";
			this->RecallMenuItem->Size = System::Drawing::Size(246, 22);
			this->RecallMenuItem->Text = L"&Recall";
			this->RecallMenuItem->Click += gcnew System::EventHandler(this, &Form1::RecallMenuItem_Click);
			// 
			// toolStripSeparator3
			// 
			this->toolStripSeparator3->Name = L"toolStripSeparator3";
			this->toolStripSeparator3->Size = System::Drawing::Size(243, 6);
			// 
			// DisplayMeasured
			// 
			this->DisplayMeasured->Checked = true;
			this->DisplayMeasured->CheckState = System::Windows::Forms::CheckState::Checked;
			this->DisplayMeasured->Name = L"DisplayMeasured";
			this->DisplayMeasured->Size = System::Drawing::Size(246, 22);
			this->DisplayMeasured->Text = L"Display &Measured";
			this->DisplayMeasured->Click += gcnew System::EventHandler(this, &Form1::DisplayMeasured_Click);
			// 
			// DisplayMenuItem
			// 
			this->DisplayMenuItem->Name = L"DisplayMenuItem";
			this->DisplayMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D));
			this->DisplayMenuItem->Size = System::Drawing::Size(246, 22);
			this->DisplayMenuItem->Text = L"&Display Storage";
			this->DisplayMenuItem->Click += gcnew System::EventHandler(this, &Form1::DisplayMenuItem_Click);
			// 
			// displayMeasuredMinusStored
			// 
			this->displayMeasuredMinusStored->Name = L"displayMeasuredMinusStored";
			this->displayMeasuredMinusStored->Size = System::Drawing::Size(246, 22);
			this->displayMeasuredMinusStored->Text = L"Display   Measured min&us Stored";
			this->displayMeasuredMinusStored->Click += gcnew System::EventHandler(this, &Form1::ToggleMeasuredMinusStored);
			// 
			// IntegrationMenuItem
			// 
			this->IntegrationMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {this->IntegrationMenu1x, 
				this->IntegrationMenu2x, this->IntegrationMenu4x, this->IntegrationMenu8x, this->IntegrationMenu16x});
			this->IntegrationMenuItem->Name = L"IntegrationMenuItem";
			this->IntegrationMenuItem->Size = System::Drawing::Size(77, 20);
			this->IntegrationMenuItem->Text = L"&Integration";
			// 
			// IntegrationMenu1x
			// 
			this->IntegrationMenu1x->Checked = true;
			this->IntegrationMenu1x->CheckState = System::Windows::Forms::CheckState::Checked;
			this->IntegrationMenu1x->Name = L"IntegrationMenu1x";
			this->IntegrationMenu1x->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D1));
			this->IntegrationMenu1x->Size = System::Drawing::Size(131, 22);
			this->IntegrationMenu1x->Text = L"1x";
			this->IntegrationMenu1x->Click += gcnew System::EventHandler(this, &Form1::IntegrationMenu1x_Click);
			// 
			// IntegrationMenu2x
			// 
			this->IntegrationMenu2x->Name = L"IntegrationMenu2x";
			this->IntegrationMenu2x->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D2));
			this->IntegrationMenu2x->Size = System::Drawing::Size(131, 22);
			this->IntegrationMenu2x->Text = L"2x";
			this->IntegrationMenu2x->Click += gcnew System::EventHandler(this, &Form1::IntegrationMenu2x_Click);
			// 
			// IntegrationMenu4x
			// 
			this->IntegrationMenu4x->Name = L"IntegrationMenu4x";
			this->IntegrationMenu4x->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D4));
			this->IntegrationMenu4x->Size = System::Drawing::Size(131, 22);
			this->IntegrationMenu4x->Text = L"4x";
			this->IntegrationMenu4x->Click += gcnew System::EventHandler(this, &Form1::IntegrationMenu4x_Click);
			// 
			// IntegrationMenu8x
			// 
			this->IntegrationMenu8x->Name = L"IntegrationMenu8x";
			this->IntegrationMenu8x->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D8));
			this->IntegrationMenu8x->Size = System::Drawing::Size(131, 22);
			this->IntegrationMenu8x->Text = L"8x";
			this->IntegrationMenu8x->Click += gcnew System::EventHandler(this, &Form1::IntegrationMenu8x_Click);
			// 
			// IntegrationMenu16x
			// 
			this->IntegrationMenu16x->Name = L"IntegrationMenu16x";
			this->IntegrationMenu16x->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D6));
			this->IntegrationMenu16x->Size = System::Drawing::Size(131, 22);
			this->IntegrationMenu16x->Text = L"16x";
			this->IntegrationMenu16x->Click += gcnew System::EventHandler(this, &Form1::IntegrationMenu16x_Click);
			// 
			// HelpItem
			// 
			this->HelpItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->HelpMenuItem, 
				this->AboutMenuItem});
			this->HelpItem->Name = L"HelpItem";
			this->HelpItem->Size = System::Drawing::Size(44, 20);
			this->HelpItem->Text = L"&Help";
			// 
			// HelpMenuItem
			// 
			this->HelpMenuItem->Name = L"HelpMenuItem";
			this->HelpMenuItem->ShortcutKeys = System::Windows::Forms::Keys::F1;
			this->HelpMenuItem->Size = System::Drawing::Size(159, 22);
			this->HelpMenuItem->Text = L"&Display Help";
			this->HelpMenuItem->Click += gcnew System::EventHandler(this, &Form1::HelpMenuItem_Click);
			// 
			// AboutMenuItem
			// 
			this->AboutMenuItem->Name = L"AboutMenuItem";
			this->AboutMenuItem->Size = System::Drawing::Size(159, 22);
			this->AboutMenuItem->Text = L"&About";
			this->AboutMenuItem->Click += gcnew System::EventHandler(this, &Form1::AboutMenuItem_Click);
			// 
			// settingsToolStripMenuItem
			// 
			this->settingsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->serialPortToolStripMenuItem, 
				this->dumpMeasurementsToolStripMenuItem});
			this->settingsToolStripMenuItem->Name = L"settingsToolStripMenuItem";
			this->settingsToolStripMenuItem->Size = System::Drawing::Size(61, 20);
			this->settingsToolStripMenuItem->Text = L"Settings";
			this->settingsToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::settingsToolStripMenuItem_Click);
			// 
			// serialPortToolStripMenuItem
			// 
			this->serialPortToolStripMenuItem->Name = L"serialPortToolStripMenuItem";
			this->serialPortToolStripMenuItem->Size = System::Drawing::Size(185, 22);
			this->serialPortToolStripMenuItem->Text = L"SerialPort";
			this->serialPortToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::serialPortToolStripMenuItem_Click);
			// 
			// dumpMeasurementsToolStripMenuItem
			// 
			this->dumpMeasurementsToolStripMenuItem->Name = L"dumpMeasurementsToolStripMenuItem";
			this->dumpMeasurementsToolStripMenuItem->Size = System::Drawing::Size(185, 22);
			this->dumpMeasurementsToolStripMenuItem->Text = L"DumpMeasurements";
			this->dumpMeasurementsToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::dumpMeasurementsToolStripMenuItem_Click);
			// 
			// RefExtnCheckBox
			// 
			this->RefExtnCheckBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->RefExtnCheckBox->AutoSize = true;
			this->RefExtnCheckBox->Location = System::Drawing::Point(914, 397);
			this->RefExtnCheckBox->Name = L"RefExtnCheckBox";
			this->RefExtnCheckBox->Size = System::Drawing::Size(15, 14);
			this->RefExtnCheckBox->TabIndex = 28;
			this->RefExtnCheckBox->UseVisualStyleBackColor = true;
			this->RefExtnCheckBox->CheckedChanged += gcnew System::EventHandler(this, &Form1::RefExtnCheckBox_CheckedChanged);
			// 
			// label6
			// 
			this->label6->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label6->BackColor = System::Drawing::Color::Transparent;
			this->label6->ForeColor = System::Drawing::SystemColors::InfoText;
			this->label6->Location = System::Drawing::Point(836, 394);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(76, 19);
			this->label6->TabIndex = 29;
			this->label6->Text = L"Refl Plane Ext";
			this->label6->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// Spectrum
			// 
			this->Spectrum->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->Spectrum->AutoSize = true;
			this->Spectrum->Location = System::Drawing::Point(819, 372);
			this->Spectrum->Name = L"Spectrum";
			this->Spectrum->Size = System::Drawing::Size(15, 14);
			this->Spectrum->TabIndex = 30;
			this->Spectrum->UseVisualStyleBackColor = true;
			this->Spectrum->CheckedChanged += gcnew System::EventHandler(this, &Form1::Spectrum_CheckedChanged);
			// 
			// MIndex
			// 
			this->MIndex->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->MIndex->Location = System::Drawing::Point(145, 318);
			this->MIndex->Maximum = 1023;
			this->MIndex->Name = L"MIndex";
			this->MIndex->Size = System::Drawing::Size(790, 45);
			this->MIndex->TabIndex = 32;
			this->MIndex->TickStyle = System::Windows::Forms::TickStyle::None;
			this->MIndex->ValueChanged += gcnew System::EventHandler(this, &Form1::MIndex_ValueChanged);
			// 
			// AddMarkerButton
			// 
			this->AddMarkerButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->AddMarkerButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"AddMarkerButton.Image")));
			this->AddMarkerButton->Location = System::Drawing::Point(133, 318);
			this->AddMarkerButton->Name = L"AddMarkerButton";
			this->AddMarkerButton->Size = System::Drawing::Size(15, 14);
			this->AddMarkerButton->TabIndex = 33;
			this->AddMarkerButton->UseVisualStyleBackColor = true;
			this->AddMarkerButton->Click += gcnew System::EventHandler(this, &Form1::AddMarkerButton_Click);
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->AutoScroll = true;
			this->BackColor = System::Drawing::Color::White;
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::None;
			this->ClientSize = System::Drawing::Size(935, 431);
			this->Controls->Add(this->AddMarkerButton);
			this->Controls->Add(this->Spectrum);
			this->Controls->Add(this->stopF);
			this->Controls->Add(this->startF);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->RefExtnCheckBox);
			this->Controls->Add(this->SweepProgressBar);
			this->Controls->Add(this->FrequencyDigitDecrease);
			this->Controls->Add(this->FrequencyDigitIncrease);
			this->Controls->Add(this->StopFrequencyDigitIndicator);
			this->Controls->Add(this->StartFrequencyDigitIndicator);
			this->Controls->Add(this->SweepSpd);
			this->Controls->Add(this->RecurrentSweep);
			this->Controls->Add(this->SingleSweep);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->calCheckBox);
			this->Controls->Add(this->refLdown);
			this->Controls->Add(this->refLup);
			this->Controls->Add(this->txLdown);
			this->Controls->Add(this->txLup);
			this->Controls->Add(this->refL);
			this->Controls->Add(this->txL);
			this->Controls->Add(this->startFup);
			this->Controls->Add(this->startFdown);
			this->Controls->Add(this->stopFup);
			this->Controls->Add(this->stopFdown);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->menuStrip1);
			this->Controls->Add(this->MIndex);
			this->Controls->Add(this->label7);
			this->MainMenuStrip = this->menuStrip1;
			this->MinimumSize = System::Drawing::Size(600, 300);
			this->Name = L"Form1";
			this->Text = L"TAPR Vector Network Analyzer";
			this->Closing += gcnew System::ComponentModel::CancelEventHandler(this, &Form1::Form_Close);
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::Form_Paint);
			this->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::Form_MouseDown);
			this->MouseHover += gcnew System::EventHandler(this, &Form1::Form_MouseHover);
			this->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::Form_MouseMove);
			this->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::Form_MouseUp);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MIndex))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}	

private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e)
		{
			FG = gcnew FrequencyGrid(200);	// Grid containing 200 frequencies (default size)
			FG->SetStartF(200000);			// Default start frequency to 200 KHz.
			FG->SetStopF(200000000);		// Default stop frequency to 100 MHz.
			FrequencyDigitIndex = 0;		// Initialize digit selector to one_hertz position

			CalData = gcnew InstrumentCalDataSet(AllUsersDataPath, VNA);		// Allocate calibration data set
			FG->ferror = CalData->FreqError;	// Load our internal crystal error

			txLevel = 0;				// default to 0 dbm
			refLevel = 0;               // default to 0 dB

			RectVertScaledB = 10.0;		// default to 10 dB/div
			RectSWRScale = 10;			// default to 10 SWR units/div (1.0 to 11.0)
			scalefactorGD = 10000;		// default to 1 microsec/div
			PolarZoom = 1.0f;			// default Polar scale to no zoom

			TDRStartTime = 0.0f;			// default TDR start time in nanoseconds
			TDRStopTime = 4255.0f;		// default TDR stop time
			TDRvf = 0.68f;				// default TDR velocity factor
			TDRMetricMode = true;		// default TDR to metric mode

			PreCharge = false;			// default to integrator precharge off

			Marker = gcnew array<__int64>(5);	// allocate and initialize 5 frequency markers
			MarkerT = gcnew array<Single>(5);		// allocate and initialize 5 time markers
			for (int i=0; i<5; i++)
			{
				Marker[i] = 0;			// Marker frequency = 0 means disabled
				MarkerT[i] = 0;		// Marker time = 0 means disabled
			}

			MarkerMousePick = -1;		// no marker picked by mouse
			ShowMarkerNumbers = true;		// start with marker number display enabled
			ShowMarkerParameters = false;	// start with marker parameter display disabled

			FixtureCalFileName = "";	// initialize fixture calibration filename to none
			plotTitle = "";			// Start with no title. User can give us one if desired

			// load numeric indicators with text
			startF->Text = FG->StartF().ToString("N0");
			stopF->Text = FG->StopF().ToString("N0");
			txL->Text = txLevel.ToString();
		    refL->Text = refLevel.ToString();

			// Add double-click handlers
			this->startF->DoubleClick += gcnew System::EventHandler(this, &Form1::startF_DoubleClick);
			this->stopF->DoubleClick += gcnew System::EventHandler(this, &Form1::stopF_DoubleClick);
			this->txL->DoubleClick += gcnew System::EventHandler(this, &Form1::txL_DoubleClick);
			this->refL->DoubleClick += gcnew System::EventHandler(this, &Form1::refL_DoubleClick);


			// Add resize event handler
			Resize += gcnew EventHandler(this, &Form1::Form_Resize);

			// Instantiate PrintDocument to hold printer settings
			pdoc = gcnew PrintDocument();
			// Add the page handler
			pdoc->PrintPage += gcnew PrintPageEventHandler(this, &Form1::PrintAPage);


			// Open configuration from last time VNAR was run, or from commandline if there's an argument passed
			OpenFileDialog^ startfile = gcnew OpenFileDialog();
			if(CommandLine->Length > 0)		// Get config file if passed from command line
				startfile->FileName = CommandLine;
			else							// Else restore instrument state from when it was last shutdown
				startfile->FileName = String::Concat(CurrentUserDataPath, "\\__last.cfg");
			ReadConfiguration(startfile);

			// VS2005 enables checking of cross-thread use of a control handle.
			// To revert to VS2003 behavior, the CheckForIllegalCrossThreadCalls property
			// needs to be set to false. In the future after the WorkerThread is recoded to
			// avoid directly setting the Form1 controls but to use an Invoke callback instead,
			// the set Check property can be eliminated.  Four controls on Form 1 are manipulated:
			// menuItem5, calibrateMenu, SweepProgressBar, SingleSweep

			Form1::CheckForIllegalCrossThreadCalls = false;

			// Create and start a background thread to poll VNA for data,
			// other routines (download, calibration) talk to VNA directly

			VNAWorkerThreadDelegate = gcnew ThreadStart(this, &Form1::VNA_Worker);
			VNAWorkerThread = gcnew Thread(VNAWorkerThreadDelegate);
			VNAWorkerThread->IsBackground = true;
			VNAWorkerThread->Name = "VNA Worker Thread";

			// debug for VS2005 but it doesn't solve the cross-thread UI access problem
			// VNAWorkerThread->ApartmentState = ApartmentState::STA;

			WorkerCollect = false;			// nothing for worker thread to do yet
			VNAWorkerThread->Start();		// start up the thread
											// Every path that can exit the application must
											// explicitly stop the VNAWorkerThread before terminating,
											// normally using Environment::Exit

			SerialThreadDelegate = gcnew ThreadStart(this, &Form1::Serial_Worker);
			SerialThread = gcnew Thread(SerialThreadDelegate);
			SerialThread->IsBackground = true;
			SerialThread->Name = "Serial Thread";

			// debug for VS2005 but it doesn't solve the cross-thread UI access problem
			// VNAWorkerThread->ApartmentState = ApartmentState::STA;

			SerialCollect = false;			// nothing for worker thread to do yet
			SerialThread->Start();		// start up the thread

			if(OpenAudio())
			{}
		 
}

private: System::Void printItem_Click(System::Object^  sender, System::EventArgs^  e)
    {
        // Create a dialog and attach it to the document
        PrintDialog^ pd = gcnew PrintDialog();
        pd->Document = pdoc;
        // show the dialog
		if (pd->ShowDialog() == ::DialogResult::OK)
        {
            // Print the page
            pdoc->Print();
        }
    }
		/// Print Setup menu item click handler
private: System::Void printSetupItem_Click(System::Object^  sender, System::EventArgs^  e)
	{
        // Create a page setup dialog and attach it to the printer document
        PageSetupDialog^ ps = gcnew PageSetupDialog();
        ps->Document = pdoc;

        // Print Setup fails when there is no printer...
        try
        {
            ps->ShowDialog();
        }
		catch(System::InvalidOperationException^ pe)
        {
            MessageBox::Show(pe->Message, "Error");
        }
		catch(System::ArgumentException^ pe)
        {
            MessageBox::Show(pe->Message, "Error");
        }
    }			
		/// Next Page Print event handler
private: System::Void PrintAPage(Object^ pSender, PrintPageEventArgs^ pe)
	{
		// Get the printer's Graphic object and Margins.
		bool printer = true;
		Form_Render(pe->Graphics, pe->MarginBounds, printer);
	}

private: System::String^ FreqToString(__int64 f)
		 {
			 if (f < 1000)
				 return (String::Concat(f.ToString("N0"), "Hz"));
			 if (f < 1000000)
				 return (String::Concat((f/1000.0).ToString("G"), "kHz"));
			 return (String::Concat((f/1000000.0).ToString("G"), "MHz"));
		 }
#define PT 1

	///	Draw screen bounded by rect
private: System::Void Form_Render(Graphics^ gr,  System::Drawing::Rectangle rect, bool printer)		// rect is our clientsize
	{

		// DpiX and DpiY are not affected by FONT SIZE or Screen Resolution, but only by
		// Control Panel->Display Properties->Settings->Advanced->DPI setting

		#define STANDARDRESOLUTION	96.0f	// Windows standard DPI
		float CurrentResolution = gr->DpiY; // Our current screen resolution
		float DisplayExpansion = 1.0;

		if(!printer)						// ignore display expansion if called by pagePrinter
			DisplayExpansion = CurrentResolution/STANDARDRESOLUTION;

		int LeftMargin = (int)(150 * DisplayExpansion);		// margin between form window and scope display
		int RightMargin = (int)(42 * DisplayExpansion);
		int TopMargin = (int)((15 + menuStrip1->Height) * DisplayExpansion); // VS2005 MenuStrip does not occupy 'space' so Margin
															// needed to expand compared to VS2003 MainMenu
		int BotMargin =	(int)(130 * DisplayExpansion);

		// pens that can change color based on user preference
		Pen^ penS11Phs = gcnew Pen(s11PhsColor, PT);
		Pen^ penS11PhsSto = gcnew Pen(s11PhsColor, 1);
		Pen^ penS11Mag = gcnew Pen(s11MagColor,PT);
		Pen^ penS11MagSto = gcnew Pen(s11MagColor, 1);
		Pen^ penS11MagDelta = gcnew Pen(s11MagColor, 3);

		Pen^ penS21Phs = gcnew Pen(s21PhsColor, PT);
		Pen^ penS21PhsSto = gcnew Pen(s21PhsColor, 1);
		Pen^ penS21Mag = gcnew Pen(s21MagColor, PT);
		Pen^ penS21MagSto = gcnew Pen(s21MagColor, 1);
		Pen^ penS21MagDelta = gcnew Pen(s21MagColor, 3);

		Pen^ penS21GD = gcnew Pen(s21GroupDelayColor, PT);

		Pen^ penS11VSWR = gcnew Pen(s11VSWRColor, PT);
		Pen^ penS11VSWRSto = gcnew Pen(s11VSWRColor, 1);

		Pen^ penR = gcnew Pen(s11RColor, PT);
		Pen^ penRSto = gcnew Pen(s11RColor, 1);
		Pen^ penjX = gcnew Pen(s11jXColor, PT);
		Pen^ penjXSto = gcnew Pen(s11jXColor, 1);

		// pens that don't change color
		Pen^ penBlack = gcnew Pen(Color::Black);
		Pen^ penBlack2 = gcnew Pen(Color::Black, PT);
		Pen^ penBrown2 = gcnew Pen(Color::Brown, PT);
		Pen^ penDarkCyan = gcnew Pen(Color::DarkCyan, PT);
		Pen^ penFuchsia = gcnew Pen(Color::Fuchsia, PT);
		Pen^ penDarkGoldenrod = gcnew Pen(Color::DarkGoldenrod, PT);
		Pen^ pengray = gcnew Pen(Color::Gray);
		Pen^ penbrown = gcnew Pen(Color::Brown);
		Pen^ pencyan = gcnew Pen(Color::Cyan);
		Pen^ penDotGray = gcnew Pen(Color::Gray);
		 penDotGray->DashStyle = DashStyle::Dot;
		Pen^ penControl = gcnew Pen(Color::FromKnownColor(KnownColor::Control));

		// brushes that can change color based on user preference
		SolidBrush^ brS11Mag = gcnew SolidBrush(s11MagColor);
		SolidBrush^ brS21Phs = gcnew SolidBrush(s21PhsColor);
		SolidBrush^ brS21Mag = gcnew SolidBrush(s21MagColor);
		SolidBrush^ brS11Phs = gcnew SolidBrush(s11PhsColor);
		SolidBrush^ brS21GD = gcnew SolidBrush(s21GroupDelayColor);
		SolidBrush^ brS11VSWR = gcnew SolidBrush(s11VSWRColor);
		SolidBrush^ brR = gcnew SolidBrush(s11RColor);
		SolidBrush^ brjX = gcnew SolidBrush(s11jXColor);


		// brushes that don't change color
		SolidBrush^ brBlack = gcnew SolidBrush(Color::Black);
		SolidBrush^ brGray = gcnew SolidBrush(Color::Gray);
		SolidBrush^ brwhite = gcnew SolidBrush(Color::White);
		SolidBrush^ brControl = gcnew SolidBrush(Color::FromKnownColor(KnownColor::Control));

		System::Drawing::Font^ Parameterfont = gcnew System::Drawing::Font("Arial", 7, FontStyle::Regular);
		System::Drawing::Font^ Markerfont = gcnew System::Drawing::Font("Arial", 8, FontStyle::Regular);
		System::Drawing::Font^ Scalefont = gcnew System::Drawing::Font("Arial", 8, FontStyle::Regular);
		System::Drawing::Font^ Statusfont = gcnew System::Drawing::Font("Arial", 10, FontStyle::Regular);
		System::Drawing::Font^ Titlefont = gcnew System::Drawing::Font("Arial", 12, FontStyle::Bold);
		System::Drawing::Font^ Warningfont = gcnew System::Drawing::Font("Arial", 12, FontStyle::Regular);

		System::Drawing::StringFormat^ Centerformat = gcnew System::Drawing::StringFormat();
		Centerformat->Alignment = StringAlignment::Center;

		Point leftTick, rightTick, topTick, bottomTick, minorTopTick, minorBotTick; // rectangular display
		Point traceStart, traceStop;							// incremental trace drawing points
		Point traceStartS, traceStopS;							// trace drawing points for storage
		Point polarCenter, polarRightCenter;					// polar display bounding box
		array<Point>^ points = gcnew array<Point>(1024+1); // used for drawlines

		long avrgRef = 0;

		// Set antialiasing for lines and circles  07-13-2008
		gr->SmoothingMode = SmoothingMode::AntiAlias;

		// clear the entire client window
		gr->ResetClip();

		if(!printer)
			gr->Clear(Form1::BackColor);	// computer screen painted with background color
		else
			gr->Clear(Color::White);		// printer uses white background to save ink


		// Determine the rectangle coordinates for the scope display
		// anchored to top/left, and leaving room for the Freq & Level controls

		System::Drawing::Rectangle tempPolarBox;					// scope display area
		scopeDisp.X = rect.Left + LeftMargin;			// leave some space on left side of display
		scopeDisp.Y = rect.Top + TopMargin;			// leave some space on top of display
		scopeDisp.Height = rect.Height - (BotMargin + TopMargin);	// leave room for controls at bottom of display
		scopeDisp.Width = rect.Width - (RightMargin + LeftMargin);		// leave some space on right side of display
		polarCenter.X = scopeDisp.X + scopeDisp.Width/2;
		polarCenter.Y = scopeDisp.Y + scopeDisp.Height/2;
		polarRightCenter.Y = polarCenter.Y;		// (X-location of right center depends on display size)

		Point topleft(scopeDisp.Left, scopeDisp.Y - 16);
		Point topleftcenter(scopeDisp.Left + scopeDisp.Width/3, scopeDisp.Y - 16);
		Point topright(scopeDisp.Right - scopeDisp.Width/6, scopeDisp.Y - 16);
		Point toprightcenter(scopeDisp.Right - scopeDisp.Width/3, scopeDisp.Y - 16);
		Point botleft(scopeDisp.Left - 20, scopeDisp.Bottom);
		Point botright(scopeDisp.Right - 20, scopeDisp.Bottom);
		Point botcenter(scopeDisp.Left + scopeDisp.Width/2-20, scopeDisp.Y + scopeDisp.Height);
		Point botleftcenter(scopeDisp.Left + scopeDisp.Width/2 + RightMargin - scopeDisp.Width/3, scopeDisp.Y + scopeDisp.Height);
		Point botrightcenter(scopeDisp.Left + scopeDisp.Width/2 + RightMargin, scopeDisp.Bottom);
		Point leftstatuspoint(scopeDisp.Left - LeftMargin + 2, scopeDisp.Y - 6);
		Point botTitleCenter(scopeDisp.Left + scopeDisp.Width/2, scopeDisp.Bottom + 15);
		Point rightScaleTop(scopeDisp.Right + 2, scopeDisp.Top - 6);
        Point rightScaleBottom(scopeDisp.Right + 2, scopeDisp.Bottom - 10);
		Point rightScaleCenter(scopeDisp.Right + 2, scopeDisp.Top + scopeDisp.Height/2 - 6);
		Point leftScaleTop(scopeDisp.Left - (int)(40.0 * DisplayExpansion), scopeDisp.Top - 6);
        Point leftScaleBottom(scopeDisp.Left - (int)(35.0 * DisplayExpansion), scopeDisp.Bottom - 10);
		Point leftScaleCenter(scopeDisp.Left - (int)(35.0 * DisplayExpansion), scopeDisp.Top + scopeDisp.Height/2 - 8);
		Point unCalText(scopeDisp.Left + 20, scopeDisp.Top + 8);
		Point unShieldText(scopeDisp.Left + 20, scopeDisp.Top + 28);
	    Point unFixComp(scopeDisp.Left + 20, scopeDisp.Top + 48);
		Point unConnected(scopeDisp.Right - 300, scopeDisp.Top + 8);

		if (rectItem->Checked == true ||  Spectrum->Checked)  // render screen in rectangular mode
		{
//--			gr->FillRectangle(brwhite, scopeDisp);
			gr->DrawRectangle(penBlack2, scopeDisp);

			for (int i=0; i<9; i++)				// draw major grid
			{
				leftTick.X = scopeDisp.X;
				leftTick.Y = scopeDisp.Y + scopeDisp.Height * (i+1) /10;
				rightTick.X = scopeDisp.X + scopeDisp.Width;
				rightTick.Y = scopeDisp.Y + scopeDisp.Height * (i+1) /10;
				topTick.X = scopeDisp.X + scopeDisp.Width * (i+1) / 10;
				topTick.Y = scopeDisp.Y;
				bottomTick.X = scopeDisp.X + scopeDisp.Width * (i+1) / 10;
				bottomTick.Y = scopeDisp.Y + scopeDisp.Height;
				gr->DrawLine(penBlack, leftTick, rightTick);
				gr->DrawLine(penBlack, topTick, bottomTick);
			};

			// draw minor grid along center horizontal line
			minorTopTick.Y = scopeDisp.Y + scopeDisp.Height/2 + 3;
			minorBotTick.Y = minorTopTick.Y - 6;
			for (int i=0; i<50; i++)
			{
				minorTopTick.X = scopeDisp.X + scopeDisp.Width * i /50;
				minorBotTick.X = scopeDisp.X + scopeDisp.Width * i /50;
				gr->DrawLine(penBlack, minorTopTick, minorBotTick);
			}

			// draw minor grid along center vertical line
			minorTopTick.X = scopeDisp.X + scopeDisp.Width/2 + 3;
			minorBotTick.X = minorTopTick.X - 6;
			for (int i=0; i<50; i++)
			{
				minorTopTick.Y = scopeDisp.Y + scopeDisp.Height * i /50;
				minorBotTick.Y = scopeDisp.Y + scopeDisp.Height * i /50;
				gr->DrawLine(penBlack, minorTopTick, minorBotTick);
			}

			
			// Display status in margin above top of scope display

			String^ text;						// Vertical Magnitude, dB/div
			if (abs(RectVertScaledB - 10.0f) < 0.001f)
				text = "VERT: 10 dB/DIV";
			else if (abs(RectVertScaledB - 5.0f) < 0.001f)
				text = "VERT:  5 dB/DIV";
			else if (abs(RectVertScaledB - 2.0f) < 0.001f)
				text = "VERT:  2 dB/DIV";
			else if (abs(RectVertScaledB - 1.0f) < 0.001f)
				text = "VERT:  1 dB/DIV";
			else if (abs(RectVertScaledB - 0.5f) < 0.001f)
				text = "VERT:  0.5 dB/DIV";

			gr->DrawString(text, Statusfont, brBlack, topleft);

			if (scale100psec->Checked)
				text = "VERT: 100 PICOSEC/DIV";
			else if (scalenanoSec->Checked)
				text = "VERT:   1 NANOSEC/DIV";
			else if (scale10nsec->Checked)
				text = "VERT:  10 NANOSEC/DIV";
			else if (scale100nsec->Checked)
				text = "VERT: 100 NANOSEC/DIV";
			else if (scalemicroSec->Checked)
				text = "VERT:   1 MICROSEC/DIV";
			else if (scale10microsec->Checked)
				text = "VERT:  10 MICROSEC/DIV";
			else if (scale100microsec->Checked)
				text = "VERT: 100 MICROSEC/DIV";
			else if (scale1millisec->Checked)
				text = "VERT:   1 MILLISEC/DIV";

			if(s21groupdelayItem->Checked)
                gr->DrawString(text, Statusfont, brBlack, topleftcenter);	// time, sec/div
			else {
				gr->DrawString(String::Concat("HOR: ",FreqToString((FG->StopF() - FG->StartF())/10), "/DIV"),
							Statusfont, brBlack, topleftcenter);
			}

            gr->DrawString(SOFTWARE_VERSION, Statusfont, brBlack, topright);	// TAPR & Version
		
			// Display selctable scale on right side of display

			if (RightScalePhase->Checked)
			{
				gr->DrawString("+180", Scalefont, brBlack, rightScaleTop);
				gr->DrawString("0 deg", Scalefont, brBlack, rightScaleCenter);
				gr->DrawString("-180", Scalefont, brBlack, rightScaleBottom);
			}
			if (RightScaleDelay->Checked)
			{
				String^ rangeTopText, ^rangeMidText;

				if(scalefactorGD == 1)
				{
					rangeTopText = "1 ns"; 
					rangeMidText = "500 ps";
				}
				else if(scalefactorGD == 10)
				{
					rangeTopText = "10 ns"; 
					rangeMidText = "5 ns";
				}
				else if(scalefactorGD == 100)
				{
					rangeTopText = "100 ns"; 
					rangeMidText = "50 ns";
				}
				else if(scalefactorGD == 1000)
				{
					rangeTopText = "1 us"; 
					rangeMidText = "500 ns";
				}
				else if(scalefactorGD == 10000)
				{
					rangeTopText = "10 us"; 
					rangeMidText = "5 us";
				}
				else if(scalefactorGD == 100000)
				{
					rangeTopText = "100 us"; 
					rangeMidText = "50 us";
				}
				else if(scalefactorGD == 1000000)
				{
					rangeTopText = "1 ms"; 
					rangeMidText = "500 us";
				}
				else
				{
					rangeTopText = "10 ms"; 
					rangeMidText = "5 ms";
				}

				gr->DrawString(rangeTopText, Scalefont, brBlack, rightScaleTop);
				gr->DrawString(rangeMidText, Scalefont, brBlack, rightScaleCenter);
				gr->DrawString(" 0", Scalefont, brBlack, rightScaleBottom);

			}
			if (RightScaleSWR->Checked)
			{
				float range = (float)RectSWRScale + 1.0f;
				String^ rangetext = String::Concat(range.ToString("N1"));
				gr->DrawString(rangetext, Scalefont, brBlack, rightScaleTop);
				range = ((float)RectSWRScale / 2.0f) + 1.0f;
				rangetext = String::Concat(range.ToString("N1"));
				gr->DrawString(rangetext, Scalefont, brBlack, rightScaleCenter);
				gr->DrawString("1.0SWR", Scalefont, brBlack, rightScaleBottom);
			}



			if (RightScaleOhms->Checked)
			{
				String^ rangeTopText, ^rangeMidText, ^rangeBotText;

				if (scale10Ohms->Checked)
				{
					rangeTopText = "100 R\n\r+50 jX";
					rangeMidText = "50 R\n\r+0 jX";
					rangeBotText = "0 R\n\r-50 jX";
				}
				else if (scale100Ohms->Checked)
				{
					rangeTopText = "1000 R\n\r+500 jX";
					rangeMidText = "500 R\n\r+0 jX";
					rangeBotText = "0 R\n\r-500 jX";
				}
				else	// 1 kohm / div
				{
					rangeTopText = "10k R\n\r+5k jX";
					rangeMidText = "5k R\n\r+0 jX";
					rangeBotText = "0 R\n\r-5k jX";
				}

				gr->DrawString(rangeTopText, Scalefont, brBlack, rightScaleTop);
				gr->DrawString(rangeMidText, Scalefont, brBlack, rightScaleCenter);
				gr->DrawString(rangeBotText, Scalefont, brBlack, rightScaleBottom);

			}
			// Display amplitude scale on left side of display

			float range = -((float)RectVertScaledB * 10.0f - (float)refLevel + (float)txLevel);
			String^ rangetext = String::Concat(range.ToString("N1"));
			gr->DrawString(rangetext, Scalefont, brBlack, leftScaleBottom);
			range = -((float)RectVertScaledB * 5.0f - (float)refLevel + (float)txLevel);
			rangetext = String::Concat(range.ToString("N1"));
			gr->DrawString(rangetext, Scalefont, brBlack, leftScaleCenter);
			range = (float)refLevel - (float)txLevel;
			rangetext = String::Concat(range.ToString("N1")," dB");
			gr->DrawString(rangetext, Scalefont, brBlack, leftScaleTop);


			// Display status in margin below bottom of scope display

//			gr->DrawString(String::Concat("START: ", FG->StartF().ToString("N0")), Statusfont, brBlack, botleft);
			gr->DrawString(FreqToString(FG->StartF()), Statusfont, brBlack, botleft);

			gr->DrawString(FreqToString(FG->StopF()),	Statusfont, brBlack, botright);

//			int span = FG->StopF() - FG->StartF();
			__int64 span = (FG->StopF() + FG->StartF())/2;
			gr->DrawString(FreqToString(span), Statusfont, brBlack, botcenter);

//			span = FG->StopF() - FG->StartF();
//			span = span / 10;			// per division
//			gr->DrawString(String::Concat(FreqToString(span), "/DIV"),
//				Statusfont, brBlack, botrightcenter);


			// Display Reference Level
//			leftstatuspoint.Y = scopeDisp.Y + 250;
//			gr->DrawString(String::Concat("Ref Lvl:\n", refLevel.ToString(), " dB"),
//				Statusfont, brBlack, leftstatuspoint);

			// Display plot title 
            gr->DrawString(plotTitle, Titlefont, brBlack, botTitleCenter, Centerformat);	// Plot Title 

			// Display "Uncalibrated" on the scope when InstrumentCal is not valid, when txLevel is not zero,
			// or when in Fast sweep mode.
#if 0
			if((CalData->RxDet->phaseCalibrated == false) || (CalData->TxDet->phaseCalibrated == false)
				|| (txLevel < 0) || String::Compare(SweepSpd->Text, "Fast") == 0)
				gr->DrawString("Uncalibrated", Warningfont, brGray, unCalText);
#endif
			if(!calCheckBox->Checked)
				gr->DrawString("Uncalibrated", Warningfont, brGray, unFixComp);
			else
				gr->DrawString(FixtureCalFileName, Warningfont, brGray, unFixComp );

			if(ExcessRxNoiseIngress(trace, FG->points) && (s21magItem->Checked || s21phsItem->Checked
				|| s21groupdelayItem->Checked))
				gr->DrawString("Unshielded: noise or interference ingress", Warningfont, brGray, unShieldText);

			if(!VNAConnected)
				gr->DrawString("VNA Instrument Not Connected", Titlefont, brBlack, unConnected);


			PixelsPerGrid = (float)scopeDisp.Width / (float)FG->points;

			double fmagnitude;			// 0 to 1
			double fphase;				// -180 to +180	
			double& rmag = fmagnitude;
			double& rphs = fphase;
			int	oldY = 0, oldYs = 0;				// for group delay
//			int dBPerDivision;			// dB per division for rectangular magnitude display

			// draw traces as selected by Trace menu checkmarks


#ifdef DUMPRAWTRACES			// Dump raw traces to a file for debugging
			FileStream^ fs;
			StreamWriter^ sw;
			fs = gcnew FileStream("VNAdebug.csv", FileMode::Create, FileAccess::Write);
			sw = gcnew StreamWriter(fs);
			sw->WriteLine("sep=,");
			sw->WriteLine("Frequency, TranPI, TranPQ, TranPILow, TranPQLow, TranMQHi, TranMQMid, TranMQLo, ReflPI, ReflPQ, ReflMQ, Ref");
#endif
#ifdef DUMPRAWDECODING			// Dump raw traces to a file for debugging
			FileStream^ fs;
			StreamWriter^ sw;
			fs = gcnew FileStream("VNAdecoded.csv", FileMode::Create, FileAccess::Write);
			sw = gcnew StreamWriter(fs);
			sw->WriteLine("sep=;");
			sw->WriteLine("index; freq; ref; mag; phase ");
			int decoded=0;
#endif
#ifdef DEBUGAUDIO
			int mm = 0;
			int mlevel = 0;
			int prevmlevel = 0;
#endif
#ifdef DEBUGAUDIOLEVELS
			int bucket[1024];
			int audioLevel;
			for (int i=0; i < 1024; i++)
				bucket[i] = 0;
			if (nextDecoded > 0) {
				for (int i = 0; i< nextDecoded; i++) {
					audioLevel =(int)(- measured[i].reference * FG->points / 100 );
					if (audioLevel < 0) audioLevel = 0;
					if (audioLevel >= FG->points ) audioLevel =  FG->points - 1;
					bucket[audioLevel]++;
				}
				audioLevel = 0;
				for (int i=0; i < 1024; i++)
				{
					if (audioLevel < bucket[i])
						audioLevel = bucket[i];
				}
				for (int i=0; i < 1024; i++)
					bucket[i] = bucket[i] * SHORT_RANGE / audioLevel;
			}
#endif

			int distance[1024];
		if (audioDistanceToolStripMenuItem->Checked) {
			int maximum;
			for (int i=0; i < 1024; i++)
				distance[i] = 0;
			if (lastMeasurement > 0) {
				for (int i = 0; i< lastMeasurement-1; i++) {
					distance[i] = measurementIndex[i+1] - measurementIndex[i];
				}
				maximum = 0;
				for (int i=0; i < 1024; i++)
				{
					if (maximum < distance[i])
						maximum = distance[i];
				}
				if (maximum > 0) {
					if (maximum < 100) maximum = 100;
					for (int i=0; i < 1024; i++)
						distance[i] = distance[i] * SHORT_RANGE / maximum;
				}
			}
		}


// ------------------ Start display rectangular -------------------------------
			for (int i=1; i<FG->points; i++)	// Display measurements on the frequency grid
			{
				traceStart.X = scopeDisp.X + (int)(((float)i-1.0f) * PixelsPerGrid); // previous point
				traceStop.X = scopeDisp.X + (int)((float)i * PixelsPerGrid);		// current point


#ifdef	DEBUGRAWREFL		
// *********************** debugging code to display raw data  ***************************
				//	Reflected Phase
				traceStop.Y = scopeDisp.Bottom - trace[i]->ReflPI * scopeDisp.Height / SHORT_RANGE;
				traceStart.Y = scopeDisp.Bottom - trace[i-1]->ReflPI * scopeDisp.Height / SHORT_RANGE;
				gr->DrawLine(penS21Mag, traceStart, traceStop);
				traceStop.Y = scopeDisp.Bottom - trace[i]->ReflPQ * scopeDisp.Height / SHORT_RANGE;
				traceStart.Y = scopeDisp.Bottom - trace[i-1]->ReflPQ * scopeDisp.Height / SHORT_RANGE;
				gr->DrawLine(penS11Phs, traceStart, traceStop);
				//	Reflected Magnitude
				traceStop.Y = scopeDisp.Bottom - trace[i]->ReflMQ * scopeDisp.Height / SHORT_RANGE;
				traceStart.Y = scopeDisp.Bottom - trace[i-1]->ReflMQ * scopeDisp.Height / SHORT_RANGE;
				gr->DrawLine(penS21Mag, traceStart, traceStop);
#endif
#ifdef DEBUGAUDIOLEVELS
				traceStop.Y = scopeDisp.Bottom - bucket[i] * scopeDisp.Height / SHORT_RANGE;
				traceStart.Y = scopeDisp.Bottom - bucket[i-1]* scopeDisp.Height / SHORT_RANGE;
				gr->DrawLine(penS21Mag, traceStart, traceStop);
				
#endif

				if (audioDistanceToolStripMenuItem->Checked) {
				traceStop.Y = scopeDisp.Bottom - distance[i] * scopeDisp.Height / SHORT_RANGE;
				traceStart.Y = scopeDisp.Bottom - distance[i-1]* scopeDisp.Height / SHORT_RANGE;
				gr->DrawLine(penS21Mag, traceStart, traceStop);
				}
#ifdef DEBUGAUDIO
#if 1
				traceStop.Y = scopeDisp.Bottom/2 + waveIn[0][2*i+0] * scopeDisp.Height / 70000;
				traceStart.Y = scopeDisp.Bottom/2 + waveIn[0][2*(i-1)+0] * scopeDisp.Height / 70000;
				gr->DrawLine(penS21Mag, traceStart, traceStop);
				traceStop.Y = scopeDisp.Bottom/2 + waveIn[0][2*i+1] * scopeDisp.Height / 70000;
				traceStart.Y = scopeDisp.Bottom/2 + waveIn[0][2*(i-1)+1] * scopeDisp.Height / 70000;
				gr->DrawLine(penS11Phs, traceStart, traceStop);
#else
				if (i > measurementIndex[mm]+13) {	
					mm++; 
					mlevel = 0;
				} else if  (i > measurementIndex[mm]) {
					mlevel = 1;
				} else {
					mlevel = 0;
				}
				traceStop.Y = scopeDisp.Bottom/2 - mlevel * scopeDisp.Height / 3;
				traceStart.Y = scopeDisp.Bottom/2 - prevmlevel * scopeDisp.Height / 3;
				gr->DrawLine(penS11Phs, traceStart, traceStop);
				prevmlevel = mlevel;


				if (mlevel) {
					traceStop.Y = scopeDisp.Bottom / 100  - measured[i].reference * scopeDisp.Height / 200.0;
					traceStart.Y = scopeDisp.Bottom / 100 - measured[i-1].reference * scopeDisp.Height / 200.0;
					gr->DrawLine(penS11Mag, traceStart, traceStop);
				}

				if (mlevel) {
					traceStop.Y = scopeDisp.Bottom /2 - measured[i].magnitude * scopeDisp.Height / 200.0;
					traceStart.Y = scopeDisp.Bottom /2 - measured[i-1].magnitude * scopeDisp.Height / 200.0;
					gr->DrawLine(penS21Mag, traceStart, traceStop);
				}
#endif
#endif



#ifdef DEBUGRAWREFV
				//	Reference Voltages
				traceStop.Y = scopeDisp.Bottom - trace[i]->Vref1 * scopeDisp.Height / SHORT_RANGE;
				traceStart.Y = scopeDisp.Bottom - trace[i-1]->Vref1 * scopeDisp.Height / SHORT_RANGE;
				gr->DrawLine(penS21GD, traceStart, traceStop);
				traceStop.Y = scopeDisp.Bottom - trace[i]->Vref2 * scopeDisp.Height / SHORT_RANGE;
				traceStart.Y = scopeDisp.Bottom - trace[i-1]->Vref2 * scopeDisp.Height / SHORT_RANGE;
				gr->DrawLine(penBrown2, traceStart, traceStop);
#endif
				avrgRef += trace[i]->Vref1;


#ifdef DEBUGRAWTRANHIPHASE
				//	Transmit Phase
				traceStop.Y = scopeDisp.Bottom - trace[i]->TranPI * scopeDisp.Height / SHORT_RANGE;
				traceStart.Y = scopeDisp.Bottom - trace[i-1]->TranPI * scopeDisp.Height / SHORT_RANGE;
				gr->DrawLine(penS21Mag, traceStart, traceStop);
				traceStop.Y = scopeDisp.Bottom - trace[i]->TranPQ * scopeDisp.Height / SHORT_RANGE;
				traceStart.Y = scopeDisp.Bottom - trace[i-1]->TranPQ * scopeDisp.Height / SHORT_RANGE;
				gr->DrawLine(penS11Phs, traceStart, traceStop);
#endif

#ifdef DEBUGRAWTRANLOPHASE
				//	Transmit Phase using Lo excitation level
				traceStop.Y = scopeDisp.Bottom - trace[i]->TranPILow * scopeDisp.Height / SHORT_RANGE;
				traceStart.Y = scopeDisp.Bottom - trace[i-1]->TranPILow * scopeDisp.Height / SHORT_RANGE;
				gr->DrawLine(penS21Mag, traceStart, traceStop);
				traceStop.Y = scopeDisp.Bottom - trace[i]->TranPQLow * scopeDisp.Height / SHORT_RANGE;
				traceStart.Y = scopeDisp.Bottom - trace[i-1]->TranPQLow * scopeDisp.Height / SHORT_RANGE;
				gr->DrawLine(penS11Phs, traceStart, traceStop);
#endif

#ifdef DUMPRAWDECODING
				while (decoded < measurementIndex[i]) {
					sw->Write(i.ToString("N1"));
					sw->Write("; ");
					sw->Write(measured[decoded].freq.ToString("N1"));
					sw->Write("; ");
					sw->Write(measured[decoded].reference.ToString("N1"));
					sw->Write("; ");
					sw->Write(measured[decoded].magnitude.ToString("N1"));
					sw->Write("; ");
					sw->WriteLine(measured[decoded].phase);
					decoded++;
				}
#endif

#ifdef DUMPRAWTRACES
				//// code to dump raw data to file
				sw->Write(FG->Frequency(i).ToString("N1"));
				sw->Write(", ");
				sw->Write(trace[i]->TranPI.ToString("N1"));
				sw->Write(", ");
				sw->Write(trace[i]->TranPQ.ToString("N1"));
				sw->Write(", ");
				sw->Write(trace[i]->TranPILow.ToString("N1"));
				sw->Write(", ");
				sw->Write(trace[i]->TranPQLow.ToString("N1"));
				sw->Write(", ");
				sw->Write(trace[i]->TranMQHi.ToString("N1"));
				sw->Write(", ");
				sw->Write(trace[i]->TranMQMid.ToString("N1"));
				sw->Write(", ");
				sw->Write(trace[i]->TranMQLo.ToString("N1"));
				sw->Write(", ");
				sw->Write(trace[i]->ReflPI.ToString("N1"));
				sw->Write(", ");
				sw->Write(trace[i]->ReflPQ.ToString("N1"));
				sw->Write(", ");
				sw->Write(trace[i]->ReflMQ.ToString("N1"));
				sw->Write(", ");
				sw->WriteLine(trace[i]->Vref1.ToString("N1"));
#endif

//
// ******************************* end debuging code **************************************


				if(s21magItem->Checked)
				{
					if(DisplayMeasured->Checked)
					{
						
						// previous point
						CalData->ResolveTranPolar(trace[i-1], FG->Frequency(i-1), rmag, rphs);
						if (calCheckBox->Checked)
							CorrectS21(CalData, FG->Frequency(i-1), fmagnitude, fphase, rmag, rphs);
						traceStart.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);

						// current point
						CalData->ResolveTranPolar(trace[i], FG->Frequency(i), rmag, rphs);
						if (calCheckBox->Checked)
							CorrectS21(CalData, FG->Frequency(i), fmagnitude, fphase, rmag, rphs);
						traceStop.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);

						DrawLineBound(gr, scopeDisp, penS21Mag, traceStart, traceStop);

						DrawMarkers(gr, penS21Mag, brS21Mag, traceStop, Markerfont, i);		
					}

					if(DisplayMenuItem->Checked)			// display storage also
					{
						CalData->ResolveTranPolar(traceSto[i-1], FG->Frequency(i-1), rmag, rphs);
						if (calCheckBox->Checked)
							CorrectS21(CalData, FG->Frequency(i-1), fmagnitude, fphase, rmag, rphs);
						traceStart.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);

						// current point
						CalData->ResolveTranPolar(traceSto[i], FG->Frequency(i), rmag, rphs);
						if (calCheckBox->Checked)
							CorrectS21(CalData, FG->Frequency(i), fmagnitude, fphase, rmag, rphs);
						traceStop.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);

						DrawLineBound(gr, scopeDisp, penS21MagSto, traceStart, traceStop);

					}

					if(displayMeasuredMinusStored->Checked)		// Display Delta between Measured minus Stored
					{
						double measMag, measPhase;
						double& rmMag = measMag;
						double& rmPhs = measPhase;

						double stoMag, stoPhase;
						double& rsMag = stoMag;
						double& rsPhs = stoPhase;


						// get the magnitude and phase of previous measured point
						CalData->ResolveTranPolar(trace[i-1], FG->Frequency(i-1), rmMag, rmPhs);
						if (calCheckBox->Checked)
							CorrectS21(CalData, FG->Frequency(i-1), measMag, measPhase, rmMag, rmPhs);

						// get the magnitude and phase of previous stored point
						CalData->ResolveTranPolar(traceSto[i-1], FG->Frequency(i-1), rsMag, rsPhs);
						if (calCheckBox->Checked)
							CorrectS21(CalData, FG->Frequency(i-1), stoMag, stoPhase, rsMag, rsPhs);

						traceStart.Y = scopeDisp.Y + ToDisplayRectMag(measMag/stoMag, scopeDisp.Height, RectVertScaledB, refLevel);

						// get the magnitude and phase of current measured point
						CalData->ResolveTranPolar(trace[i], FG->Frequency(i-1), rmMag, rmPhs);
						if (calCheckBox->Checked)
							CorrectS21(CalData, FG->Frequency(i), measMag, measPhase, rmMag, rmPhs);

						// get the magnitude and phase of current stored point
						CalData->ResolveTranPolar(traceSto[i], FG->Frequency(i-1), rsMag, rsPhs);
						if (calCheckBox->Checked)
							CorrectS21(CalData, FG->Frequency(i), stoMag, stoPhase, rsMag, rsPhs);

						traceStop.Y = scopeDisp.Y + ToDisplayRectMag(measMag/stoMag, scopeDisp.Height, RectVertScaledB, refLevel);

						DrawLineBound(gr, scopeDisp, penS21MagDelta, traceStart, traceStop);
					}


	// debug using the MQ indicators
	//
	//
					//CalData->ResolveTranPolar(trace[i-1], FG->Frequency(i), rmag, rphs);
					//if (calCheckBox->Checked)
					//	CorrectS21(CalData, FG->Frequency(i-1), fmagnitude, fphase, rmag, rphs);
					//traceStart.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);

					//// current point
					//CalData->ResolveTranPolar(trace[i], FG->Frequency(i), rmag, rphs);
					//if (calCheckBox->Checked)
					//	CorrectS21(CalData, FG->Frequency(i), fmagnitude, fphase, rmag, rphs);
					//traceStop.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);

					//gr->DrawLine(penBrown2, traceStart, traceStop);
	//
	//
	// end using MQ indicators


				}
				if(s21phsItem->Checked && !Spectrum->Checked)
				{
					if(DisplayMeasured->Checked)
					{
						// previous point
						CalData->ResolveTranPolar(trace[i-1], FG->Frequency(i-1), rmag, rphs);
						if (calCheckBox->Checked)
							CorrectS21(CalData, FG->Frequency(i-1), fmagnitude, fphase, rmag, rphs);
						traceStart.Y = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);

						// current point
						CalData->ResolveTranPolar(trace[i], FG->Frequency(i), rmag, rphs);
						if (calCheckBox->Checked)
							CorrectS21(CalData, FG->Frequency(i), fmagnitude, fphase, rmag, rphs);
						traceStop.Y = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);

						DrawLineBound(gr, scopeDisp, penS21Phs, traceStart, traceStop);

						DrawMarkers(gr, penS21Phs, brS21Phs, traceStop, Markerfont, i);
					}

					if(DisplayMenuItem->Checked)		// display stored data
					{
						// previous point
						CalData->ResolveTranPolar(traceSto[i-1], FG->Frequency(i-1), rmag, rphs);
						if (calCheckBox->Checked)
							CorrectS21(CalData, FG->Frequency(i-1), fmagnitude, fphase, rmag, rphs);
						traceStart.Y = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);

						// current point
						CalData->ResolveTranPolar(traceSto[i], FG->Frequency(i), rmag, rphs);
						if (calCheckBox->Checked)
							CorrectS21(CalData, FG->Frequency(i), fmagnitude, fphase, rmag, rphs);
						traceStop.Y = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);

						DrawLineBound(gr, scopeDisp, penS21PhsSto, traceStart, traceStop);

					}
				}

				if(s11magItem->Checked && !Spectrum->Checked)
				{
					if(DisplayMeasured->Checked)		// display measured data
					{
					// previous point
						CalData->ResolveReflPolar(trace[i-1], FG->Frequency(i-1), rmag, rphs, true);
						if (calCheckBox->Checked)
							CorrectS11(CalData, FG->Frequency(i-1), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);
						traceStart.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);

						// current point
						CalData->ResolveReflPolar(trace[i], FG->Frequency(i), rmag, rphs, true);
						if (calCheckBox->Checked)
							CorrectS11(CalData, FG->Frequency(i), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);
						traceStop.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);

						DrawLineBound(gr, scopeDisp, penS11Mag, traceStart, traceStop);

						DrawMarkers(gr, penS11Mag, brS11Mag, traceStop, Markerfont, i);		
					}

					if(DisplayMenuItem->Checked)		// display stored data
					{
						// previous point
						CalData->ResolveReflPolar(traceSto[i-1], FG->Frequency(i-1), rmag, rphs, true);
						if (calCheckBox->Checked)
							CorrectS11(CalData, FG->Frequency(i-1), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);
						traceStart.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);

						// current point
						CalData->ResolveReflPolar(traceSto[i], FG->Frequency(i), rmag, rphs, true);
						if (calCheckBox->Checked)
							CorrectS11(CalData, FG->Frequency(i), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);
						traceStop.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);

						DrawLineBound(gr, scopeDisp, penS11MagSto, traceStart, traceStop);
					}

					if(displayMeasuredMinusStored->Checked)		// Display Delta between Measured minus Stored
					{
						double measMag, measPhase;
						double& rmMag = measMag;
						double& rmPhs = measPhase;

						double stoMag, stoPhase;
						double& rsMag = stoMag;
						double& rsPhs = stoPhase;


						// get the magnitude and phase of previous measured point
						CalData->ResolveReflPolar(trace[i-1], FG->Frequency(i-1), rmMag, rmPhs, true);
						if (calCheckBox->Checked)
							CorrectS11(CalData, FG->Frequency(i-1), RefExtnCheckBox->Checked, measMag, measPhase, rmMag, rmPhs);

						// get the magnitude and phase of previous stored point
						CalData->ResolveReflPolar(traceSto[i-1], FG->Frequency(i-1), rsMag, rsPhs, true);
						if (calCheckBox->Checked)
							CorrectS11(CalData, FG->Frequency(i-1), RefExtnCheckBox->Checked, stoMag, stoPhase, rsMag, rsPhs);

						traceStart.Y = scopeDisp.Y + ToDisplayRectMag(measMag/stoMag, scopeDisp.Height, RectVertScaledB, refLevel);

						// get the magnitude and phase of current measured point
						CalData->ResolveReflPolar(trace[i], FG->Frequency(i-1), rmMag, rmPhs, true);
						if (calCheckBox->Checked)
							CorrectS11(CalData, FG->Frequency(i), RefExtnCheckBox->Checked, measMag, measPhase, rmMag, rmPhs);

						// get the magnitude and phase of current stored point
						CalData->ResolveReflPolar(traceSto[i], FG->Frequency(i-1), rsMag, rsPhs, true);
						if (calCheckBox->Checked)
							CorrectS11(CalData, FG->Frequency(i), RefExtnCheckBox->Checked, stoMag, stoPhase, rsMag, rsPhs);

						traceStop.Y = scopeDisp.Y + ToDisplayRectMag(measMag/stoMag, scopeDisp.Height, RectVertScaledB, refLevel);

						DrawLineBound(gr, scopeDisp, penS11MagDelta, traceStart, traceStop);
					}

		// debug using RelMQ indicators
		//
		//
					//CalData->ResolveReflPolar(trace3[i-1], trace4[i-1], trace2[i-1], rmag, rphs);
					//if (calCheckBox->Checked)
					//	CorrectS11(CalData, FG->Frequency(i-1), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);
					//traceStart.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);

					//// current point
					//CalData->ResolveReflPolar(trace3[i], trace4[i], trace2[i], rmag, rphs);
					//if (calCheckBox->Checked)
					//	CorrectS11(CalData, FG->Frequency(i), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);
					//traceStop.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);

					//gr->DrawLine(penDarkGoldenrod, traceStart, traceStop);
		//
		//
		// end debug using refl MQ indicators

				}

				if(VSWRdisplay->Checked && !Spectrum->Checked)
				{
					if(DisplayMeasured->Checked)
					{
						// previous point
						CalData->ResolveReflPolar(trace[i-1], FG->Frequency(i-1), rmag, rphs, true);
						if (calCheckBox->Checked)
							CorrectS11(CalData, FG->Frequency(i-1), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);

						traceStart.Y = scopeDisp.Y + ToDisplayAsSWR(fmagnitude, scopeDisp.Height, RectSWRScale);

						// current point
						CalData->ResolveReflPolar(trace[i], FG->Frequency(i), rmag, rphs, true);
						if (calCheckBox->Checked)
							CorrectS11(CalData, FG->Frequency(i), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);

						traceStop.Y = scopeDisp.Y + ToDisplayAsSWR(fmagnitude, scopeDisp.Height, RectSWRScale);

						DrawLineBound(gr, scopeDisp, penS11VSWR, traceStart, traceStop);

						DrawMarkers(gr, penS11VSWR, brS11VSWR, traceStop, Markerfont, i);
					}

					if(DisplayMenuItem->Checked)		// also display storage
					{
						// previous point
						CalData->ResolveReflPolar(traceSto[i-1], FG->Frequency(i-1), rmag, rphs, true);
						if (calCheckBox->Checked)
							CorrectS11(CalData, FG->Frequency(i-1), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);

						traceStart.Y = scopeDisp.Y + ToDisplayAsSWR(fmagnitude, scopeDisp.Height, RectSWRScale);

						// current point
						CalData->ResolveReflPolar(traceSto[i], FG->Frequency(i), rmag, rphs, true);
						if (calCheckBox->Checked)
							CorrectS11(CalData, FG->Frequency(i), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);

						traceStop.Y = scopeDisp.Y + ToDisplayAsSWR(fmagnitude, scopeDisp.Height, RectSWRScale);

						DrawLineBound(gr, scopeDisp, penS11VSWRSto, traceStart, traceStop);


					}

				}

				if(s11phsItem->Checked && !Spectrum->Checked)
				{
					if(DisplayMeasured->Checked)
					{
						// previous point
						CalData->ResolveReflPolar(trace[i-1], FG->Frequency(i-1), rmag, rphs, true);
						if (calCheckBox->Checked)
							CorrectS11(CalData, FG->Frequency(i-1), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);
						traceStart.Y = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);

						// current point
						CalData->ResolveReflPolar(trace[i], FG->Frequency(i), rmag, rphs, true);
						if (calCheckBox->Checked)
							CorrectS11(CalData, FG->Frequency(i), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);
						traceStop.Y = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);

						DrawLineBound(gr, scopeDisp, penS11Phs, traceStart, traceStop);

						DrawMarkers(gr, penS11Phs, brS11Phs, traceStop, Markerfont, i);	
					}

					if(DisplayMenuItem->Checked)		// also display storage
					{
						// previous point
						CalData->ResolveReflPolar(traceSto[i-1], FG->Frequency(i-1), rmag, rphs, true);
						if (calCheckBox->Checked)
							CorrectS11(CalData, FG->Frequency(i-1), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);
						traceStart.Y = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);

						// current point
						CalData->ResolveReflPolar(traceSto[i], FG->Frequency(i), rmag, rphs, true);
						if (calCheckBox->Checked)
							CorrectS11(CalData, FG->Frequency(i), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);
						traceStop.Y = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);

						DrawLineBound(gr, scopeDisp, penS11PhsSto, traceStart, traceStop);
					}


				}

				if(s21groupdelayItem->Checked && !Spectrum->Checked)
				{
					double prevphs, currphs, groupdelay;
					__int64 prevfreq, currfreq;
					int aperture, lowindex, highindex;	// aperture size, indexes representing bottom and top of aperture

					if (GDAperture64->Checked)			// get aperture size from menu
						aperture = 64;
					if (GDAperture16->Checked)
						aperture = 16;
					if (GDAperture4->Checked)
						aperture = 4;
					if (GDAperture1->Checked)
						aperture = 1;

					if (aperture == 1)
						lowindex =  i - 1;
					else
                        lowindex = i - aperture / 2;		// set lower aperture sample
					if (lowindex < 0)					// make sure it's in range
						lowindex = 0;

                    highindex = i + aperture / 2;		// set upper aperture sample
					if (highindex >= FG->points)			// make sure it's in range
						highindex = FG->points - 1;

					// previous point
					CalData->ResolveTranPolar(trace[lowindex], FG->Frequency(lowindex), rmag, rphs);
					if (calCheckBox->Checked)
						CorrectS21(CalData, FG->Frequency(lowindex), fmagnitude, fphase, rmag, rphs);
					prevphs = fphase;
					prevfreq = FG->Frequency(lowindex);

					// current point
					CalData->ResolveTranPolar(trace[highindex], FG->Frequency(highindex), rmag, rphs);
					if (calCheckBox->Checked)
						CorrectS21(CalData, FG->Frequency(highindex), fmagnitude, fphase, rmag, rphs);
					currphs = fphase;
					currfreq = FG->Frequency(highindex);

	// New group delay algorithm - keep track of incremental phase change across the aperture

					double accumphase = 0.0;			// adds up the incremental phase shifts
					double trackphase = prevphs;		// keeps track of phase at last point
					if(aperture > 1)
						for(int k=lowindex+1; k<highindex; k++)
						{
							CalData->ResolveTranPolar(trace[k], FG->Frequency(k), rmag, rphs);
							if (calCheckBox->Checked)
								CorrectS21(CalData, FG->Frequency(k), fmagnitude, fphase, rmag, rphs);

							if(fphase - trackphase > 25)		// big backwards phase jump - assume wrap
								accumphase -= trackphase + 360.0 - fphase;
							else
								accumphase -= trackphase - fphase;	// incremental phase change

							trackphase = fphase;		// update phase tracker
						}
					else								// aperture = 1
					{
						if (currphs > (prevphs + 25))	// if phase wraps [within 25 degree error band],
							prevphs += 360.0;			// then adjust current phase reading by 360 degrees
						accumphase = currphs - prevphs;				
					}

					// test to see if phase wrapped around -180 degrees point, and correct for G.D.
	//				if ((prevphs < -135.0) && (currphs > 135.0))
	//					prevphs += 360.0;

					// phs is in degrees, frequency in hertz), need to divide by 360.0
					// to convert to (radians / radians-per-second) to yield group delay in seconds
	
//					if (currphs > (prevphs + 6.0))	// if phase wraps [within 6 degree error band],
//						prevphs += 360.0;			// then adjust current phase reading by 360 degrees

//					groupdelay = - ((currphs - prevphs) / (double)(currfreq - prevfreq) / 360.0);
					groupdelay = - (accumphase / (double)(currfreq - prevfreq) / 360.0);

					if (groupdelay < 0.0)		// prevent display of negative (i.e. wrong) values
						groupdelay = 0.0;

					traceStop.Y = scopeDisp.Bottom - ToDisplayRectGD(groupdelay, scopeDisp.Height, scalefactorGD);

					traceStart.Y = oldY;

					if (i>1)
						DrawLineBound(gr, scopeDisp, penS21GD, traceStart, traceStop);

					DrawMarkers(gr, penS21GD, brS21GD, traceStop, Markerfont, i);

					oldY = traceStop.Y;		// save stopping point for next iteration

					if(DisplayMenuItem->Checked)			// display storage also
					{
						// previous point
						CalData->ResolveTranPolar(traceSto[lowindex], FG->Frequency(lowindex), rmag, rphs);
						if (calCheckBox->Checked)
							CorrectS21(CalData, FG->Frequency(lowindex), fmagnitude, fphase, rmag, rphs);
						prevphs = fphase;
						prevfreq = FG->Frequency(lowindex);

						// current point
						CalData->ResolveTranPolar(traceSto[highindex], FG->Frequency(highindex), rmag, rphs);
						if (calCheckBox->Checked)
							CorrectS21(CalData, FG->Frequency(highindex), fmagnitude, fphase, rmag, rphs);
						currphs = fphase;
						currfreq = FG->Frequency(highindex);

	// New group delay algorithm - keep track of incremental phase change across the aperture

						double accumphase = 0.0;			// adds up the incremental phase shifts
						double trackphase = prevphs;		// keeps track of phase at last point
						if(aperture > 1)
							for(int k=lowindex+1; k<highindex; k++)
							{
								CalData->ResolveTranPolar(trace[k], FG->Frequency(k), rmag, rphs);
								if (calCheckBox->Checked)
									CorrectS21(CalData, FG->Frequency(k), fmagnitude, fphase, rmag, rphs);

								if(fphase - trackphase > 25)		// big backwards phase jump - assume wrap
									accumphase -= trackphase + 360.0 - fphase;
								else
									accumphase -= trackphase - fphase;	// incremental phase change

								trackphase = fphase;		// update phase tracker
							}
							else								// aperture = 1
							{
								if (currphs > (prevphs + 25))	// if phase wraps [within 25 degree error band],
									prevphs += 360.0;			// then adjust current phase reading by 360 degrees
								accumphase = currphs - prevphs;				
							}


		// test to see if phase wrapped around -180 degrees point, and correct for G.D.
		//				if ((prevphs < -135.0) && (currphs > 135.0))
		//					prevphs += 360.0;

						// phs is in degrees, frequency in hertz), need to divide by 360.0
						// to convert to (radians / radians-per-second) to yield group delay in seconds
		
		//				if (currphs > (prevphs + 6.0))	// if phase wraps, then adjust current phase reading by 360 degrees
		//					prevphs += 360.0;

		//				groupdelay = - ((currphs - prevphs) / (double)(currfreq - prevfreq) / 360.0);

						groupdelay = - (accumphase / (double)(currfreq - prevfreq) / 360.0);

						if (groupdelay < 0.0)		// prevent display of negative (i.e. wrong) values
							groupdelay = 0.0;

						traceStop.Y = scopeDisp.Bottom - ToDisplayRectGD(groupdelay, scopeDisp.Height, scalefactorGD);

						traceStart.Y = oldYs;

						if (i>1)
							DrawLineBound(gr, scopeDisp, penS21GD, traceStart, traceStop);

						oldYs = traceStop.Y;		// save stopping point for next iteration
					}


				}

				if((RDisplay->Checked || jXDisplay->Checked) && !Spectrum->Checked)
				{

					float X, Y, Rprev, jXprev, Rcurr, jXcurr;
					int scale;

					if(scale10Ohms->Checked)
						scale = 10;
					else if(scale100Ohms->Checked)
						scale = 100;
					else scale = 1000;

					// previous point
					CalData->ResolveReflPolar(trace[i-1], FG->Frequency(i-1), rmag, rphs, true);
					if (calCheckBox->Checked)
						CorrectS11(CalData, FG->Frequency(i-1), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);

					// derive R and jX from rmag and fphase
					X = (float)(fmagnitude * cos(DEGR2RAD * fphase));
					Y = (float)(fmagnitude * sin(DEGR2RAD * fphase));

					std::complex<float> S11prev(X, Y);		// S11
					std::complex<float> One(1.0, 0.0);	// unity
					std::complex<float> Zo(50.0, 0.0);	// Zo
					std::complex<float> Z;				// result

					Z = Zo * ( One + S11prev) / (One - S11prev);	// calculate normalized Z at mouse point

					Rprev = real(Z);
					jXprev = imag(Z);


					// current point
					CalData->ResolveReflPolar(trace[i], FG->Frequency(i), rmag, rphs, true);
					if (calCheckBox->Checked)
						CorrectS11(CalData, FG->Frequency(i), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);

					X = (float)(fmagnitude * cos(DEGR2RAD * fphase));
					Y = (float)(fmagnitude * sin(DEGR2RAD * fphase));

					std::complex<float> S11curr(X, Y);

					Z = Zo * ( One + S11curr) / (One - S11curr);	// calculate normalized Z at mouse point

					Rcurr = real(Z);
					jXcurr = imag(Z);

					if(DisplayMeasured->Checked)
					{
						if(RDisplay->Checked)		// draw R-ohms part of S11
						{
							traceStart.Y = scopeDisp.Bottom - ToDisplayRectR(Rprev, scale, scopeDisp.Height);
							traceStop.Y = scopeDisp.Bottom - ToDisplayRectR(Rcurr, scale, scopeDisp.Height);
							DrawLineBound(gr, scopeDisp, penR, traceStart, traceStop);
							DrawMarkers(gr, penR, brR, traceStop, Markerfont, i);	
						}
						if(jXDisplay->Checked)		// draw jX-ohms part of S11
						{
							traceStart.Y = scopeDisp.Bottom - ToDisplayRectjX(jXprev, scale, scopeDisp.Height);
							traceStop.Y = scopeDisp.Bottom - ToDisplayRectjX(jXcurr, scale, scopeDisp.Height);
							DrawLineBound(gr, scopeDisp, penjX, traceStart, traceStop);
							DrawMarkers(gr, penjX, brjX, traceStop, Markerfont, i);	
						}
					}

					if(DisplayMenuItem->Checked)		// also display storage
					{
						// previous point
						CalData->ResolveReflPolar(traceSto[i-1], FG->Frequency(i-1), rmag, rphs, true);
						if (calCheckBox->Checked)
							CorrectS11(CalData, FG->Frequency(i-1), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);

						// derive R and jX from rmag and fphase
						X = (float)(fmagnitude * cos(DEGR2RAD * fphase));
						Y = (float)(fmagnitude * sin(DEGR2RAD * fphase));

						std::complex<float> S11Storprev(X, Y);		// S11

						Z = Zo * ( One + S11Storprev) / (One - S11Storprev);	// calculate normalized Z at mouse point

						Rprev = real(Z);
						jXprev = imag(Z);

						// current point
						CalData->ResolveReflPolar(traceSto[i], FG->Frequency(i), rmag, rphs, true);
						if (calCheckBox->Checked)
							CorrectS11(CalData, FG->Frequency(i), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);

						// derive R and jX from rmag and fphase
						X = (float)(fmagnitude * cos(DEGR2RAD * fphase));
						Y = (float)(fmagnitude * sin(DEGR2RAD * fphase));

						std::complex<float> S11Stornext(X, Y);		// S11

						Z = Zo * ( One + S11Stornext) / (One - S11Stornext);	// calculate normalized Z at mouse point

						Rcurr = real(Z);
						jXcurr = imag(Z);

						if(RDisplay->Checked)		// draw R-ohms part of S11
						{
							traceStart.Y = scopeDisp.Bottom - ToDisplayRectR(Rprev, scale, scopeDisp.Height);
							traceStop.Y = scopeDisp.Bottom - ToDisplayRectR(Rcurr, scale, scopeDisp.Height);
							DrawLineBound(gr, scopeDisp, penRSto, traceStart, traceStop);
						}
						if(jXDisplay->Checked)		// draw jX-ohms part of S11
						{
							traceStart.Y = scopeDisp.Bottom - ToDisplayRectjX(jXprev, scale, scopeDisp.Height);
							traceStop.Y = scopeDisp.Bottom - ToDisplayRectjX(jXcurr, scale, scopeDisp.Height);
							DrawLineBound(gr, scopeDisp, penjXSto, traceStart, traceStop);
						}
					}

				}
				if (EtTrace->Checked == true && !Spectrum->Checked)
				{
					// Convert results to polar coordinates
					// Previous point
					double xx = CalData->EtReal[i-1];
					double yy = CalData->EtImag[i-1];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);

					traceStart.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);
					int PhstraceStart = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);

					// Current point

					// Convert results to polar coordinates
					xx = CalData->EtReal[i];
					yy = CalData->EtImag[i];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);

					float scaleFactor = GetVerticalScaleFactor();

					// current point - magnitude
					traceStop.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, scaleFactor, refLevel);
					DrawLineBound(gr, scopeDisp, penS21GD, traceStart, traceStop);

					// current point - phase
					traceStart.Y = PhstraceStart;
					traceStop.Y = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);
					DrawLineBound(gr, scopeDisp, penS21Phs, traceStart, traceStop);

				}
				if (EsTrace->Checked == true && !Spectrum->Checked)
				{
					// Convert results to polar coordinates
					// Previous point
					double xx = CalData->EsReal[i-1];
					double yy = CalData->EsImag[i-1];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);

					traceStart.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);
					int PhstraceStart = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);

					// Current point

					// Convert results to polar coordinates
					xx = CalData->EsReal[i];
					yy = CalData->EsImag[i];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);

					float scaleFactor = GetVerticalScaleFactor();

					// current point - magnitude
					traceStop.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, scaleFactor, refLevel);
					DrawLineBound(gr, scopeDisp, penBrown2, traceStart, traceStop);

					// current point - phase
					traceStart.Y = PhstraceStart;
					traceStop.Y = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);
					DrawLineBound(gr, scopeDisp, penS21Mag, traceStart, traceStop);

				}


				if (EdTrace->Checked == true && !Spectrum->Checked)
				{
					// Convert results to polar coordinates
					// Previous point
					double xx = CalData->EdReal[i-1];
					double yy = CalData->EdImag[i-1];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);

					traceStart.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);
					int PhstraceStart = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);

					// Current point

					// Convert results to polar coordinates
					xx = CalData->EdReal[i];
					yy = CalData->EdImag[i];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);


					float scaleFactor = GetVerticalScaleFactor();

					// current point - magnitude
					traceStop.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, scaleFactor, refLevel);
					DrawLineBound(gr, scopeDisp, penDarkCyan, traceStart, traceStop);

					// current point - phase
					traceStart.Y = PhstraceStart;
					traceStop.Y = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);
					DrawLineBound(gr, scopeDisp, penS11Phs, traceStart, traceStop);
				}

				if (calSthru->Checked && !Spectrum->Checked)
				{
					// Convert results to polar coordinates
					// Previous point
					double xx = CalData->ThReal[i-1];
					double yy = CalData->ThImag[i-1];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);

					traceStart.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);
					int PhstraceStart = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);

					// Current point

					// Convert results to polar coordinates
					xx = CalData->ThReal[i];
					yy = CalData->ThImag[i];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);


					float scaleFactor = GetVerticalScaleFactor();

					// current point - magnitude
					traceStop.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, scaleFactor, refLevel);
					DrawLineBound(gr, scopeDisp, penS21GD, traceStart, traceStop);

					// current point - phase
					traceStart.Y = PhstraceStart;
					traceStop.Y = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);
					DrawLineBound(gr, scopeDisp, penS21Phs, traceStart, traceStop);

				}

				if (calSshort->Checked && !Spectrum->Checked)
				{
					// Previous point
					double xx = CalData->S11shortReal[i-1];
					double yy = CalData->S11shortImag[i-1];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);

					traceStart.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);
					int PhstraceStart = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);

					// Current point

					// Convert results to polar coordinates
					xx = CalData->S11shortReal[i];
					yy = CalData->S11shortImag[i];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);


					float scaleFactor = GetVerticalScaleFactor();

					// current point - magnitude
					traceStop.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, scaleFactor, refLevel);
					DrawLineBound(gr, scopeDisp, penS21GD, traceStart, traceStop);

					// current point - phase
					traceStart.Y = PhstraceStart;
					traceStop.Y = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);
					DrawLineBound(gr, scopeDisp, penS21Phs, traceStart, traceStop);
				}

				if (calSopen->Checked && !Spectrum->Checked)
				{
					// Convert results to polar coordinates
					// Previous point
					double xx = CalData->S11openReal[i-1];
					double yy = CalData->S11openImag[i-1];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);

					traceStart.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);
					int PhstraceStart = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);

					// Current point

					// Convert results to polar coordinates
					xx = CalData->S11openReal[i];
					yy = CalData->S11openImag[i];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);


					float scaleFactor = GetVerticalScaleFactor();

					// current point - magnitude
					traceStop.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, scaleFactor, refLevel);
					DrawLineBound(gr, scopeDisp, penBrown2, traceStart, traceStop);

					// current point - phase
					traceStart.Y = PhstraceStart;
					traceStop.Y = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);
					DrawLineBound(gr, scopeDisp, penS21Mag, traceStart, traceStop);

				}

				if (calSterm->Checked && !Spectrum->Checked)
				{
					// Convert results to polar coordinates
					// Previous point
					double xx = CalData->S11termReal[i-1];
					double yy = CalData->S11termImag[i-1];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);

					traceStart.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);
					int PhstraceStart = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);

					// Current point

					// Convert results to polar coordinates
					xx = CalData->S11termReal[i];
					yy = CalData->S11termImag[i];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);


					float scaleFactor = GetVerticalScaleFactor();

					// current point - magnitude
					traceStop.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, scaleFactor, refLevel);
					DrawLineBound(gr, scopeDisp, penDarkCyan, traceStart, traceStop);

					// current point - phase
					traceStart.Y = PhstraceStart;
					traceStop.Y = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);
					DrawLineBound(gr, scopeDisp, penS11Phs, traceStart, traceStop);
				}
				if (rawDetectorDataToolStripMenuItem->Checked) {
					//	Reference level
#if 1
					traceStop.Y = scopeDisp.Bottom - trace[i]->Vref1 * scopeDisp.Height / SHORT_RANGE;
					traceStart.Y = scopeDisp.Bottom - trace[i-1]->Vref1 * scopeDisp.Height / SHORT_RANGE;
#else
					traceStop.Y = scopeDisp.Bottom - trace[i]->Vref2 * scopeDisp.Height / 1030;
					traceStart.Y = scopeDisp.Bottom - trace[i-1]->Vref2 * scopeDisp.Height / 1030;
#endif
					gr->DrawLine(penBrown2, traceStart, traceStop);
				}
#ifdef DEBUGLONGCAL
				if (true)
				{
					// Look at difference between open and short calibration data.
					// Magnitude is difference in dB between them, Phase of 0 degrees
					// Means that they point in opposite directions (180 degrees apart)
					// Perfect cal data would indicate dB magnitude is large negative
					// (same magnitude for both components) and 0 degrees phase (they point
					// exactly in opposite directions).

					// Previous point
					double xx = CalData->S11shortReal[i-1];
					double yy = CalData->S11shortImag[i-1];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);

					xx = CalData->S11openReal[i-1];
					yy = CalData->S11openImag[i-1];

					fphase -= RAD2DEGR * atan2(yy, xx);
					fphase += 180.0;
					fmagnitude -= sqrt(xx*xx + yy*yy);
					fphase = NormalizePhaseDegr(fphase);

					traceStart.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, RectVertScaledB, refLevel);
					int PhstraceStart = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);

					// Current point

					// Convert results to polar coordinates
					xx = CalData->S11shortReal[i];
					yy = CalData->S11shortImag[i];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);

					xx = CalData->S11openReal[i];
					yy = CalData->S11openImag[i];

					fphase -= RAD2DEGR * atan2(yy, xx);
					fphase += 180.0;
					fmagnitude -= sqrt(xx*xx + yy*yy);
					fphase = NormalizePhaseDegr(fphase);


					int scaleFactor;
					if (Scale10dB->Checked)	scaleFactor = 10;
					else if (Scale5dB->Checked)	scaleFactor = 5;
					else if (Scale2dB->Checked)	scaleFactor = 2;
					else if (Scale1dB->Checked)	scaleFactor = 1;

					// current point - magnitude
					traceStop.Y = scopeDisp.Y + ToDisplayRectMag(fmagnitude, scopeDisp.Height, scaleFactor, refLevel);
					DrawLineBound(gr, scopeDisp, penS21GD, traceStart, traceStop);

					// current point - phase
					traceStart.Y = PhstraceStart;
					traceStop.Y = scopeDisp.Bottom - ToDisplayRectPhs(fphase, scopeDisp.Height);
					DrawLineBound(gr, scopeDisp, penS21Phs, traceStart, traceStop);
				}

#endif
	
		}
		//avrgRef = avrgRef / FG->points;
//		refLevelBox->Text = avrgRef.ToString("N1");
//		brBlack

//		gr->DrawString("Average ref", Parameterfont, brS11Mag, leftstatuspoint);
//		leftstatuspoint.Y += (int)(10.0 * DisplayExpansion);

#ifdef DUMPRAWTRACES
			sw->Flush();	
			sw->Close();	// close VNAdebug.csv file
#endif
#ifdef DUMPRAWDECODING
			sw->Flush();	
			sw->Close();	// close VNAdecodig.csv file
#endif
		}

		if (polarItem->Checked == true  && !Spectrum->Checked)							// render screen in polar mode
		{

			// Display plot title 
			gr->DrawString(plotTitle, Titlefont, brBlack, botTitleCenter, Centerformat);	// Plot Title 

			// Display software version
            gr->DrawString(SOFTWARE_VERSION, Statusfont, brBlack, topright);	// TAPR & Version

			if(!calCheckBox->Checked)
				gr->DrawString("Fixture Uncompensated", Warningfont, brGray, unFixComp);

			if(!VNAConnected)
				gr->DrawString("VNA Instrument Not Connected", Titlefont, brBlack, unConnected);


			// find the smaller of the horizontal or vertical size & set RightCenter coordinates
			if (scopeDisp.Height > scopeDisp.Width)
				polarRadius = scopeDisp.Width/2;
			else
				polarRadius = scopeDisp.Height/2;

			polarRightCenter.X = polarCenter.X + polarRadius;

			// build the bounding rectangle for the polar display
			polarBox.X = polarCenter.X - polarRadius;
			polarBox.Width = polarRadius * 2;
			polarBox.Y = polarCenter.Y - polarRadius;
			polarBox.Height = polarRadius *2;

			// draw polar display and outside circle
			gr->FillEllipse(brwhite, polarBox);
			gr->DrawEllipse(penBlack2, polarBox);

			// Display "Uncalibrated" on the scope when InstrumentCal is not valid, when txLevel is not zero,
			// or when in Fast sweep mode.
#if 0
			if((CalData->RxDet->phaseCalibrated == false) || (CalData->TxDet->phaseCalibrated == false)
				|| (txLevel < 0) || String::Compare(SweepSpd->Text, "Fast") == 0)
				gr->DrawString("Uncalibrated", Warningfont, brGray, unCalText);
#endif
			// build and set circular clipping region for subsequent drawing
			GraphicsPath^ gpUnit = gcnew GraphicsPath();		// Holds just the unit circle
			GraphicsPath^ gpLarger = gcnew GraphicsPath();	// Holds more that unit circle

			// to make polar plot larger than unit circle
			System::Drawing::Rectangle tempbox;
			tempbox.X = polarBox.X -10;
			tempbox.Y = polarBox.Y -10;
			tempbox.Width = polarBox.Width + 20;
			tempbox.Height = polarBox.Height + 20;

			gpUnit->AddEllipse(polarBox);
			gpLarger->AddEllipse(tempbox);

            gr->SetClip(gpUnit);	// Set unit circle for drawing background grids


			// Zoom the polar display.  Leave the clipping region as previously set.
			// Artifically increase the value of polarbox by the zoom factor.
			// Leave the center of the PolarBox in the same place.
			// Thus the X and Y components of the polarbox have to be adjusted.

			int centerX = polarBox.X + polarBox.Width/2;
			int centerY = polarBox.Y + polarBox.Height/2;
			// now scale the polarBox & PolarRadius
			polarBox.Height = (int)(polarBox.Height * PolarZoom);	// increase width & height
			polarBox.Width = (int)(polarBox.Width * PolarZoom);
			polarBox.X = centerX - polarBox.Width/2;	// adjust X and Y offsets
			polarBox.Y = centerY - polarBox.Height/2;
			polarRadius = (int)(polarRadius * PolarZoom);

//
//	Table for value of gamma (reflection coefficient) vs. resistance, and radius of resistance circle
//
			//  0 ohms  ->  gamma = 1       radius = (1+1.00)/2 = 1.000 (no need to draw)
			//	10 ohms ->  gamma = .666	radius = (1+.666)/2 = 0.833
			//	25 ohms ->  gamma = .333	radius = (1+.333)/2 = 0.666
			//	50 ohms ->  gamma = 0		radius = (1+.000)/2 = 0.500
			//	100 ohms -> gamma = .333	radius = (1-.333)/2 = 0.333
			//	250 ohms -> gamma = .666	radius = (1-.666)/2 = 0.167
			//	500 ohms -> gamma = .818	radius = (1-.818)/2 = 0.091

			int resCircleRadius[6] = { 833, 667, 500, 333, 167, 91 };		// radius * 1000

//
//  Table for reactance circles and equivalent radius
//  Taking a guess that the formula for radius is 50/reactance
//
			//  0 ohms			radius = 50/0 = infinite (straight line)
			//	10 ohms			radius = 50/10 = 5
			//	25 ohms			radius = 50/25 = 2
			//	50 ohms			radius = 50/50 = 1
			// 100 ohms			radius = 50/100 = 0.50
			// 250 ohms			radius = 50/250 = 0.20
			// 500 ohms			radius = 50/500 = 0.10

			int reacCircleRadius[6] = { 500, 200, 100, 50, 20, 10 };	// radius * 100


			// draw resistance circles
			for (int i=0; i<6; i++)
			{
				// resize the radius (using integer math)
				tempPolarBox.Width = polarBox.Width * resCircleRadius[i]; tempPolarBox.Width /= 1000;
				tempPolarBox.Height = polarBox.Height * resCircleRadius[i]; tempPolarBox.Height /= 1000;
				// recenter circle on the right-hand side, centered vertically to chart center
				tempPolarBox.X = polarBox.X + (polarBox.Width - tempPolarBox.Width);
				tempPolarBox.Y = polarBox.Y + (polarBox.Height - tempPolarBox.Height)/2;

				gr->DrawEllipse(pengray, tempPolarBox);	
			}

			// draw center horizontal line (zero reactance line, infinite radius)
			gr->DrawLine(pengray, polarBox.X, polarBox.Y + polarRadius, polarBox.Right, polarBox.Y + polarRadius);

			// draw upper reactance circles
			for (int i=0; i<6; i++)
			{
				// resize the radius (using integer math)
				tempPolarBox.Width = polarBox.Width * reacCircleRadius[i]; tempPolarBox.Width /= 100;
				tempPolarBox.Height = polarBox.Height * reacCircleRadius[i]; tempPolarBox.Height /= 100;
				// recenter circle on the right hand side, bottom of circle at vertical center of chart
				tempPolarBox.X = polarBox.X + polarBox.Width - tempPolarBox.Width/2;
				tempPolarBox.Y = polarBox.Y + polarBox.Height/2 - tempPolarBox.Height;

				gr->DrawEllipse(pengray, tempPolarBox);
			}

			// draw lower reactance circles
			for (int i=0; i<6; i++)
			{
				// resize the radius (using integer math)
				tempPolarBox.Width = polarBox.Width * reacCircleRadius[i]; tempPolarBox.Width /= 100;
				tempPolarBox.Height = polarBox.Height * reacCircleRadius[i]; tempPolarBox.Height /= 100;
				// recenter circle on the right hand side, top of circle at vertical center of chart
				tempPolarBox.X = polarBox.X + polarBox.Width - tempPolarBox.Width/2;
				tempPolarBox.Y = polarBox.Y + polarBox.Height/2;

				gr->DrawEllipse(pengray, tempPolarBox);
			}


			// draw constant-SWR circles onthe chart using dashed line

			// polarRadius is int radius of the chart. Gamma is radius of SWR circle normalized
			// SWR = 1.5 ->  Gamma = 0.200
			// SWR = 2  -->  Gamma = 0.333
			// SWR = 3  -->  Gamma = 0.500
			// SWR = 5  -->  Gamma = 0.667
			// SWR = 10 -->  Gamma = 0.818
			
			int SWRCircleRadius[5] = { 200, 333, 500, 667, 818 };			// radius * 1000

			for (int i=0; i<5; i++)
			{
				// resize the radius using integer math
				tempPolarBox.Width = polarBox.Width * SWRCircleRadius[i]; tempPolarBox.Width /= 1000;
				tempPolarBox.Height = polarBox.Height * SWRCircleRadius[i]; tempPolarBox.Height /= 1000;
				// recenter circle on the horizontal and vertical center of chart
				tempPolarBox.X = polarBox.X + polarBox.Width/2 - tempPolarBox.Width/2;
				tempPolarBox.Y = polarBox.Y + polarBox.Height/2 - tempPolarBox.Height/2;

				gr->DrawEllipse(penDotGray, tempPolarBox);
			}



			// Draw reflection measurements on Polar Chart
			// Requires resolving the phase quandrant from I/Q measurements
			//   and magnitude from the reflection magnitude (which is a logrithmic value).
			//
			// when ReflPI is low,  the phase is near   0 degrees
			// when ReflPI is high, the phase is near 180 degrees
			// when ReflPQ is low,  the phase is near  90 degrees
			// when ReflPQ is high, the phase is near -90 degrees
			//
			// The I and Q phase readings saturates near the bottom and top of the reading,
			//   but the other reading is then in the middle of it's useful range.
			//   This it's always possible to accurately resolve the phase and quadrant.
			//
			// Quadrant 1 is 0 to +90 degrees, quadrant 2 is +90 to +180,
			//   quadrant 3 is -180 to -90, quadrant 4 is 0 to -90 degrees.
			//


			double fmagnitude;			// 0 to 1
			double fphase;				// -180 to +180	
			double& rmag = fmagnitude;
			double& rphs = fphase;
			int	x,y;					// display coordinate
			int& rx = x;
			int& ry = y;

			gr->SetClip(gpLarger);	// Set enlarged circle for drawing points a bit outside the unit circle

			for (int i=0; i<FG->points; i++)	// Display the frequency grid
			{
				avrgRef += trace[i]->Vref1;

				if (DisplayMeasured->Checked)
				{
					// convert measurements into polar S11
					CalData->ResolveReflPolar(trace[i], FG->Frequency(i), rmag, rphs, true);

					// Correct S11 measured values against Calibration data

					if (calCheckBox->Checked)
						CorrectS11(CalData, FG->Frequency(i), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);

					// Convert from polar to rectangular format, scale to chart size,
					// and align with respect the center of the chart.
					ToDisplayPolar(fmagnitude, fphase, polarRadius, polarBox.X, polarBox.Y, rx, ry);

					// store the display point
					traceStop.X = x;
					traceStop.Y = y;

					// draw from previous to current point
					if (i > 0)
						gr->DrawLine(penS11Mag, traceStart, traceStop);

					DrawMarkers(gr, penS11Mag, brS11Mag, traceStop, Markerfont, i);					

					// store as previous point
					traceStart.X = x;
					traceStart.Y = y;
				}

				if(DisplayMenuItem->Checked)		// also display storage
				{
					// convert measurements into polar S11
					CalData->ResolveReflPolar(traceSto[i], FG->Frequency(i), rmag, rphs, true);

					// Correct S11 measured values against Calibration data

					if (calCheckBox->Checked)
						CorrectS11(CalData, FG->Frequency(i), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);

					// Convert from polar to rectangular format, scale to chart size,
					// and align with respect the center of the chart.
					ToDisplayPolar(fmagnitude, fphase, polarRadius, polarBox.X, polarBox.Y, rx, ry);

					// store the display point
					traceStopS.X = x;
					traceStopS.Y = y;

					// draw from previous to current point
					if (i > 0)
						DrawLineBound(gr, tempbox, penS11MagSto, traceStartS, traceStopS);


					// store as previous point
					traceStartS.X = x;
					traceStartS.Y = y;

				}

				//Display error correction parameters -- for debugging
				// There are 1024 calibration frequencies, find the one nearest to the measured frequency.

				double delta = (120000000 - 200000)/ (1024 - 1);		// frequency separation of cal points
				int j = (int)((((float)FG->Frequency(i) - 200000) + delta/2.0) / delta); // nearest one

				if (EtTrace->Checked == true)
				{
							// Convert results to polar coordinates
					double xx = CalData->EtReal[i];
					double yy = CalData->EtImag[i];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);

					ToDisplayPolar(fmagnitude, fphase, polarRadius, polarBox.X, polarBox.Y, rx, ry);
					DrawLineBound(gr, tempbox, penS21Phs, x, y, x+2, y+2);
				}


				if (EsTrace->Checked == true)
				{
							// Convert results to polar coordinates
					double xx = CalData->EsReal[i];
					double yy = CalData->EsImag[i];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);

					ToDisplayPolar(fmagnitude, fphase, polarRadius, polarBox.X, polarBox.Y, rx, ry);
					DrawLineBound(gr, tempbox, penS21Mag, x, y, x+2, y+2);
				}


				if (EdTrace->Checked == true)
				{
							// Convert results to polar coordinates
					double xx = CalData->EdReal[i];
					double yy = CalData->EdImag[i];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);

					ToDisplayPolar(fmagnitude, fphase, polarRadius, polarBox.X, polarBox.Y, rx, ry);
					DrawLineBound(gr, tempbox, penS11Phs, x, y, x+2, y+2);
				}

				if (calSshort->Checked == true)
				{
							// Convert results to polar coordinates
					double xx = CalData->S11shortReal[i];
					double yy = CalData->S11shortImag[i];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);

					ToDisplayPolar(fmagnitude, fphase, polarRadius, polarBox.X, polarBox.Y, rx, ry);
					DrawLineBound(gr, tempbox, penS21Phs, x, y, x+2, y+2);
				}

				if (calSopen->Checked == true)
				{
							// Convert results to polar coordinates
					double xx = CalData->S11openReal[i];
					double yy = CalData->S11openImag[i];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);

					ToDisplayPolar(fmagnitude, fphase, polarRadius, polarBox.X, polarBox.Y, rx, ry);
					DrawLineBound(gr, tempbox, penS21Mag, x, y, x+2, y+2);
				}

				if (calSterm->Checked == true)
				{
							// Convert results to polar coordinates
					double xx = CalData->S11termReal[i];
					double yy = CalData->S11termImag[i];

					fphase = RAD2DEGR * atan2(yy, xx);
					fmagnitude = sqrt(xx*xx + yy*yy);

					ToDisplayPolar(fmagnitude, fphase, polarRadius, polarBox.X, polarBox.Y, rx, ry);
					DrawLineBound(gr, tempbox, penS11Phs, x, y, x+2, y+2);
				}



			}
		}

		if (TDRItem->Checked == true  && !Spectrum->Checked)		// render screen in TDR mode
		{
			double Real[2048], Imag[2048];
			double fmagnitude;			// 0 to 1
			double fphase;				// -180 to +180	
			double& rmag = fmagnitude;
			double& rphs = fphase;
			const bool Inverse = true;		// Inverse FFT mode
			String^ rangetext;

			StartIndex = (int)(TDRStartTime/TDRTIMETICK);
			StopIndex = (int)(TDRStopTime/TDRTIMETICK);

			PixelsPerGrid = (float)scopeDisp.Width / (float)(StopIndex - StartIndex);		// Fixed Size Display of IFFT

//--			gr->FillRectangle(brwhite, scopeDisp);
			gr->DrawRectangle(penBlack2, scopeDisp);


            gr->DrawString(SOFTWARE_VERSION, Statusfont, brBlack, topright);	// TAPR & Version

			// Display plot title 
            gr->DrawString(plotTitle, Titlefont, brBlack, botTitleCenter, Centerformat);

			if(!calCheckBox->Checked)
				gr->DrawString("Fixture Uncompensated", Warningfont, brGray, unFixComp);

			if(!VNAConnected)
				gr->DrawString("VNA Instrument Not Connected", Titlefont, brBlack, unConnected);

			// Display Vertical Scale
			rangetext = "Short";
			gr->DrawString(rangetext, Scalefont, brBlack, leftScaleBottom);
			rangetext = "Z=50";
			gr->DrawString(rangetext, Scalefont, brBlack, leftScaleCenter);
			rangetext = "Open";
			gr->DrawString(rangetext, Scalefont, brBlack, leftScaleTop);

			// Display Horizontal Scale
			gr->DrawString(String::Concat("START: ", TDRStartTime.ToString("N1"), " nSec"), Statusfont, brBlack, botleft);
			gr->DrawString(String::Concat("STOP: ", TDRStopTime.ToString("N1"), " nSec"), Statusfont, brBlack, botright);

			int span = (int)(TDRStopTime - TDRStartTime);
			gr->DrawString(String::Concat("SPAN: ", span.ToString("N1"), " nSec"), Statusfont, brBlack, botleftcenter);

			span = span / 10;			// per division
			gr->DrawString(String::Concat("HORIZ: ", span.ToString("N1"), " nSec/DIV"),
				Statusfont, brBlack, botrightcenter);


			for (int i=0; i<9; i++)				// draw major grid
			{
				leftTick.X = scopeDisp.X;
				leftTick.Y = scopeDisp.Y + scopeDisp.Height * (i+1) /10;
				rightTick.X = scopeDisp.X + scopeDisp.Width;
				rightTick.Y = scopeDisp.Y + scopeDisp.Height * (i+1) /10;
				topTick.X = scopeDisp.X + scopeDisp.Width * (i+1) / 10;
				topTick.Y = scopeDisp.Y;
				bottomTick.X = scopeDisp.X + scopeDisp.Width * (i+1) / 10;
				bottomTick.Y = scopeDisp.Y + scopeDisp.Height;
				gr->DrawLine(penBlack, leftTick, rightTick);
				gr->DrawLine(penBlack, topTick, bottomTick);
			};

			// draw minor grid along center horizontal line
			minorTopTick.Y = scopeDisp.Y + scopeDisp.Height/2 + 3;
			minorBotTick.Y = minorTopTick.Y - 6;
			for (int i=0; i<50; i++)
			{
				minorTopTick.X = scopeDisp.X + scopeDisp.Width * i /50;
				minorBotTick.X = scopeDisp.X + scopeDisp.Width * i /50;
				gr->DrawLine(penBlack, minorTopTick, minorBotTick);
			}

			// draw minor grid along center vertical line
			minorTopTick.X = scopeDisp.X + scopeDisp.Width/2 + 3;
			minorBotTick.X = minorTopTick.X - 6;
			for (int i=0; i<50; i++)
			{
				minorTopTick.Y = scopeDisp.Y + scopeDisp.Height * i /50;
				minorBotTick.Y = scopeDisp.Y + scopeDisp.Height * i /50;
				gr->DrawLine(penBlack, minorTopTick, minorBotTick);
			}	

			// Display "Uncalibrated" on the scope when InstrumentCal is not valid, when txLevel is not zero,
			// or when in Fast sweep mode.
#if 0
			if((CalData->RxDet->phaseCalibrated == false) || (CalData->TxDet->phaseCalibrated == false)
				|| (txLevel < 0) || String::Compare(SweepSpd->Text, "Fast") == 0)
				gr->DrawString("Uncalibrated", Warningfont, brGray, unCalText);
#endif
			// Gather polar reflection data into Real[], Imag[] arrays.
			// Take Inverse FFT. This yields time response. Plot time and distance.
			// Distance requires knowing the cable velocity factor.

			for(int i=5; i<1025; i++)
			{
				CalData->ResolveReflPolar(trace[i-5], FG->Frequency(i-5), rmag, rphs, true);
				if (calCheckBox->Checked)
					CorrectS11(CalData, FG->Frequency(i-5), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);

				Real[i] = rmag * cos(rphs * DEGR2RAD);
				Imag[i] = rmag * sin(rphs * DEGR2RAD);
			}


			for (int i=0; i<5; i++)			// Extrapolate low frequency terms down to DC
			{
				Real[i] = Real[5];						// flat magnitude assumption		
				Imag[i] = Imag[5] * 0.2 * (double)i;	// linear phase assumption to zero at DC		
			}

			for(int i=1; i<1024; i++)		// Create the negative freuqency components
			{								// using complex conjugate
				Real[2048-i] = Real[i];
				Imag[2048-i] = -Imag[i];
			}

			FFT(Real, Imag, 2048, Inverse);	// Inverse FFT

			for (int i=StartIndex+1; i<=StopIndex; i++)	// Draw Real time coefficients
			{
				traceStart.X = scopeDisp.X + (int)(((float)i - 1.0f - (float)StartIndex) * PixelsPerGrid); // previous point
				traceStop.X = scopeDisp.X + (int)(((float)i - (float)StartIndex) * PixelsPerGrid);		// current point

                traceStart.Y = scopeDisp.Y + (int)((float)scopeDisp.Height * (float)(-Real[i-1]+1.0)/2.0);
				traceStop.Y = scopeDisp.Y + (int)((float)scopeDisp.Height * (float)(-Real[i]+1.0)/2.0);

				if(traceStart.Y > scopeDisp.Height)		// Clip display in case Y-values are not valid
					traceStart.Y = scopeDisp.Height;
				if(traceStart.Y < scopeDisp.Y)
					traceStart.Y = scopeDisp.Y;
				if(traceStop.Y > scopeDisp.Height)
					traceStop.Y = scopeDisp.Height;
				if(traceStop.Y < scopeDisp.Y)
					traceStop.Y = scopeDisp.Y;

				DrawLineBound(gr, scopeDisp, penS11Mag, traceStart, traceStop);		// red

				DrawMarkerT(gr, penS11Mag, brS11Mag, traceStop, Markerfont, i);					
			}
// Imaginary part will be zero since IFFT symmetry conditions are met. No need to plot.
//			for (int i=StartIndex+1; i<StopIndex; i++)	// Draw Imaginary time coefficients
//			{
//				traceStart.X = scopeDisp.X + (int)(((float)i-1.0) * PixelsPerGrid); // previous point
//				traceStop.X = scopeDisp.X + (int)((float)i * PixelsPerGrid);		// current point
//
//                traceStart.Y = scopeDisp.Y + (float)scopeDisp.Height * (Imag[i-1]+1.0)/2.0;
//				traceStop.Y = scopeDisp.Y + (float)scopeDisp.Height * (Imag[i]+1.0)/2.0;
//				gr->DrawLine(penS21Mag, traceStart, traceStop);		// green
//
//				DrawMarkerT(gr, penS21Mag, brS21Mag, traceStop, Markerfont, i);					
//			}

			if(DisplayMenuItem->Checked)		// also display storage
			{
				for(int i=5; i<1025; i++)
				{
					CalData->ResolveReflPolar(traceSto[i-5], FG->Frequency(i-5), rmag, rphs, true);
					if (calCheckBox->Checked)
						CorrectS11(CalData, FG->Frequency(i-5), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);

					Real[i] = rmag * cos(rphs * DEGR2RAD);
					Imag[i] = rmag * sin(rphs * DEGR2RAD);
				}

				for (int i=0; i<5; i++)			// Extrapolate low frequency terms down to DC
				{
					Real[i] = Real[5];						// flat magnitude assumption		
					Imag[i] = Imag[5] * 0.2 * (double)i;	// linear phase assumption to zero at DC		
				}

				for(int i=1; i<1024; i++)		// Create the negative freuqency components
				{								// using complex conjugate
					Real[2048-i] = Real[i];
					Imag[2048-i] = -Imag[i];
				}

				FFT(Real, Imag, 2048, Inverse);	// Inverse FFT
				for (int i=StartIndex+1; i<StopIndex; i++)	// Draw Real time coefficients
				{
					traceStart.X = scopeDisp.X + (int)(((float)i-1.0) * PixelsPerGrid); // previous point
					traceStop.X = scopeDisp.X + (int)((float)i * PixelsPerGrid);		// current point

					traceStart.Y = scopeDisp.Y + (int)((float)scopeDisp.Height * (-Real[i-1]+1.0)/2.0);
					traceStop.Y = scopeDisp.Y + (int)((float)scopeDisp.Height * (-Real[i]+1.0)/2.0);

					if(traceStart.Y > scopeDisp.Height)		// Clip display in case Y-values are not valid
						traceStart.Y = scopeDisp.Height;
					if(traceStart.Y < scopeDisp.Y)
						traceStart.Y = scopeDisp.Y;
					if(traceStop.Y > scopeDisp.Height)
						traceStop.Y = scopeDisp.Height;
					if(traceStop.Y < scopeDisp.Y)
						traceStop.Y = scopeDisp.Y;

					DrawLineBound(gr, scopeDisp, penS11MagSto, traceStart, traceStop);		// red
				}
			}

 
		}  // End if TDR mode

		gr->ResetClip();

		//------------------ Trace labels -----------------------------------------------------

		avrgRef = avrgRef / FG->points;
		double avrgdBRef = SHORT2DB(avrgRef);
//		refLevelBox->Text = avrgRef.ToString("N1");
		gr->DrawString(String::Concat("Ref level = ",avrgdBRef.ToString("N1"), "dB"), Parameterfont, brBlack, leftstatuspoint);
		leftstatuspoint.Y += (int)(10.0 * DisplayExpansion);


		if (s11magItem->Checked )
		{
			gr->DrawString("S11 Magnitude", Parameterfont, brS11Mag, leftstatuspoint);
			leftstatuspoint.Y += (int)(10.0 * DisplayExpansion);
		}
		if (s11phsItem->Checked)
		{
			gr->DrawString("S11 Phase", Parameterfont, brS11Phs, leftstatuspoint);
			leftstatuspoint.Y += (int)(10.0 * DisplayExpansion);
		}

		if (s21magItem->Checked)
		{
			gr->DrawString("S21 Magnitude", Parameterfont, brS21Mag, leftstatuspoint);
			leftstatuspoint.Y += (int)(10.0 * DisplayExpansion);
		}
		if (s21phsItem->Checked)
		{
			gr->DrawString("S21 Phase", Parameterfont, brS21Phs, leftstatuspoint);
			leftstatuspoint.Y += (int)(10.0 * DisplayExpansion);

		}
		if(RDisplay->Checked)		// draw R-ohms part of S11
		{
			gr->DrawString("R", Parameterfont, brR, leftstatuspoint);
			leftstatuspoint.Y += (int)(10.0 * DisplayExpansion);
		}
		if(jXDisplay->Checked)		// draw jX-ohms part of S11
		{
			gr->DrawString("jX", Parameterfont, brjX, leftstatuspoint);
			leftstatuspoint.Y += (int)(10.0 * DisplayExpansion);
		}

		leftstatuspoint.Y += (int)(10.0 * DisplayExpansion); // Gap before markers

//-----------------------------------------------
		// Display marker status in margin on left side of scope display - rectangular and polar modes
		if (rectItem->Checked || polarItem->Checked)
			for (int i = 0; i<5; i++)
				{
					if (Marker[i] > 0)
					{
						gr->DrawString(String::Concat("Mkr",Convert::ToString(i+1),"  ", Marker[i].ToString("N0"), " Hz."),
							Scalefont, brBlack, leftstatuspoint);

						leftstatuspoint.Y += (int)(14.0 * DisplayExpansion);		// have to manually keep track of vertical position

					// draw the measured parameter value specified by marker on the screen
						if (ShowMarkerParameters)
						{
							int index = FG->GridPoint(Marker[i]);		// get array index equivalent to Marker frequency
							if (index > 0)								// if index is valid
							{
								double fmagnitude;			// 0 to 1
								double fphase;				// -180 to +180	
								double& rmag = fmagnitude;
								double& rphs = fphase;

								if (s11magItem->Checked || s11phsItem->Checked || polarItem->Checked)
								{
									CalData->ResolveReflPolar(trace[index], FG->Frequency(index), rmag, rphs, true);
									if (calCheckBox->Checked)
										CorrectS11(CalData, FG->Frequency(index), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);

									// convert fmagnitude to dB

									fmagnitude = 20 * log10(fmagnitude);

									gr->DrawString(String::Concat("S11Mag: ",fmagnitude.ToString("N1"), " dB."),
										Parameterfont, brS11Mag, leftstatuspoint);
									leftstatuspoint.Y += (int)(10.0 * DisplayExpansion);

									gr->DrawString(String::Concat("S11Phs: ",fphase.ToString("N1"), " deg."),
										Parameterfont, brS11Phs, leftstatuspoint);
									leftstatuspoint.Y += (int)(10.0 * DisplayExpansion);
								}

								if (s21magItem->Checked && rectItem->Checked)
								{
									CalData->ResolveTranPolar(trace[index], FG->Frequency(index), rmag, rphs);
									if (calCheckBox->Checked)
										CorrectS21(CalData, FG->Frequency(index), fmagnitude, fphase, rmag, rphs);

									// convert fmagnitude to dB

									fmagnitude = 20 * log10(fmagnitude);

									gr->DrawString(String::Concat("S21Mag: ",fmagnitude.ToString("N1")," dB."),
										Parameterfont, brS21Mag, leftstatuspoint);
									leftstatuspoint.Y += (int)(10.0 * DisplayExpansion);

									gr->DrawString(String::Concat("S21Phs: ",fphase.ToString("N1"), " deg."),
										Parameterfont, brS21Phs, leftstatuspoint);
									leftstatuspoint.Y += (int)(10.0 * DisplayExpansion);

								}


								if (VSWRdisplay->Checked || polarItem->Checked)
								{
									CalData->ResolveReflPolar(trace[index], FG->Frequency(index), rmag, rphs, true);
									if (calCheckBox->Checked)
										CorrectS11(CalData, FG->Frequency(index), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);

									if (fmagnitude > 1.0)		// clip if S11 greater than unity
										fmagnitude = 1.0;

									// convert fmagnitude to dB

									double swr = (1.0 + fmagnitude)/(1.0 - fmagnitude);

									gr->DrawString(String::Concat("SWR: ",swr.ToString("N2")),
										Parameterfont, brS11VSWR, leftstatuspoint);

									leftstatuspoint.Y += (int)(10.0 * DisplayExpansion);
								}
								if (polarItem->Checked || (rectItem->Checked &&
									(s11magItem->Checked || RDisplay->Checked || jXDisplay->Checked)))
								{
									CalData->ResolveReflPolar(trace[index], FG->Frequency(index), rmag, rphs, true);
									if (calCheckBox->Checked)
										CorrectS11(CalData, FG->Frequency(index), RefExtnCheckBox->Checked, fmagnitude, fphase, rmag, rphs);

									// convert magnitude+phase to R + jX

									float X = (float)(fmagnitude * cos(DEGR2RAD * fphase));
									float Y = (float)(fmagnitude * sin(DEGR2RAD * fphase));

									std::complex<float> S11(X, Y);		// S11
									std::complex<float> One(1.0, 0.0);	// unity
									std::complex<float> Zo(50.0, 0.0);	// Zo
									std::complex<float> Z;				// result

									Z = Zo * ( One + S11) / (One - S11);	// calculate normalized Z at mouse point

									float R = real(Z);
									float jX = imag(Z);

									if (jX >= 0.0)
									{
										String^ exponent = gcnew String("");
										// positive series reactance .. compute Lseries
										double Ls = jX / ( 2.0 * Math::PI * FG->Frequency(index));
										// format Ls as nHy, uHy, or mHy
										if (Ls < 0.000001)	// nHy
										{
											exponent = " nHy)";
											Ls *= 1000000000;
										}
										else if (Ls < 0.001)
										{
											exponent = " uHy)";
											Ls *= 1000000;
										}
										else
										{
											exponent = " mHy)";
											Ls *= 1000;
										}
										
										if (polarItem->Checked)
										gr->DrawString(String::Concat("Z  ",R.ToString("N1")," + j ", jX.ToString("N1")," ohms",
											"  (Ls = ",Ls.ToString("N1"),exponent), Parameterfont, brS11Mag, leftstatuspoint);
										else
										gr->DrawString(String::Concat("Z  ",R.ToString("N1")," + j ", jX.ToString("N1")," ohms"),
											Parameterfont, brS11Mag, leftstatuspoint);

									}
									else
									{
										String^ exponent = gcnew String("");
										complex <double> Zt (R, jX), Zr (R, 0.0), Zc, one(1.0,0.0), Rt, Rr,Rc ;
#if 1									// Calculate parallel C
										Rt = one / Zt;
										jX = (float)( 1 / imag(Rt));
#else									// calculate series C
										Zc = one/((one/Zt)-(one/Zr));
										jX = (float)imag(Zc);
										// negative series reactance .. compute Cseries
										jX = -jX;
#endif
										double Cs = 1.0 / ( 2.0 * Math::PI * FG->Frequency(index) * jX);
										// format Cs as pF, nF, or uF
										if (Cs < 0.000000001)	// pF
										{
											exponent = " pF)";
											Cs *= 1000000000000;
										}
										else if (Cs < 0.000001)
										{
											exponent = " nF)";
											Cs *= 1000000000;
										}
										else
										{
											exponent = " uF)";
											Cs *= 1000000;
										}
										if (polarItem->Checked)
										gr->DrawString(String::Concat("Z  ",R.ToString("N1")," - j ", jX.ToString("N1")," ohms",
											"  (Cp = ",Cs.ToString("N1"),exponent), Parameterfont, brS11Mag, leftstatuspoint);
										else
										gr->DrawString(String::Concat("Z  ",R.ToString("N1")," - j ", jX.ToString("N1")," ohms"),
											Parameterfont, brS11Mag, leftstatuspoint);

									}
									leftstatuspoint.Y += (int)(10.0 * DisplayExpansion);
								}
							}
						}
					}
					leftstatuspoint.Y += (int)(5.0 * DisplayExpansion);
				}
		// Display marker status in margin on left side of scope display - TDR mode
		if (TDRItem->Checked)
			for (int i = 0; i<5; i++)
				{
					if (MarkerT[i] > 0.1)
					{
						gr->DrawString(String::Concat("Mkr",Convert::ToString(i+1),"  ", MarkerT[i].ToString("N1"), " nSec."),
							Scalefont, brBlack, leftstatuspoint);

						leftstatuspoint.Y += (int)(14.0 * DisplayExpansion);		// have to manually keep track of vertical position

					// draw the measured parameter value specified by marker on the screen
						if (ShowMarkerParameters)
						{
							if(TDRMetricMode == true)
							{
								float distance = (float)METERSPERNSEC * 0.5f * MarkerT[i] * TDRvf;
								// display Time, cable length, value of IFFT coefficient
								gr->DrawString(String::Concat("Distance: ", distance.ToString("N2"),
									" meters."),	Scalefont, brBlack, leftstatuspoint);
							}
							else
							{
								float distance = (float)FEETPERNSEC * 0.5f * MarkerT[i] * TDRvf;
								// display Time, cable length, value of IFFT coefficient
								gr->DrawString(String::Concat("Distance: ", distance.ToString("N1"),
									" feet."),	Scalefont, brBlack, leftstatuspoint);
							}
						leftstatuspoint.Y += 10;		// have to manually keep track of vertical position
						}
					}
					leftstatuspoint.Y += (int)(5.0 * DisplayExpansion);
				}

		// Display zoom factor on polar display
		if(polarItem->Checked)
		{
			String^ ZoomStatus = String::Concat("Zoom = ", PolarZoom.ToString("N1"));
			gr->DrawString(ZoomStatus, Statusfont, brBlack, toprightcenter, Centerformat);
		}

	}

	/// Convert event into renderable size 
private: System::Void Form_Render(Graphics^ gr, System::Drawing::Size size)
{
    // Resize & Paint give us their size.
    // Convert the size to a rectangle with 0,0 as the (Top,Left) origin
    // then call the Render with the resultant rectangle.
    // (PrintPage calls Render directly with a rectangle region).
	bool printer = false;
    rect.Size = size;
    rect.X = 0;
    rect.Y = 0;
    Form_Render(gr, rect, printer);
}
	/// Mouse Down button event handler
private: System::Void Form_MouseDown(Object^ pSender, MouseEventArgs^ pe)
{
	if(pe->Button == ::MouseButtons::Left)
    {
		// Add a new Marker at Frequency of mouse cursor (assuming an idle MArker exists)
		int	Xpos, Ypos;		// where the mouse points relative to our display surface

		// Let left-right mouse motion select & drag marker for
		// both rectangular and polar display modes.
		if ((rectItem->Checked == true) || (polarItem->Checked == true))
		{
			Xpos = pe->X - scopeDisp.Left;
			Ypos = pe->Y - scopeDisp.Top;
											// no status if mouse is off the Scope Display surface
			if ((Xpos >= scopeDisp.Width) || (Xpos < 0) || (Ypos > scopeDisp.Height) || (Ypos < 0))
				return;

			__int64 Freq = FG->Frequency(Xpos * FG->points / scopeDisp.Width);

			MarkerMousePick = -1;			// no marker active
			for (int i=0; i<5; i++)
			{
				if (Marker[i] == Freq)		// if mouse picks an active marker
				{
					MarkerMousePick = i;	// record which one is picked
					break;
				}
				if (Marker[i] == 0)			// if mouse picks where no marker is present
				{							// find an idle marker
					Marker[i] = Freq;		// set it to cursor frequency
					Refresh();
					MarkerMousePick = i;	// save the marker we just picked
					break;
				}
			}
		}

		// Left mouse click inserts a time marker for TDR mode
		if (TDRItem->Checked == true)
		{
			Xpos = pe->X - scopeDisp.Left;
			Ypos = pe->Y - scopeDisp.Top;
											// no status if mouse is off the Scope Display surface
			if ((Xpos > scopeDisp.Width) || (Xpos < 0) || (Ypos > scopeDisp.Height) || (Ypos < 0))
				return;

			int Index = (int)((Xpos / PixelsPerGrid) + 0.5);
			float Indexer = TDRStartTime + (float)TDRTIMETICK * (float)Index;

			MarkerMousePick = -1;			// no marker active
			for (int i=0; i<5; i++)
			{
				if (MarkerT[i] == Indexer)		// if mouse picks an active marker
				{
					MarkerMousePick = i;	// record which one is picked
					break;
				}
				if (MarkerT[i] == 0)			// if mouse picks where no marker is present
				{							// find an idle marker
					MarkerT[i] = Indexer;		// set it to cursor time
					Refresh();
					MarkerMousePick = i;	// save the marker we just picked
					break;
				}
			}
		}
    }
	else if(pe->Button == ::MouseButtons::Right)
    {
		// construct cursor status display as 3 text lines, based on display

		if (cs != nullptr)		// delete any cursor status that may still be open
			cs->Close();

        String^ line1;
		String^ line2;
		String^ line3;
		int	Xpos, Ypos;		// where the mouse points relative to our display surface

		if (rectItem->Checked == true)		// build rectangular status
		{
			Xpos = pe->X - scopeDisp.Left;
			Ypos = pe->Y - scopeDisp.Top;
											// no status if mouse is off the Scope Display surface
			if ((Xpos >= scopeDisp.Width) || (Xpos < 0) || (Ypos > scopeDisp.Height) || (Ypos < 0))
				return;

			__int64 Freq = FG->Frequency(Xpos * FG->points / scopeDisp.Width);
			float Ampl = -(((float)Ypos / (float)scopeDisp.Height) * (float)RectVertScaledB * 10.0f - (float)refLevel + (float)txLevel);
			float SWR = 1.0f + (float)RectSWRScale - (((float)Ypos / (float)scopeDisp.Height) * (float)RectSWRScale);

			line1 = String::Concat("Frequency:  ",Freq.ToString("N0")," Hz.");
			line2 = String::Concat("Amplitude:  ",Ampl.ToString("N1")," dB.");
			line3 = String::Concat("SWR:        ",SWR.ToString("N2"));

		}
		if (polarItem->Checked == true)								// build polar status
		{
			Xpos = pe->X - polarBox.Left;
			Ypos = pe->Y - polarBox.Top;
											// no status if mouse far outside polar bounding box
			if ((Xpos >= polarBox.Width + 10) || (Xpos < -10) || (Ypos > polarBox.Height + 10) || (Ypos < -10))
				return;

			// set X and Y to value from -1.0 to +1.0, both zero at chart center

			float X = ((float)Xpos - (float)polarRadius) / (float)polarRadius;
			float Y = ((float)polarRadius - (float)Ypos) / (float)polarRadius;

											// no status if mouse far outside polar circle
			if (((X * X) + (Y * Y)) > 1.2)
				return;

			float Gamma = sqrt(X*X + Y*Y);
			float SWR = (1.0f + Gamma) / ((float)1.0 - Gamma);

			float MagdB = 10.0f * log10(X*X + Y*Y);		// length of reflection vector
			float ArgDeg = (float)(RAD2DEGR * atan2(Y, X));		// argument, degrees

			std::complex<float> S11(X, Y);		// S11
			std::complex<float> One(1.0f, 0.0f);	// unity
			std::complex<float> Zo(50.0f, 0.0f);	// Zo
			std::complex<float> Z;				// result

			Z = Zo * ( One + S11) / (One - S11);	// calculate normalized Z at mouse point

			float R = real(Z);
			float jX = imag(Z);

			line1 = String::Concat("Magnitude =  ",MagdB.ToString("N1")," dB.  Angle = ",
				ArgDeg.ToString("N0")," Deg.");
			line2 = String::Concat("SWR      =  ",SWR.ToString("N2"));

			if (jX >= 0.0)
				line3 = String::Concat("Impedance =   ",R.ToString("N1")," + j ",jX.ToString("N1")," ohms");
			else
			{
				jX = -jX;
				line3 = String::Concat("Impedance =   ",R.ToString("N1")," - j ",jX.ToString("N1")," ohms");
			}
		}

		if (TDRItem->Checked == true)
		{
			Xpos = pe->X - scopeDisp.Left;
			Ypos = pe->Y - scopeDisp.Top;
											// no status if mouse is off the Scope Display surface
			if ((Xpos >= scopeDisp.Width) || (Xpos < 0) || (Ypos > scopeDisp.Height) || (Ypos < 0))
				return;

			int Index = (int)((Xpos / PixelsPerGrid) + 0.5);
			float Indexer = TDRStartTime + (float)TDRTIMETICK * (float)Index;

			line1 = String::Concat("Time:              ",Indexer.ToString("N1")," nSec.");
			line2 = "";
			line3 = "";
		}
		cs = gcnew CursorStatus(line1, line2, line3);
		cs->Location = Point(this->Left + pe->X - 90, this->Top + pe->Y + 65);
        cs->Show();
    }

}
	/// Mouse Up button event handler
private: System::Void Form_MouseUp(Object^ pSender, MouseEventArgs^ pe)
{

	if(pe->Button == ::MouseButtons::Left)
    {
		MarkerMousePick = -1;				// clear marker active flag
    }
	else if(pe->Button == ::MouseButtons::Right)
    {
    }

}

	/// Mouse Move event handler
private: System::Void Form_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  pe)
		 {

			 bool moved = false;

			 if ((abs(p1.X - pe->X) > 2) | (abs(p1.Y - pe->Y) > 2))
				 moved = true;

 			 p1.X = pe->X;				// Save coordinate that mouse is pointing to
			 p1.Y = pe->Y;

			 if (moved)					// if the mouse moved
				if(cs != nullptr)			// then close CursorStatus window
					cs->Close();

			 if (MarkerMousePick >= 0)	// if Marker actively being dragged by mouse
			 {
		 		int	Xpos, Ypos;		// where the mouse points relative to our display surface
				Xpos = pe->X - scopeDisp.Left;
				Ypos = pe->Y - scopeDisp.Top;
												// no change if mouse is off the Scope Display surface
				if ((Xpos >= scopeDisp.Width) || (Xpos < 0) || (Ypos > scopeDisp.Height) || (Ypos < 0))
					return;

				if (rectItem->Checked || polarItem->Checked)
				{
					__int64 Freq = FG->Frequency(Xpos * FG->points / scopeDisp.Width);
					Marker[MarkerMousePick] = Freq;		// update the Marker frequency
				}
				if (TDRItem->Checked)
				{
					int Index = (int)((Xpos / PixelsPerGrid) + 0.5);
					float Indexer = TDRStartTime + (float)TDRTIMETICK * (float)Index;
					MarkerT[MarkerMousePick] = Indexer;	// update the Marker time
				}
				Refresh();				// and redraw
			 }
		 }

	/// Initialize VNA hardware, download binary image
private: System::Void VNA_Initialize(void)
		{
			// Allocate space for raw measurements and storage set

			trace = gcnew array<MeasurementSet^>(1024);		// 1024 data points make a complete sweep
			traceSto = gcnew array<MeasurementSet^>(1024);	// storage for a complete sweep

			for(int i=0; i<1024; i++)				// allocate space for sweep & storage
			{
				trace[i] = gcnew MeasurementSet;
				traceSto[i] = gcnew MeasurementSet;
			}
   
			stS11Mag = gcnew array<Double>(1024);		// allocate space for S-param storage
			stS11Phs = gcnew array<Double>(1024);		
			stS21Mag = gcnew array<Double>(1024);
			stS21Phs = gcnew array<Double>(1024);		
			stS12Mag = gcnew array<Double>(1024);
			stS12Phs = gcnew array<Double>(1024);		
			stS22Mag = gcnew array<Double>(1024);
			stS22Phs = gcnew array<Double>(1024);		

			StartUpPath = Directory::GetCurrentDirectory();		// Save start-up directory path

	// For Vista and Windows 7 compatibility. All user created data is outside the Program Files directory
			CurrentUserDataPath = Environment::GetEnvironmentVariable("APPDATA") + "\\TAPR\\VNA";
			
			if(!Directory::Exists(CurrentUserDataPath))			// If the user app data directory is not there
				Directory::CreateDirectory(CurrentUserDataPath); // then create it

	// For Vista and Windows 7 compatibility. The .ICA instrument calibration file is common to all users
			AllUsersDataPath = Environment::GetEnvironmentVariable("ALLUSERSPROFILE") + "\\TAPR\\VNA";
			
			if(!Directory::Exists(AllUsersDataPath))			// If the user app data directory is not there
				Directory::CreateDirectory(AllUsersDataPath); // then create it



	// Open the target 8051 binary file, and prepare to download it
#if 0
			array<Byte>^ Codebuffer;
			int Length;
			FileStream^ fs;

			OpenFileDialog^ infile = gcnew OpenFileDialog();

	// User-select the name of the code to download to the VNA
	//		infile->Filter = "binary files (*.bin)|*.bin";
	//		infile->Title = "Select binary file to download to target";
	//		infile->ShowDialog();

	// Hard code the name of the code to download to the VNA
			infile->FileName = String::Concat(StartUpPath, "\\VNA_target.bin");

			try
			{
				fs = gcnew FileStream(infile->FileName,FileMode::Open, FileAccess::Read);
			}
			catch(System::IO::IOException^ pe)
			{
				MessageBox::Show(pe->Message,"Error");
				if (fs)
					fs->Close();
				Environment::Exit(1);
			}
			
			Length = static_cast<int>(fs->Length);

			if (Length < 8196)
				Codebuffer = gcnew array<Byte>(8196);
			else
			{
				MessageBox::Show(" Download file too long","Error");
				fs->Close();
				Environment::Exit(1);
			}

			fs->Read(Codebuffer, 0, Length);
			fs->Close();
#endif
			VNA = gcnew VNADevice(serialPort1);

			if (VNA->State() == 1)		// EZUSBDRV found our device handle
			{
				bool result;

				result = VNA->Stop();				// Make sure 8051 is halted
				if (!result)
					MessageBox::Show("VNA Device Error: Stop","Error");

//				result = VNA->Download(Codebuffer, Length, 0);
//				if (!result)
//					MessageBox::Show("VNA Device Error: Download","Error");

				result = VNA->Start();				// Start the 8051 processor
				if (!result)
					MessageBox::Show("VNA Device Error: Start","Error");

//				for (int k=0; k<3; k++)
				{
					result = VNA->Init();				// Build VNA descriptor tables, get pipes
					if (!result)
						MessageBox::Show("VNA Device Error: Init","Error");
					else
					{
						VNAConnected = true;			// Clears display of VNA Disconnected Message
						SingleSweep->Enabled = true;
					}
				};

			}
			else
			{
				MessageBox::Show("Cannot open VNA Device","Error");

				// Enable the display of previously measured files even if the instrument is not available.
				// Environment::Exit(1);
			}
		}

		/// Background thread that reads & writes VNA hardware
private: System::Void Serial_Worker(void)			// runs as a background thread
		 {
			 int done = false;
//			 unsigned long freq;
//			 unsigned int level;
			 SerialThread->Priority= System::Threading::ThreadPriority::AboveNormal;
			 while(true)								// thread runs until terminated by program exit
			 {
				 while(WorkerCollect == false)		// while nothing to do
				 {
					 SerialThread->Sleep(10);	// go to sleep for 50 milliseconds (since nothing to do)
				 }
				 done = false;
#if 0

				 if (serialPort1->IsOpen) {
//					 while (!done) {
						 String ^s = serialPort1->ReadLine();

						 if (b == 'x') {
//							 freq = ((unsigned long)serialPort1->ReadByte());
//							 freq += ((unsigned long)serialPort1->ReadByte())<<8;
//							 freq += ((unsigned long)serialPort1->ReadByte())<<16;
//							 freq += ((unsigned long)serialPort1->ReadByte())<<24;
							 level = ((unsigned long)serialPort1->ReadByte());
							 level += ((unsigned long)serialPort1->ReadByte())<<8;
								freq = level;
							 MarkFrequency(freq);
						 } else if (b == 'e')
							 done = true;
					 }
					 while(WorkerCollect == true)		// while nothing to do
					{
						SerialThread->Sleep(10);	// go to sleep for 50 milliseconds (since nothing to do)

					 }
				 }
//				 if(serialPort1->IsOpen) serialPort1->Close();
#endif
			 }
		 }
		 private: System::Void AddMarkerButton_Click(System::Object^  sender, System::EventArgs^  e) {
			for (int i=0; i<5; i++)
			{
				if (Marker[i] == 0)			// if mouse picks where no marker is present
				{							// find an idle marker
					Marker[i] = FG->Frequency((int)MIndex->Value * FG->points / (MIndex ->Maximum +1));
					Refresh();
					break;
				}
			}

				  }

private: System::Void MIndex_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				int i;
				i = MIndex->Value * FG->points / (MIndex ->Maximum +1);
				Marker[0] = FG->Frequency(i);
				Refresh();
			}

				  /// Background thread that reads & writes VNA hardware
private: System::Void VNA_Worker(void)			// runs as a background thread
		{

		VNA_RXBUFFER * RxBuf = new VNA_RXBUFFER;
		VNA_RXBUFF_FAST * RxBufast = new VNA_RXBUFF_FAST;
		VNA_TXBUFFER * TxBuf = new VNA_TXBUFFER;
		__int64 freq;

		long Keep, TotalSize;			// Amount of exponential integrator to keep, and total integrator count

		// Buffers to hold integrand for integration of sweep values
		// New in V1.5

		array<MeasurementSet^>^ ITrace;
		ITrace = gcnew array<MeasurementSet^>(1024);
		for (int i=0; i<1024; i++)
			ITrace[i] = gcnew MeasurementSet;



		while(true)								// thread runs until terminated by program exit
		{
			while(WorkerCollect == false)		// while nothing to do
			{
				if(TDRItem->Checked == false)
				{
					menuItem5->Enabled = true;		// allow freq grid to be changed
					calibrateMenu->Enabled = true;	// enable calibration menu launch while collecting data
				}
				VNAWorkerThread->Sleep(10);	// go to sleep for 50 milliseconds (since nothing to do)
			}
	
		menuItem5->Enabled = false;				// disable change of FreqGrid while collecting data
		calibrateMenu->Enabled = false;			// disable calibration menu launch while collecting data

		for (int i=0; i<FG->points; i++)			// Save current values to integrand
		{
			ITrace[i]->ReflMI = trace[i]->ReflMI;
			ITrace[i]->ReflMQ = trace[i]->ReflMQ;
			ITrace[i]->ReflPI = trace[i]->ReflPI;
			ITrace[i]->ReflPQ = trace[i]->ReflPQ;
			ITrace[i]->TranMI = trace[i]->TranMI;
			ITrace[i]->TranMQHi = trace[i]->TranMQHi;
			ITrace[i]->TranPI = trace[i]->TranPI;
			ITrace[i]->TranPQ = trace[i]->TranPQ;
			ITrace[i]->TranMQLo = trace[i]->TranMQLo;
			ITrace[i]->TranMQMid = trace[i]->TranMQMid;
			ITrace[i]->TranPILow = trace[i]->TranPILow;
			ITrace[i]->TranPQLow = trace[i]->TranPQLow;
		}




			TxBuf->ReplyType = VNA_REPLYTYPE_FULL;
			TxBuf->MeasureDelay = MeasureDelayStringToCount(SweepSpd->Text);
			TxBuf->QDAClevel = QDAC_ZERODBM;			// Reference level

			// Set the Lo magnitude and Mid magnitude generator levels for the target
			// (Hi level is always 0 db.)
			TxBuf->IDAClevelHi = TxLevLinear(txLevel);					// High Tx Level
			TxBuf->IDAClevelLo = TxLevLinear(txLevel - TARGETLOMAG);	// Low TX Level
			TxBuf->IDAClevelMid = TxLevLinear(txLevel - TARGETMIDMAG);	// Mid TX Level

			// Set the Lo phase tran measurement level for the target   09-30-2007
			TxBuf->IDAClevelPhLow = TxLevLinear(txLevel - TARGETPHLOMAG);	// Lo Phase TX Level

			SweepProgressBar->Maximum = FG->points;		// Bar's maximum = number of points to sweep

			VNA->Sweep(FG->Frequency(0), FG->Frequency(1) - FG->Frequency(0), FG->points, TxBuf->MeasureDelay, Spectrum->Checked);
			for (int m=0; m<FG->points; m++)
			{
				if (!WorkerCollect) {
					MessageBox::Show("Sweep aborted", "Abort", MessageBoxButtons::OK, MessageBoxIcon::Error);
					break;
				}
				// calculate linear frequency spot for each sweep
				TxBuf->TxAccum = m; 
				TxBuf->Freq2 = FG->Frequency(m);
				freq = FG->Frequency(m);
				if (!VNA->WriteRead(TxBuf, RxBuf, DIR_REFL)) {
					//WorkerCollect = false;
					//SingleSweep->Enabled = true;		// re-enable the single sweep button
					RecurrentSweep->Text = "Free Run";	// stop free run
					//Refresh();							// Force a redraw of the screen	
					if (actualMeasurement.reference < -40.0)
						MessageBox::Show("Measurement signal level too low", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
					else if (actualMeasurement.reference > 15.0) 
						MessageBox::Show("Measurement signal level too high", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
					else
						MessageBox::Show("Sweep failed for unknown reason", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
 
					break;
				}
				RxBuf->Vref2 = (unsigned short)(512 + ( (freq* 1.0 - RxBuf->Freq) / ((FG->Frequency(0) + FG->Frequency(FG->points-1))/ 2.0)) * 512.0);
//				if ( !Spectrum->Checked  && ( freq* 1.0 / RxBuf->Freq < 0.99 || 1.01 < freq* 1.0 / RxBuf->Freq ) ) {
//				if (RxBuf->Vref2 > 1000) {
//					MessageBox::Show(String::Concat("More then 2% frequency tracking error expacted=",FreqToString(freq), ", actual=",FreqToString(RxBuf->Freq )), "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
//					break;
//				}
//				if (!VNA->WriteRead(TxBuf, RxBuf, DIR_REFL))
//					break;
//				if (!VNA->WriteRead(TxBuf, RxBuf, DIR_REFL))
//					break;

				// Test for ADC Write Error - retry if error
				
				//if(RxBuf->Header != 1)
				//{
				//	if(m)
				//		m--;
				//	TxBuf->TxAccum = FG->DDS(FG->Frequency(m));
				//	VNA->WriteRead(TxBuf, RxBuf);
				//}



				// Save received data by grid point

				trace[m]->ReflMI = RxBuf->ReflMI;
				trace[m]->ReflMQ = RxBuf->ReflMQ;
				trace[m]->ReflPI = RxBuf->ReflPI;
				trace[m]->ReflPQ = RxBuf->ReflPQ;
				trace[m]->Vref1 = RxBuf->Vref1;

			//}
			//for (int m=0; m<FG->points; m++)
			//{

				// calculate linear frequency spot for each sweep
			//	TxBuf->TxAccum = FG->DDS(FG->Frequency(m));
			//	VNA->WriteRead(TxBuf, RxBuf, DIR_TRANS);

				// Test for ADC Write Error - retry if error
				
				//if(RxBuf->Header != 1)
				//{
				//	if(m)
				//		m--;
				//	TxBuf->TxAccum = FG->DDS(FG->Frequency(m));
				//	VNA->WriteRead(TxBuf, RxBuf);
				//}



				// Save received data by grid point

				trace[m]->TranMI = RxBuf->TranMI;
				trace[m]->TranMQHi = RxBuf->TranMQHi;
				trace[m]->TranPI = RxBuf->TranPI;
				trace[m]->TranPQ = RxBuf->TranPQ;
				trace[m]->Vref2 = RxBuf->Vref2;

				trace[m]->TranMQLo = RxBuf->TranMQLo;
				trace[m]->TranMQMid = RxBuf->TranMQMid;

				// New 09-30-2007
				trace[m]->TranPILow = RxBuf->TranPILow;
				trace[m]->TranPQLow = RxBuf->TranPQLow;

				// Update Sweep Progress
				if ((m % 10) == 0) SweepProgressBar->Value = (m+1);


			// Glitch detection using median filtering algorithm,
			// faster but not as accurate as slow version.
			// Modified for V2.1 to examine the range of samples
			// in a group of 7, and to predict the value from slope
			// of neighboring values to avoid un-necessary filtering.

#ifdef FASTGLITCH
			DeGlitch(trace, FG->points);
#endif

		}
		SweepProgressBar->Value = FG->points;
			


		// V1.5 - Integration of readings

		if(PreCharge)							// If we want to Precharge the Integrator, then
			PreCharge = false;					// don't integrate values, and disable precharge (for next sweep)
		else
		{
			if(IntegrationMenu1x->Checked)
				Keep = 0, TotalSize = 1;
			if(IntegrationMenu2x->Checked)
				Keep = 1, TotalSize = 2;
			if(IntegrationMenu4x->Checked)
				Keep = 3, TotalSize = 4;
			if(IntegrationMenu8x->Checked)
				Keep = 7, TotalSize = 8;
			if(IntegrationMenu16x->Checked)
				Keep = 15, TotalSize = 16;

			for(int i=0; i<FG->points; i++)		// Exponentially Integrate the Current Readings
			{
				trace[i]->ReflMI = (unsigned short)(((long)ITrace[i]->ReflMI * Keep + (long)trace[i]->ReflMI ) / TotalSize);
				trace[i]->ReflMQ = (unsigned short)(((long)ITrace[i]->ReflMQ * Keep + (long)trace[i]->ReflMQ ) / TotalSize);
				trace[i]->ReflPI = (unsigned short)(((long)ITrace[i]->ReflPI * Keep + (long)trace[i]->ReflPI ) / TotalSize);
				trace[i]->ReflPQ = (unsigned short)(((long)ITrace[i]->ReflPQ * Keep + (long)trace[i]->ReflPQ ) / TotalSize);
				trace[i]->TranMI = (unsigned short)(((long)ITrace[i]->TranMI * Keep + (long)trace[i]->TranMI ) / TotalSize);
				trace[i]->TranMQHi = (unsigned short)(((long)ITrace[i]->TranMQHi * Keep + (long)trace[i]->TranMQHi ) / TotalSize);
				trace[i]->TranPI = (unsigned short)(((long)ITrace[i]->TranPI * Keep + (long)trace[i]->TranPI ) / TotalSize);
				trace[i]->TranPQ = (unsigned short)(((long)ITrace[i]->TranPQ * Keep + (long)trace[i]->TranPQ ) / TotalSize);
				trace[i]->TranMQLo = (unsigned short)(((long)ITrace[i]->TranMQLo * Keep + (long)trace[i]->TranMQLo ) / TotalSize);
				trace[i]->TranMQMid = (unsigned short)(((long)ITrace[i]->TranMQMid * Keep + (long)trace[i]->TranMQMid) / TotalSize);
				trace[i]->TranPILow = (unsigned short)(((long)ITrace[i]->TranPILow * Keep + (long)trace[i]->TranPILow) / TotalSize);
				trace[i]->TranPQLow = (unsigned short)(((long)ITrace[i]->TranPQLow * Keep + (long)trace[i]->TranPQLow) / TotalSize);
			}

		}
//		if(serialPort1->IsOpen) serialPort1->Close();

			// if not triggered by a recurrent sweep

		if (String::Compare(RecurrentSweep->Text, "Running") ==0)	// triggered by a recurrent sweep
				Refresh();
			else									// triggered by a single sweep
			{
				WorkerCollect = false;				// thread has completed a single sweep
				SingleSweep->Text = "SglSwp";
//				SingleSweep->Enabled = true;		// re-enable the single sweep button
				Refresh();							// Force a redraw of the screen	
			}
		}	//end	while(true)

		}


		/// Start Frequency Up (increment) button click event handler
private: System::Void startFup_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 FG->SetStartF(FG->StartF() + (int)Math::Pow(10.0, FrequencyDigitIndex));
			 if (FG->StartF() > VNA->GetMaxFreq())  // max allowed frequency
					FG->SetStartF(VNA->GetMaxFreq());
			 startF->Text = FG->StartF().ToString("N0");
		 }
		/// Start Frequency Down (decrement) button click event handler
private: System::Void startFdown_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 FG->SetStartF(FG->StartF() - (int)Math::Pow(10.0, FrequencyDigitIndex));
			 if (FG->StartF() <= VNA->GetMinFreq()) // min allowed frequency
				FG->SetStartF(VNA->GetMinFreq());
			 startF->Text = FG->StartF().ToString("N0");
		 }
		/// Stop Frequency Up (increment) button click event handler
private: System::Void stopFup_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 FG->SetStopF(FG->StopF() + (int)Math::Pow(10.0, FrequencyDigitIndex));
			 if (FG->StopF() > VNA->GetMaxFreq())  // max allowed frequency
					FG->SetStopF(VNA->GetMaxFreq());
			 stopF->Text = FG->StopF().ToString("N0");
		 }
		/// Stop Frequency Down (decrement) button click event handler
private: System::Void stopFdown_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 FG->SetStopF(FG->StopF() - (int)Math::Pow(10.0, FrequencyDigitIndex));
			 if (FG->StopF() <= VNA->GetMinFreq()) // min allowed frequency
				FG->SetStopF(VNA->GetMinFreq());
			 stopF->Text = FG->StopF().ToString("N0");
		 }
		/// Increase Transmit Level button event handler
private: System::Void txLup_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			if (txLevel < 0)   // maximum allowed value
				{
					txLevel += 1;
					txL->Text = txLevel.ToString();
				}
		 }
		/// Decrease Transmit Level button event handler
private: System::Void txLdown_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			if (txLevel > -40)  // minimum allowed value
				{
					if(txLevel == -10)
						MessageBox::Show("Warning: TxLevel Uncalibrated below - 10 db.", "Range Warning");

					txLevel -= 1;
					txL->Text = txLevel.ToString();
				}
		 }
		/// Increase Reference Level button event handler
private: System::Void refLup_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			if (refLevel < 50)  // maximum allowed value
			{
				refLevel += 1;
				refL->Text = refLevel.ToString();
			}
			Refresh();					// entire screen immeidately changes as a result of reflevel change
		 }
		/// Decrease Reference Level button event handler
private: System::Void refLdown_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			if (refLevel > -50) // minimum allowed value
				{
					refLevel -= 1;
					refL->Text = refLevel.ToString();
				}
			Refresh();
		 }
		/// Start Frequency double-click event handler
private: System::Void startF_DoubleClick(System::Object^  sender, System::EventArgs^  e)
			{
				// pop up a direct entry window
				NumEntBox^ nb = gcnew NumEntBox();
				nb->set_NumericValue(FG->StartF());
				if (nb->ShowDialog() == ::DialogResult::OK)
				{
					__int64 tempF = nb->get_NumericValue();
					// check for legitimate values for startF
					if (tempF >= 200000 && tempF <= 220000000)
					{
						FG->SetStartF(tempF);
						startF->Text = FG->StartF().ToString("N0");
					}
					else
						MessageBox::Show("Frequency must be between 200,000 and 200,000,000",
							"Out of Range");
				}
			}
		/// Stop Frequency double-click event handler
private: System::Void stopF_DoubleClick(System::Object^  sender, System::EventArgs^  e)
			 {
				// pop up a direct entry window
				NumEntBox^ nb = gcnew NumEntBox();
				nb->set_NumericValue(FG->StopF());
				if (nb->ShowDialog() == ::DialogResult::OK)
				{
					__int64 tempF = nb->get_NumericValue();
					if (tempF >= 200000 && tempF <= 220000000)
					{
						FG->SetStopF(tempF);
						stopF->Text = FG->StopF().ToString("N0");
					}
					else
						MessageBox::Show("Frequency must be between 200,000 and 200,000,000",
							"Out of Range");
				}
			 }
		/// Transmit Level double-click event handler
private: System::Void txL_DoubleClick(System::Object^  sender, System::EventArgs^  e)
		 	{
				// pop up a direct entry window
				NumEntBox^ nb = gcnew NumEntBox();
				nb->set_NumericValue(txLevel);
				if (nb->ShowDialog() == ::DialogResult::OK)
				{
					int tempL = (int)nb->get_NumericValue();
					if (tempL >= -10 && tempL <= 0)
					{
						txLevel = tempL;
						txL->Text = txLevel.ToString("N0");
					}
					else if (tempL >= -40 && tempL < -10)
					{
						MessageBox::Show("Warning: Tx Level is Uncalibrated below -10 db.", "Range Warning");
						txLevel = tempL;
						txL->Text = txLevel.ToString("N0");
					}
					else
						MessageBox::Show("Level must be between -40 and 0",
							"Out of Range");
				}
			}
		/// Reference Level double-click event handler
private: System::Void refL_DoubleClick(System::Object^  sender, System::EventArgs^  e)
		 	{
				// pop up a direct entry window
				NumEntBox^ nb = gcnew NumEntBox();
				nb->set_NumericValue(refLevel);
				if (nb->ShowDialog() == ::DialogResult::OK)
				{
					int tempL = (int)nb->get_NumericValue();
					if (tempL >= -50 && tempL <= 50)
					{
						refLevel = tempL;
						refL->Text = refLevel.ToString("N0");
					}
					else
						MessageBox::Show("Level must be between -50 and +50",
							"Out of Range");
				
					Refresh();		// value changed
				}
			}



		/// Print Preview menu item click handler
private: System::Void printPreviewItem_Click(System::Object^  sender, System::EventArgs^  e)
        {
            // Create a print preview control and attach it to the printer document
            PrintPreviewDialog^ pp = gcnew PrintPreviewDialog();
            pp->Document = pdoc;
            try
            {
                pp->ShowDialog();
            }
			catch(System::InvalidOperationException^ pe)
            {
                MessageBox::Show(pe->Message, "Error");
            }
			catch(System::ArgumentException^ pe)
            {
                MessageBox::Show(pe->Message, "Error");
            }

        }

		 /// Exit menu item click handler
		 /// Save configuration, close threads, terminate.
private: System::Void exitItem_Click(System::Object^  sender, System::EventArgs^  e)
        {
			if (CommandLine->Length == 0)	// Only save if invoked without a commandline argument
			{
				SaveFileDialog^ outfile = gcnew SaveFileDialog();
				outfile->FileName = String::Concat(CurrentUserDataPath, "\\__last.cfg");
				WriteConfiguration(outfile);
			}
			Environment::Exit(0);	// Close all threads then terminate.
        }

		/// Run Fixture Calibration menu item click handler
private: System::Void runItem_Click(System::Object^  sender, System::EventArgs^  e)
        {
			Calibration^ pcal = gcnew Calibration(CalData, VNA, FG);
			if (pcal->ShowDialog() == ::DialogResult::OK)
			{
				calCheckBox->Enabled = true;		// Fixture calibration ran OK and saved a file
				FixtureCalFileName = pcal->outfile->FileName;	// Update FixtureCalFileName
			}
        }
		/// Load Fixture Calibration File menu item handler
private: System::Void loadItem_Click(System::Object^  sender, System::EventArgs^  e)
        {
            OpenFileDialog^ infile = gcnew OpenFileDialog();
            infile->Filter = "calibration files (*.cal)|*.cal";
			if (infile->ShowDialog() == ::DialogResult::OK)
			{
                if (LoadCalDataSet(infile, CalData))
				{
					calCheckBox->Enabled = true;
                    FixtureCalFileName = infile->FileName;		// store name in case we need to save configuration
				}
				else		// Load of Cal DataSet failed
				{
					calCheckBox->Enabled = false;
					FixtureCalFileName = "";
				}
			}
        }
		/// Rectangular View menu item click handler
private: System::Void rectItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			int points;
			StringBuilder^ tempstr;
			// If in TDR mode then need to stop sweeping first
			if (TDRItem->Checked)	
				if ((String::Compare(RecurrentSweep->Text, "Running") == 0) ||
					(String::Compare(SingleSweep->Text,"Abort") == 0))
				{
					MessageBox::Show("Note: Must stop all sweeping before changing from TDR view. \n\r"
						"Disable Recurrent Sweep or allow Single Sweep Mode to complete.","Stop Sweeping",
						MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
					return;
				}
				else	// Ok to switch out of TDR view. 	
				{
					startF->Enabled = true;			// re-enable frequency controls
					startFup->Enabled = true;
					startFdown->Enabled = true;
					stopF->Enabled = true;
					stopFup->Enabled = true;
					stopFdown->Enabled = true;
					menuItem5->Enabled = true;		// FrequencyGrid Menu

//					TDRSetupmenuItem->Enabled = false;

					// Rebuild FrequencyGrid from control values

					if (grid101menu->Checked) points = 100;
					else if(grid201menu->Checked) points = 200;
					else if(grid401menu->Checked) points = 400;
					else points = 1020;

					FG = gcnew FrequencyGrid(points);
					FG->ferror = CalData->FreqError;

					tempstr = gcnew StringBuilder(startF->Text);
					tempstr->Replace("," , "");	// delete comma characters from string [North America]
					tempstr->Replace("." , "");	// delete period characters from string [Europe]
					FG->SetStartF(Convert::ToInt32(tempstr->ToString()));

					tempstr = gcnew StringBuilder(stopF->Text);
					tempstr->Replace("," , "");	// delete comma characters from string [North America]
					tempstr->Replace("." , "");	// delete period characters from string [Europe]
					FG->SetStopF(Convert::ToInt32(tempstr->ToString()));
				}

			rectItem->Checked = true;
			polarItem->Checked = false;
			TDRItem->Checked = false;
			Refresh();
		}
		/// Polar View menu item click handler
private: System::Void polarItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			int points;
			StringBuilder^ tempstr;
			// If in TDR mode then need to stop sweeping first
			if (TDRItem->Checked)		
				if ((String::Compare(RecurrentSweep->Text, "Running") == 0) ||
					(String::Compare(SingleSweep->Text,"Abort") == 0))
				{
					MessageBox::Show("Note: Must stop all sweeping before changing from TDR view. \n\r"
						"Disable Recurrent Sweep or allow Single Sweep Mode to complete.","Stop Sweeping",
						MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
					return;
				}
				else	// Ok to switch view.
				{
					startF->Enabled = true;			// re-enable frequency controls
					startFup->Enabled = true;
					startFdown->Enabled = true;
					stopF->Enabled = true;
					stopFup->Enabled = true;
					stopFdown->Enabled = true;
					menuItem5->Enabled = true;		// FrequencyGrid Menu

//					TDRSetupmenuItem->Enabled = false;

					// Rebuild FrequencyGrid from the controls
					
					if (grid101menu->Checked) points = 100;
					else if(grid201menu->Checked) points = 200;
					else if(grid401menu->Checked) points = 400;
					else points = 1020;

					FG = gcnew FrequencyGrid(points);
					FG->ferror = CalData->FreqError;

					tempstr = gcnew StringBuilder(startF->Text);
					tempstr->Replace("," , "");	// delete comma characters from string [North America]
					tempstr->Replace("." , "");	// delete period characters from string [Europe]
					FG->SetStartF(Convert::ToInt32(tempstr->ToString()));

					tempstr = gcnew StringBuilder(stopF->Text);
					tempstr->Replace("," , "");	// delete comma characters from string [North America]
					tempstr->Replace("." , "");	// delete period characters from string [Europe]
					FG->SetStopF(Convert::ToInt32(tempstr->ToString()));
				}

			rectItem->Checked = false;
			polarItem->Checked = true;
			TDRItem->Checked = false;
			Refresh();
							
		}
		/// Time Domain Reflectometer View menu item click handler
private: System::Void TDRItem_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			// If in Polar or Rectangular mode then need to stop sweeping first
			if ((rectItem->Checked) || (polarItem->Checked))
				if ((String::Compare(RecurrentSweep->Text, "Running") == 0) ||
					(String::Compare(SingleSweep->Text,"Abort") == 0))
				{
					MessageBox::Show("Note: Must stop all sweeping before changing to TDR view. \n\r"
						"Disable Recurrent Sweep or allow Single Sweep Mode to complete.","Stop Sweeping",
						MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
					return;
				}
			// Ok to switch view.	

			// Build Special FrequencyGrid just for TDR mode
			// A frequency array will be extrapolated from the measurement array
			// Deltaf = 119999488/N = 117187.
			// Highest frequency scanned = Deltaf * (N-1) = 119999488 Hz	
			// Deltat = 1/( N * Deltaf) = 8.333 nSec


			startF->Enabled = false;		// disable frequency controls in TDR mode
			startFup->Enabled = false;
			startFdown->Enabled = false;
			stopF->Enabled = false;
			stopFup->Enabled = false;
			stopFdown->Enabled = false;

			menuItem5->Enabled = false;		// disable FrequencyGrid menu in TDR mode

//			TDRSetupmenuItem->Enabled = true;	// enable TDR menu


			FG = gcnew FrequencyGrid(1020);
			FG->ferror = CalData->FreqError;
			FG->SetStartF(585935);			// becomes point[5] in the post-extrapolation array
			FG->SetStopF(119999488);		// would become point[1025] in the array (not used)
											// points [0] .. [4] get extrapolated so IFFT works

//			FG->SetStopF(120116675);	old code - before gridstop fix (dividing by points-1)
											

			rectItem->Checked = false;
			polarItem->Checked = false;
			TDRItem->Checked = true;
			Refresh();
		}

		/// S11Magnitude Display enable/disable menu item click handler
private: System::Void s11magItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if (s11magItem->Checked == true)
				s11magItem->Checked = false;
			else
				s11magItem->Checked = true;

			Refresh();	// Redraw window
		}
		/// S11Phase Display enable/disable meni item click handler
private: System::Void s11phsItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if (s11phsItem->Checked == true)
				s11phsItem->Checked = false;
			else
				s11phsItem->Checked = true;

			Refresh();	// Redraw window
		}
		/// S21Magnitude Display enable/disable menu item click handler
private: System::Void s21magItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if (s21magItem->Checked == true)
				s21magItem->Checked = false;
			else
				s21magItem->Checked = true;

			Refresh();	// Redraw window
		}
		/// S21Phase Display enable/disable menu item click handler
private: System::Void s21phsItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if (s21phsItem->Checked == true)
				s21phsItem->Checked = false;
			else
				s21phsItem->Checked = true;

			Refresh();	// Redraw window
		}

		 /// S21GroupDelay Display enable/disable menu item click handler
private: System::Void s21groupdelayItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if (s21groupdelayItem->Checked == true)
				s21groupdelayItem->Checked = false;
			else
				s21groupdelayItem->Checked = true;

			Refresh();	// Redraw window
		}

		 /// Calibration Applied button changed event handler
private: System::Void calCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
		 {
			if (calCheckBox->Checked)
				RefExtnCheckBox->Enabled = true;
			else
				RefExtnCheckBox->Enabled = false;

			Refresh();	// Force Screen redraw
		 }
		 /// Error Term Trace enable/disable menu item click handler
private: System::Void EtTrace_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 if (EtTrace->Checked == false)
				 EtTrace->Checked = true;
			 else
				 EtTrace->Checked = false;
			 
			 Refresh();
		 }
		 /// Error SourceZ Trace enable/disable menu item click handler
private: System::Void EsTrace_Click(System::Object^  sender, System::EventArgs^  e)
		 {
 			 if (EsTrace->Checked == false)
				 EsTrace->Checked = true;
			 else
				 EsTrace->Checked = false;
			
			 Refresh();
		 }
		 /// Error Directivity enable/disable menu item click handler
private: System::Void EdTrace_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 if (EdTrace->Checked == false)
				 EdTrace->Checked = true;
			 else
				 EdTrace->Checked = false;
			 
			 Refresh();
		 }
	 	 /// Fixture Calibration SHORT data enable/disable menu item click handler
private: System::Void calSshort_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 if (calSshort->Checked == false)
				 calSshort->Checked = true;
			 else
				 calSshort->Checked = false;
			 
			 Refresh();
		 }
	 	 /// Fixture Calibration OPEN data enable/disable menu item click handler
private: System::Void calSopen_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 if (calSopen->Checked == false)
				 calSopen->Checked = true;
			 else
				 calSopen->Checked = false;
			 
			 Refresh();
		 }
	 	 /// Fixture Calibration TERM data enable/disable menu item click handler
private: System::Void calSterm_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 if (calSterm->Checked == false)
				 calSterm->Checked = true;
			 else
				 calSterm->Checked = false;
			 
			 Refresh();
		 }
	 	 ///  10 dB/division menu item click handler
private: System::Void Scale10dB_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			Scale05dB->Checked = false;
			Scale1dB->Checked = false;
			Scale5dB->Checked = false;
			Scale2dB->Checked = false;
			Scale10dB->Checked = true;
			RectVertScaledB = 10.0f;
			Refresh();
		 }
	 	 ///  5 dB/division menu item click handler
private: System::Void Scale5dB_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			Scale10dB->Checked = false;
			Scale2dB->Checked = false;
			Scale1dB->Checked = false;
			Scale05dB->Checked = false;
			Scale5dB->Checked = true;
			RectVertScaledB = 5.0f;
			Refresh();
		 }
	 	 ///  2 dB/division menu item click handler
private: System::Void Scale2dB_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			Scale10dB->Checked = false;
			Scale5dB->Checked = false;
			Scale1dB->Checked = false;
			Scale05dB->Checked = false;
			Scale2dB->Checked = true;
			RectVertScaledB = 2.0f;
			Refresh();
		 }
	 	 ///  1 dB/division menu item click handler
private: System::Void Scale1dB_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			Scale10dB->Checked = false;
			Scale5dB->Checked = false;
			Scale2dB->Checked = false;
			Scale05dB->Checked = false;
			Scale1dB->Checked = true;
			RectVertScaledB = 1.0f;
			Refresh();
		 }
	 	 ///  0.5 dB/division menu item click handler
private: System::Void Scale05dB_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			Scale10dB->Checked = false;
			Scale5dB->Checked = false;
			Scale2dB->Checked = false;
			Scale05dB->Checked = true;
			Scale1dB->Checked = false;
			RectVertScaledB = 0.5f;
			Refresh();
		 }

	 	 ///  1 microsecond/division menu item click handler
private: System::Void scalemicroSec_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 scale100psec->Checked = false;
			 scalenanoSec->Checked = false;
			 scale10nsec->Checked = false;
			 scale100nsec->Checked = false;
			 scalemicroSec->Checked = true;
			 scale10microsec->Checked = false;
			 scale100microsec->Checked = false;
			 scale1millisec->Checked = false;
			 scalefactorGD = 10000;
			 Refresh();
		 }
	 	 ///  1 nanosecond/division menu item click handler
private: System::Void scalenanoSec_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 scale100psec->Checked = false;
			 scalenanoSec->Checked = true;
			 scale10nsec->Checked = false;
			 scale100nsec->Checked = false;
			 scalemicroSec->Checked = false;
			 scale10microsec->Checked = false;
			 scale100microsec->Checked = false;
			 scale1millisec->Checked = false;
			 scalefactorGD = 10;
			 Refresh();
		 }
	 	 ///  100 picosecond/division menu item click handler
private: System::Void scale100psec_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 scale100psec->Checked = true;
			 scalenanoSec->Checked = false;
			 scale10nsec->Checked = false;
			 scale100nsec->Checked = false;
			 scalemicroSec->Checked = false;
			 scale10microsec->Checked = false;
			 scale100microsec->Checked = false;
			 scale1millisec->Checked = false;
			 scalefactorGD = 1;
			 Refresh();
		 }
	 	 ///  10 nanosecond/division menu item click handler
private: System::Void scale10nsec_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 scale100psec->Checked = false;
			 scalenanoSec->Checked = false;
			 scale10nsec->Checked = true;
			 scale100nsec->Checked = false;
			 scalemicroSec->Checked = false;
			 scale10microsec->Checked = false;
			 scale100microsec->Checked = false;
			 scale1millisec->Checked = false;
			 scalefactorGD = 100;
             Refresh();
		 }
	 	 ///  100 nanosecond/division menu item click handler
private: System::Void scale100nsec_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 scale100psec->Checked = false;
			 scalenanoSec->Checked = false;
			 scale10nsec->Checked = false;
			 scale100nsec->Checked = true;
			 scalemicroSec->Checked = false;
			 scale10microsec->Checked = false;
			 scale100microsec->Checked = false;
			 scale1millisec->Checked = false;
			 scalefactorGD = 1000;
			 Refresh();
		 }
	 	 ///  10 microsecond/division menu item click handler
private: System::Void scale10microsec_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 scale100psec->Checked = false;
			 scalenanoSec->Checked = false;
			 scale10nsec->Checked = false;
			 scale100nsec->Checked = false;
			 scalemicroSec->Checked = false;
			 scale10microsec->Checked = true;
			 scale100microsec->Checked = false;
			 scale1millisec->Checked = false;
			 scalefactorGD = 100000;
			 Refresh();
		 }
		 ///  100 microsecond/division menu item click handler
private: System::Void scale100microsec_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 scale100psec->Checked = false;
			 scalenanoSec->Checked = false;
			 scale10nsec->Checked = false;
			 scale100nsec->Checked = false;
			 scalemicroSec->Checked = false;
			 scale10microsec->Checked = false;
			 scale100microsec->Checked = true;
			 scale1millisec->Checked = false;
			 scalefactorGD = 1000000;
			 Refresh();
		 }
		 ///  1 millisecond/division menu item click handler
private: System::Void scale1millisec_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 scale100psec->Checked = false;
			 scalenanoSec->Checked = false;
			 scale10nsec->Checked = false;
			 scale100nsec->Checked = false;
			 scalemicroSec->Checked = false;
			 scale10microsec->Checked = false;
			 scale100microsec->Checked = false;
			 scale1millisec->Checked = true;
			 scalefactorGD = 10000000;
			 Refresh();
		 }

 	 	 /// Through Calibration Data  Display enable/disable menu item click handler
private: System::Void calSthru_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 if (calSthru->Checked == false)
				 calSthru->Checked = true;
			 else
				 calSthru->Checked = false;
			 
			 Refresh();
		 }
		 /// Frequency Grid 100 points menu item click handler
private: System::Void grid101menu_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 grid101menu->Checked = true;
			 grid201menu->Checked = false;
			 grid401menu->Checked = false;
			 grid1024menu->Checked = false;
			 __int64 start = FG->StartF();
			 __int64 stop = FG->StopF();
			 FG = gcnew FrequencyGrid(100);	// Grid containing 100 frequencies
			 FG->SetStartF(start);			// pick up previous values for start
			 FG->SetStopF(stop);			// and stop
			 FG->ferror = CalData->FreqError;
			 PreCharge = true;				// Reset Integration when grid size is changed
		 }
		 /// Frequency Grid 200 points menu item click handler
private: System::Void grid201menu_Click(System::Object^  sender, System::EventArgs^  e)
		 {			 
			 grid101menu->Checked = false;
			 grid201menu->Checked = true;
			 grid401menu->Checked = false;
			 grid1024menu->Checked = false;
             __int64 start = FG->StartF();
			 __int64 stop = FG->StopF();
			 FG = gcnew FrequencyGrid(200);	// Grid containing 200 frequencies
			 FG->SetStartF(start);			// pick up previous values for start
			 FG->SetStopF(stop);			// and stop
			 FG->ferror = CalData->FreqError;
			 PreCharge = true;				// Reset Integration when grid size is changed
		 }
		 /// Frequency Grid 400 points menu item click handler
private: System::Void grid401menu_Click(System::Object^  sender, System::EventArgs^  e)
		 {			 
			 grid101menu->Checked = false;
			 grid201menu->Checked = false;
			 grid401menu->Checked = true;
			 grid1024menu->Checked = false;
			 __int64 start = FG->StartF();
			 __int64 stop = FG->StopF();
			 FG = gcnew FrequencyGrid(400);	// Grid containing 401 frequencies
			 FG->SetStartF(start);			// pick up previous values for start
			 FG->SetStopF(stop);			// and stop
			 FG->ferror = CalData->FreqError;
			 PreCharge = true;				// Reset Integration when grid size is changed
		 }
		 /// Frequency Grid 1020 points menu item click handler
private: System::Void grid1024menu_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 grid101menu->Checked = false;
			 grid201menu->Checked = false;
			 grid401menu->Checked = false;
			 grid1024menu->Checked = true;
			 __int64 start = FG->StartF();
			 __int64 stop = FG->StopF();
			 FG = gcnew FrequencyGrid(1020);	// Grid containing 1020 frequencies
			 FG->SetStartF(start);			// pick up previous values for start
			 FG->SetStopF(stop);			// and stop
			 FG->ferror = CalData->FreqError;
			 PreCharge = true;				// Reset Integration when grid size is changed
		 }
		 /// Run Instrument Calibration menu item click handler
private: System::Void instrumentCalItem_Click(System::Object^  sender, System::EventArgs^  e)
		 {
//			InstrumentCal^ Ical = gcnew InstrumentCal(CalData, VNA, FG, AllUsersDataPath);
//			Ical->ShowDialog();
			Refresh();
		 }
		 /// Help menu item click handler
private: System::Void HelpMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 Help::ShowHelp(this, String::Concat(StartUpPath, "\\VNAR3 Help.chm"));
		 }
		 /// About menu item click handler
private: System::Void AboutMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 Assembly^ _assembly;
			 _assembly = Assembly::GetExecutingAssembly();
			 //MessageBox::Show(_assembly->ToString(),"TAPR VNA Host Program");

			 About^ AboutBox = gcnew About(_assembly->ToString());
			 AboutBox->ShowDialog();
		 }
		 /// Set Marker menu item click handler
private: System::Void ChangeMarker_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 MarkerEntry^ Mkr = gcnew MarkerEntry(Marker, MarkerT);	// preload markers into dialog box
			 if (Mkr->ShowDialog() == ::DialogResult::OK)
			 {
				 for (int i=0; i<5; i++)
				 {
					 Marker[i] = Mkr->MarkerF[i];			// retrieve updated markers
					 MarkerT[i] = Mkr->MarkerT[i];
				 }
				 Refresh();
			 }
			 else
				 Update();
		 }
		/// Export S-parameters in Rectangular format menu item click handler
private: System::Void ExportRectFmt_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 if (!storeForward->Checked)
			 {
				 MessageBox::Show("No Forward parameters stored. Cannot generate file","Error",
					 MessageBoxButtons::OK, MessageBoxIcon::Error);
				return;
			 }
			 if (!storeReverse->Checked)
			 {
				 MessageBox::Show("No Reverse parameters stored. Cannot generate file","Error",
					 MessageBoxButtons::OK, MessageBoxIcon::Error);
				return;
			 }
			 storeForward->Checked = false;
			 storeReverse->Checked = false;

			 ExportSParams(0, FG,
				 stS11Mag, stS11Phs, stS21Mag, stS21Phs,
				 stS22Mag, stS22Phs, stS12Mag, stS12Phs);	//Rectangular Mode
		 }

		/// Export S-parameters in Polar format menu item click handler
private: System::Void ExportPolarFmt_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 if (!storeForward->Checked)
			 {
				MessageBox::Show("No Forward parameters stored. Cannot generate file","Error",
					MessageBoxButtons::OK, MessageBoxIcon::Error);
				return;
			 }
			 if (!storeReverse->Checked)
			 {
				MessageBox::Show("No Reverse parameters stored. Cannot generate file","Error",
					MessageBoxButtons::OK, MessageBoxIcon::Error);
				return;
			 }
			 storeForward->Checked = false;
			 storeReverse->Checked = false;

			 ExportSParams(1, FG,
				 stS11Mag, stS11Phs, stS21Mag, stS21Phs,
				 stS22Mag, stS22Phs, stS12Mag, stS12Phs);	//Polar Mode
		 }

		// Export S-parameters in CSV Rectangular Format menu item click handler North American CSV Format
private: System::Void ExportCSVRectFormat_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 if (!storeForward->Checked)
			 {
				MessageBox::Show("No Forward parameters stored. Cannot generate file","Error",
					MessageBoxButtons::OK, MessageBoxIcon::Error);
				return;
			 }
			 if (!storeReverse->Checked)
			 {
				MessageBox::Show("No Reverse parameters stored. Cannot generate file","Error",
					MessageBoxButtons::OK, MessageBoxIcon::Error);
				return;
			 }
			 storeForward->Checked = false;
			 storeReverse->Checked = false;

			 ExportSParams(2, FG,
				 stS11Mag, stS11Phs, stS21Mag, stS21Phs,
				 stS22Mag, stS22Phs, stS12Mag, stS12Phs);	//Rectangular North American CSV Mode

		 }

		// Export S-parameters in CSV Rectangular Format menu item click handler European CSV Format
private: System::Void ExportCSVRectFormatEurope_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 if (!storeForward->Checked)
			 {
				MessageBox::Show("No Forward parameters stored. Cannot generate file","Error",
					MessageBoxButtons::OK, MessageBoxIcon::Error);
				return;
			 }
			 if (!storeReverse->Checked)
			 {
				MessageBox::Show("No Reverse parameters stored. Cannot generate file","Error",
					MessageBoxButtons::OK, MessageBoxIcon::Error);
				return;
			 }
			 storeForward->Checked = false;
			 storeReverse->Checked = false;

			 ExportSParams(3, FG,
				 stS11Mag, stS11Phs, stS21Mag, stS21Phs,
				 stS22Mag, stS22Phs, stS12Mag, stS12Phs);	//Rectangular Europe CSV Mode

		 }

		/// Enable Marker Numbers menu item click handler
private: System::Void MarkerNumEnable_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 ShowMarkerNumbers = true;
			 Refresh();
		 }
		/// Disable Marker Numbers menu item click handler
private: System::Void MarkerNumDisable_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 ShowMarkerNumbers = false;
			 Refresh();
		 }
		/// Draw Frequency Marker #index using pen, brush at p with mfont
private: System::Void DrawMarkers(Graphics^ gr, Pen^ pen, Brush^ brush, Point p,
			 System::Drawing::Font^ mfont, int index)
		 {
			 for (int j=0; j<5; j++)						// five markers
				{
					if(FG->GridPoint(Marker[j]) == index)	// If this is a marker display point
					{										// draw marker
						String^ text; text = Convert::ToString(j+1);
						Point ul(p.X - 5, p.Y - 10);		// upper left corner of marker triangle
						Point ur(p.X + 5, p.Y - 10);		// upper right corner of marker triangle
						Point tx(p.X - 4, p.Y - 23);		// position where number is drawn
						DrawLineBound(gr, scopeDisp, pen, p, ul);
						DrawLineBound(gr, scopeDisp, pen, ul, ur);
						DrawLineBound(gr, scopeDisp, pen, ur, p);
						if (ShowMarkerNumbers)
							if((tx.Y < scopeDisp.Bottom) && (tx.Y > (scopeDisp.Top - 6)))	// only display if on-screen
								gr->DrawString(text, mfont, brush, tx);
					}
				}
		 }
		/// Draw Time Marker #index using pen, brush at p with mfont
private: System::Void DrawMarkerT(Graphics^ gr, Pen^ pen, Brush^ brush, Point p,
			 System::Drawing::Font^ mfont, int index)
		 {
			 for (int j=0; j<5; j++)						// five markers
				{
					if(abs(MarkerT[j] - index * TDRTIMETICK) < TDRTIMETICK/2.0)	// If this is a marker display point
					{										// draw marker
						String^ text;
						text = Convert::ToString(j+1);
						Point ul(p.X - 5, p.Y - 10);		// upper left corner of marker triangle
						Point ur(p.X + 5, p.Y - 10);		// upper right corner of marker triangle
						Point tx(p.X - 4, p.Y - 23);		// position where number is drawn
						DrawLineBound(gr, scopeDisp, pen, p, ul);
						DrawLineBound(gr, scopeDisp, pen, ul, ur);
						DrawLineBound(gr, scopeDisp, pen, ur, p);
						if (ShowMarkerNumbers)
							if((tx.Y < scopeDisp.Bottom) && (tx.Y > (scopeDisp.Top - 6)))	// only display if on-screen
	                            gr->DrawString(text, mfont, brush, tx);
					}
				}
		 }
		/// Store Forward S-parameters menu item click handler
private: System::Void storeForward_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 if (storeForward->Checked)
			 {
				MessageBox::Show("Forward Parameter Storage in use.\nExport Parameters to clear storage","Error");
				return;
			 }
			 storeForward->Checked = true;
			 StoreSParams(calCheckBox->Checked, RefExtnCheckBox->Checked, FG, CalData, trace, 
				 stS11Mag, stS11Phs, stS21Mag, stS21Phs);	//Forward Params
		 }
		/// Store Reverse S-parameters menu item click handler
private: System::Void storeReverse_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 if (storeReverse->Checked)
			 {
				MessageBox::Show("Reverse Parameter Storage in use.\nExport Parameters to clear storage","Error");
				return;
			 }
			 storeReverse->Checked = true;
			 StoreSParams(calCheckBox->Checked, RefExtnCheckBox->Checked, FG, CalData, trace,
				 stS22Mag, stS22Phs, stS12Mag, stS12Phs);	//Reverse Params
		 }
		/// Single Sweep button click handler
private: System::Void SingleSweep_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 if (String::Compare(SingleSweep->Text,"SglSwp") == 0) {
				 SingleSweep->Text = "Abort";
				 WorkerCollect = true;				// tell background thread to collect a sweep
				 // SingleSweep->Enabled = false;		// disable control. Thread will re-enable it
			 } else {
				 SingleSweep->Text = "SglSwp";
				 WorkerCollect = false;				// tell background thread to collect a sweep
			 }
		 }
		/// Free Run/Running button click handler
private: System::Void RecurrentSweep_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 if (String::Compare(RecurrentSweep->Text, "Free Run") == 0)
			 {
                 RecurrentSweep->Text = "Running";
				 SingleSweep->Text = "Abort";
				 WorkerCollect = true;
			 }
			 else
			 {
				 RecurrentSweep->Text = "Free Run";
				 SingleSweep->Text = "SglSwp";
				 WorkerCollect = false;
			 }
		 }
		/// VSWR Display enable/disable menu item click handler
private: System::Void VSWRdisplay_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 if (VSWRdisplay->Checked == true)
				VSWRdisplay->Checked = false;
			 else
				 VSWRdisplay->Checked = true;

			 Refresh();
		 }
		/// Fast/Slow button click handler
private: System::Void SweepSpd_Click(System::Object^  sender, System::EventArgs^  e)
		 {
/*			 if (String::Compare(SweepSpd->Text, "Fast") == 0)
				 SweepSpd->Text = "30 us";
			 else if (String::Compare(SweepSpd->Text, "30 us") == 0)
				 SweepSpd->Text = "100 us";
			 else if (String::Compare(SweepSpd->Text, "100 us") == 0)
				 SweepSpd->Text = "300 us";
			 else if (String::Compare(SweepSpd->Text, "300 us") == 0)
				 SweepSpd->Text = "1 ms";
			 else
*/			 if (String::Compare(SweepSpd->Text, "1 ms") == 0)
				 SweepSpd->Text = "3 ms";
			 else if (String::Compare(SweepSpd->Text, "3 ms") == 0)
				 SweepSpd->Text = "10 ms";
			 else if (String::Compare(SweepSpd->Text, "10 ms") == 0)
				 SweepSpd->Text = "30 ms";
//			 else if (String::Compare(SweepSpd->Text, "30 ms") == 0)
//				 SweepSpd->Text = "100 ms";
			 else
				 SweepSpd->Text = "1 ms";	// go to Fast mode if speed = 10 ms or anything unknown		
		 }

		/// Group Delay Aperture Size=1 menu item click handler
private: System::Void GDAperture1_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 GDAperture1->Checked = true;
			 GDAperture4->Checked = false;
			 GDAperture16->Checked = false;
			 GDAperture64->Checked = false;
			 Refresh();
		 }

		/// Group Delay Aperture Size=4 menu item click handler
private: System::Void GDAperture4_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 GDAperture1->Checked = false;
			 GDAperture4->Checked = true;
			 GDAperture16->Checked = false;
			 GDAperture64->Checked = false;
			 Refresh();
		 }

		/// Group Delay Aperture Size=16 menu item click handler
private: System::Void GDAperture16_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 GDAperture1->Checked = false;
			 GDAperture4->Checked = false;
			 GDAperture16->Checked = true;
			 GDAperture64->Checked = false;
			 Refresh();
		 }

		/// Group Delay Aperture Size=64 menu item click handler
private: System::Void GDAperture64_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 GDAperture1->Checked = false;
			 GDAperture4->Checked = false;
			 GDAperture16->Checked = false;
			 GDAperture64->Checked = true;
			 Refresh();
		 }

		/// Marker Parameter Enable menu item click handler
private: System::Void MarkerParamEnable_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 ShowMarkerParameters = true;
			 Refresh();
		 }
		/// Marker Parameter Disable menu item click handler
private: System::Void MarkerParamDisable_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 ShowMarkerParameters = false;
			 Refresh();
		 }

		/// Set Plot Title menu item click handler
private: System::Void SetTitleMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 TextEntry^ PlotT = gcnew TextEntry("Enter Plot Title");
			 PlotT->set_DisplayedText(plotTitle);
			 PlotT->ShowDialog();
			 if (PlotT->DialogResult == ::DialogResult::OK)
				 plotTitle = PlotT->EnteredText();
			 Refresh();
		 }
		/// Mouse Hovering Over Control event handler
private: System::Void Form_MouseHover(System::Object^  sender, System::EventArgs^  e)
		 {
		 }

		/// Right Scale=Phase menu item click handler
private: System::Void RightScalePhase_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 RightScalePhase->Checked = true;
			 RightScaleSWR->Checked = false;
			 RightScaleDelay->Checked = false;
			 RightScaleOhms->Checked = false;
			 Refresh();
		 }
		/// Right Scale=SWR menu item click handler
private: System::Void RightScaleSWR_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 RightScalePhase->Checked = false;
			 RightScaleSWR->Checked = true;
			 RightScaleDelay->Checked = false;
			 RightScaleOhms->Checked = false;
			 Refresh();
		 }
		/// Right Scale=Delay menu item click handler
private: System::Void RightScaleDelay_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 RightScalePhase->Checked = false;
			 RightScaleSWR->Checked = false;
			 RightScaleDelay->Checked = true;
			 RightScaleOhms->Checked = false;
			 Refresh();
		 }
		/// Clear All Markers menu item click handler
private: System::Void ClearAllMarkers_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 for (int i=0; i<5; i++)
			 {
				 Marker[i] = 0;			// clears all markers
				 MarkerT[i] = 0;
			 }
			 Refresh();
		 }

		/// SWR Scale=1 to 11 menu item click handler
private: System::Void ScaleSWR10_Click(System::Object^  sender, System::EventArgs^  e)
		 {
 			ScaleSWR10->Checked = true;
			ScaleSWR5->Checked = false;
			ScaleSWR2->Checked = false;
			ScaleSWR1->Checked = false;
			RectSWRScale = 10;
			Refresh();
		 }
		/// SWR Scale=1 to 6 menu item click handler
private: System::Void ScaleSWR5_Click(System::Object^  sender, System::EventArgs^  e)
		 {
  			ScaleSWR10->Checked = false;
			ScaleSWR5->Checked = true;
			ScaleSWR2->Checked = false;
			ScaleSWR1->Checked = false;
			RectSWRScale = 5;
			Refresh();
		 }

		/// SWR Scale=1 to 3 menu item click handler
private: System::Void ScaleSWR2_Click(System::Object^  sender, System::EventArgs^  e)
		 {
  			ScaleSWR10->Checked = false;
			ScaleSWR5->Checked = false;
			ScaleSWR2->Checked = true;
			ScaleSWR1->Checked = false;
			RectSWRScale = 2;
			Refresh();
		 }

		/// SWR Scale=1 to 2 menu item click handler
private: System::Void ScaleSWR1_Click(System::Object^  sender, System::EventArgs^  e)
		 {
  			ScaleSWR10->Checked = false;
			ScaleSWR5->Checked = false;
			ScaleSWR2->Checked = false;
			ScaleSWR1->Checked = true;
			RectSWRScale = 1;
			Refresh();
		 }

		/// Storage Store menu item click handler
private: System::Void StoreMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 for(int i=0; i<1024; i++)		// store display traces to memory
			 {
				 traceSto[i]->ReflMI = trace[i]->ReflMI;
				 traceSto[i]->ReflMQ = trace[i]->ReflMQ;
				 traceSto[i]->ReflPI = trace[i]->ReflPI;
				 traceSto[i]->ReflPQ = trace[i]->ReflPQ;
				 traceSto[i]->Vref1 = trace[i]->Vref1;
				 traceSto[i]->TranMI = trace[i]->TranMI;
				 traceSto[i]->TranMQHi = trace[i]->TranMQHi;
				 traceSto[i]->TranPI = trace[i]->TranPI;
				 traceSto[i]->TranPQ = trace[i]->TranPQ;
				 traceSto[i]->Vref2 = trace[i]->Vref2;
				 traceSto[i]->TranMQLo = trace[i]->TranMQLo;
				 traceSto[i]->TranMQMid = trace[i]->TranMQMid;
				 traceSto[i]->TranPILow = trace[i]->TranPILow;
				 traceSto[i]->TranPQLow = trace[i]->TranPQLow;
			 }
		 }

		/// Storage Recall menu item click handler
private: System::Void RecallMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		 {
 			 for(int i=0; i<1024; i++)		// recall display traces from memory
			 {
				 trace[i]->ReflMI = traceSto[i]->ReflMI;
				 trace[i]->ReflMQ = traceSto[i]->ReflMQ;
				 trace[i]->ReflPI = traceSto[i]->ReflPI;
				 trace[i]->ReflPQ = traceSto[i]->ReflPQ;
				 trace[i]->Vref1 = traceSto[i]->Vref1;
				 trace[i]->TranMI = traceSto[i]->TranMI;
				 trace[i]->TranMQHi = traceSto[i]->TranMQHi;
				 trace[i]->TranPI = traceSto[i]->TranPI;
				 trace[i]->TranPQ = traceSto[i]->TranPQ;
				 trace[i]->Vref2 = traceSto[i]->Vref2;
				 trace[i]->TranMQLo = traceSto[i]->TranMQLo;
				 trace[i]->TranMQMid = traceSto[i]->TranMQMid;
				 trace[i]->TranPILow = traceSto[i]->TranPILow;
				 trace[i]->TranPQLow = traceSto[i]->TranPQLow;
			 }
			 Refresh();
		 }

		/// Storage Display enable/disable menu item click handler
private: System::Void DisplayMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 if(DisplayMenuItem->Checked == false)	// toggle storage_display check-mark
				 DisplayMenuItem->Checked = true;
			 else
				 DisplayMenuItem->Checked = false;

			 Refresh();
		 }

private: System::Void FrequencyDigitDecrease_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 FrequencyDigitIndex--;
			 if(FrequencyDigitIndex < 0)
				 FrequencyDigitIndex = 8;
			 StartFrequencyDigitIndicator->Text = FrequencyDigitText[FrequencyDigitIndex];
			 StopFrequencyDigitIndicator->Text = FrequencyDigitText[FrequencyDigitIndex];

		 }

private: System::Void FrequencyDigitIncrease_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 FrequencyDigitIndex++;
			 if(FrequencyDigitIndex > 8)
				 FrequencyDigitIndex = 0;
			 StartFrequencyDigitIndicator->Text = FrequencyDigitText[FrequencyDigitIndex];
			 StopFrequencyDigitIndicator->Text = FrequencyDigitText[FrequencyDigitIndex];
		 }

private: System::Void PolarZoom10Menu_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 PolarZoom = 1.0;
			 PolarZoom10Menu->Checked = true;
			 PolarZoom15Menu->Checked = false;
			 PolarZoom20Menu->Checked = false;
			 PolarZoom25Menu->Checked = false;
			 Refresh();
		 }

private: System::Void PolarZoom15Menu_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 PolarZoom = 1.5;
			 PolarZoom10Menu->Checked = false;
			 PolarZoom15Menu->Checked = true;
			 PolarZoom20Menu->Checked = false;
			 PolarZoom25Menu->Checked = false;
			 Refresh();
		 }

private: System::Void PolarZoom20Menu_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 PolarZoom = 2.0;
			 PolarZoom10Menu->Checked = false;
			 PolarZoom15Menu->Checked = false;
			 PolarZoom20Menu->Checked = true;
			 PolarZoom25Menu->Checked = false;
			 Refresh();
		 }

private: System::Void PolarZoom25Menu_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 PolarZoom = 2.5;
			 PolarZoom10Menu->Checked = false;
			 PolarZoom15Menu->Checked = false;
			 PolarZoom20Menu->Checked = false;
			 PolarZoom25Menu->Checked = true;
			 Refresh();
		 }

private: System::Void SaveConfigurationMenu_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 /// Save the current instrument state to a configuration file:
			 /// 0. File identifier and version
			 /// 1. Whether or not a detector.ica file was loaded.
			 /// 2. Current fixture calibration filename (if loaded).
			 /// 3. Main window size, location.
			 /// 4. Window control values (start & stop frequency, levels, fast/slow button, plot title).
			 /// 5. Menu selections: polar/rectangular view, traces selected, vertical scales, etc
 			 /// 6. Marker values and display properties.
			 /// 7. Stored trace results.


			if(String::Compare(SingleSweep->Text,"SglSwp") != 0)
			{
				MessageBox::Show("All sweeping must be stopped in order to save the configuration",
					"Warning",MessageBoxButtons::OK, MessageBoxIcon::Warning);
				return;
			}

			SaveFileDialog^ outfile = gcnew SaveFileDialog();
			outfile->Filter = "configuration file (*.cfg)|*.cfg";
			outfile->AddExtension = true;
			if (outfile->ShowDialog() != ::DialogResult::OK)
				return;

			WriteConfiguration(outfile);

		 };

private: System::Void WriteConfiguration(SaveFileDialog^ outfile)
		{
			String^ id = "TAPR VNA Configuration File Version 2.0.1";
			FileStream^ fs;
			BinaryWriter^ bw;

			try
			{
				fs = gcnew FileStream(outfile->FileName, FileMode::Create, FileAccess::Write);
				bw = gcnew BinaryWriter(fs);

			 // 0. File identifier and version
				bw->Write(id);

			 // 1. Whether or not a detector.ica file was loaded.
#if 0
				if(CalData->RxDet->phaseCalibrated || CalData->TxDet->phaseCalibrated)
					bw->Write(true);
				else
					bw->Write(false);
#endif
             // 2. Current fixture calibration filename & applied state.
				if(calCheckBox->Enabled && FixtureCalFileName)
				{
					bw->Write(calCheckBox->Enabled);
					bw->Write(calCheckBox->Checked);
                    bw->Write(FixtureCalFileName);
				}
				else							// not able to identify valid Cal settings - disable fixture cal
				{
					bw->Write(false);			// calCheckedBox->Enabled
					bw->Write(false);			// calCheckBox->Checked
					bw->Write("");				// FixtureCalFileName
				}

             // 3. Main window size, location.
				bw->Write(this->Top);
				bw->Write(this->Left);
				bw->Write(this->Width);
				bw->Write(this->Height);

			 // 4. Window control values (start & stop frequency, levels, fast/slow button, plot title).
				bw->Write(FG->StartF());
				bw->Write(FG->StopF());
				bw->Write(FG->ferror);
				bw->Write(FG->points);
				bw->Write(FrequencyDigitIndex);
				bw->Write(txLevel);
				bw->Write(refLevel);

				bw->Write(startF->Enabled);
				bw->Write(stopF->Enabled);
				bw->Write(startFup->Enabled);
				bw->Write(startFdown->Enabled);
				bw->Write(stopFup->Enabled);
				bw->Write(stopFdown->Enabled);
				bw->Write(menuItem5->Enabled);

				if(SweepSpd->Text)
					bw->Write(SweepSpd->Text);
				else
					bw->Write("slow");

				bw->Write("Free Run");				//bw->Write(RecurrentSweep->Text) - force single sweep mode
				bw->Write(String::Compare(SingleSweep->Text,"SglSwp") != 0);

				if(plotTitle)
					bw->Write(plotTitle);
				else
					bw->Write("");

			 // 5. Menu selections: polar/rectangular view,	traces selected, vertical scales, etc.
				bw->Write(rectItem->Checked);
				bw->Write(polarItem->Checked);
				bw->Write(TDRItem->Checked);

				bw->Write(s11magItem->Checked);
				bw->Write(s11phsItem->Checked);
				bw->Write(s21magItem->Checked);
				bw->Write(s21phsItem->Checked);
				bw->Write(s21groupdelayItem->Checked);
				bw->Write(EtTrace->Checked);
				bw->Write(EsTrace->Checked);
				bw->Write(EdTrace->Checked);
				bw->Write(calSshort->Checked);
				bw->Write(calSopen->Checked);
				bw->Write(calSterm->Checked);

				bw->Write(Scale05dB->Checked);
				bw->Write(Scale1dB->Checked);
				bw->Write(Scale2dB->Checked);
				bw->Write(Scale5dB->Checked);
				bw->Write(Scale10dB->Checked);
				bw->Write(RectVertScaledB);

				bw->Write(ScaleSWR1->Checked);
				bw->Write(ScaleSWR2->Checked);
				bw->Write(ScaleSWR5->Checked);
				bw->Write(ScaleSWR10->Checked);
				bw->Write(RectSWRScale);

				bw->Write(scale100psec->Checked);
				bw->Write(scalenanoSec->Checked);
				bw->Write(scale10nsec->Checked);
				bw->Write(scale100nsec->Checked);
				bw->Write(scalemicroSec->Checked);
				bw->Write(scale10microsec->Checked);
				bw->Write(scale100microsec->Checked);	// NEW 12-28-2005
				bw->Write(scalefactorGD);

				bw->Write(PolarZoom10Menu->Checked);
				bw->Write(PolarZoom15Menu->Checked);
				bw->Write(PolarZoom20Menu->Checked);
				bw->Write(PolarZoom25Menu->Checked);
				bw->Write(PolarZoom);

				bw->Write(grid101menu->Checked);
				bw->Write(grid201menu->Checked);
				bw->Write(grid401menu->Checked);
				bw->Write(grid1024menu->Checked);

				bw->Write(GDAperture1->Checked);
				bw->Write(GDAperture4->Checked);
				bw->Write(GDAperture16->Checked);
				bw->Write(GDAperture64->Checked);

				bw->Write(RightScalePhase->Checked);
				bw->Write(RightScaleSWR->Checked);
				bw->Write(RightScaleDelay->Checked);

				bw->Write(DisplayMenuItem->Checked);    // Display Stored Traces

				bw->Write(IntegrationMenu1x->Checked);	// New 03-23-2006
				bw->Write(IntegrationMenu2x->Checked);	// New 03-23-2006
				bw->Write(IntegrationMenu4x->Checked);	// New 03-23-2006
				bw->Write(IntegrationMenu8x->Checked);	// New 03-23-2006
				bw->Write(IntegrationMenu16x->Checked);	// New 03-23-2006

 			 // 6. Marker values and display properties.
				for (int i=0; i<5; i++)
				{
					bw->Write(Marker[i]);
					bw->Write(MarkerT[i]);
				}
				bw->Write(ShowMarkerNumbers);
				bw->Write(ShowMarkerParameters);

				bw->Write(TDRStartTime);
				bw->Write(TDRStopTime);
				bw->Write(TDRvf);
				bw->Write(TDRMetricMode);

				// 7. Stored trace results.

				for (int i=0; i<FG->points; i++)
				{
					bw->Write(trace[i]->ReflMI);	// traces on scope display
					bw->Write(trace[i]->ReflMQ);
					bw->Write(trace[i]->ReflPI);
					bw->Write(trace[i]->ReflPQ);
					bw->Write(trace[i]->Vref1);
					bw->Write(trace[i]->TranMI);
					bw->Write(trace[i]->TranMQHi);
					bw->Write(trace[i]->TranPI);
					bw->Write(trace[i]->TranPQ);
					bw->Write(trace[i]->Vref2);
					bw->Write(trace[i]->TranMQLo);
					bw->Write(trace[i]->TranMQMid);
					//bw->Write(trace[i]->TranPILow);
					//bw->Write(trace[i]->TranPQLow);	// New 3-23-2006

					bw->Write(traceSto[i]->ReflMI);	// storage traces
					bw->Write(traceSto[i]->ReflMQ);
					bw->Write(traceSto[i]->ReflPI);
					bw->Write(traceSto[i]->ReflPQ);
					bw->Write(traceSto[i]->Vref1);
					bw->Write(traceSto[i]->TranMI);
					bw->Write(traceSto[i]->TranMQHi);
					bw->Write(traceSto[i]->TranPI);
					bw->Write(traceSto[i]->TranPQ);
					bw->Write(traceSto[i]->Vref2);
					bw->Write(traceSto[i]->TranMQLo);
					bw->Write(traceSto[i]->TranMQMid);
					//bw->Write(traceSto[i]->TranPILow);
					//bw->Write(traceSto[i]->TranPQLow);	// New 3-23-2006
				}

				// 8. Trace colors - New 01-31-2007

				bw->Write(s21MagColor.ToArgb());
				bw->Write(s21PhsColor.ToArgb());
				bw->Write(s11MagColor.ToArgb());
				bw->Write(s11PhsColor.ToArgb());
				bw->Write(s21GroupDelayColor.ToArgb());
				bw->Write(s11VSWRColor.ToArgb());

				// 9. One we forgot - New 02-01-2007

				bw->Write(VSWRdisplay->Checked);

				// 10. A new one - 04-18-2007
				bw->Write(scale1millisec->Checked);

				// 11. TranPhaseLo values - 09-30-2007

				for(int i=0; i<FG->points; i++)
				{
					bw->Write(trace[i]->TranPILow);
					bw->Write(trace[i]->TranPQLow);
					bw->Write(traceSto[i]->TranPILow);
					bw->Write(traceSto[i]->TranPQLow);
				}

				// 12. R+jX display selectors NEW 10-21-2007

				bw->Write(RightScaleOhms->Checked);
				bw->Write(scale10Ohms->Checked);
				bw->Write(scale100Ohms->Checked);
				bw->Write(scale1kOhms->Checked);
				bw->Write(RDisplay->Checked);
				bw->Write(jXDisplay->Checked);
				bw->Write(s11RColor.ToArgb());
				bw->Write(s11jXColor.ToArgb());

				// 8. Background color - New 09-15-2008

				bw->Write(this->BackColor.ToArgb());

				// 9. Display MeasuredMinusStored Magnitude traces

				bw->Write(this->displayMeasuredMinusStored->Checked);

				if (serialPort1->IsOpen)
					bw->Write(serialPort1->PortName);
				else
					bw->Write("");
				bw->Write(selectedAudio);
				bw->Write(refLevel);
				bw->Write(IFREQ);
				bw->Write((int)(VNA->GetMinFreq()/1000));
				bw->Write((int)(VNA->GetMaxFreq()/1000));
			}
			catch(System::IO::IOException^ pe)
			{
				MessageBox::Show(pe->ToString(),"Configuration File Write Error",
					 MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
			__finally
			{
				bw->Close();
				fs->Close();
			}

		};
private: System::Void LoadConfigurationMenu_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 /// Load the instrument state from a configuration file
			 /// 0. File identifier and version
			 /// 1. Whether or not a detector.ica file was loaded.
			 /// 2. Current fixture calibration filename (if loaded).
			 /// 3. Main window size, location.
			 /// 4. Window control values (start & stop frequency, levels, fast/slow button, plot title).
			 /// 5. Menu selections: polar/rectangular view, traces selected, vertical scales, etc
 			 /// 6. Marker values and display properties.
			 /// 7. Stored trace results.

			OpenFileDialog^ infile = gcnew OpenFileDialog();
            infile->Filter = "configuration file (*.cfg)|*.cfg";
			if (infile->ShowDialog() != ::DialogResult::OK)
				return;

			ReadConfiguration(infile);

		};
private: System::Void ReadConfiguration(OpenFileDialog^ infile)
		 {
			String^ id201 = "TAPR VNA Configuration File Version 2.0.1";
			//String^ id104 = "TAPR VNA Configuration File Version 1.0.4";
			String^ idread;
			FileStream^ fs;
			BinaryReader^ br;
			int fileversion = 0;

			try
			{
				fs = gcnew FileStream(infile->FileName, FileMode::Open, FileAccess::Read);
				br = gcnew BinaryReader(fs);
				
			 // 0. File identifier and version
				idread = br->ReadString();
				
				if(String::Compare(id201, idread) == 0)
					fileversion = 201;
//				else if(String::Compare(id104, idread) == 0)
//					fileversion = 104;
				else
				{
					MessageBox::Show("Configuration File Version no longer compatible"
						"\n\rExpecting version 2.0.1","Version Mismatch", MessageBoxButtons::OK,
						MessageBoxIcon::Asterisk);
					return;
				}

			 // 1. Whether or not a detector.ica file was loaded.

//				bool CalSetPresent = br->ReadBoolean();

             // 2. Current fixture calibration filename & applied state.
				calCheckBox->Enabled = br->ReadBoolean();
				calCheckBox->Checked = br->ReadBoolean();
				FixtureCalFileName = br->ReadString();	// Could be null string (no file), process later

             // 3. Main window size, location.
				this->Top = br->ReadInt32();
				this->Left = br->ReadInt32();
				this->Width = br->ReadInt32();
				this->Height = br->ReadInt32();

			 // 4. Window control values (start & stop frequency, levels, fast/slow button, plot title).
				__int64 start  = br->ReadInt64();
				__int64 stop  = br->ReadInt64();
				int ferr  = br->ReadInt32();
				int pts = br->ReadInt32();

				FG = gcnew FrequencyGrid(pts);	// create a new grid of size pts
				FG->SetStartF(start);
				FG->SetStopF(stop);
				FG->ferror = ferr;

				startF->Text = FG->StartF().ToString("N0");
				stopF->Text = FG->StopF().ToString("N0");

				FrequencyDigitIndex = br->ReadInt32();
				StartFrequencyDigitIndicator->Text = FrequencyDigitText[FrequencyDigitIndex];
				StopFrequencyDigitIndicator->Text = FrequencyDigitText[FrequencyDigitIndex];

				txLevel = br->ReadInt32();
				txL->Text = txLevel.ToString();
				refLevel = br->ReadInt32();
				refL->Text = refLevel.ToString();

				startF->Enabled = br->ReadBoolean();
				stopF->Enabled = br->ReadBoolean();
				startFup->Enabled = br->ReadBoolean();
				startFdown->Enabled = br->ReadBoolean();
				stopFup->Enabled = br->ReadBoolean();
				stopFdown->Enabled = br->ReadBoolean();
				menuItem5->Enabled = br->ReadBoolean();

				SweepSpd->Text = br->ReadString();
				RecurrentSweep->Text = br->ReadString();
				RecurrentSweep->Text = "Free Run";			// over-ride the sweep mode (10-14-2007)
				SingleSweep->Enabled = br->ReadBoolean();
				SingleSweep->Enabled = true;				// over-ride the enabled state (10-14-2007)
				
				plotTitle = Convert::ToString(br->ReadString());

			 // 5. Menu selections: polar/rectangular view,	traces selected, vertical scales, etc.
				rectItem->Checked = br->ReadBoolean();
				polarItem->Checked = br->ReadBoolean();
				TDRItem->Checked = br->ReadBoolean();
                s11magItem->Checked = br->ReadBoolean();
				s11phsItem->Checked = br->ReadBoolean();
				s21magItem->Checked = br->ReadBoolean();
				s21phsItem->Checked = br->ReadBoolean();
				s21groupdelayItem->Checked = br->ReadBoolean();
				EtTrace->Checked = br->ReadBoolean();
				EsTrace->Checked = br->ReadBoolean();
				EdTrace->Checked = br->ReadBoolean();
				calSshort->Checked = br->ReadBoolean();
				calSopen->Checked = br->ReadBoolean();
				calSterm->Checked = br->ReadBoolean();

//				if (fileversion == 105)
					Scale05dB->Checked = br->ReadBoolean();
//				else Scale05dB->Checked = false;
				
				Scale1dB->Checked = br->ReadBoolean();
				Scale2dB->Checked = br->ReadBoolean();
				Scale5dB->Checked = br->ReadBoolean();
				Scale10dB->Checked = br->ReadBoolean();

//				if (fileversion == 105)
					RectVertScaledB = br->ReadSingle();
//				else RectVertScaledB = (float)br->ReadInt32();
					

				ScaleSWR1->Checked = br->ReadBoolean();
				ScaleSWR2->Checked = br->ReadBoolean();
				ScaleSWR5->Checked = br->ReadBoolean();
				ScaleSWR10->Checked = br->ReadBoolean();
				RectSWRScale = br->ReadInt32();

				scale100psec->Checked = br->ReadBoolean();
				scalenanoSec->Checked = br->ReadBoolean();
				scale10nsec->Checked = br->ReadBoolean();
				scale100nsec->Checked = br->ReadBoolean();
				scalemicroSec->Checked = br->ReadBoolean();
				scale10microsec->Checked = br->ReadBoolean();
				scale100microsec->Checked = br->ReadBoolean();	// NEW 12-28-2005
				scalefactorGD = br->ReadInt32();

				PolarZoom10Menu->Checked = br->ReadBoolean();
				PolarZoom15Menu->Checked = br->ReadBoolean();
				PolarZoom20Menu->Checked = br->ReadBoolean();
				PolarZoom25Menu->Checked = br->ReadBoolean();
				PolarZoom = br->ReadSingle();

				grid101menu->Checked = br->ReadBoolean();
				grid201menu->Checked = br->ReadBoolean();
				grid401menu->Checked = br->ReadBoolean();
				grid1024menu->Checked = br->ReadBoolean();

				GDAperture1->Checked = br->ReadBoolean();
				GDAperture4->Checked = br->ReadBoolean();
				GDAperture16->Checked = br->ReadBoolean();
				GDAperture64->Checked = br->ReadBoolean();

				RightScalePhase->Checked = br->ReadBoolean();
				RightScaleSWR->Checked = br->ReadBoolean();
				RightScaleDelay->Checked = br->ReadBoolean();

				DisplayMenuItem->Checked = br->ReadBoolean();

				IntegrationMenu1x->Checked = br->ReadBoolean();	// New 03-23-2006
				IntegrationMenu2x->Checked = br->ReadBoolean();	// New 03-23-2006
				IntegrationMenu4x->Checked = br->ReadBoolean();	// New 03-23-2006
				IntegrationMenu8x->Checked = br->ReadBoolean();	// New 03-23-2006
				IntegrationMenu16x->Checked = br->ReadBoolean();	// New 03-23-2006


 			 // 6. Marker values and display properties.
				for (int i=0; i<5; i++)
				{
					Marker[i] = br->ReadUInt64();
					MarkerT[i] = br->ReadSingle();
				}
				ShowMarkerNumbers = br->ReadBoolean();
				ShowMarkerParameters = br->ReadBoolean();

				TDRStartTime = br->ReadSingle();
				TDRStopTime = br->ReadSingle();
				TDRvf = br->ReadSingle();
				TDRMetricMode = br->ReadBoolean();

			 // 7. Stored trace results.

				for (int i=0; i<FG->points; i++)
				{
					trace[i]->ReflMI = br->ReadInt16();	// traces on scope display
					trace[i]->ReflMQ = br->ReadInt16();
					trace[i]->ReflPI = br->ReadInt16();
					trace[i]->ReflPQ = br->ReadInt16();
					trace[i]->Vref1 = br->ReadInt16();
					trace[i]->TranMI = br->ReadInt16();
					trace[i]->TranMQHi = br->ReadInt16();
					trace[i]->TranPI = br->ReadInt16();
					trace[i]->TranPQ = br->ReadInt16();
					trace[i]->Vref2 = br->ReadInt16();
					trace[i]->TranMQLo = br->ReadInt16();
					trace[i]->TranMQMid = br->ReadInt16();	// New 3-23-2006
					trace[i]->TranPILow = 0;		// New 10-02-2007. Default to zero, read in later if available
					trace[i]->TranPQLow = 0;	

					traceSto[i]->ReflMI = br->ReadInt16();	// storage traces 
					traceSto[i]->ReflMQ = br->ReadInt16();
					traceSto[i]->ReflPI = br->ReadInt16();
					traceSto[i]->ReflPQ = br->ReadInt16();
					traceSto[i]->Vref1 = br->ReadInt16();
					traceSto[i]->TranMI = br->ReadInt16();
					traceSto[i]->TranMQHi = br->ReadInt16();
					traceSto[i]->TranPI = br->ReadInt16();
					traceSto[i]->TranPQ = br->ReadInt16();
					traceSto[i]->Vref2 = br->ReadInt16();
					traceSto[i]->TranMQLo = br->ReadInt16();
					traceSto[i]->TranMQMid = br->ReadInt16();	// New 3-23-2006

					traceSto[i]->TranPILow = 0;	// New 10-02-2007. Default to zero, read in later if available
					traceSto[i]->TranPQLow = 0;	

				}

				// New 05-11-2008. Default to off, 1000 ohms. Read in later if they are available.
				RightScaleOhms->Checked = false;
				scale10Ohms->Checked = false;
				scale100Ohms->Checked = false;
				scale1kOhms->Checked = true;
				RDisplay->Checked = false;
				jXDisplay->Checked = false;

			}
			catch(System::IO::FileNotFoundException^ /*pe*/)		// Not an error if __last.cfg does not exist
			{
				if (String::Compare(infile->FileName, String::Concat(CurrentUserDataPath, "\\__last.cfg")) != 0)
				{
					MessageBox::Show(String::Concat("Can't find configuration file:  ", infile->FileName),
						"VNA Configuration File Read Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				}
				// Ignore error if __last.cfg was the requested file, and it's not found
				if(br)
                    br->Close();
				if(fs)
                    fs->Close();
				return;
			}
			catch(System::IO::IOException^ pe)
			{
				MessageBox::Show(pe->ToString(),"VNA Configuration File Read Error",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
				if(br)
                    br->Close();
				if(fs)
                    fs->Close();
				return;
			}


			try
			{
			// Try to read in the trace colors. If we're at the end-of-file, just
			//   ignore the file error and we will use the default color values.
			// This allows us to accept older configuration files that don't have
			//   stored trace colors.

				// 8. Trace Colors - New 01-31-2007

				s21MagColor = Color::FromArgb(br->ReadInt32());
				s21PhsColor = Color::FromArgb(br->ReadInt32());
				s11MagColor = Color::FromArgb(br->ReadInt32());
				s11PhsColor = Color::FromArgb(br->ReadInt32());
				s21GroupDelayColor = Color::FromArgb(br->ReadInt32());
				s11VSWRColor = Color::FromArgb(br->ReadInt32());

				// 9. One we forgot - New 02-01-2007

				VSWRdisplay->Checked = br->ReadBoolean();

				// 10. A new one - 04-18-2007
				scale1millisec->Checked = br->ReadBoolean();

				// 11. TranPhaseLo values - 09-30-2007
				// If these are not available, they will be defaulted to zero (above),
				// which effectively disables them.

				for(int i=0; i<FG->points; i++)
				{
					trace[i]->TranPILow = br->ReadInt16();
					trace[i]->TranPQLow = br->ReadInt16();
					traceSto[i]->TranPILow = br->ReadInt16();
					traceSto[i]->TranPQLow = br->ReadInt16();
				}

				// 12. R+jX display selectors NEW for file version 105.
				// They've been initialized to false, 1000 ohms previously, in case we're reading pre-105 file.

				RightScaleOhms->Checked = br->ReadBoolean();
				scale10Ohms->Checked = br->ReadBoolean();
				scale100Ohms->Checked = br->ReadBoolean();
				scale1kOhms->Checked = br->ReadBoolean();
				RDisplay->Checked = br->ReadBoolean();
				jXDisplay->Checked = br->ReadBoolean();
				s11RColor = Color::FromArgb(br->ReadInt32());
				s11jXColor = Color::FromArgb(br->ReadInt32());

				// 13. Background color - New 09-15-2008
				// Initialized by the property on the Form1 designer

				this->BackColor = Color::FromArgb(br->ReadInt32());

				// 14. Display MeasuredMinusStored Magnitude traces - New 07-02-2011

				displayMeasuredMinusStored->Checked = br->ReadBoolean();

				if (serialPort1->IsOpen) serialPort1->Close();
				try 
				{
					serialPort1->PortName = br->ReadString();
				    serialPort1->BaudRate = 115200;

					selectedAudio = (unsigned int)(br->ReadInt32());
					refLevel = br->ReadInt32();
					IFREQ = br->ReadInt32();
					VNA->SetMinFreq(br->ReadInt32() * (__int64) 1000);
					VNA->SetMaxFreq(br->ReadInt32() * (__int64) 1000);


					OpenAudio();

					serialPort1->Open();
					 System::Threading::Thread::Sleep(1000);
					 serialPort1->ReadExisting();
					 serialPort1->WriteLine("3");
					 System::Threading::Thread::Sleep(1000);
					String ^reply = serialPort1->ReadExisting();
					if (!reply->StartsWith("TAPR VNA v4")) {
						MessageBox::Show("No VNA connected to stored port", "Error",
						 MessageBoxButtons::OK, MessageBoxIcon::Error);
					    throw; 
					}
					VNA->SetFreq(1000000L, true);
					System::Threading::Thread::Sleep(500);
					if (actualMeasurement.reference < -35.0 || actualMeasurement.reference > 10.0) {
						MessageBox::Show("Measurement signal level out of range", "Error",
						 MessageBoxButtons::OK, MessageBoxIcon::Error);
					    throw; 
					}
					//if (serialPort1->IsOpen) serialPort1->Close();
				}
				catch( Exception^ /* e */ )	// Don't bother warning the user ...
				{											// They probably don't care anyway
//					MessageBox::Show("Can not open stored serial port.", serialPort1->PortName,
//									 MessageBoxButtons::OK, MessageBoxIcon::Information);
					SerialPortBox = gcnew SerialPort (serialPort1, VNA);
					SerialPortBox->ShowDialog();

				}
			}
			catch(System::IO::IOException^ /* pe */)	// Don't bother warning the user ...
			{											// They probably don't care anyway
				// MessageBox::Show("No custom trace colors found, using default trace colors.", "",
				//	MessageBoxButtons::OK, MessageBoxIcon::Information);
			}

			// close the file
			if(br)
                br->Close();
			if(fs)
                fs->Close();


//			Assume that the detector.ica that was loaded at startup, don't reload it
//				if(CalSetPresent)								// load the detector.ica file
//					CalData = new InstrumentCalDataSet(AllUsersDataPath);		// Allocate calibration data set

			if(calCheckBox->Enabled)						// load Fixture cal file
			{
				//if FixtureCalFileName points to a valid filename string
				if((FixtureCalFileName != nullptr) && (String::Compare(FixtureCalFileName, "") != 0))
					try
					{
						OpenFileDialog^ infile = gcnew OpenFileDialog();
						infile->FileName = FixtureCalFileName;
						if (LoadCalDataSet(infile, CalData))
							calCheckBox->Enabled = true;
						else	// Load of DataSet failed
							calCheckBox->Enabled = false;
					}
					catch(System::ArgumentException^ a)
					{
						MessageBox::Show(String::Concat("Can't find path to a previously used calibration file.\n\n\r",
							infile->FileName, "\n\n\rYou will need to re-select the Fixture Calibration file from the\n\r"
							"Calibration->Load Fixture Calibration File menu.\n\n\r", a->ToString()),"Can't find Calibration File Path",
							MessageBoxButtons::OK, MessageBoxIcon::Warning);

						calCheckBox->Enabled = false;
					}
				else										// No string pointed at by filename
					calCheckBox->Enabled = false;
			}

			Refresh();
		 };

private: System::Single GetVerticalScaleFactor(System::Void)
		 {
			if (Scale10dB->Checked)	return 10.0f;
			else if (Scale5dB->Checked)	return 5.0f;
			else if (Scale2dB->Checked)	return 2.0f;
			else if (Scale1dB->Checked)	return 1.0f;
			else if (Scale05dB->Checked) return 0.5f;
			
			//throw new System::Exception("Invalid value of VerticalScaleFactor. No Scale_dB checked");
			return 1.0f;	// default in case no Scale_dB is checked in the menu.
		 };
		 /// Paint event handler
private: System::Void Form_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e)
		 {
#if 1
			 Graphics^ pg = e->Graphics;			// copy from the PaintEvent so we get proper clipping bounds
			Form_Render(pg, ClientSize);
			delete(pg);
#else
		 // Double-buffered version of paint
			Graphics^ pg = e->Graphics;			// copy from the PaintEvent so we get proper clipping bounds

			Bitmap ^ localBitmap = gcnew Bitmap(ClientRectangle.Width, ClientRectangle.Height);
			Graphics^ bitmapGraphics = Graphics::FromImage(localBitmap);
			bitmapGraphics->Clip = pg->Clip;

			Form_Render(bitmapGraphics, ClientSize);	// redraw to our local bitmap
			pg->DrawImage(localBitmap, 0, 0);			// draw our local bitmap to the screen

//			bitmapGraphics->Dispose();
//			localBitmap->Dispose();
//			pg->Dispose();
			//delete(bitmapGraphics);
			//delete(localBitmap);
			//delete(pg);

#endif
		 }

	/// Resize event handler
private: System::Void Form_Resize(System::Object^  sender, System::EventArgs^  e)
		 {
			Graphics^ pg = CreateGraphics();	// don't set any clipping region on a resize event
			Form_Render(pg, ClientSize);		// EventArgs is the empty argument set
			delete(pg);
		 }

private: System::Void Form_Close(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e)
		 {
			 exitItem_Click(sender, e);		// Save configuration, close all threads, terminate
		 }
		 
	/// Set TDR display Start and Stop times
private: System::Void TDRSetupmenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 TDRSetupDialog^ td = gcnew TDRSetupDialog();
			 td->set_Start(TDRStartTime);
			 td->set_Stop(TDRStopTime);
			 td->set_VF(TDRvf);
			 td->set_MetricMode(TDRMetricMode);

			 if (td->ShowDialog() == ::DialogResult::OK)
			 {
				 float str, stp, velfact;
				 str = td->get_Start();
				 stp = td->get_Stop();
				 velfact = td->get_VF();
				 if ((stp > str) && (str >= 0.0) && (stp <= 4350.0))	// check time value validity
				 {
                    TDRStartTime = str;
					TDRStopTime = stp;
                 }
				 else
					MessageBox::Show("Stop time must be greater than Start time.\n\r"
						"Stop time must be greater than or equal to 0.0\n\r"
						"Stop time must be less than or equal to 4350.0",
						"Out of Range");
				 if ((velfact > 0.39) && (velfact < 1.0))
					 TDRvf = velfact;
				 else
					MessageBox::Show("Velocity factor must have a value between\n\r"
						"0.4 and 0.99.\n\r", "Out of Range");

				 TDRMetricMode = td->get_MetricMode();

				 Refresh();		// TDR setup changes the screen display
			 }
			 else
				Update();		// No change to screen - just clean up under the TDR window
		 }


private: System::Void IntegrationMenu1x_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 IntegrationMenu1x->Checked = true;
			 IntegrationMenu2x->Checked = false;
			 IntegrationMenu4x->Checked = false;
			 IntegrationMenu8x->Checked = false;
			 IntegrationMenu16x->Checked = false;
			 PreCharge = true;							// Integrator PreCharge when Selected
		 }

private: System::Void IntegrationMenu2x_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 IntegrationMenu1x->Checked = false;
			 IntegrationMenu2x->Checked = true;
			 IntegrationMenu4x->Checked = false;
			 IntegrationMenu8x->Checked = false;
			 IntegrationMenu16x->Checked = false;
			 PreCharge = true;
		 }

private: System::Void IntegrationMenu4x_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 IntegrationMenu1x->Checked = false;
			 IntegrationMenu2x->Checked = false;
			 IntegrationMenu4x->Checked = true;
			 IntegrationMenu8x->Checked = false;
			 IntegrationMenu16x->Checked = false;
			 PreCharge = true;
		 }

private: System::Void IntegrationMenu8x_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 IntegrationMenu1x->Checked = false;
			 IntegrationMenu2x->Checked = false;
			 IntegrationMenu4x->Checked = false;
			 IntegrationMenu8x->Checked = true;
			 IntegrationMenu16x->Checked = false;
			 PreCharge = true;
		 }

private: System::Void IntegrationMenu16x_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 IntegrationMenu1x->Checked = false;
			 IntegrationMenu2x->Checked = false;
			 IntegrationMenu4x->Checked = false;
			 IntegrationMenu8x->Checked = false;
			 IntegrationMenu16x->Checked = true;
			 PreCharge = true;
		 }

private: System::Void sWRColorToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		 {
//			 colorDialog1->AllowFullOpen = false;	// prohibits custom color selection
			 colorDialog1->Color = s11VSWRColor;
			 if (colorDialog1->ShowDialog() == ::DialogResult::OK)
			 {
				 s11VSWRColor = colorDialog1->Color;
				 Refresh();
			 }
		 }
private: System::Void s11MagnitudeColorToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 colorDialog1->Color = s11MagColor;
			 if (colorDialog1->ShowDialog() == ::DialogResult::OK)
			 {
				 s11MagColor = colorDialog1->Color;
				 Refresh();
			 }
		 }
private: System::Void s11PhaseColorToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 colorDialog1->Color = s11PhsColor;
			 if (colorDialog1->ShowDialog() == ::DialogResult::OK)
			 {
				 s11PhsColor = colorDialog1->Color;
				 Refresh();
			 }
		 }
private: System::Void s21MagnitudeColorToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 colorDialog1->Color = s21MagColor;
			 if (colorDialog1->ShowDialog() == ::DialogResult::OK)
			 {
				 s21MagColor = colorDialog1->Color;
				 Refresh();
			 }
		 }
private: System::Void s21PhaseColorToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 colorDialog1->Color = s21PhsColor;
			 if (colorDialog1->ShowDialog() == ::DialogResult::OK)
			 {
				 s21PhsColor = colorDialog1->Color;
				 Refresh();
			 }
		 }
private: System::Void s21GroupDelayColorToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 colorDialog1->Color = s21GroupDelayColor;
			 if (colorDialog1->ShowDialog() == ::DialogResult::OK)
			 {
				 s21GroupDelayColor = colorDialog1->Color;
				 Refresh();
			 }
		 }
private: System::Void RefExtnCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
		 {
 			Refresh();	// Force Screen redraw.  The checkbox is not accessible at this time.
		 }
private: System::Void RightScaleOhms_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 RightScalePhase->Checked = false;
			 RightScaleSWR->Checked = false;
			 RightScaleDelay->Checked = false;
			 RightScaleOhms->Checked = true;
			 Refresh();
		 }
private: System::Void scale10Ohms_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 scale10Ohms->Checked = true;
			 scale100Ohms->Checked = false;
			 scale1kOhms->Checked = false;
			 Refresh();
		 }
private: System::Void scale100Ohms_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 scale10Ohms->Checked = false;
			 scale100Ohms->Checked = true;
			 scale1kOhms->Checked = false;
			 Refresh();
		 }
private: System::Void scale1kOhms_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 scale10Ohms->Checked = false;
			 scale100Ohms->Checked = false;
			 scale1kOhms->Checked = true;
			 Refresh();
		 }
private: System::Void RDisplay_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 if(RDisplay->Checked)
				 RDisplay->Checked = false;
			 else
				 RDisplay->Checked = true;
			 Refresh();
		 }
private: System::Void jXDisplay_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 if(jXDisplay->Checked)
				 jXDisplay->Checked = false;
			 else
				 jXDisplay->Checked = true;
			 Refresh();
		 }
private: System::Void s11AsRColorToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 //			 colorDialog1->AllowFullOpen = false;	// prohibits custom color selection
			 colorDialog1->Color = s11RColor;
			 if (colorDialog1->ShowDialog() == ::DialogResult::OK)
			 {
				 s11RColor = colorDialog1->Color;
				 Refresh();
			 }

		 }
private: System::Void s11AsJXColorToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 //			 colorDialog1->AllowFullOpen = false;	// prohibits custom color selection
			 colorDialog1->Color = s11jXColor;
			 if (colorDialog1->ShowDialog() == ::DialogResult::OK)
			 {
				 s11jXColor = colorDialog1->Color;
				 Refresh();
			 }

		 }
private: System::Void backgroundColorToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 //			 colorDialog1->AllowFullOpen = false;	// prohibits custom color selection
			 colorDialog1->Color = this->BackColor;
			 if (colorDialog1->ShowDialog() == ::DialogResult::OK)
			 {
				 this->BackColor = colorDialog1->Color;
				 Refresh();
			 }
		 }
private: System::Void ToggleMeasuredMinusStored(System::Object^  sender, System::EventArgs^  e)
		 {
			 // toggle the checked property
			 displayMeasuredMinusStored->Checked = !displayMeasuredMinusStored->Checked;
			 Refresh();
		 }
private: System::Void DisplayMeasured_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 // toggle the checked property
			 DisplayMeasured->Checked = !DisplayMeasured->Checked;
			 Refresh();
		 }

private: System::Void mockupDeviceToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

//			 Assembly^ _assembly;
//			 _assembly = Assembly::GetExecutingAssembly();
			 //MessageBox::Show(_assembly->ToString(),"TAPR VNA Host Program");

			 MockupBox = gcnew Mockup (VNA);
			 MockupBox->Show();

		 }
private: System::Void settingsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

//			 Assembly^ _assembly;
//			 _assembly = Assembly::GetExecutingAssembly();
			 //MessageBox::Show(_assembly->ToString(),"TAPR VNA Host Program");

		 }
private: System::Void audioDevicesToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 //AudioDeviceBox = gcnew AudioDevice();
			 //AudioDeviceBox->Show();

		 }
private: System::Void serialPortToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 SerialPortBox = gcnew SerialPort (serialPort1, VNA);
			 SerialPortBox->ShowDialog();
		 }
private: System::Void label10_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void textBox1_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void label11_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void label12_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void textBox2_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void textBox3_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void signalGeneratorToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 SignalGeneratorBox = gcnew SignalGenerator (VNA);
			 SignalGeneratorBox->ShowDialog();
		 }
private: System::Void rawDetectorDataToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if (rawDetectorDataToolStripMenuItem->Checked == false)
				 rawDetectorDataToolStripMenuItem->Checked = true;
			 else
				 rawDetectorDataToolStripMenuItem->Checked = false;
			 
			 Refresh();
		 }

		 private: System::Void audioDistanceToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if (audioDistanceToolStripMenuItem->Checked == false)
				 audioDistanceToolStripMenuItem->Checked = true;
			 else
				 audioDistanceToolStripMenuItem->Checked = false;
			 
			 Refresh();

		  }

private: System::Void Spectrum_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			Refresh();	// Force Screen redraw
		 }

private: System::Void dumpMeasurementsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 DumpMeasurement();
		 }

#define MOUSE_WHEEL_STEP 30

private: System::Void startF_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  pe) {
	int	Xpos, Ypos;		// where the mouse points relative to our display surface
	Xpos = pe->X;// - startF->Left;
	Ypos = pe->Y;// - startF->Top;
											// no status if mouse is off the Scope Display surface
	//if ((Xpos >= startF->Width) || (Xpos < 0) || (Ypos > startF->Height) || (Ypos < 0))
	//	return;

	int digit = 12 - (Xpos + 13) / 14; 
	if (digit >= 12)
		digit--;
	if (digit >= 8)
		digit--;
	if (digit >= 4)
		digit--;
	if (digit >= 7)
		digit = 7;
	if (pe->Delta != 0) {
		FG->SetStartF(FG->StartF() + (__int64)Math::Pow(10.0, digit) * (__int64)(pe->Delta / MOUSE_WHEEL_STEP));
		if (FG->StartF() < VNA->GetMinFreq())  // max allowed frequency
			FG->SetStartF(VNA->GetMinFreq());
		startF->Text = FG->StartF().ToString("N0");
		return;
	}
	if(pe->Button == ::MouseButtons::None)
		return;
	if(pe->Button == ::MouseButtons::Left)
    {
		FG->SetStartF(FG->StartF() + (__int64)Math::Pow(10.0, digit));
		startF->Text = FG->StartF().ToString("N0");
    }
	else if(pe->Button == ::MouseButtons::Right)
    {
		FG->SetStartF(FG->StartF() - (__int64)Math::Pow(10.0, digit));
		startF->Text = FG->StartF().ToString("N0");
    }
  }
private: System::Void stopF_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  pe) {
	int	Xpos, Ypos;		// where the mouse points relative to our display surface
	Xpos = pe->X;// - startF->Left;
	Ypos = pe->Y;// - startF->Top;
											// no status if mouse is off the Scope Display surface
	//if ((Xpos >= startF->Width) || (Xpos < 0) || (Ypos > startF->Height) || (Ypos < 0))
	//	return;

	int digit = 12 - (Xpos + 13) / 14; 
	if (digit >= 12)
		digit--;
	if (digit >= 8)
		digit--;
	if (digit >= 4)
		digit--;
	if (digit >= 7)
		digit = 7;
	if (pe->Delta != 0) {
		
		FG->SetStopF(FG->StopF() + (__int64)Math::Pow(10.0, digit) * (__int64)(pe->Delta / MOUSE_WHEEL_STEP));
		if (FG->StopF() < VNA->GetMinFreq())  // max allowed frequency
			FG->SetStopF(VNA->GetMinFreq());
		stopF->Text = FG->StopF().ToString("N0");
		return;
	}
	if(pe->Button == ::MouseButtons::None)
		return;
	if(pe->Button == ::MouseButtons::Left)
    {
		FG->SetStopF(FG->StopF() + (__int64)Math::Pow(10.0, digit));
		stopF->Text = FG->StopF().ToString("N0");
    }
	else if(pe->Button == ::MouseButtons::Right)
    {
		FG->SetStopF(FG->StopF() - (__int64)Math::Pow(10.0, digit));
		stopF->Text = FG->StopF().ToString("N0");
    }
  }
}
;};

