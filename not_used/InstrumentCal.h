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
//	  Modified: 05-10-2005	TCM Add routines for Internal Crystal Calibration
//	  Modified: 08-05-2006  TCM Add Error-Checking in case the cal results are poor
//


#pragma once

#include "DisplayRoutines.h"
#include "USB_EZ_interface.h"
#include "Constants.h"
#include <math.h>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::IO;

#define		NUMCALSTEPS		8		// 8 mandatory calibration steps

namespace VNAR3
{

	/// <summary> 
	/// Summary for InstrumentCal
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>

	/// Instrument Detector Calibration
	public ref class InstrumentCal : public System::Windows::Forms::Form
	{
	public: 
		InstrumentCal(InstrumentCalDataSet^ CalDS, VNADevice^ VNADev, FrequencyGrid^ fg, String^ StartDir)
		{
			InitializeComponent();
			VNA = VNADev;				///< VNA hardware device
			Cal = CalDS;				///< Calibration Dataset
			AllUsersDir = StartDir;		///< Application data directory common to all users
			FG = fg;					///< Frequency Grid from parent
			Iphase = gcnew array<UInt16>(1024);	///< I-Phase measurements
			Qphase = gcnew array<UInt16>(1024);	///< Q-Phase Measurements
			IphaseLo = gcnew array<UInt16>(1024);	///< I-phase low ref measurements
			ReflMagRegression = gcnew array<UInt16,2>(21, 60); ///< Shorted Reflection amplitude measurements
			ReflMagPhDir = gcnew array<UInt16,2>(21, 3);	///< Directivity Reflection raw ADC counts
			ReflMagPhOpen = gcnew array<UInt16,2>(21, 3);	///< Open Reflection raw ADC counts
			ReflMagPhShort = gcnew array<UInt16,2>(21, 3);	///< Shorted Reflection raw ADC counts
			ReflDetailMagPhShort = gcnew array<UInt16,2>(1024, 3);  ///< Shorted Detailed Reflection raw ADC counts
			ReflDetailMagPhOpen = gcnew array<UInt16,2>(1024, 3);  ///< Open Detailed Reflection raw ADC counts
			ReflLongDetailMagPhOpen = gcnew array<UInt16,2>(1024, 3);  ///< Terminated Detailed Reflection raw ADC counts
			ReflLongDetailMagPhShort = gcnew array<UInt16,2>(1024, 3);  ///< Terminated Detailed Reflection raw ADC counts
			//ReflOpen100MHz = new int __gc[3];	///< ConnectorOpen at 100 MHz raw ADC counts
			TranMag = gcnew array<UInt16,2>(21, 60);  ///< Transmission Amplitude raw ADC counts 
			BufferI = gcnew array<UInt16>(7);		///< holds raw I reading set
			BufferQ = gcnew array<UInt16>(7);		///< holds raw Q reading set
			BufferILo = gcnew array<UInt16>(7);	///< holds raw I-Low readging set
			BufferM = gcnew array<UInt16>(7);		///< holds raw magnitude reading set Lo
			BufferN = gcnew array<UInt16>(7);		///< holds raw magnitude reading set Hi
			BufferP = gcnew array<UInt16>(7);		///< holds raw magnitude reading set Mid
			CalStepPass = gcnew array<Boolean>(NUMCALSTEPS);

			for(int k=0; k<NUMCALSTEPS; k++) 
				CalStepPass[k] = false;		///< pass/fail for each calibration step
		}
        
	protected: 
		~InstrumentCal()
		{
			if(components)
				delete(components);
		}

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  OKbutton;
	private: System::Windows::Forms::Button^  Cancelbutton;

	private: System::Windows::Forms::Label^  label2;

	private: System::Windows::Forms::Button^  RxPhaseButton;

	private: System::Windows::Forms::Button^  TxPhaseButton;

	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  RxPhaseStat;

	private: System::Windows::Forms::Label^  TxPhaseStat;

	private: System::Windows::Forms::ProgressBar^  progressBar1;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Button^  TxLowAmpButton;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  TxLowAmpStat;
	private: System::Windows::Forms::Button^  FrequencyCalButton;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  FrequencyCalStat;
	private: System::Windows::Forms::TextBox^  MeasFreqTextBox;
	private: System::Windows::Forms::Label^  label9;

	private: System::Windows::Forms::Button^  FreqEnterButton;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::Label^  label11;
	private: System::Windows::Forms::Label^  label12;

	private: System::Windows::Forms::Button^  DirectivityCalButton;
	private: System::Windows::Forms::Label^  DirectivityCalStat;
	private: System::Windows::Forms::Label^  label14;

	private: System::Windows::Forms::Button^  RxAmpButtonOpen;
	private: System::Windows::Forms::Label^  RxAmpStatShort;
	private: System::Windows::Forms::Label^  label15;
	private: System::Windows::Forms::Label^  RxAmpStatOpen;
	private: System::Windows::Forms::Button^  RxAmpButtonShort;
	private: System::Windows::Forms::Label^  ICO1Label;
	private: System::Windows::Forms::Label^  ICO2Label;
	private: System::Windows::Forms::GroupBox^  Optional;

	private: System::Windows::Forms::Button^  TxLongCableOpenButton;
	private: System::Windows::Forms::Label^  label13;
	private: System::Windows::Forms::Label^  label16;
	private: System::Windows::Forms::Label^  TxLongCableShortStat;
	private: System::Windows::Forms::Label^  TxLongCableOpenStat;
	private: System::Windows::Forms::Button^  TxLongCableShortButton;
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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(InstrumentCal::typeid));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->OKbutton = (gcnew System::Windows::Forms::Button());
			this->Cancelbutton = (gcnew System::Windows::Forms::Button());
			this->RxPhaseButton = (gcnew System::Windows::Forms::Button());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->TxPhaseButton = (gcnew System::Windows::Forms::Button());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->RxPhaseStat = (gcnew System::Windows::Forms::Label());
			this->RxAmpStatShort = (gcnew System::Windows::Forms::Label());
			this->TxPhaseStat = (gcnew System::Windows::Forms::Label());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->TxLowAmpButton = (gcnew System::Windows::Forms::Button());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->TxLowAmpStat = (gcnew System::Windows::Forms::Label());
			this->FrequencyCalButton = (gcnew System::Windows::Forms::Button());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->FrequencyCalStat = (gcnew System::Windows::Forms::Label());
			this->MeasFreqTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->FreqEnterButton = (gcnew System::Windows::Forms::Button());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->DirectivityCalButton = (gcnew System::Windows::Forms::Button());
			this->DirectivityCalStat = (gcnew System::Windows::Forms::Label());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->RxAmpButtonOpen = (gcnew System::Windows::Forms::Button());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->RxAmpStatOpen = (gcnew System::Windows::Forms::Label());
			this->RxAmpButtonShort = (gcnew System::Windows::Forms::Button());
			this->ICO1Label = (gcnew System::Windows::Forms::Label());
			this->ICO2Label = (gcnew System::Windows::Forms::Label());
			this->Optional = (gcnew System::Windows::Forms::GroupBox());
			this->TxLongCableOpenButton = (gcnew System::Windows::Forms::Button());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->TxLongCableShortStat = (gcnew System::Windows::Forms::Label());
			this->TxLongCableOpenStat = (gcnew System::Windows::Forms::Label());
			this->TxLongCableShortButton = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Font = (gcnew System::Drawing::Font(L"Verdana", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(144, 6);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(426, 29);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Detector Calibration";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// OKbutton
			// 
			this->OKbutton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->OKbutton->Location = System::Drawing::Point(456, 578);
			this->OKbutton->Name = L"OKbutton";
			this->OKbutton->Size = System::Drawing::Size(101, 30);
			this->OKbutton->TabIndex = 1;
			this->OKbutton->Text = L"Save";
			this->OKbutton->Click += gcnew System::EventHandler(this, &InstrumentCal::OKbutton_Click);
			// 
			// Cancelbutton
			// 
			this->Cancelbutton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->Cancelbutton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->Cancelbutton->Location = System::Drawing::Point(584, 578);
			this->Cancelbutton->Name = L"Cancelbutton";
			this->Cancelbutton->Size = System::Drawing::Size(105, 28);
			this->Cancelbutton->TabIndex = 2;
			this->Cancelbutton->Text = L"Cancel";
			this->Cancelbutton->Click += gcnew System::EventHandler(this, &InstrumentCal::Cancelbutton_Click);
			// 
			// RxPhaseButton
			// 
			this->RxPhaseButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->RxPhaseButton->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->RxPhaseButton->Location = System::Drawing::Point(520, 64);
			this->RxPhaseButton->Name = L"RxPhaseButton";
			this->RxPhaseButton->Size = System::Drawing::Size(176, 48);
			this->RxPhaseButton->TabIndex = 3;
			this->RxPhaseButton->Text = L"Refl Phase Detector Calibration";
			this->RxPhaseButton->Click += gcnew System::EventHandler(this, &InstrumentCal::RxPhaseButton_Click);
			// 
			// label2
			// 
			this->label2->BackColor = System::Drawing::Color::Transparent;
			this->label2->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(96, 66);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(414, 48);
			this->label2->TabIndex = 5;
			this->label2->Text = L"Connect one meter long 50-ohm cable, shorted at far end, to Tx connector.";
			// 
			// TxPhaseButton
			// 
			this->TxPhaseButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->TxPhaseButton->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->TxPhaseButton->Location = System::Drawing::Point(520, 208);
			this->TxPhaseButton->Name = L"TxPhaseButton";
			this->TxPhaseButton->Size = System::Drawing::Size(176, 48);
			this->TxPhaseButton->TabIndex = 6;
			this->TxPhaseButton->Text = L"Trans Phase Detector Calibration";
			this->TxPhaseButton->Click += gcnew System::EventHandler(this, &InstrumentCal::TxPhaseButton_Click);
			// 
			// label4
			// 
			this->label4->BackColor = System::Drawing::Color::Transparent;
			this->label4->Location = System::Drawing::Point(152, 32);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(415, 28);
			this->label4->TabIndex = 9;
			this->label4->Text = L"(This calibration normally needs to be run only one time.)";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label5
			// 
			this->label5->BackColor = System::Drawing::Color::Transparent;
			this->label5->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label5->Location = System::Drawing::Point(16, 38);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(67, 20);
			this->label5->TabIndex = 10;
			this->label5->Text = L"Status";
			// 
			// RxPhaseStat
			// 
			this->RxPhaseStat->BackColor = System::Drawing::Color::Transparent;
			this->RxPhaseStat->Location = System::Drawing::Point(17, 56);
			this->RxPhaseStat->Name = L"RxPhaseStat";
			this->RxPhaseStat->Size = System::Drawing::Size(48, 48);
			this->RxPhaseStat->TabIndex = 11;
			this->RxPhaseStat->Visible = false;
			// 
			// RxAmpStatShort
			// 
			this->RxAmpStatShort->BackColor = System::Drawing::Color::Transparent;
			this->RxAmpStatShort->Location = System::Drawing::Point(17, 104);
			this->RxAmpStatShort->Name = L"RxAmpStatShort";
			this->RxAmpStatShort->Size = System::Drawing::Size(48, 48);
			this->RxAmpStatShort->TabIndex = 12;
			this->RxAmpStatShort->Visible = false;
			// 
			// TxPhaseStat
			// 
			this->TxPhaseStat->BackColor = System::Drawing::Color::Transparent;
			this->TxPhaseStat->Location = System::Drawing::Point(17, 200);
			this->TxPhaseStat->Name = L"TxPhaseStat";
			this->TxPhaseStat->Size = System::Drawing::Size(48, 48);
			this->TxPhaseStat->TabIndex = 13;
			this->TxPhaseStat->Visible = false;
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(32, 610);
			this->progressBar1->Maximum = 1024;
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(664, 13);
			this->progressBar1->TabIndex = 15;
			// 
			// label6
			// 
			this->label6->BackColor = System::Drawing::Color::Transparent;
			this->label6->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label6->Location = System::Drawing::Point(96, 211);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(408, 45);
			this->label6->TabIndex = 16;
			this->label6->Text = L"Connect three meter long 50-ohm cable and 40 dB attenuator from Rx to Tx.";
			// 
			// label7
			// 
			this->label7->BackColor = System::Drawing::Color::Transparent;
			this->label7->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label7->Location = System::Drawing::Point(96, 114);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(414, 48);
			this->label7->TabIndex = 17;
			this->label7->Text = L"Connect one meter long 50-ohm cable, shorted at far end, to Tx connector.";
			// 
			// TxLowAmpButton
			// 
			this->TxLowAmpButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->TxLowAmpButton->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->TxLowAmpButton->Location = System::Drawing::Point(520, 256);
			this->TxLowAmpButton->Name = L"TxLowAmpButton";
			this->TxLowAmpButton->Size = System::Drawing::Size(176, 48);
			this->TxLowAmpButton->TabIndex = 18;
			this->TxLowAmpButton->Text = L"Trans Amplitude Detector Calibration";
			this->TxLowAmpButton->Click += gcnew System::EventHandler(this, &InstrumentCal::TxLowAmpButton_Click);
			// 
			// label8
			// 
			this->label8->BackColor = System::Drawing::Color::Transparent;
			this->label8->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label8->Location = System::Drawing::Point(96, 261);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(408, 45);
			this->label8->TabIndex = 19;
			this->label8->Text = L"Connect 40 dB attenuator (50 ohm type) from Rx to Tx. (Use three-meter cable and " 
				L"setup from previous step).";
			// 
			// TxLowAmpStat
			// 
			this->TxLowAmpStat->BackColor = System::Drawing::Color::Transparent;
			this->TxLowAmpStat->Location = System::Drawing::Point(17, 256);
			this->TxLowAmpStat->Name = L"TxLowAmpStat";
			this->TxLowAmpStat->Size = System::Drawing::Size(48, 48);
			this->TxLowAmpStat->TabIndex = 20;
			this->TxLowAmpStat->Visible = false;
			// 
			// FrequencyCalButton
			// 
			this->FrequencyCalButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->FrequencyCalButton->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->FrequencyCalButton->Location = System::Drawing::Point(512, 474);
			this->FrequencyCalButton->Name = L"FrequencyCalButton";
			this->FrequencyCalButton->Size = System::Drawing::Size(176, 48);
			this->FrequencyCalButton->TabIndex = 21;
			this->FrequencyCalButton->Text = L"Internal Crystal Frequency Calibration";
			this->FrequencyCalButton->Click += gcnew System::EventHandler(this, &InstrumentCal::FrequencyCalButton_Click);
			// 
			// label3
			// 
			this->label3->BackColor = System::Drawing::Color::Transparent;
			this->label3->Location = System::Drawing::Point(96, 458);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(408, 24);
			this->label3->TabIndex = 22;
			this->label3->Text = L"1. Connect frequency counter to TX.";
			// 
			// FrequencyCalStat
			// 
			this->FrequencyCalStat->BackColor = System::Drawing::Color::Transparent;
			this->FrequencyCalStat->Location = System::Drawing::Point(16, 474);
			this->FrequencyCalStat->Name = L"FrequencyCalStat";
			this->FrequencyCalStat->Size = System::Drawing::Size(56, 49);
			this->FrequencyCalStat->TabIndex = 23;
			this->FrequencyCalStat->Visible = false;
			// 
			// MeasFreqTextBox
			// 
			this->MeasFreqTextBox->Location = System::Drawing::Point(256, 546);
			this->MeasFreqTextBox->Name = L"MeasFreqTextBox";
			this->MeasFreqTextBox->Size = System::Drawing::Size(111, 23);
			this->MeasFreqTextBox->TabIndex = 24;
			this->MeasFreqTextBox->Text = L"100000000";
			// 
			// label9
			// 
			this->label9->BackColor = System::Drawing::Color::Transparent;
			this->label9->Location = System::Drawing::Point(96, 546);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(153, 23);
			this->label9->TabIndex = 25;
			this->label9->Text = L"Measured Frequency";
			// 
			// FreqEnterButton
			// 
			this->FreqEnterButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->FreqEnterButton->Location = System::Drawing::Point(376, 546);
			this->FreqEnterButton->Name = L"FreqEnterButton";
			this->FreqEnterButton->Size = System::Drawing::Size(56, 24);
			this->FreqEnterButton->TabIndex = 26;
			this->FreqEnterButton->Text = L"Enter";
			this->FreqEnterButton->Click += gcnew System::EventHandler(this, &InstrumentCal::FreqEnterButton_Click);
			// 
			// label10
			// 
			this->label10->BackColor = System::Drawing::Color::Transparent;
			this->label10->Location = System::Drawing::Point(96, 474);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(408, 16);
			this->label10->TabIndex = 27;
			this->label10->Text = L"2. Click \"Internal Crystal Frequency Calibration\" button.";
			// 
			// label11
			// 
			this->label11->BackColor = System::Drawing::Color::Transparent;
			this->label11->Location = System::Drawing::Point(96, 498);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(408, 32);
			this->label11->TabIndex = 28;
			this->label11->Text = L"3. Measure and enter frequency to nearest hertz (approximately 100 MHz).";
			// 
			// label12
			// 
			this->label12->BackColor = System::Drawing::Color::Transparent;
			this->label12->Location = System::Drawing::Point(96, 530);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(408, 16);
			this->label12->TabIndex = 29;
			this->label12->Text = L"4. Click \"Enter\" button.";
			// 
			// DirectivityCalButton
			// 
			this->DirectivityCalButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->DirectivityCalButton->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->DirectivityCalButton->Location = System::Drawing::Point(520, 304);
			this->DirectivityCalButton->Name = L"DirectivityCalButton";
			this->DirectivityCalButton->Size = System::Drawing::Size(176, 48);
			this->DirectivityCalButton->TabIndex = 31;
			this->DirectivityCalButton->Text = L"Coupler Directivity Calibration";
			this->DirectivityCalButton->Click += gcnew System::EventHandler(this, &InstrumentCal::DirectivityCalButton_Click);
			// 
			// DirectivityCalStat
			// 
			this->DirectivityCalStat->BackColor = System::Drawing::Color::Transparent;
			this->DirectivityCalStat->Location = System::Drawing::Point(17, 304);
			this->DirectivityCalStat->Name = L"DirectivityCalStat";
			this->DirectivityCalStat->Size = System::Drawing::Size(48, 48);
			this->DirectivityCalStat->TabIndex = 32;
			this->DirectivityCalStat->Visible = false;
			// 
			// label14
			// 
			this->label14->BackColor = System::Drawing::Color::Transparent;
			this->label14->Location = System::Drawing::Point(96, 312);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(408, 32);
			this->label14->TabIndex = 33;
			this->label14->Text = L"Connect 40 dB attenuator directly to the TX connector (no cable).";
			// 
			// RxAmpButtonOpen
			// 
			this->RxAmpButtonOpen->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->RxAmpButtonOpen->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->RxAmpButtonOpen->Location = System::Drawing::Point(520, 160);
			this->RxAmpButtonOpen->Name = L"RxAmpButtonOpen";
			this->RxAmpButtonOpen->Size = System::Drawing::Size(176, 48);
			this->RxAmpButtonOpen->TabIndex = 34;
			this->RxAmpButtonOpen->Text = L"Refl Amplitude Detector Cal Open";
			this->RxAmpButtonOpen->Click += gcnew System::EventHandler(this, &InstrumentCal::RxAmpButtonOpen_Click);
			// 
			// label15
			// 
			this->label15->BackColor = System::Drawing::Color::Transparent;
			this->label15->Location = System::Drawing::Point(96, 161);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(408, 48);
			this->label15->TabIndex = 35;
			this->label15->Text = L"Connect one meter long 50-ohm cable, open at far end, to Tx connector.";
			// 
			// RxAmpStatOpen
			// 
			this->RxAmpStatOpen->BackColor = System::Drawing::Color::Transparent;
			this->RxAmpStatOpen->Location = System::Drawing::Point(17, 152);
			this->RxAmpStatOpen->Name = L"RxAmpStatOpen";
			this->RxAmpStatOpen->Size = System::Drawing::Size(48, 48);
			this->RxAmpStatOpen->TabIndex = 36;
			this->RxAmpStatOpen->Visible = false;
			// 
			// RxAmpButtonShort
			// 
			this->RxAmpButtonShort->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->RxAmpButtonShort->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->RxAmpButtonShort->Location = System::Drawing::Point(520, 112);
			this->RxAmpButtonShort->Name = L"RxAmpButtonShort";
			this->RxAmpButtonShort->Size = System::Drawing::Size(176, 48);
			this->RxAmpButtonShort->TabIndex = 37;
			this->RxAmpButtonShort->Text = L"Refl Amplitude Detector Cal Short";
			this->RxAmpButtonShort->Click += gcnew System::EventHandler(this, &InstrumentCal::RxAmpButtonShort_Click);
			// 
			// ICO1Label
			// 
			this->ICO1Label->BackColor = System::Drawing::Color::Transparent;
			this->ICO1Label->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"ICO1Label.Image")));
			this->ICO1Label->Location = System::Drawing::Point(19, 538);
			this->ICO1Label->Name = L"ICO1Label";
			this->ICO1Label->Size = System::Drawing::Size(37, 25);
			this->ICO1Label->TabIndex = 38;
			this->ICO1Label->Visible = false;
			// 
			// ICO2Label
			// 
			this->ICO2Label->BackColor = System::Drawing::Color::Transparent;
			this->ICO2Label->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"ICO2Label.Image")));
			this->ICO2Label->Location = System::Drawing::Point(25, 570);
			this->ICO2Label->Name = L"ICO2Label";
			this->ICO2Label->Size = System::Drawing::Size(25, 25);
			this->ICO2Label->TabIndex = 39;
			this->ICO2Label->Visible = false;
			// 
			// Optional
			// 
			this->Optional->BackColor = System::Drawing::Color::Transparent;
			this->Optional->Location = System::Drawing::Point(87, 442);
			this->Optional->Name = L"Optional";
			this->Optional->Size = System::Drawing::Size(609, 134);
			this->Optional->TabIndex = 40;
			this->Optional->TabStop = false;
			this->Optional->Text = L"Optional";
			// 
			// TxLongCableOpenButton
			// 
			this->TxLongCableOpenButton->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->TxLongCableOpenButton->Location = System::Drawing::Point(520, 400);
			this->TxLongCableOpenButton->Name = L"TxLongCableOpenButton";
			this->TxLongCableOpenButton->Size = System::Drawing::Size(176, 48);
			this->TxLongCableOpenButton->TabIndex = 42;
			this->TxLongCableOpenButton->Text = L"Directional Coupler Ripple Cal Open";
			this->TxLongCableOpenButton->UseVisualStyleBackColor = true;
			this->TxLongCableOpenButton->Click += gcnew System::EventHandler(this, &InstrumentCal::TxLongCableOpenButton_Click);
			// 
			// label13
			// 
			this->label13->BackColor = System::Drawing::Color::Transparent;
			this->label13->Location = System::Drawing::Point(96, 358);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(394, 35);
			this->label13->TabIndex = 43;
			this->label13->Text = L"Connect three meter long 50-ohm cable, shorted at far end, to Tx connector.";
			// 
			// label16
			// 
			this->label16->BackColor = System::Drawing::Color::Transparent;
			this->label16->Location = System::Drawing::Point(96, 403);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(394, 38);
			this->label16->TabIndex = 44;
			this->label16->Text = L"Connect three meter long 50-ohm cable, open at far end, to Tx connector.";
			// 
			// TxLongCableShortStat
			// 
			this->TxLongCableShortStat->BackColor = System::Drawing::Color::Transparent;
			this->TxLongCableShortStat->Location = System::Drawing::Point(17, 349);
			this->TxLongCableShortStat->Name = L"TxLongCableShortStat";
			this->TxLongCableShortStat->Size = System::Drawing::Size(48, 48);
			this->TxLongCableShortStat->TabIndex = 45;
			this->TxLongCableShortStat->Visible = false;
			// 
			// TxLongCableOpenStat
			// 
			this->TxLongCableOpenStat->BackColor = System::Drawing::Color::Transparent;
			this->TxLongCableOpenStat->Location = System::Drawing::Point(17, 395);
			this->TxLongCableOpenStat->Name = L"TxLongCableOpenStat";
			this->TxLongCableOpenStat->Size = System::Drawing::Size(48, 48);
			this->TxLongCableOpenStat->TabIndex = 46;
			this->TxLongCableOpenStat->Visible = false;
			// 
			// TxLongCableShortButton
			// 
			this->TxLongCableShortButton->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->TxLongCableShortButton->Location = System::Drawing::Point(520, 352);
			this->TxLongCableShortButton->Name = L"TxLongCableShortButton";
			this->TxLongCableShortButton->Size = System::Drawing::Size(176, 48);
			this->TxLongCableShortButton->TabIndex = 48;
			this->TxLongCableShortButton->Text = L"Directional Coupler Ripple Cal Short";
			this->TxLongCableShortButton->UseVisualStyleBackColor = true;
			this->TxLongCableShortButton->Click += gcnew System::EventHandler(this, &InstrumentCal::TxLongCableShortButton_Click);
			// 
			// InstrumentCal
			// 
			this->AcceptButton = this->OKbutton;
			this->AutoScroll = true;
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"$this.BackgroundImage")));
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->CancelButton = this->Cancelbutton;
			this->ClientSize = System::Drawing::Size(736, 629);
			this->Controls->Add(this->TxLongCableShortButton);
			this->Controls->Add(this->TxLongCableOpenStat);
			this->Controls->Add(this->TxLongCableShortStat);
			this->Controls->Add(this->label16);
			this->Controls->Add(this->label13);
			this->Controls->Add(this->TxLongCableOpenButton);
			this->Controls->Add(this->ICO2Label);
			this->Controls->Add(this->ICO1Label);
			this->Controls->Add(this->RxAmpButtonShort);
			this->Controls->Add(this->RxAmpStatOpen);
			this->Controls->Add(this->label15);
			this->Controls->Add(this->RxAmpButtonOpen);
			this->Controls->Add(this->label14);
			this->Controls->Add(this->DirectivityCalStat);
			this->Controls->Add(this->DirectivityCalButton);
			this->Controls->Add(this->label12);
			this->Controls->Add(this->label11);
			this->Controls->Add(this->label10);
			this->Controls->Add(this->FreqEnterButton);
			this->Controls->Add(this->label9);
			this->Controls->Add(this->MeasFreqTextBox);
			this->Controls->Add(this->FrequencyCalStat);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->FrequencyCalButton);
			this->Controls->Add(this->TxLowAmpStat);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->TxLowAmpButton);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->progressBar1);
			this->Controls->Add(this->TxPhaseStat);
			this->Controls->Add(this->RxAmpStatShort);
			this->Controls->Add(this->RxPhaseStat);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->TxPhaseButton);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->RxPhaseButton);
			this->Controls->Add(this->Cancelbutton);
			this->Controls->Add(this->OKbutton);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->Optional);
			this->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->Location = System::Drawing::Point(128, 239);
			this->Name = L"InstrumentCal";
			this->Text = L"Detector Cal";
			this->Load += gcnew System::EventHandler(this, &InstrumentCal::InstrumentCal_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}		

private: VNADevice^ VNA;					///< Vector Network Analyzer hardware object
private: InstrumentCalDataSet^ Cal;					///< Calibration Data Set
private: FrequencyGrid^ FG;					///< Our frequency grid
private: String^ AllUsersDir;				///< Application data directory common to all users
private: array<UInt16>^ Iphase, ^Qphase;	///< hold phase detector results from sweep
private: array<UInt16>^  IphaseLo;				///< hold tran I phase from -20 db sweep.
private: array<UInt16,2>^  ReflMagRegression;	///< Reflection Magnitude vs level sweep
private: array<UInt16,2>^  ReflMagPhDir;		///< Directivity Magnitude & Phase data from reflection sweep
private: array<UInt16,2>^  ReflMagPhOpen;		///< Open Magnitude & Phase data from reflection sweep
private: array<UInt16,2>^  ReflMagPhShort;		///< Shorted Magnitude & Phase data from reflection sweep
private: array<UInt16,2>^  ReflDetailMagPhShort; ///< Shorted Cable Detailed Mag & Phase data from reflection sweep
private: array<UInt16,2>^  ReflDetailMagPhOpen; ///< Open Cable Detailed Mag & Phase data from reflection sweep
private: array<UInt16,2>^  ReflLongDetailMagPhOpen; ///< Terminated 3m Cable Detailed Mag & Phase data from reflection sweep
private: array<UInt16,2>^  ReflLongDetailMagPhShort; ///< Terminated 3m Cable Detailed Mag & Phase data from reflection sweep
private: array<UInt16,2>^  TranMag;				///< Transmission magnitude detector vs level sweep
private: array<UInt16>^  BufferI, ^BufferQ, ^BufferILo;
private: array<UInt16>^  BufferN, ^BufferM; ///< Temporary raw readings
private: array<UInt16>^  BufferP;
private: int MeasFrequency;					///< Measured Frequency from crystal calibration
private: array<Boolean>^CalStepPass;		///< Array of Calibrations Steps. TRUE = step passed

		 // TODO: Measure surge impedance of calibration cable (R+jX) using 50 ohm load,
		 //		then compute complex reflection expected from that surge impedance vs. angle,
		 //		and fit the S11 measurements to it. NB: The jX component of the surge impedance
		 //		causes negative return loss at angles in the same direction as the sign of jX !

		 /// Reflection Phase Detector Calibration button click handler
private: System::Void RxPhaseButton_Click(System::Object^  sender, System::EventArgs^  e)
		 {

			 // Reflection Phase Detector Calibration

			VNA_RXBUFFER *RxBuf = new VNA_RXBUFFER;
			VNA_TXBUFFER *TxBuf = new VNA_TXBUFFER;
			int Fdesired;

			progressBar1->Value = 0;
	

			// run a sweep of 1024 points, calibrating Rx phase detector
			VNA->Sweep(GetFreqFromPhaseCalGrid(0), GetFreqFromPhaseCalGrid(1) - GetFreqFromPhaseCalGrid(0), PHASECALGRIDSIZE, 10);
			for (long i=0; i<PHASECALGRIDSIZE; i++)
			{
				// Compute spot frequency

				Fdesired = GetFreqFromPhaseCalGrid(i);

				TxBuf->TxAccum = i; //FG->DDS(Fdesired);
				TxBuf->IDAClevelHi = MAX_DDS_LEVEL;		// Max transmit level
				TxBuf->ReplyType = 0;
				TxBuf->MeasureDelay = 0;
				TxBuf->QDAClevel = QDAC_ZERODBM;	// Reference level

				// Take 7 readings at the same frequency

				for(int k=0; k<7; k++)
				{
                    VNA->WriteRead(TxBuf, RxBuf, DIR_REFL);
					BufferI[k] = RxBuf->ReflPI;
					BufferQ[k] = RxBuf->ReflPQ;
				}

				Iphase[i] = Median7(BufferI);	// use median filter to smooth raw value
				Qphase[i] = Median7(BufferQ);

				progressBar1->Value = i;
				if(i%20 == 0)
					progressBar1->Update();
			}


			// Set calibrations to false. PhaseCal sets phaseCalibrated to false when
			// it starts. Thus, this needs to be the first step in the instrument cal
			// process and makes instrument cal irreversible at this point.
			// If the user cancels the cal, then nothing is saved to disk, but
			// the user will have to close then restart the application.
			// A better solution would be to have each cal step just measure raw data, 
			// and put all processing and flag alterations in the 'OK' loop.

			Cal->DirCoupler->DirCalibrated = false;
			Cal->DirCoupler->RippleCalibrated = false;



			// Note that IphaseLo is not meaningful for REFL measurements.
			if(Cal->RxDet->PhaseCal(Iphase, Qphase, IphaseLo))		// TRUE if cal sucessful
			{
				RxPhaseStat->Image = ICO1Label->Image;				// checkmark
				CalStepPass[0] = true;
			}
			else
			{
				RxPhaseStat->Image = ICO2Label->Image;				// 'x'
				CalStepPass[0] = false;
				ShowCalStepFailMessage();
			}

			RxPhaseStat->Visible = true;
		 }
		 /// Transmission Phase Detector Calibration button click handler
private: System::Void TxPhaseButton_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 // Transmission Phase Detector Calibration

			VNA_RXBUFFER *RxBuf = new VNA_RXBUFFER;
			VNA_TXBUFFER *TxBuf = new VNA_TXBUFFER;
			int Fdesired;

			progressBar1->Value = 0;

			// run a sweep of ~1024 points, calibrating Tx phase detector
			// skip really low frequencies and really high frequencies
			VNA->Sweep(GetFreqFromPhaseCalGrid(0), GetFreqFromPhaseCalGrid(1) - GetFreqFromPhaseCalGrid(0), PHASECALGRIDSIZE, 10);
			for (long i=0; i<PHASECALGRIDSIZE; i++)
			{
				// Compute spot frequency

				Fdesired = GetFreqFromPhaseCalGrid(i);

				TxBuf->TxAccum = i; //FG->DDS(Fdesired);	
				TxBuf->IDAClevelHi = MAX_DDS_LEVEL;		// Max transmit level
				TxBuf->ReplyType = 0;
				TxBuf->MeasureDelay = 0;
				TxBuf->QDAClevel = QDAC_ZERODBM;	// Reference level

				TxBuf->IDAClevelPhLow = TxLevLinear(0-TARGETPHLOMAG); // Phase Low Ref Level  09-30-2007

				// Take 7 readings at the same frequency
				for(int k=0; k<7; k++)
				{
                    VNA->WriteRead(TxBuf, RxBuf, DIR_TRANS);
					BufferI[k] = RxBuf->TranPI;
					BufferQ[k] = RxBuf->TranPQ;
					BufferILo[k] = RxBuf->TranPILow;
				}

				Iphase[i] = Median7(BufferI);		// use median filter to smooth raw reading
				Qphase[i] = Median7(BufferQ);
				IphaseLo[i] = Median7(BufferILo);

				progressBar1->Value = i;
				if(i%20 == 0)
					progressBar1->Update();
			}
		
			if(Cal->TxDet->PhaseCal(Iphase, Qphase, IphaseLo))	// TRUE if phase cal was sucessful
			{
				TxPhaseStat->Image = ICO1Label->Image;				// checkmark
				CalStepPass[1] = true;
			}
			else
			{
				TxPhaseStat->Image = ICO2Label->Image;				// 'x'
				CalStepPass[1] = false;
				ShowCalStepFailMessage();
			}

			TxPhaseStat->Visible = true;
		 }

		 /// OK button click handler

		 /// Transmission Amplitude Detector Calibration button click handler
private: System::Void TxLowAmpButton_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			
			// Transmission Amplitude detector Calibration - with 40 dB attenuator in path
			 
			VNA_RXBUFFER *RxBuf = new VNA_RXBUFFER;
			VNA_TXBUFFER *TxBuf = new VNA_TXBUFFER;
			int Fdesired;

			TxBuf->ReplyType = 0;
			TxBuf->MeasureDelay = 0;		    // No delay
			TxBuf->QDAClevel = QDAC_ZERODBM;	// Reference level

			progressBar1->Value = 0;

			// run a sweep of 21 frequencies, calibrating Trans Amplitude detector
			VNA->Sweep(GetFreqFromDetMagCalGrid(0), GetFreqFromDetMagCalGrid(1) - GetFreqFromDetMagCalGrid(0), 21, 10);
			for (long FreqIdx=0; FreqIdx<21; FreqIdx++)
			{

				// Compute spot frequency from DetCalGrid Point
				Fdesired = GetFreqFromDetMagCalGrid(FreqIdx);

				TxBuf->TxAccum = FreqIdx; //FG->DDS(Fdesired);

//				for (int levelDB=0; levelDB<30; levelDB++)	// Measure over 60 dB range
//				{
// 					TxBuf->IDAClevelHi = TxLevLinear(-levelDB);	// 0 to -29 dbm
//					TxBuf->IDAClevelLo = TxLevLinear(-levelDB-30);	// -30 to -59 dbm

					for(int k=0; k<7; k++)				// Take 7 readings at each point
					{
						VNA->WriteRead(TxBuf, RxBuf, DIR_TRANS);
						BufferM[k] = RxBuf->TranMQHi;
						BufferN[k] = RxBuf->TranMQLo;
					}
//					TranMag[FreqIdx, levelDB] = Median7(BufferM);		// Filter the readings
//					TranMag[FreqIdx, levelDB+30] = Median7(BufferN);

//				}
				progressBar1->Value = FreqIdx * progressBar1->Maximum/20;
				progressBar1->Update();
			}
//			if(Cal->TxDet->AmpCal(TranMag))		// AmpCal forms y = mx + b for transmission detector
			{
                CalStepPass[2] = true;
				TxLowAmpStat->Image = ICO1Label->Image;
			}
//			else
//			{
//                CalStepPass[2] = false;
//				TxLowAmpStat->Image = ICO2Label->Image;
//				ShowCalStepFailMessage();
//			}

			TxLowAmpStat->Visible = true;

		 }

private: System::Void FrequencyCalButton_Click(System::Object^  sender, System::EventArgs^  e)
		 {

			// Internal Crystal Frequency Calibration
			// Starts out with whatever initial value of FG->Error happens to be.
			// This allows iterating to a better error estimate over several
			// frequency cal cycles if desired.

			VNA_RXBUFFER *RxBuf = new VNA_RXBUFFER;
			VNA_TXBUFFER *TxBuf = new VNA_TXBUFFER;
			int Fdesired;

			Fdesired = 100000000;	// output 100 MHz

			TxBuf->TxAccum = FG->DDS(Fdesired);	
			TxBuf->IDAClevelHi = MAX_DDS_LEVEL;		// Max transmit level
			TxBuf->ReplyType = 0;
			TxBuf->MeasureDelay = 0;
			TxBuf->QDAClevel = QDAC_ZERODBM;	// Reference level

			VNA->WriteRead(TxBuf, RxBuf, DIR_TRANS);	// write the VNA with CW frequency output

		 }

private: System::Void FreqEnterButton_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 // Get frequency error from the text box and error check it.
			 // If valid, update the frequency error.

			try
			{
                MeasFrequency = Convert::ToInt32(MeasFreqTextBox->Text);
			}
			catch (System::FormatException^ pe)
			{
				MessageBox::Show(pe->Message, "Error");
			}
			catch (System::OverflowException^ pe)
			{
				MessageBox::Show(pe->Message, "Error");
			}

			 if((MeasFrequency > 103000000) || (MeasFrequency < 97000000))	// bad data likely
				MessageBox::Show("Value appears to be invalid, re-enter","Error");
			 else
			 {
				 FrequencyCalStat->Visible = true;
				 FG->ferror += MeasFrequency - 100000000;
			 }

		 }

private: System::Void DirectivityCalButton_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 // Measure the directivity of the coupler at 21 frequency points 
			 // by using a well-terminated load directly on the TX connector.

			VNA_RXBUFFER *RxBuf = new VNA_RXBUFFER;
			VNA_TXBUFFER *TxBuf = new VNA_TXBUFFER;
			int Fdesired;
			
			TxBuf->ReplyType = 0;
			TxBuf->MeasureDelay = 0;		
			TxBuf->QDAClevel = QDAC_ZERODBM;	// Reference level
			TxBuf->IDAClevelHi = MAX_DDS_LEVEL;		// Max transmit level

			progressBar1->Value = 0;

			// run a sweep of 21 frequencies, measuring residual Refl Amplitude
			VNA->Sweep(GetFreqFromDetMagCalGrid(0), GetFreqFromDetMagCalGrid(1) - GetFreqFromDetMagCalGrid(0), 21, 10);
			for (long FreqIdx=0; FreqIdx<21; FreqIdx++)
			{

				// Compute spot frequency for each DetectorCalGrid Point
				Fdesired = GetFreqFromDetMagCalGrid(FreqIdx);

				TxBuf->TxAccum = FreqIdx; //FG->DDS(Fdesired);

				// Take 7 readings, use median value
				for(int k=0; k<7; k++)
				{
					VNA->WriteRead(TxBuf, RxBuf, DIR_REFL);
					BufferM[k] = RxBuf->ReflMQ;
					BufferI[k] = RxBuf->ReflPI;
					BufferQ[k] = RxBuf->ReflPQ;
				}
				ReflMagPhDir[FreqIdx, MagQ] = Median7(BufferM);
				ReflMagPhDir[FreqIdx, PhaseI] = Median7(BufferI);
				ReflMagPhDir[FreqIdx, PhaseQ] = Median7(BufferQ);

				progressBar1->Value = FreqIdx * progressBar1->Maximum/20;
			}


			Cal->DirCoupler->DirectivityCal(ReflMagPhDir);		// Save the raw counts in the DirCoupler object
			Cal->DirCoupler->DirCalibrated = true;

			CalStepPass[3] = true;
			DirectivityCalStat->Image = ICO1Label->Image;
			DirectivityCalStat->Visible = true;


		 }

private: System::Void RxAmpButtonOpen_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 // Reflection Amplitude Detector Calibration Open
			 // Captures Magnitude, I-phase, and Q-phase at zero dbm for coupler correction
			 
			VNA_RXBUFFER *RxBuf = new VNA_RXBUFFER;
			VNA_TXBUFFER *TxBuf = new VNA_TXBUFFER;
			int Fdesired;
			int i;
			

			TxBuf->ReplyType = 0;
			TxBuf->MeasureDelay = 0;			// No Measruement Delay
			TxBuf->QDAClevel = QDAC_ZERODBM;	// Reference level

			progressBar1->Value = 0;

			// run a sweep of 21 frequencies, calibrating Refl Amplitude detector
			VNA->Sweep(GetFreqFromDetMagCalGrid(0), GetFreqFromDetMagCalGrid(1) - GetFreqFromDetMagCalGrid(0), 21, 10);
			for (long FreqIdx=0; FreqIdx<21; FreqIdx++)
			{

				// Compute spot frequency

				Fdesired = GetFreqFromDetMagCalGrid(FreqIdx);

				TxBuf->TxAccum = FreqIdx; //FG->DDS(Fdesired);

 				TxBuf->IDAClevelHi = TxLevLinear(0);	// 0  dbm

				// Take 7  magnitude & phase readings, use median value
				for(int k=0; k<7; k++)
				{
					VNA->WriteRead(TxBuf, RxBuf, DIR_REFL);
					BufferM[k] = RxBuf->ReflMQ;
					BufferI[k] = RxBuf->ReflPI;
					BufferQ[k] = RxBuf->ReflPQ;
				}
				ReflMagPhOpen[FreqIdx, MagQ] = Median7(BufferM);
				ReflMagPhOpen[FreqIdx, PhaseI] = Median7(BufferI);
				ReflMagPhOpen[FreqIdx, PhaseQ] = Median7(BufferQ);

				progressBar1->Value = FreqIdx * progressBar1->Maximum/40;
				progressBar1->Update();
			}

			// Collect detailed 1024 point sweep for Open Reflection Data

			VNA->Sweep(GetFreqFromPhaseCalGrid(0), GetFreqFromPhaseCalGrid(1) - GetFreqFromPhaseCalGrid(0), 1024, 10);
			for (i=0; i<1024; i++)
            {
				// Compute spot frequency

				Fdesired = GetFreqFromPhaseCalGrid(i);

				TxBuf->TxAccum = i; //FG->DDS(Fdesired);		
				TxBuf->IDAClevelHi = MAX_DDS_LEVEL;		// Max transmit level
				TxBuf->ReplyType = 0;
				TxBuf->MeasureDelay = 0;
				TxBuf->QDAClevel = QDAC_ZERODBM;	// Reference level

				for (int k=0; k<7; k++)
				{
                    VNA->WriteRead(TxBuf, RxBuf, DIR_REFL);
					BufferI[k] = RxBuf->ReflPI;
					BufferQ[k] = RxBuf->ReflPQ;
					BufferM[k] = RxBuf->ReflMQ;
				}
				ReflDetailMagPhOpen[i, MagQ] = Median7(BufferM);
				ReflDetailMagPhOpen[i, PhaseI] = Median7(BufferI);		// filter the raw readings
				ReflDetailMagPhOpen[i, PhaseQ] = Median7(BufferQ);

				progressBar1->Value = progressBar1->Maximum/2 + i * progressBar1->Maximum/2048;
				if(i%50 == 0)
                    progressBar1->Update();

			}

			CalStepPass[4] = true;
			RxAmpStatOpen->Image = ICO1Label->Image;
			RxAmpStatOpen->Visible = true;

		 }

private: System::Void RxAmpButtonShort_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 // Reflection Amplitude Detector Calibration Shorted
			 // Performs two functions:
			 //		1. Captures amplitude over 60 dB range for linear regression
			 //		2. Captures Mag, I-phase and Q-phase at 0 dbm for coupler correction
			 
			VNA_RXBUFFER *RxBuf = new VNA_RXBUFFER;
			VNA_TXBUFFER *TxBuf = new VNA_TXBUFFER;
			int Fdesired;
			int i;
			long FreqIdx;
			
			double mag, phase;
			double& rmag = mag;
			double& rphase = phase;



			TxBuf->ReplyType = 0;				// Full reply type
			TxBuf->MeasureDelay = 0;			// No Measurement Delay
			TxBuf->QDAClevel = QDAC_ZERODBM;	// Reference level

			progressBar1->Value = 0;

			// run a sweep of 21 frequencies, calibrating Refl Amplitude detector
			VNA->Sweep(GetFreqFromDetMagCalGrid(0), GetFreqFromDetMagCalGrid(1) - GetFreqFromDetMagCalGrid(0), 21, 10);
			for (FreqIdx=0; FreqIdx<21; FreqIdx++)
			{

				// Compute spot frequency from DetCalGrid Point
				Fdesired = GetFreqFromDetMagCalGrid(FreqIdx);

				TxBuf->TxAccum = FreqIdx; //FG->DDS(Fdesired);

//				for (int levelDB=0; levelDB<=59; levelDB++)	// Measure over 60 dB range
//				{
// 					TxBuf->IDAClevelHi = TxLevLinear(-levelDB);	// 0 to -59 dbm

					// Take 7 readings, use median value
					for(int k=0; k<7; k++)
					{
						VNA->WriteRead(TxBuf, RxBuf, DIR_REFL);
						BufferM[k] = RxBuf->ReflMQ;
						BufferI[k] = RxBuf->ReflPI;
						BufferQ[k] = RxBuf->ReflPQ;
					}
//					ReflMagRegression[FreqIdx, levelDB] = Median7(BufferM);

//					if (levelDB == 0)			// Also record Short Mag & Phase just at the zero dbm level.
					{
						ReflMagPhShort[FreqIdx, MagQ] = Median7(BufferM);
						ReflMagPhShort[FreqIdx, PhaseI] = Median7(BufferI);
						ReflMagPhShort[FreqIdx, PhaseQ] = Median7(BufferQ);
					}
//				}
				progressBar1->Value = FreqIdx * progressBar1->Maximum/40;
				progressBar1->Update();
			}

			bool RegressionOK = true; //Cal->RxDet->AmpCal(ReflMagRegression);		// Linear regression - sets y = mx + b

			// Collect detailed 1024 point sweep for Shorted Reflection Data

			VNA->Sweep(GetFreqFromPhaseCalGrid(0), GetFreqFromPhaseCalGrid(1) - GetFreqFromPhaseCalGrid(0), 1024, 10);
			for (i=0; i<1024; i++)
            {
				// Compute spot frequency

				Fdesired = GetFreqFromPhaseCalGrid(i);

				TxBuf->TxAccum = i; //FG->DDS(Fdesired);		
				TxBuf->IDAClevelHi = MAX_DDS_LEVEL;		// Max transmit level
				TxBuf->ReplyType = 0;
				TxBuf->MeasureDelay = 0;
				TxBuf->QDAClevel = QDAC_ZERODBM;	// Reference level

				for (int k=0; k<7; k++)
				{
                    VNA->WriteRead(TxBuf, RxBuf, DIR_REFL);
					BufferI[k] = RxBuf->ReflPI;
					BufferQ[k] = RxBuf->ReflPQ;
					BufferM[k] = RxBuf->ReflMQ;
				}
				ReflDetailMagPhShort[i, MagQ] = Median7(BufferM);
				ReflDetailMagPhShort[i, PhaseI] = Median7(BufferI);		// filter the raw readings
				ReflDetailMagPhShort[i, PhaseQ] = Median7(BufferQ);

				progressBar1->Value = progressBar1->Maximum/2 + i * progressBar1->Maximum/2048;
				if(i%50 == 0)
                    progressBar1->Update();

			}
			if(RegressionOK)
			{
				CalStepPass[5] = true;
				RxAmpStatShort->Image = ICO1Label->Image;
			}
			else
			{
				CalStepPass[5] = false;
				RxAmpStatShort->Image = ICO2Label->Image;
				ShowCalStepFailMessage();
			}
			RxAmpStatShort->Visible = true;
														// Leave coupler correction for 'OK' handler
		 }


private: System::Void TxLongCableOpenButton_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 // Reflection Amplitude Detector Calibration Terminated
			 // Captures Magnitude, I-phase, and Q-phase at zero dbm for determing characteristic
			 // impedance of the 1-meter calibration cable (it's Surge Impedance).
			 
			VNA_RXBUFFER *RxBuf = new VNA_RXBUFFER;
			VNA_TXBUFFER *TxBuf = new VNA_TXBUFFER;
			int Fdesired;
			int i;
			

			TxBuf->ReplyType = 0;
			TxBuf->MeasureDelay = 0;			// No Measurement delay
			TxBuf->QDAClevel = QDAC_ZERODBM;	// Reference level

			progressBar1->Value = 0;

			// Collect detailed 1024 point sweep for Terminated Reflection Data

			VNA->Sweep(GetFreqFromPhaseCalGrid(0), GetFreqFromPhaseCalGrid(1) - GetFreqFromPhaseCalGrid(0), 1024, 10);
			for (i=0; i<1024; i++)
            {
				// Compute spot frequency

				Fdesired = GetFreqFromPhaseCalGrid(i);

				TxBuf->TxAccum = i; // FG->DDS(Fdesired);		
				TxBuf->IDAClevelHi = MAX_DDS_LEVEL;		// Max transmit level
				TxBuf->ReplyType = 0;
				TxBuf->MeasureDelay = 0;
				TxBuf->QDAClevel = QDAC_ZERODBM;	// Reference level

				for (int k=0; k<7; k++)
				{
                    VNA->WriteRead(TxBuf, RxBuf, DIR_REFL);
					BufferI[k] = RxBuf->ReflPI;
					BufferQ[k] = RxBuf->ReflPQ;
					BufferM[k] = RxBuf->ReflMQ;
				}
				ReflLongDetailMagPhOpen[i, MagQ] = Median7(BufferM);
				ReflLongDetailMagPhOpen[i, PhaseI] = Median7(BufferI);		// filter the raw readings
				ReflLongDetailMagPhOpen[i, PhaseQ] = Median7(BufferQ);

				progressBar1->Value = i * progressBar1->Maximum/1024;
				if(i%10 == 0)
                    progressBar1->Update();

			}

			CalStepPass[6] = true;
			TxLongCableOpenStat->Image = ICO1Label->Image;	// check mark (OK)
			TxLongCableOpenStat->Visible = true;
		 }
private: System::Void ShowCalStepFailMessage()
		{
			MessageBox::Show("This Step returned defective results."
				"  Check: \n\r\t calibration setup \n\r\t cables \n\r\t adaptors \n\r\t attenuator"
				"\n\r\t all connections to be sure they are good."
				"\n\n\rThis step will need to be re-run before finishing calibration."
				"\n\rClose this window, then 'Cancel' if you do not wish to save Detector Cal data.",
				"Error: Failed Step", MessageBoxButtons::OK, MessageBoxIcon::Error);
		}
private: System::Void OKbutton_Click(System::Object^  sender, System::EventArgs^  e)
		 {

			double mag, phase;
			double& rmag = mag;
			double& rphase = phase;
			double bopen, bshorted;
			int Fdesired;
			int FreqIdx;
			MeasurementSet^ calPoint = gcnew MeasurementSet;

			 // Check that all relevant groups of data have been run, else
			 // display error message and give chance to rerun missing sets.

			for (int k = 0; k<NUMCALSTEPS; k++)
				if(CalStepPass[k] == false)
				{
					MessageBox::Show("Some calibration steps not completed or failed."
						"  Be sure to complete all required steps before finishing."
						"\n\rClose this window, then 'Cancel' if you do not wish to save Detector Cal data.",
						"Error: Missing Steps", MessageBoxButtons::OK, MessageBoxIcon::Error);
					return;
				}


			// Normalize 'b'  in y = mx + b   for (ShortDB + OpenDB) / 2  at 21 points
			// Reduces reflection detector magnitude calibration error
			//   caused by directional coupler magnitude (vs. phase angle) ripple.
			//   
			for (FreqIdx=0; FreqIdx<21; FreqIdx++)
			{

				// Compute spot frequency

				Fdesired = GetFreqFromDetMagCalGrid(FreqIdx);
				
				calPoint->ReflPI = ReflMagPhOpen[FreqIdx, PhaseI];
				calPoint->ReflPQ = ReflMagPhOpen[FreqIdx, PhaseQ];
				calPoint->ReflMQ = ReflMagPhOpen[FreqIdx, MagQ];

                Cal->ResolveReflPolar(calPoint, Fdesired, rmag, rphase, false);
				bopen = 20.0 * log10(mag);	// convert to dB

				calPoint->ReflPI = ReflMagPhShort[FreqIdx, PhaseI];
				calPoint->ReflPQ = ReflMagPhShort[FreqIdx, PhaseQ];
				calPoint->ReflMQ = ReflMagPhShort[FreqIdx, MagQ];


                Cal->ResolveReflPolar(calPoint, Fdesired, rmag, rphase, false);
				bshorted = 20.0 * log10(mag);	// convert to dB

				Cal->RxDet->b[FreqIdx] -= (bopen+bshorted)/2.0;		// 'b' value is halfway between open and short values
			}

			// September 2007 routine - uses gradient descent technique to build a periodic error model
			bool success = Cal->DirCoupler->GenerateShortOpenComp(Cal, ReflLongDetailMagPhShort, ReflLongDetailMagPhOpen);

			if(!success)		// OpenShortComp failed - give error message
			{
				MessageBox::Show("OpenShort calibration failed."
					"\n\rClose this window, then 'Cancel' if you do not wish to save Detector Cal data.",
					"Error: failed convergence", MessageBoxButtons::OK, MessageBoxIcon::Error);
				return;
			}

			// Save Instrument Calibration values to detector.ica
			FileStream^ fs;
			BinaryWriter^ bw;

			String^ filename = String::Concat(AllUsersDir, "\\detector.ica");
			try
			{
				// Create a filestream & binary writer

				fs = gcnew FileStream(filename, FileMode::Create, FileAccess::Write);
				bw = gcnew BinaryWriter(fs);
			}
			catch(System::IO::IOException^ pe)
			{
				MessageBox::Show("Error. Could not Write Detector Calibration file.", pe->Message,
					MessageBoxButtons::OK, MessageBoxIcon::Error);
				return;
			}

			// Define header to match file identifying type and version

			String^ recognized = gcnew String("VNA Detector Calibration Data Set Version 2.1.1");
			bw->Write(recognized);		// put string header on outfile

			// write the AD8302 Phase Detector constants and error tables
			bw->Write(Cal->RxDet->Qmid);
			bw->Write(Cal->RxDet->Qmag);
			bw->Write(Cal->RxDet->Imid);
			bw->Write(Cal->RxDet->Imag);
			bw->Write(Cal->RxDet->diffQDelayNs);
			bw->Write(Cal->RxDet->Iasym);
			bw->Write(Cal->RxDet->Qasym);
			for (int degree = 0; degree<360; degree++)
				bw->Write(Cal->RxDet->pherror[degree]);

			bw->Write(Cal->TxDet->Qmid);
			bw->Write(Cal->TxDet->Qmag);
			bw->Write(Cal->TxDet->Imid);
			bw->Write(Cal->TxDet->Imag);
			bw->Write(Cal->TxDet->diffQDelayNs);
			bw->Write(Cal->TxDet->Iasym);
			bw->Write(Cal->TxDet->Qasym);
			for (int degree = 0; degree<360; degree++)
				bw->Write(Cal->TxDet->pherror[degree]);

			// write the AD8302 Amplitude Detector tables

			for (int i=0; i<21; i++)
			{
                bw->Write(Cal->RxDet->m[i]);
                bw->Write(Cal->RxDet->b[i]);
                bw->Write(Cal->RxDet->r[i]);
				bw->Write(Cal->RxDet->flat[i]);
			}
			
			for (int i=0; i<21; i++)
			{
                bw->Write(Cal->TxDet->m[i]);
                bw->Write(Cal->TxDet->b[i]);
                bw->Write(Cal->TxDet->r[i]);
				bw->Write(Cal->TxDet->flat[i]);
			}

			// write the frequency error (or zero if none entered)

			bw->Write(FG->ferror);

			// Write the Directivity Calibration raw values 10-18-2005

			for (int i=0; i<21; i++)
			{
				bw->Write(Cal->DirCoupler->DirIphs[i]);
				bw->Write(Cal->DirCoupler->DirQphs[i]);
				bw->Write(Cal->DirCoupler->DirMag[i]);
			}

			// 09-25-2007 - write out the Ripple periodic error model parameters
			for (int i=0; i<6; i++)
			{
				bw->Write(Cal->DirCoupler->MagRipCoeff[i]->freq);
				bw->Write(Cal->DirCoupler->MagRipCoeff[i]->offset);
				bw->Write(Cal->DirCoupler->MagRipCoeff[i]->mag);
				bw->Write(Cal->DirCoupler->PhRipCoeff[i]->freq);
				bw->Write(Cal->DirCoupler->PhRipCoeff[i]->offset);
				bw->Write(Cal->DirCoupler->PhRipCoeff[i]->mag);
			}

			// 11-17-2007 - Write out the DC Offset of the Phase and MAg Open-Short calibrations

			bw->Write(Cal->DirCoupler->PhaseDCOffset);
			bw->Write(Cal->DirCoupler->MagDCOffset);

			// 10-01-2007 - Write out the low-ref-level Iphase values for TRAN

			bw->Write(Cal->TxDet->ImidLo);
			bw->Write(Cal->TxDet->ImagLo);

			// Write out the directional coupler calibration data - to ease debugging.
			for(int i=0; i<PHASECALGRIDSIZE; i++)
			{
				bw->Write(Cal->DirCoupler->phaseError[i]);
				bw->Write(Cal->DirCoupler->magError[i]);
				bw->Write(Cal->DirCoupler->openAngle[i]);
				bw->Write(Cal->DirCoupler->shortAngle[i]);
			}

			fs->Flush();
			fs->Close();

			DialogResult = ::DialogResult::OK;		// Close the form
		 }


private: System::Void InstrumentCal_Load(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void Cancelbutton_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			MessageBox::Show("Instrument calibration canceled. The instrument cal data has not been saved."
				"\n\rThe VNA program should be exited then restarted if you wish to continue "
				"operation with the previous instrument calibration data.",
				"Warning: Instrument Cal Canceled", MessageBoxButtons::OK, MessageBoxIcon::Warning);

			DialogResult = ::DialogResult::Cancel;
		 }

private: System::Void TxLongCableShortButton_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 			 // Reflection Amplitude Detector Calibration Terminated
			 // Captures Magnitude, I-phase, and Q-phase at zero dbm for determing characteristic
			 // impedance of the 1-meter calibration cable (it's Surge Impedance).
			 
			VNA_RXBUFFER *RxBuf = new VNA_RXBUFFER;
			VNA_TXBUFFER *TxBuf = new VNA_TXBUFFER;
			int Fdesired;
			int i;
			

			TxBuf->ReplyType = 0;
			TxBuf->MeasureDelay = 0;			// No Measurement delay
			TxBuf->QDAClevel = QDAC_ZERODBM;	// Reference level

			progressBar1->Value = 0;

			// Collect detailed 1024 point sweep for Terminated Reflection Data

			for (i=0; i<1024; i++)
            {
				// Compute spot frequency

				Fdesired = GetFreqFromPhaseCalGrid(i);

				TxBuf->TxAccum = FG->DDS(Fdesired);		
				TxBuf->IDAClevelHi = MAX_DDS_LEVEL;		// Max transmit level
				TxBuf->ReplyType = 0;
				TxBuf->MeasureDelay = 0;
				TxBuf->QDAClevel = QDAC_ZERODBM;	// Reference level

				for (int k=0; k<7; k++)
				{
                    VNA->WriteRead(TxBuf, RxBuf, DIR_REFL);
					BufferI[k] = RxBuf->ReflPI;
					BufferQ[k] = RxBuf->ReflPQ;
					BufferM[k] = RxBuf->ReflMQ;
				}
				ReflLongDetailMagPhShort[i, MagQ] = Median7(BufferM);
				ReflLongDetailMagPhShort[i, PhaseI] = Median7(BufferI);		// filter the raw readings
				ReflLongDetailMagPhShort[i, PhaseQ] = Median7(BufferQ);

				progressBar1->Value = i * progressBar1->Maximum/1024;
				if(i%10 == 0)
                    progressBar1->Update();

			}

			CalStepPass[7] = true;
			TxLongCableShortStat->Image = ICO1Label->Image;	// check mark (OK)
			TxLongCableShortStat->Visible = true;
		 }
};
}