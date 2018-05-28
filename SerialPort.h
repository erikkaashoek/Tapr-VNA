#pragma once
#include "stdafx.h"
#include "DataDisplay.h"
#include "Mockup.h"
#include "Constants.h"
#using <mscorlib.dll>

#using<system.dll>

//#include "objbase.h"

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
		SerialPort(System::IO::Ports::SerialPort^  serialPort1)
		{
			InitializeComponent();
			Port = serialPort1;
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


	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		System::IO::Ports::SerialPort^  Port;
		/// </summary>
		System::ComponentModel::Container ^components;
	private: System::Windows::Forms::Button^  button3;
			 
#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->OK_Button = (gcnew System::Windows::Forms::Button());
			this->CANCEL_Button = (gcnew System::Windows::Forms::Button());
			this->button3 = (gcnew System::Windows::Forms::Button());
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
			this->OK_Button->Location = System::Drawing::Point(67, 170);
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
			this->CANCEL_Button->Location = System::Drawing::Point(146, 170);
			this->CANCEL_Button->Name = L"CANCEL_Button";
			this->CANCEL_Button->Size = System::Drawing::Size(64, 28);
			this->CANCEL_Button->TabIndex = 2;
			this->CANCEL_Button->Text = L"Cancel";
			this->CANCEL_Button->UseVisualStyleBackColor = true;
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(67, 136);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(143, 28);
			this->button3->TabIndex = 3;
			this->button3->Text = L"Refresh";
			this->button3->UseVisualStyleBackColor = true;
			// 
			// SerialPort
			// 
			this->AcceptButton = this->OK_Button;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->CANCEL_Button;
			this->ClientSize = System::Drawing::Size(284, 261);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->CANCEL_Button);
			this->Controls->Add(this->OK_Button);
			this->Controls->Add(this->comboBox1);
			this->Name = L"SerialPort";
			this->Text = L"SerialPort";
			this->Load += gcnew System::EventHandler(this, &SerialPort::SerialPort_Load);
			this->ResumeLayout(false);

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
			 }
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
				if (this->Port->IsOpen) this->Port->Close();
				Port->PortName = this->comboBox1->Text;
				Port->BaudRate = 115200;
//        this->Port->Parity = SetPortParity(_serialPort->Parity);
//        this->Port->DataBits = SetPortDataBits(_serialPort->DataBits);
//        this->Port->StopBits = SetPortStopBits(_serialPort->StopBits);
//        this->Port->Handshake = SetPortHandshake(_serialPort->Handshake);
				try {
				this->Port->Open();
				}
				catch (System::Exception^ e)
				{
					MessageBox::Show(e->ToString(),"Open port Error",
						 MessageBoxButtons::OK, MessageBoxIcon::Error);
				}

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