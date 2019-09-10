#pragma once
#include "stdafx.h"
#include "DataDisplay.h"
#include "Mockup.h"
#include "Constants.h"
#using <mscorlib.dll>
#include <Windows.h>
#include <MMSystem.h>
#using<system.dll>

#include "Constants.h"
#include "AudioInput.h"
#include "USB_EZ_interface.h"
 
extern HWAVEIN      hWaveIn;

//#include "objbase.h"

extern int IFREQ;
extern int audioRefLevel;
extern int sampleRate;
extern int oldSampleRate;
#define SAMPLERATES	6
static int sampleRateTable [SAMPLERATES] = {
	192000,
	96000,
	48000,
	44100,
	24000,
	12000
};

#if 0
extern 			MMRESULT mRes;
extern 			HMIXER  hmx;
extern 						MIXERLINE						mixerLine;
	extern									MIXERLINECONTROLS mlc;
extern MIXERCONTROL mc;
	extern									MIXERCONTROLDETAILS mcd ;
	extern									MIXERCONTROLDETAILS_UNSIGNED mcdu;
#endif


extern int OpenAudio();

using namespace System;


namespace VNAR3 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for SerialPort
	/// </summary>
	public ref class SerialPort : public System::Windows::Forms::Form
	{
	public:
		SerialPort(System::IO::Ports::SerialPort^  serialPort1,VNADevice^ VNADev)
		{
			InitializeComponent();
			Port = serialPort1;
			VNA = VNADev;

			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~SerialPort()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ComboBox^  comboBox1;
	private: System::Windows::Forms::Button^  OK_Button;
	private: System::Windows::Forms::Button^  CANCEL_Button;
	private: System::Windows::Forms::TextBox^  reference;


	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		System::IO::Ports::SerialPort^  Port;
	private: System::ComponentModel::IContainer^  components;
			 /// </summary>

	private: System::Windows::Forms::ComboBox^  sampleRateBox;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  IFreq;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Timer^  timer1;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::ComboBox^  comboBox2;
	private: System::Windows::Forms::ComboBox^  comboBox3;
	private: System::Windows::Forms::TrackBar^  volumeBar;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::TextBox^  refLevelBox;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::TextBox^  minFreqBox;
	private: System::Windows::Forms::TextBox^  maxFreqBox;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::Label^  label11;
	private: System::Windows::Forms::Label^  label12;
	private: System::Windows::Forms::Label^  label13;
	private: System::Windows::Forms::ComboBox^  hardwareBox;


			 VNADevice^ VNA;					///< Vector Network Analyzer hardware object

			 
#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->OK_Button = (gcnew System::Windows::Forms::Button());
			this->CANCEL_Button = (gcnew System::Windows::Forms::Button());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->sampleRateBox = (gcnew System::Windows::Forms::ComboBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->IFreq = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->reference = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->comboBox2 = (gcnew System::Windows::Forms::ComboBox());
			this->comboBox3 = (gcnew System::Windows::Forms::ComboBox());
			this->volumeBar = (gcnew System::Windows::Forms::TrackBar());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->refLevelBox = (gcnew System::Windows::Forms::TextBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->minFreqBox = (gcnew System::Windows::Forms::TextBox());
			this->maxFreqBox = (gcnew System::Windows::Forms::TextBox());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->hardwareBox = (gcnew System::Windows::Forms::ComboBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->volumeBar))->BeginInit();
			this->SuspendLayout();
			// 
			// comboBox1
			// 
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Location = System::Drawing::Point(383, 12);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(143, 21);
			this->comboBox1->TabIndex = 0;
			this->comboBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &SerialPort::comboBox1_SelectedIndexChanged);
			// 
			// OK_Button
			// 
			this->OK_Button->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->OK_Button->Location = System::Drawing::Point(383, 205);
			this->OK_Button->Name = L"OK_Button";
			this->OK_Button->Size = System::Drawing::Size(64, 28);
			this->OK_Button->TabIndex = 1;
			this->OK_Button->Text = L"OK";
			this->OK_Button->UseVisualStyleBackColor = true;
			this->OK_Button->Click += gcnew System::EventHandler(this, &SerialPort::button1_Click);
			// 
			// CANCEL_Button
			// 
			this->CANCEL_Button->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->CANCEL_Button->Location = System::Drawing::Point(462, 205);
			this->CANCEL_Button->Name = L"CANCEL_Button";
			this->CANCEL_Button->Size = System::Drawing::Size(64, 28);
			this->CANCEL_Button->TabIndex = 2;
			this->CANCEL_Button->Text = L"Cancel";
			this->CANCEL_Button->UseVisualStyleBackColor = true;
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(383, 171);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(143, 28);
			this->button3->TabIndex = 3;
			this->button3->Text = L"Refresh";
			this->button3->UseVisualStyleBackColor = true;
			// 
			// sampleRateBox
			// 
			this->sampleRateBox->FormattingEnabled = true;
			this->sampleRateBox->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"192 kHz", L"  96 kHz", L"  48 kHz", L" 4.1 kHz", 
				L"  24 kHz", L"  12 kHz"});
			this->sampleRateBox->Location = System::Drawing::Point(383, 90);
			this->sampleRateBox->Name = L"sampleRateBox";
			this->sampleRateBox->Size = System::Drawing::Size(143, 21);
			this->sampleRateBox->TabIndex = 4;
			this->sampleRateBox->SelectedIndexChanged += gcnew System::EventHandler(this, &SerialPort::sampleRateBox_SelectedIndexChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(352, 93);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(25, 13);
			this->label1->TabIndex = 5;
			this->label1->Text = L"SR:";
			// 
			// IFreq
			// 
			this->IFreq->Location = System::Drawing::Point(442, 117);
			this->IFreq->Name = L"IFreq";
			this->IFreq->Size = System::Drawing::Size(84, 20);
			this->IFreq->TabIndex = 6;
			this->IFreq->Text = L"10000";
			this->IFreq->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->IFreq->TextChanged += gcnew System::EventHandler(this, &SerialPort::IFreq_TextChanged);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(352, 120);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(19, 13);
			this->label2->TabIndex = 7;
			this->label2->Text = L"IF:";
			this->label2->Click += gcnew System::EventHandler(this, &SerialPort::label2_Click);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(352, 146);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(60, 13);
			this->label3->TabIndex = 8;
			this->label3->Text = L"Reference:";
			// 
			// reference
			// 
			this->reference->Location = System::Drawing::Point(441, 143);
			this->reference->Name = L"reference";
			this->reference->Size = System::Drawing::Size(83, 20);
			this->reference->TabIndex = 9;
			this->reference->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(530, 146);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(20, 13);
			this->label4->TabIndex = 10;
			this->label4->Text = L"dB";
			// 
			// timer1
			// 
			this->timer1->Enabled = true;
			this->timer1->Interval = 500;
			this->timer1->Tick += gcnew System::EventHandler(this, &SerialPort::timer1_Tick);
			// 
			// comboBox2
			// 
			this->comboBox2->FormattingEnabled = true;
			this->comboBox2->Location = System::Drawing::Point(116, 39);
			this->comboBox2->Name = L"comboBox2";
			this->comboBox2->Size = System::Drawing::Size(410, 21);
			this->comboBox2->TabIndex = 11;
			this->comboBox2->SelectedIndexChanged += gcnew System::EventHandler(this, &SerialPort::comboBox2_SelectedIndexChanged);
			// 
			// comboBox3
			// 
			this->comboBox3->FormattingEnabled = true;
			this->comboBox3->Location = System::Drawing::Point(116, 66);
			this->comboBox3->Name = L"comboBox3";
			this->comboBox3->Size = System::Drawing::Size(410, 21);
			this->comboBox3->TabIndex = 12;
			// 
			// volumeBar
			// 
			this->volumeBar->Location = System::Drawing::Point(30, 205);
			this->volumeBar->Maximum = 100;
			this->volumeBar->Name = L"volumeBar";
			this->volumeBar->Size = System::Drawing::Size(275, 45);
			this->volumeBar->TabIndex = 13;
			this->volumeBar->Scroll += gcnew System::EventHandler(this, &SerialPort::volumeBar_Scroll);
			this->volumeBar->ValueChanged += gcnew System::EventHandler(this, &SerialPort::volumeBar_ValueChanged);
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(27, 93);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(83, 13);
			this->label5->TabIndex = 14;
			this->label5->Text = L"Ref attenuation:";
			this->label5->Click += gcnew System::EventHandler(this, &SerialPort::label5_Click);
			// 
			// refLevelBox
			// 
			this->refLevelBox->Location = System::Drawing::Point(116, 90);
			this->refLevelBox->Name = L"refLevelBox";
			this->refLevelBox->Size = System::Drawing::Size(83, 20);
			this->refLevelBox->TabIndex = 15;
			this->refLevelBox->Text = L"0";
			this->refLevelBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->refLevelBox->TextChanged += gcnew System::EventHandler(this, &SerialPort::refLevelBox_TextChanged);
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(27, 42);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(63, 13);
			this->label6->TabIndex = 16;
			this->label6->Text = L"Audio input:";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(352, 15);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(29, 13);
			this->label7->TabIndex = 17;
			this->label7->Text = L"Port:";
			// 
			// minFreqBox
			// 
			this->minFreqBox->Location = System::Drawing::Point(116, 117);
			this->minFreqBox->Name = L"minFreqBox";
			this->minFreqBox->Size = System::Drawing::Size(84, 20);
			this->minFreqBox->TabIndex = 18;
			this->minFreqBox->Text = L"0.1";
			this->minFreqBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->minFreqBox->TextChanged += gcnew System::EventHandler(this, &SerialPort::minFreqBox_TextChanged);
			// 
			// maxFreqBox
			// 
			this->maxFreqBox->Location = System::Drawing::Point(116, 143);
			this->maxFreqBox->Name = L"maxFreqBox";
			this->maxFreqBox->Size = System::Drawing::Size(84, 20);
			this->maxFreqBox->TabIndex = 19;
			this->maxFreqBox->Text = L"4400";
			this->maxFreqBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->maxFreqBox->TextChanged += gcnew System::EventHandler(this, &SerialPort::maxFreqBox_TextChanged);
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(27, 120);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(73, 13);
			this->label8->TabIndex = 20;
			this->label8->Text = L"Min HW Freq:";
			this->label8->Click += gcnew System::EventHandler(this, &SerialPort::label8_Click);
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(27, 146);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(76, 13);
			this->label9->TabIndex = 21;
			this->label9->Text = L"Max HW Freq:";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(206, 120);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(29, 13);
			this->label10->TabIndex = 22;
			this->label10->Text = L"MHz";
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(206, 146);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(29, 13);
			this->label11->TabIndex = 23;
			this->label11->Text = L"MHz";
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Location = System::Drawing::Point(206, 93);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(20, 13);
			this->label12->TabIndex = 24;
			this->label12->Tag = L"";
			this->label12->Text = L"dB";
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Location = System::Drawing::Point(27, 174);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(56, 13);
			this->label13->TabIndex = 25;
			this->label13->Text = L"Hardware:";
			this->label13->Click += gcnew System::EventHandler(this, &SerialPort::label13_Click);
			// 
			// hardwareBox
			// 
			this->hardwareBox->FormattingEnabled = true;
			this->hardwareBox->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"SI5351", L"ADF4351", L"NanoVNA"});
			this->hardwareBox->Location = System::Drawing::Point(116, 171);
			this->hardwareBox->Name = L"hardwareBox";
			this->hardwareBox->Size = System::Drawing::Size(83, 21);
			this->hardwareBox->TabIndex = 27;
			this->hardwareBox->Text = L"SI5351";
			// 
			// SerialPort
			// 
			this->AcceptButton = this->OK_Button;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->CANCEL_Button;
			this->ClientSize = System::Drawing::Size(551, 261);
			this->Controls->Add(this->hardwareBox);
			this->Controls->Add(this->label13);
			this->Controls->Add(this->label12);
			this->Controls->Add(this->label11);
			this->Controls->Add(this->label10);
			this->Controls->Add(this->label9);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->maxFreqBox);
			this->Controls->Add(this->minFreqBox);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->refLevelBox);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->volumeBar);
			this->Controls->Add(this->comboBox3);
			this->Controls->Add(this->comboBox2);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->reference);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->IFreq);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->sampleRateBox);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->CANCEL_Button);
			this->Controls->Add(this->OK_Button);
			this->Controls->Add(this->comboBox1);
			this->Name = L"SerialPort";
			this->Text = L"SerialPort";
			this->Load += gcnew System::EventHandler(this, &SerialPort::SerialPort_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->volumeBar))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void comboBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 }
	private: System::Void SerialPort_Load(System::Object^  sender, System::EventArgs^  e) {

		        for each (String^ s in System::IO::Ports::SerialPort::GetPortNames())
				{
					 this->comboBox1->Items->Add(System::String::Concat(s,  L""));

				}

				for (unsigned int i = 0; i < waveInGetNumDevs(); i++){
					MMRESULT mRes;
					WAVEINCAPS stWIC={0};
					ZeroMemory(&stWIC,sizeof(WAVEINCAPS));
					mRes=waveInGetDevCaps(i,&stWIC,sizeof(WAVEINCAPS));
					if (mRes == MMSYSERR_NOERROR)
					{
						String ^s = gcnew String(stWIC.szPname);
//						s = stWIC.szPname;
					 this->comboBox2->Items->Add(s);

					}
				}

				this->comboBox1->SelectedIndex = this->comboBox1->Items->Count-1;
				this->comboBox2->SelectedIndex = selectedAudio;
				this->refLevelBox->Text = audioRefLevel.ToString();
				this->sampleRateBox->SelectedIndex = 0;
				int ifr = VNA->GetIF();
				this->IFreq->Text = ifr.ToString();
				VNA->SetFreq(50000000L,true);
				this->volumeBar->Value = MixerGetVolume();
				this->minFreqBox->Text = (VNA->GetMinFreq()/1000000.0).ToString();
				this->maxFreqBox->Text = (VNA->GetMaxFreq()/1000000.0).ToString();
				this->hardwareBox->SelectedIndex = VNA->GetHardware();
			 }
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {

				selectedAudio = this->comboBox2->SelectedIndex;
				OpenAudio();

				 
				 if (this->Port->IsOpen) {  
					 this->Port->Close();
					 System::Threading::Thread::Sleep(2000);
				 }

				Port->PortName = this->comboBox1->Text;
				Port->BaudRate = 115200;
				sampleRate = sampleRateTable[this->sampleRateBox->SelectedIndex];
				VNA->SetMinFreq((__int64) ( Convert::ToDouble(this->minFreqBox->Text)*1000000.0));
				VNA->SetMaxFreq((__int64) ( Convert::ToDouble(this->maxFreqBox->Text)*1000000.0));
				VNA->SelectHardware(Convert::ToInt32(this->hardwareBox->SelectedIndex));

//        this->Port->Parity = SetPortParity(_serialPort->Parity);
//        this->Port->DataBits = SetPortDataBits(_serialPort->DataBits);
//        this->Port->StopBits = SetPortStopBits(_serialPort->StopBits);
//        this->Port->Handshake = SetPortHandshake(_serialPort->Handshake);
#if 0
				try {
					this->Port->Open();
					 System::Threading::Thread::Sleep(2000);
					this->Port->ReadExisting();
					this->Port->WriteLine("F3");
					 System::Threading::Thread::Sleep(2000);
					String ^reply = this->Port->ReadExisting();
					if (!reply->StartsWith("TAPR VNA v4")) {
						MessageBox::Show("No VNA connected to this port", "Error",
						 MessageBoxButtons::OK, MessageBoxIcon::Error);
					    this->DialogResult = System::Windows::Forms::DialogResult::None;
					}
					//if (this->Port->IsOpen) this->Port->Close();

					delete reply;
				}
				catch (Exception^ e)
				{
					MessageBox::Show(e->ToString(),"Open port Error",
						 MessageBoxButtons::OK, MessageBoxIcon::Error);
				}
#endif
			 }
private: System::Void sampleRateBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
				sampleRate = sampleRateTable[this->sampleRateBox->SelectedIndex];
				if (oldSampleRate!=sampleRate) {
					OpenAudio();
					oldSampleRate=sampleRate;
				}
		 }
private: System::Void label2_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void IFreq_TextChanged(System::Object^  sender, System::EventArgs^  e) {
			VNA->SetIF(Convert::ToInt32(this->IFreq->Text));	
		 }

private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e) {
			//magnitude->Text = String::Format("{0}",actualMeasurement.magnitude);
			//phase->Text = String::Format("{0}",actualMeasurement.phase);
			this->reference->Text = String::Format("{0}",actualMeasurement.reference);
		 }
private: System::Void comboBox2_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 	selectedAudio = this->comboBox2->SelectedIndex;
				OpenAudio();

		 }
private: System::Void volumeBar_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 MixerSetVolume(volumeBar->Value);
		 }
private: System::Void volumeBar_Scroll(System::Object^  sender, System::EventArgs^  e) {
			 MixerSetVolume(volumeBar->Value);
		 }
private: System::Void label5_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void refLevelBox_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		try {
			 audioRefLevel = Convert::ToInt32(this->refLevelBox->Text);	
			}
		 catch (Exception^) { }
		 }
private: System::Void label8_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void label13_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void minFreqBox_TextChanged(System::Object^  sender, System::EventArgs^  e) {
				VNA->SetMinFreq((__int64) ( Convert::ToDouble(this->minFreqBox->Text)*1000000.0));
		 }
private: System::Void maxFreqBox_TextChanged(System::Object^  sender, System::EventArgs^  e) {
				VNA->SetMaxFreq((__int64) ( Convert::ToDouble(this->maxFreqBox->Text)*1000000.0));
		 }
};
}

/*
#using <System.dll>

using namespace System;
using namespace System::IO::Ports;
using namespace System::Threading;

public ref class PortChat
{
private:
    static bool _continue;
    static SerialPort^ _serialPort;

public:
    static void Main()
    {
        String^ name;
        String^ message;
        StringComparer^ stringComparer = StringComparer::OrdinalIgnoreCase;
        Thread^ readThread = gcnew Thread(gcnew ThreadStart(PortChat::Read));

        // Create a new SerialPort object with default settings.
        _serialPort = gcnew SerialPort();

        // Allow the user to set the appropriate properties.
        _serialPort->PortName = SetPortName(_serialPort->PortName);
        _serialPort->BaudRate = SetPortBaudRate(_serialPort->BaudRate);
        _serialPort->Parity = SetPortParity(_serialPort->Parity);
        _serialPort->DataBits = SetPortDataBits(_serialPort->DataBits);
        _serialPort->StopBits = SetPortStopBits(_serialPort->StopBits);
        _serialPort->Handshake = SetPortHandshake(_serialPort->Handshake);

        // Set the read/write timeouts
        _serialPort->ReadTimeout = 500;
        _serialPort->WriteTimeout = 500;

        _serialPort->Open();
        _continue = true;
        readThread->Start();

        Console::Write("Name: ");
        name = Console::ReadLine();

        Console::WriteLine("Type QUIT to exit");

        while (_continue)
        {
            message = Console::ReadLine();

            if (stringComparer->Equals("quit", message))
            {
                _continue = false;
            }
            else
            {
                _serialPort->WriteLine(
                    String::Format("<{0}>: {1}", name, message) );
            }
        }

        readThread->Join();
        _serialPort->Close();
    }

    static void Read()
    {
        while (_continue)
        {
            try
            {
                String^ message = _serialPort->ReadLine();
                Console::WriteLine(message);
            }
            catch (TimeoutException ^) { }
        }
    }

*/