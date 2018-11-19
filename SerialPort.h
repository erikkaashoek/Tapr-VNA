#pragma once
#include "stdafx.h"
#include "DataDisplay.h"
#include "Mockup.h"
#include "Constants.h"
#using <mscorlib.dll>

#using<system.dll>

#include "Constants.h"
#include "AudioInput.h"
#include "USB_EZ_interface.h"
 

//#include "objbase.h"

extern int IFREQ;
extern int oldIFREQ;
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
			this->SuspendLayout();
			// 
			// comboBox1
			// 
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Location = System::Drawing::Point(67, 16);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(143, 21);
			this->comboBox1->TabIndex = 0;
			this->comboBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &SerialPort::comboBox1_SelectedIndexChanged);
			// 
			// OK_Button
			// 
			this->OK_Button->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->OK_Button->Location = System::Drawing::Point(67, 189);
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
			this->CANCEL_Button->Location = System::Drawing::Point(146, 189);
			this->CANCEL_Button->Name = L"CANCEL_Button";
			this->CANCEL_Button->Size = System::Drawing::Size(64, 28);
			this->CANCEL_Button->TabIndex = 2;
			this->CANCEL_Button->Text = L"Cancel";
			this->CANCEL_Button->UseVisualStyleBackColor = true;
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(67, 155);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(143, 28);
			this->button3->TabIndex = 3;
			this->button3->Text = L"Refresh";
			this->button3->UseVisualStyleBackColor = true;
			// 
			// sampleRateBox
			// 
			this->sampleRateBox->FormattingEnabled = true;
			this->sampleRateBox->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"192 kHz", L"96 kHz", L"48 kHz", L"44.1 kHz", 
				L"24 kHz", L"12 kHz"});
			this->sampleRateBox->Location = System::Drawing::Point(67, 54);
			this->sampleRateBox->Name = L"sampleRateBox";
			this->sampleRateBox->Size = System::Drawing::Size(143, 21);
			this->sampleRateBox->TabIndex = 4;
			this->sampleRateBox->SelectedIndexChanged += gcnew System::EventHandler(this, &SerialPort::sampleRateBox_SelectedIndexChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(36, 57);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(25, 13);
			this->label1->TabIndex = 5;
			this->label1->Text = L"SR:";
			// 
			// IFreq
			// 
			this->IFreq->Location = System::Drawing::Point(125, 88);
			this->IFreq->Name = L"IFreq";
			this->IFreq->Size = System::Drawing::Size(84, 20);
			this->IFreq->TabIndex = 6;
			this->IFreq->TextChanged += gcnew System::EventHandler(this, &SerialPort::IFreq_TextChanged);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(36, 91);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(19, 13);
			this->label2->TabIndex = 7;
			this->label2->Text = L"IF:";
			this->label2->Click += gcnew System::EventHandler(this, &SerialPort::label2_Click);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(42, 127);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(60, 13);
			this->label3->TabIndex = 8;
			this->label3->Text = L"Reference:";
			// 
			// reference
			// 
			this->reference->Location = System::Drawing::Point(126, 124);
			this->reference->Name = L"reference";
			this->reference->Size = System::Drawing::Size(83, 20);
			this->reference->TabIndex = 9;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(215, 127);
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
			// SerialPort
			// 
			this->AcceptButton = this->OK_Button;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->CANCEL_Button;
			this->ClientSize = System::Drawing::Size(284, 261);
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
				this->comboBox1->SelectedIndex = this->comboBox1->Items->Count-1;
				this->sampleRateBox->SelectedIndex = 0;
				this->IFreq->Text = IFREQ.ToString();
				VNA->SetFreq(1000000,true);
			 }
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {

				if (this->Port->IsOpen) this->Port->Close();
				Port->PortName = this->comboBox1->Text;
				Port->BaudRate = 115200;
				sampleRate = sampleRateTable[this->sampleRateBox->SelectedIndex];

//        this->Port->Parity = SetPortParity(_serialPort->Parity);
//        this->Port->DataBits = SetPortDataBits(_serialPort->DataBits);
//        this->Port->StopBits = SetPortStopBits(_serialPort->StopBits);
//        this->Port->Handshake = SetPortHandshake(_serialPort->Handshake);
				try {
					this->Port->Open();
					this->Port->WriteLine("3");
					 System::Threading::Thread::Sleep(500);
					String ^reply = this->Port->ReadExisting();
					if (!reply->StartsWith("TAPR VNA v4")) {
						MessageBox::Show("No VNA connected to this port", "Error",
						 MessageBoxButtons::OK, MessageBoxIcon::Error);
					    this->DialogResult = System::Windows::Forms::DialogResult::None;
					}
					if (this->Port->IsOpen) this->Port->Close();

					delete reply;
				}
				catch (Exception^)
				{
					MessageBox::Show(e->ToString(),"Open port Error",
						 MessageBoxButtons::OK, MessageBoxIcon::Error);
				}

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
			IFREQ = Convert::ToInt32(this->IFreq->Text);	
			if (oldIFREQ != IFREQ) {
				OpenAudio();
				oldIFREQ=IFREQ;
		 }
		 }

private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e) {
			//magnitude->Text = String::Format("{0}",actualMeasurement.magnitude);
			//phase->Text = String::Format("{0}",actualMeasurement.phase);
			this->reference->Text = String::Format("{0}",actualMeasurement.reference);
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