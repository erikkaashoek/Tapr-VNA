#pragma once
#include "stdafx.h"
#using <mscorlib.dll>
#using <System.dll>
//#using <Winmm.dll>

//#include <Windows.h>
//#include <MMSystem.h>
#pragma comment(lib,"winmm.lib")

namespace VNAR3 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for AudioDevice
	/// </summary>
	public ref class AudioDevice : public System::Windows::Forms::Form
	{
	public:
		AudioDevice(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~AudioDevice()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ComboBox^  AudioInputDevices;
	protected: 

	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->AudioInputDevices = (gcnew System::Windows::Forms::ComboBox());
			this->SuspendLayout();
			// 
			// AudioInputDevices
			// 
			this->AudioInputDevices->FormattingEnabled = true;
			this->AudioInputDevices->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"Item 1", L"Item 2"});
			this->AudioInputDevices->Location = System::Drawing::Point(35, 20);
			this->AudioInputDevices->Name = L"AudioInputDevices";
			this->AudioInputDevices->Size = System::Drawing::Size(155, 21);
			this->AudioInputDevices->TabIndex = 0;
			// 
			// AudioDevice
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 261);
			this->Controls->Add(this->AudioInputDevices);
			this->Name = L"AudioDevice";
			this->Text = L"AudioDevice";
			this->ResumeLayout(false);

		}
#pragma endregion

	private:		int FillDevices()
		{
			//	CComboBox *pBox=(CComboBox*)GetDlgItem(IDC_DEVICES);
			System::Windows::Forms::ComboBox^ pBox=this->AudioInputDevices;
			int nDevices,nC1;
			WAVEINCAPS stWIC={0};
			MMRESULT mRes;

			//pBox->Clear();
			nDevices=waveInGetNumDevs();

			for(nC1=0;nC1<nDevices;++nC1)
			{
				ZeroMemory(&stWIC,sizeof(WAVEINCAPS));
				mRes=waveInGetDevCaps(nC1,&stWIC,sizeof(WAVEINCAPS));
				//if(mRes==0)
				//pBox->Items->Add((System::Object^) stWIC.szPname);
				// else
				//StoreError(mRes,TRUE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
			}
			if(pBox->Items->Count)
			{
				//pBox->SetCurSel(0);
				//OnCbnSelchangeDevices();
			}
			return nDevices;
		}
	};
}
