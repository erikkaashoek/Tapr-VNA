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
#include "DisplayRoutines.h"
#include "USB_EZ_interface.h"
#include "Constants.h"
#include "Mockup.h"
#include <math.h>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace VNAR3
{

	/// <summary> 
	/// Summary for Calibration
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>

	/// Fixture Calibration
	public ref class Calibration : public System::Windows::Forms::Form
	{
	public: 
		Calibration(InstrumentCalDataSet^ CalDS,	///< Instrument Calibration Dataset
			 VNADevice^ VNADev,			///< VNA Hardware Device
			 FrequencyGrid^ fg)			///< Frequency Grid of parent
		{
			InitializeComponent();
			VNA = VNADev;			///< VNA hardware device
			Cal = CalDS;			///< Calibration Dataset
			FG = fg;				///< Frequency Grid

			BufferI = gcnew array<Int32>(7);
			BufferQ = gcnew array<Int32>(7);
			BufferM = gcnew array<Int32>(7);
			BufferN = gcnew array<Int32>(7);
			BufferP = gcnew array<Int32>(7);
			BufferR = gcnew array<Int32>(7);
			BufferS = gcnew array<Int32>(7);
		}
        
	protected: 
		~Calibration()
		{
			if(components)
				delete(components);
		}
	private: System::Windows::Forms::Button^  shortCal;
	private: System::Windows::Forms::Button^  openCal;
	private: System::Windows::Forms::Button^  termCal;
	private: System::Windows::Forms::Button^  thruCal;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  CancelButton;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  statShort;
	private: System::Windows::Forms::Button^  calSave;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  statOpen;
	private: System::Windows::Forms::Label^  statTerm;
	private: System::Windows::Forms::Label^  statThru;
	private: System::Windows::Forms::ProgressBar^  calProgressBar;
	private: System::Windows::Forms::GroupBox^  LinLogBox;
	private: System::Windows::Forms::RadioButton^  LogFreqButton;
	private: System::Windows::Forms::RadioButton^  LinearFreqButton;




	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Calibration::typeid));
			this->shortCal = (gcnew System::Windows::Forms::Button());
			this->openCal = (gcnew System::Windows::Forms::Button());
			this->termCal = (gcnew System::Windows::Forms::Button());
			this->thruCal = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->CancelButton = (gcnew System::Windows::Forms::Button());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->statShort = (gcnew System::Windows::Forms::Label());
			this->calSave = (gcnew System::Windows::Forms::Button());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->statOpen = (gcnew System::Windows::Forms::Label());
			this->statTerm = (gcnew System::Windows::Forms::Label());
			this->statThru = (gcnew System::Windows::Forms::Label());
			this->calProgressBar = (gcnew System::Windows::Forms::ProgressBar());
			this->LinLogBox = (gcnew System::Windows::Forms::GroupBox());
			this->LogFreqButton = (gcnew System::Windows::Forms::RadioButton());
			this->LinearFreqButton = (gcnew System::Windows::Forms::RadioButton());
			this->LinLogBox->SuspendLayout();
			this->SuspendLayout();
			// 
			// shortCal
			// 
			this->shortCal->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->shortCal->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->shortCal->Location = System::Drawing::Point(410, 156);
			this->shortCal->Name = L"shortCal";
			this->shortCal->Size = System::Drawing::Size(184, 40);
			this->shortCal->TabIndex = 0;
			this->shortCal->Text = L"\"Short\" Calibration";
			this->shortCal->Click += gcnew System::EventHandler(this, &Calibration::shortCal_Click);
			// 
			// openCal
			// 
			this->openCal->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->openCal->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->openCal->Location = System::Drawing::Point(410, 212);
			this->openCal->Name = L"openCal";
			this->openCal->Size = System::Drawing::Size(184, 40);
			this->openCal->TabIndex = 1;
			this->openCal->Text = L"\"Open\" Calibration";
			this->openCal->Click += gcnew System::EventHandler(this, &Calibration::openCal_Click);
			// 
			// termCal
			// 
			this->termCal->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->termCal->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->termCal->Location = System::Drawing::Point(410, 268);
			this->termCal->Name = L"termCal";
			this->termCal->Size = System::Drawing::Size(184, 40);
			this->termCal->TabIndex = 2;
			this->termCal->Text = L"\"50-ohm Term\" Calibration";
			this->termCal->Click += gcnew System::EventHandler(this, &Calibration::termCal_Click);
			// 
			// thruCal
			// 
			this->thruCal->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->thruCal->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->thruCal->Location = System::Drawing::Point(410, 324);
			this->thruCal->Name = L"thruCal";
			this->thruCal->Size = System::Drawing::Size(184, 40);
			this->thruCal->TabIndex = 3;
			this->thruCal->Text = L"\"Through\" Calibration";
			this->thruCal->Click += gcnew System::EventHandler(this, &Calibration::thruCal_Click);
			// 
			// label1
			// 
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Font = (gcnew System::Drawing::Font(L"Verdana", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(192, 24);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(248, 23);
			this->label1->TabIndex = 4;
			this->label1->Text = L"Fixture Calibration";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// CancelButton
			// 
			this->CancelButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->CancelButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->CancelButton->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->CancelButton->Location = System::Drawing::Point(386, 396);
			this->CancelButton->Name = L"CancelButton";
			this->CancelButton->Size = System::Drawing::Size(96, 23);
			this->CancelButton->TabIndex = 6;
			this->CancelButton->Text = L"Cancel";
			// 
			// label2
			// 
			this->label2->BackColor = System::Drawing::Color::Transparent;
			this->label2->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->label2->Location = System::Drawing::Point(114, 164);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(272, 32);
			this->label2->TabIndex = 7;
			this->label2->Text = L"1. Connect \'short\' to TX at measurement plane.";
			// 
			// label3
			// 
			this->label3->BackColor = System::Drawing::Color::Transparent;
			this->label3->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label3->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->label3->Location = System::Drawing::Point(114, 220);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(272, 32);
			this->label3->TabIndex = 8;
			this->label3->Text = L"2. Connect \'open\' to TX at measurement plane.";
			// 
			// label4
			// 
			this->label4->BackColor = System::Drawing::Color::Transparent;
			this->label4->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label4->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->label4->Location = System::Drawing::Point(114, 276);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(272, 32);
			this->label4->TabIndex = 9;
			this->label4->Text = L"3. Connect \'50-ohm termination\' to TX at measurement plane.";
			// 
			// label5
			// 
			this->label5->BackColor = System::Drawing::Color::Transparent;
			this->label5->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label5->Location = System::Drawing::Point(114, 332);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(280, 32);
			this->label5->TabIndex = 10;
			this->label5->Text = L"4. Connect TX through to RX at measurement plane.";
			// 
			// statShort
			// 
			this->statShort->BackColor = System::Drawing::Color::Transparent;
			this->statShort->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"statShort.Image")));
			this->statShort->Location = System::Drawing::Point(34, 164);
			this->statShort->Name = L"statShort";
			this->statShort->Size = System::Drawing::Size(32, 32);
			this->statShort->TabIndex = 11;
			this->statShort->Visible = false;
			// 
			// calSave
			// 
			this->calSave->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->calSave->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->calSave->Location = System::Drawing::Point(186, 396);
			this->calSave->Name = L"calSave";
			this->calSave->Size = System::Drawing::Size(152, 24);
			this->calSave->TabIndex = 15;
			this->calSave->Text = L"Save Cal Results...";
			this->calSave->Click += gcnew System::EventHandler(this, &Calibration::calSave_Click);
			// 
			// label6
			// 
			this->label6->BackColor = System::Drawing::Color::Transparent;
			this->label6->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label6->Location = System::Drawing::Point(26, 132);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(56, 23);
			this->label6->TabIndex = 16;
			this->label6->Text = L"Status";
			// 
			// statOpen
			// 
			this->statOpen->BackColor = System::Drawing::Color::Transparent;
			this->statOpen->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"statOpen.Image")));
			this->statOpen->Location = System::Drawing::Point(34, 220);
			this->statOpen->Name = L"statOpen";
			this->statOpen->Size = System::Drawing::Size(32, 32);
			this->statOpen->TabIndex = 17;
			this->statOpen->Visible = false;
			// 
			// statTerm
			// 
			this->statTerm->BackColor = System::Drawing::Color::Transparent;
			this->statTerm->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"statTerm.Image")));
			this->statTerm->Location = System::Drawing::Point(34, 276);
			this->statTerm->Name = L"statTerm";
			this->statTerm->Size = System::Drawing::Size(32, 32);
			this->statTerm->TabIndex = 18;
			this->statTerm->Visible = false;
			// 
			// statThru
			// 
			this->statThru->BackColor = System::Drawing::Color::Transparent;
			this->statThru->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"statThru.Image")));
			this->statThru->Location = System::Drawing::Point(34, 332);
			this->statThru->Name = L"statThru";
			this->statThru->Size = System::Drawing::Size(32, 32);
			this->statThru->TabIndex = 19;
			this->statThru->Visible = false;
			// 
			// calProgressBar
			// 
			this->calProgressBar->Location = System::Drawing::Point(114, 444);
			this->calProgressBar->Maximum = 1024;
			this->calProgressBar->Name = L"calProgressBar";
			this->calProgressBar->Size = System::Drawing::Size(472, 16);
			this->calProgressBar->TabIndex = 20;
			// 
			// LinLogBox
			// 
			this->LinLogBox->BackColor = System::Drawing::Color::Transparent;
			this->LinLogBox->Controls->Add(this->LogFreqButton);
			this->LinLogBox->Controls->Add(this->LinearFreqButton);
			this->LinLogBox->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->LinLogBox->Location = System::Drawing::Point(108, 64);
			this->LinLogBox->Name = L"LinLogBox";
			this->LinLogBox->Size = System::Drawing::Size(443, 73);
			this->LinLogBox->TabIndex = 21;
			this->LinLogBox->TabStop = false;
			this->LinLogBox->Text = L"Linear / Log Frequency Grid";
			// 
			// LogFreqButton
			// 
			this->LogFreqButton->AutoSize = true;
			this->LogFreqButton->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->LogFreqButton->Location = System::Drawing::Point(7, 43);
			this->LogFreqButton->Name = L"LogFreqButton";
			this->LogFreqButton->Size = System::Drawing::Size(420, 20);
			this->LogFreqButton->TabIndex = 1;
			this->LogFreqButton->TabStop = true;
			this->LogFreqButton->Text = L"Use Log Frequency Grid for Fixture Calibration (constant Q)";
			this->LogFreqButton->UseVisualStyleBackColor = true;
			this->LogFreqButton->CheckedChanged += gcnew System::EventHandler(this, &Calibration::FreqModeChanged);
			// 
			// LinearFreqButton
			// 
			this->LinearFreqButton->AutoSize = true;
			this->LinearFreqButton->Checked = true;
			this->LinearFreqButton->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->LinearFreqButton->Location = System::Drawing::Point(7, 20);
			this->LinearFreqButton->Name = L"LinearFreqButton";
			this->LinearFreqButton->Size = System::Drawing::Size(408, 20);
			this->LinearFreqButton->TabIndex = 0;
			this->LinearFreqButton->TabStop = true;
			this->LinearFreqButton->Text = L"Use Linear Frequency Grid for Fixture Calibration (default)";
			this->LinearFreqButton->UseVisualStyleBackColor = true;
			this->LinearFreqButton->CheckedChanged += gcnew System::EventHandler(this, &Calibration::FreqModeChanged);
			// 
			// Calibration
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->AutoValidate = System::Windows::Forms::AutoValidate::EnableAllowFocusChange;
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"$this.BackgroundImage")));
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->ClientSize = System::Drawing::Size(640, 492);
			this->Controls->Add(this->LinLogBox);
			this->Controls->Add(this->calProgressBar);
			this->Controls->Add(this->statThru);
			this->Controls->Add(this->statTerm);
			this->Controls->Add(this->statOpen);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->calSave);
			this->Controls->Add(this->statShort);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->CancelButton);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->thruCal);
			this->Controls->Add(this->termCal);
			this->Controls->Add(this->openCal);
			this->Controls->Add(this->shortCal);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->Name = L"Calibration";
			this->Text = L"Calibration";
			this->LinLogBox->ResumeLayout(false);
			this->LinLogBox->PerformLayout();
			this->ResumeLayout(false);

		}		

		private: VNADevice^ VNA;		///< Vector Network Analyzer hardware object
		private: FrequencyGrid^ FG;		///< Our Frequency Grid
		private: InstrumentCalDataSet^ Cal;		///< Calibration Data Set for the instrument
		public: SaveFileDialog^ outfile;	///< Saved File Structure

		array<Int32>^ BufferI;			///< Holds raw readings from VNA during data collection
		array<Int32>^ BufferQ;
		array<Int32>^ BufferM;
		array<Int32>^ BufferN;
		array<Int32>^ BufferP;
		array<Int32>^ BufferR;
		array<Int32>^ BufferS;

		/// Fixture SHORTed Load Calibration button click handler
	private: System::Void shortCal_Click(System::Object^  sender, System::EventArgs^  e)
			 {
				VNA_RXBUFFER *RxBuf = new VNA_RXBUFFER;
				VNA_TXBUFFER *TxBuf = new VNA_TXBUFFER;
				double fmagnitude;			// 0 to 1
				double fphase;				// -180 to +180	
				double& rmag = fmagnitude;
				double& rphs = fphase;
				MeasurementSet^ calPoint = gcnew MeasurementSet;
				VNA->SetMode(M_SHORT);

				for (long i=0; i<PHASECALGRIDSIZE; i++)
				{
					// Compute spot frequency

					int Fdesired;
					Fdesired = GetFreqFromFixtureCalGrid(i, LogFreqButton->Checked);

					TxBuf->TxAccum = FG->DDS(Fdesired);		
					TxBuf->IDAClevelHi = MAX_DDS_LEVEL;		// Max transmit level
					TxBuf->ReplyType = 0;
					TxBuf->MeasureDelay = 0;
					TxBuf->QDAClevel = QDAC_ZERODBM;	// Reference level


					for (int k=0; k<7; k++)
					{
                        VNA->WriteRead(TxBuf, RxBuf);
						BufferI[k] = RxBuf->ReflPI;
						BufferQ[k] = RxBuf->ReflPQ;
						BufferM[k] = RxBuf->ReflMQ;
					}
					calPoint->ReflPI = Median7(BufferI);		// filter the raw readings
					calPoint->ReflPQ = Median7(BufferQ);
					calPoint->ReflMQ = Median7(BufferM);

					Cal->ResolveReflPolar(calPoint, (int)Fdesired, rmag, rphs, true);

					// convert it to rectangular
					double fx = fmagnitude * cos(fphase * DEGR2RAD);
					double fy = fmagnitude * sin(fphase * DEGR2RAD);

					// save it as S11-meas-short
					Cal->S11shortReal[i] = fx; Cal->S11shortImag[i] = fy;

					calProgressBar->Value = i;
					if(i%20 == 0)
						calProgressBar->Update();
				}
				statShort->Visible = true;
				// update the type of Fixture Calibration Frequency mode in the dataset
				Cal->FixtureCalLogFreqMode = LogFreqButton->Checked;

			 }

		/// Fixture OPEN Load Calibration button click handler
	private: System::Void openCal_Click(System::Object^  sender, System::EventArgs^  e)
			{
				VNA_RXBUFFER *RxBuf = new VNA_RXBUFFER;
				VNA_TXBUFFER *TxBuf = new VNA_TXBUFFER;
				double fmagnitude;			// 0 to 1
				double fphase;				// -180 to +180	
				double& rmag = fmagnitude;
				double& rphs = fphase;
				MeasurementSet^ calPoint = gcnew MeasurementSet;
				VNA->SetMode(M_OPEN);

				 // run a sweep of 1024 points, collecting S11 data 'short'

				for (long i=0; i<PHASECALGRIDSIZE; i++)
				{
					// Compute spot frequency

					int Fdesired;
					Fdesired = GetFreqFromFixtureCalGrid(i, LogFreqButton->Checked);

					TxBuf->TxAccum = FG->DDS(Fdesired);		
					TxBuf->IDAClevelHi = MAX_DDS_LEVEL;		// Max transmit level
					TxBuf->ReplyType = 0;
					TxBuf->MeasureDelay = 0;
					TxBuf->QDAClevel = QDAC_ZERODBM;	// Reference level


					for (int k=0; k<7; k++)
					{
                        VNA->WriteRead(TxBuf, RxBuf);
						BufferI[k] = RxBuf->ReflPI;
						BufferQ[k] = RxBuf->ReflPQ;
						BufferM[k] = RxBuf->ReflMQ;
					}
					calPoint->ReflPI = Median7(BufferI);		// filter the raw readings
					calPoint->ReflPQ = Median7(BufferQ);
					calPoint->ReflMQ = Median7(BufferM);

					Cal->ResolveReflPolar(calPoint, (int)Fdesired, rmag, rphs, true);

					// convert it to rectangular
					double fx = fmagnitude * cos(fphase * DEGR2RAD);
					double fy = fmagnitude * sin(fphase * DEGR2RAD);

					// save it as S11-meas-open
					Cal->S11openReal[i] = fx; Cal->S11openImag[i] = fy;

					calProgressBar->Value = i;
					if(i%20 == 0)
						calProgressBar->Update();
				}
				statOpen->Visible = true;
				// update the type of Fixture Calibration Frequency mode in the dataset
				Cal->FixtureCalLogFreqMode = LogFreqButton->Checked;

			}

		/// Fixture TERMINATED Load Calibration button click handler
	private: System::Void termCal_Click(System::Object^  sender, System::EventArgs^  e)
			{
				VNA_RXBUFFER *RxBuf = new VNA_RXBUFFER;
				VNA_TXBUFFER *TxBuf = new VNA_TXBUFFER;
				double fmagnitude;			// 0 to 1
				double fphase;				// -180 to +180	
				double& rmag = fmagnitude;
				double& rphs = fphase;
				MeasurementSet^ calPoint = gcnew MeasurementSet;

				VNA->SetMode(M_LOAD);
				 // run a sweep of 1024 points, collecting S11 data 'short'

				for (long i=0; i<PHASECALGRIDSIZE; i++)
				{
					// Compute spot frequency

					int Fdesired;
					Fdesired = GetFreqFromFixtureCalGrid(i, LogFreqButton->Checked);

					TxBuf->TxAccum = FG->DDS(Fdesired);		
					TxBuf->IDAClevelHi = MAX_DDS_LEVEL;		// Max transmit level
					TxBuf->ReplyType = 0;
					TxBuf->MeasureDelay = 0;
					TxBuf->QDAClevel = QDAC_ZERODBM;	// Reference level

					for (int k=0; k<7; k++)
					{
                        VNA->WriteRead(TxBuf, RxBuf);
						BufferI[k] = RxBuf->ReflPI;
						BufferQ[k] = RxBuf->ReflPQ;
						BufferM[k] = RxBuf->ReflMQ;
					}
					calPoint->ReflPI = Median7(BufferI);		// filter the raw readings
					calPoint->ReflPQ = Median7(BufferQ);
					calPoint->ReflMQ = Median7(BufferM);
					VNA->WriteRead(TxBuf, RxBuf);

					Cal->ResolveReflPolar(calPoint, (int)Fdesired, rmag, rphs, true);

					// convert it to rectangular
					double fx = fmagnitude * cos(fphase * DEGR2RAD);
					double fy = fmagnitude * sin(fphase * DEGR2RAD);

					// save it as S11-meas-short
					Cal->S11termReal[i] = fx; Cal->S11termImag[i] = fy;

					calProgressBar->Value = i;
					if(i%20 == 0)
						calProgressBar->Update();
				}
				statTerm->Visible = true;
				// update the type of Fixture Calibration Frequency mode in the dataset
				Cal->FixtureCalLogFreqMode = LogFreqButton->Checked;

			}

		/// Fixture Through Calibration button click handler
	private: System::Void thruCal_Click(System::Object^  sender, System::EventArgs^  e)
			{
				VNA_RXBUFFER *RxBuf = new VNA_RXBUFFER;
				VNA_TXBUFFER *TxBuf = new VNA_TXBUFFER;
				double fmagnitude;			// 0 to 1
				double fphase;				// -180 to +180	
				double& rmag = fmagnitude;
				double& rphs = fphase;
				MeasurementSet^ calPoint = gcnew MeasurementSet;

				VNA->SetMode(M_THROUGH);

				// run a sweep of 1024 points, collecting S21 data 'thru'

				for (long i=0; i<PHASECALGRIDSIZE; i++)
				{
					// Compute spot frequency

					int Fdesired;
					Fdesired = GetFreqFromFixtureCalGrid(i, LogFreqButton->Checked);

					TxBuf->TxAccum = FG->DDS(Fdesired);

					TxBuf->IDAClevelHi = TxLevLinear(0);	// Set High Tx Level
					TxBuf->IDAClevelLo = TxLevLinear(0 - TARGETLOMAG);	// Set Low TX Level
					TxBuf->IDAClevelMid = TxLevLinear(0 - TARGETMIDMAG);	// Set Mid TX Level

					// Set the Lo phase tran measurement level for the target   09-30-2007
					TxBuf->IDAClevelPhLow = TxLevLinear(0 - TARGETPHLOMAG);	// Set Lo Phase Level

					TxBuf->ReplyType = 0;
					TxBuf->MeasureDelay = 0;
					TxBuf->QDAClevel = QDAC_ZERODBM;	// Reference level


					for (int k=0; k<7; k++)
					{
                        VNA->WriteRead(TxBuf, RxBuf);
						BufferI[k] = RxBuf->TranPI;
						BufferQ[k] = RxBuf->TranPQ;
						BufferM[k] = RxBuf->TranMQHi;
						BufferN[k] = RxBuf->TranMQLo;
						BufferP[k] = RxBuf->TranMQMid;
						BufferR[k] = RxBuf->TranPILow;
						BufferS[k] = RxBuf->TranPQLow;
					}

					calPoint->TranPI = Median7(BufferI);		// filter the raw readings
					calPoint->TranPQ = Median7(BufferQ);
					calPoint->TranMQHi = Median7(BufferM);
					calPoint->TranMQLo = Median7(BufferN);
					calPoint->TranMQMid = Median7(BufferP);
					calPoint->TranPILow = Median7(BufferR);
					calPoint->TranPQLow = Median7(BufferS);

					Cal->ResolveTranPolar(calPoint, (int)Fdesired, rmag, rphs);

					// convert it to rectangular
					double fx = fmagnitude * cos(fphase * DEGR2RAD);
					double fy = fmagnitude * sin(fphase * DEGR2RAD);

					// save it as S21-meas-through
					Cal->ThReal[i] = fx; Cal->ThImag[i] = fy;

					calProgressBar->Value = i;
					if(i%20 == 0)
						calProgressBar->Update();
				}
				statThru->Visible = true;
				// update the type of Fixture Calibration Frequency mode in the dataset
				Cal->FixtureCalLogFreqMode = LogFreqButton->Checked;

			}

		/// Fixture Save Calibration button click handler
	private: System::Void calSave_Click(System::Object^  sender, System::EventArgs^  e)
			{
				// Compute Error Terms
				CalToErrorTerms(Cal);

				// update the type of Fixture Calibration Frequency mode in the dataset
				Cal->FixtureCalLogFreqMode = LogFreqButton->Checked;

				// Save results to file
				outfile = gcnew SaveFileDialog();
				outfile->Filter = "calibration files (*.cal)|*.cal";
				outfile->AddExtension = true;
				if (outfile->ShowDialog() == ::DialogResult::OK)
				{
					SaveCalDataSet(outfile, Cal);		// InstrumentCalDataSet holds Fixture CalData also.
														// Extract only Fixture CalData during save.
					DialogResult = ::DialogResult::OK;	// Let caller know that Fixture Cal saved a valid file
				}
				else
					DialogResult = ::DialogResult::Cancel;
			}


private: System::Void FreqModeChanged(System::Object^  sender, System::EventArgs^  e)
		 {
			 // user changed the Linear / Log Mode. Invalidate the calibrations.

			 statThru->Visible = false;
			 statTerm->Visible = false;
			 statOpen->Visible = false;
			 statShort->Visible = false;

			 Refresh();
		 }
};
}