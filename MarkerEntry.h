//
//    Copyright 2004-2010, Thomas C. McDermott, N5EG
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

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace VNAR3
{
	/// <summary> 
	/// Summary for MarkerEntry
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>

	/// MarkerEntry Class
	public ref class MarkerEntry : public System::Windows::Forms::Form
	{

	public: array<UInt32>^ MarkerF;				// Our caller retrieves the changed markers 
	public: array<Single>^ MarkerT;				// by reading these values before disposing of the class

	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::GroupBox^  groupBox2;

	public: 
		MarkerEntry(array<UInt32>^ MarkerFIn, array<Single>^ MarkerTIn)	///< Array of Markers (frequency, time of each marker)
		{
			InitializeComponent();

			MarkerF = gcnew array<UInt32>(5);
			MarkerT = gcnew array<Single>(5);

			for (int i=0; i<5; i++)							// read in marker frequencies
			{
                MarkerF[i] = MarkerFIn[i];
				MarkerT[i] = MarkerTIn[i];
			}

				Marker1Freq->Text = MarkerF[0].ToString();		// and display in dialog box
				Marker2Freq->Text = MarkerF[1].ToString();
				Marker3Freq->Text = MarkerF[2].ToString();
				Marker4Freq->Text = MarkerF[3].ToString();
				Marker5Freq->Text = MarkerF[4].ToString();
				Marker1Time->Text = MarkerT[0].ToString();		
				Marker2Time->Text = MarkerT[1].ToString();
				Marker3Time->Text = MarkerT[2].ToString();
				Marker4Time->Text = MarkerT[3].ToString();
				Marker5Time->Text = MarkerT[4].ToString();

		}
        
	protected: 
		~MarkerEntry()
		{
			if(components)
				delete(components);
		}

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  Marker1Freq;
	private: System::Windows::Forms::TextBox^  Marker2Freq;
	private: System::Windows::Forms::TextBox^  Marker3Freq;
	private: System::Windows::Forms::TextBox^  Marker4Freq;
	private: System::Windows::Forms::TextBox^  Marker5Freq;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Button^  OKButton;
	private: System::Windows::Forms::Button^  CancelButton;
	private: System::Windows::Forms::TextBox^  Marker1Time;
	private: System::Windows::Forms::TextBox^  Marker2Time;
	private: System::Windows::Forms::TextBox^  Marker3Time;
	private: System::Windows::Forms::TextBox^  Marker4Time;
	private: System::Windows::Forms::TextBox^  Marker5Time;
	private: System::Windows::Forms::Label^  label8;

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MarkerEntry::typeid));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->Marker1Freq = (gcnew System::Windows::Forms::TextBox());
			this->Marker2Freq = (gcnew System::Windows::Forms::TextBox());
			this->Marker3Freq = (gcnew System::Windows::Forms::TextBox());
			this->Marker4Freq = (gcnew System::Windows::Forms::TextBox());
			this->Marker5Freq = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->OKButton = (gcnew System::Windows::Forms::Button());
			this->CancelButton = (gcnew System::Windows::Forms::Button());
			this->Marker1Time = (gcnew System::Windows::Forms::TextBox());
			this->Marker2Time = (gcnew System::Windows::Forms::TextBox());
			this->Marker3Time = (gcnew System::Windows::Forms::TextBox());
			this->Marker4Time = (gcnew System::Windows::Forms::TextBox());
			this->Marker5Time = (gcnew System::Windows::Forms::TextBox());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(168, 8);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(104, 29);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Markers";
			this->label1->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// Marker1Freq
			// 
			this->Marker1Freq->Location = System::Drawing::Point(48, 96);
			this->Marker1Freq->Name = L"Marker1Freq";
			this->Marker1Freq->Size = System::Drawing::Size(112, 20);
			this->Marker1Freq->TabIndex = 1;
			this->Marker1Freq->TextChanged += gcnew System::EventHandler(this, &MarkerEntry::Marker1Freq_TextChanged);
			// 
			// Marker2Freq
			// 
			this->Marker2Freq->Location = System::Drawing::Point(48, 128);
			this->Marker2Freq->Name = L"Marker2Freq";
			this->Marker2Freq->Size = System::Drawing::Size(112, 20);
			this->Marker2Freq->TabIndex = 2;
			// 
			// Marker3Freq
			// 
			this->Marker3Freq->Location = System::Drawing::Point(48, 160);
			this->Marker3Freq->Name = L"Marker3Freq";
			this->Marker3Freq->Size = System::Drawing::Size(112, 20);
			this->Marker3Freq->TabIndex = 3;
			// 
			// Marker4Freq
			// 
			this->Marker4Freq->Location = System::Drawing::Point(48, 192);
			this->Marker4Freq->Name = L"Marker4Freq";
			this->Marker4Freq->Size = System::Drawing::Size(112, 20);
			this->Marker4Freq->TabIndex = 4;
			// 
			// Marker5Freq
			// 
			this->Marker5Freq->Location = System::Drawing::Point(48, 224);
			this->Marker5Freq->Name = L"Marker5Freq";
			this->Marker5Freq->Size = System::Drawing::Size(112, 20);
			this->Marker5Freq->TabIndex = 5;
			// 
			// label2
			// 
			this->label2->BackColor = System::Drawing::Color::Transparent;
			this->label2->Location = System::Drawing::Point(192, 96);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(56, 21);
			this->label2->TabIndex = 6;
			this->label2->Text = L"Marker 1";
			this->label2->TextAlign = System::Drawing::ContentAlignment::BottomLeft;
			// 
			// label3
			// 
			this->label3->BackColor = System::Drawing::Color::Transparent;
			this->label3->Location = System::Drawing::Point(192, 128);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(56, 21);
			this->label3->TabIndex = 7;
			this->label3->Text = L"Marker 2";
			this->label3->TextAlign = System::Drawing::ContentAlignment::BottomLeft;
			// 
			// label4
			// 
			this->label4->BackColor = System::Drawing::Color::Transparent;
			this->label4->Location = System::Drawing::Point(192, 160);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(56, 21);
			this->label4->TabIndex = 8;
			this->label4->Text = L"Marker 3";
			this->label4->TextAlign = System::Drawing::ContentAlignment::BottomLeft;
			// 
			// label5
			// 
			this->label5->BackColor = System::Drawing::Color::Transparent;
			this->label5->Location = System::Drawing::Point(192, 192);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(56, 21);
			this->label5->TabIndex = 9;
			this->label5->Text = L"Marker 4";
			this->label5->TextAlign = System::Drawing::ContentAlignment::BottomLeft;
			// 
			// label6
			// 
			this->label6->BackColor = System::Drawing::Color::Transparent;
			this->label6->Location = System::Drawing::Point(192, 224);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(56, 21);
			this->label6->TabIndex = 10;
			this->label6->Text = L"Marker 5";
			this->label6->TextAlign = System::Drawing::ContentAlignment::BottomLeft;
			// 
			// label7
			// 
			this->label7->BackColor = System::Drawing::Color::Transparent;
			this->label7->Location = System::Drawing::Point(44, 65);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(116, 29);
			this->label7->TabIndex = 11;
			this->label7->Text = L"Frequency, Hz.        ( 0 turns off Marker )";
			this->label7->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// OKButton
			// 
			this->OKButton->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->OKButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->OKButton->Location = System::Drawing::Point(128, 272);
			this->OKButton->Name = L"OKButton";
			this->OKButton->Size = System::Drawing::Size(75, 23);
			this->OKButton->TabIndex = 11;
			this->OKButton->Text = L"OK";
			this->OKButton->Click += gcnew System::EventHandler(this, &MarkerEntry::OKButton_Click);
			// 
			// CancelButton
			// 
			this->CancelButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->CancelButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->CancelButton->Location = System::Drawing::Point(232, 272);
			this->CancelButton->Name = L"CancelButton";
			this->CancelButton->Size = System::Drawing::Size(75, 23);
			this->CancelButton->TabIndex = 12;
			this->CancelButton->Text = L"Cancel";
			// 
			// Marker1Time
			// 
			this->Marker1Time->Location = System::Drawing::Point(272, 96);
			this->Marker1Time->Name = L"Marker1Time";
			this->Marker1Time->Size = System::Drawing::Size(112, 20);
			this->Marker1Time->TabIndex = 6;
			// 
			// Marker2Time
			// 
			this->Marker2Time->Location = System::Drawing::Point(272, 128);
			this->Marker2Time->Name = L"Marker2Time";
			this->Marker2Time->Size = System::Drawing::Size(112, 20);
			this->Marker2Time->TabIndex = 7;
			// 
			// Marker3Time
			// 
			this->Marker3Time->Location = System::Drawing::Point(272, 160);
			this->Marker3Time->Name = L"Marker3Time";
			this->Marker3Time->Size = System::Drawing::Size(112, 20);
			this->Marker3Time->TabIndex = 8;
			// 
			// Marker4Time
			// 
			this->Marker4Time->Location = System::Drawing::Point(272, 192);
			this->Marker4Time->Name = L"Marker4Time";
			this->Marker4Time->Size = System::Drawing::Size(112, 20);
			this->Marker4Time->TabIndex = 9;
			// 
			// Marker5Time
			// 
			this->Marker5Time->Location = System::Drawing::Point(272, 224);
			this->Marker5Time->Name = L"Marker5Time";
			this->Marker5Time->Size = System::Drawing::Size(112, 20);
			this->Marker5Time->TabIndex = 10;
			// 
			// label8
			// 
			this->label8->BackColor = System::Drawing::Color::Transparent;
			this->label8->Location = System::Drawing::Point(272, 64);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(104, 29);
			this->label8->TabIndex = 19;
			this->label8->Text = L"Time, nSec.            (0 turns off Marker)";
			this->label8->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// groupBox1
			// 
			this->groupBox1->BackColor = System::Drawing::Color::Transparent;
			this->groupBox1->ForeColor = System::Drawing::SystemColors::ControlText;
			this->groupBox1->Location = System::Drawing::Point(32, 40);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(144, 224);
			this->groupBox1->TabIndex = 20;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Rectangular / Polar";
			// 
			// groupBox2
			// 
			this->groupBox2->BackColor = System::Drawing::Color::Transparent;
			this->groupBox2->ForeColor = System::Drawing::SystemColors::ControlText;
			this->groupBox2->Location = System::Drawing::Point(256, 40);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(144, 224);
			this->groupBox2->TabIndex = 21;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"TDR";
			// 
			// MarkerEntry
			// 
			this->AcceptButton = this->OKButton;
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->BackColor = System::Drawing::Color::White;
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"$this.BackgroundImage")));
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->ClientSize = System::Drawing::Size(438, 316);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->Marker5Time);
			this->Controls->Add(this->Marker4Time);
			this->Controls->Add(this->Marker3Time);
			this->Controls->Add(this->Marker2Time);
			this->Controls->Add(this->Marker1Time);
			this->Controls->Add(this->CancelButton);
			this->Controls->Add(this->OKButton);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->Marker5Freq);
			this->Controls->Add(this->Marker4Freq);
			this->Controls->Add(this->Marker3Freq);
			this->Controls->Add(this->Marker2Freq);
			this->Controls->Add(this->Marker1Freq);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->groupBox2);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->Name = L"MarkerEntry";
			this->Text = L"Marker Entry";
			this->ResumeLayout(false);
			this->PerformLayout();

		}		




		/// Marker OK button click event handler
private: System::Void OKButton_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 try
			 {
                 MarkerF[0] = Convert::ToUInt32(Marker1Freq->Text);
                 MarkerF[1] = Convert::ToUInt32(Marker2Freq->Text);
                 MarkerF[2] = Convert::ToUInt32(Marker3Freq->Text);
                 MarkerF[3] = Convert::ToUInt32(Marker4Freq->Text);
                 MarkerF[4] = Convert::ToUInt32(Marker5Freq->Text);

				 MarkerT[0] = Convert::ToSingle(Marker1Time->Text);
                 MarkerT[1] = Convert::ToSingle(Marker2Time->Text);
                 MarkerT[2] = Convert::ToSingle(Marker3Time->Text);
                 MarkerT[3] = Convert::ToSingle(Marker4Time->Text);
                 MarkerT[4] = Convert::ToSingle(Marker5Time->Text);

			 }
			catch (System::FormatException^ pe)
			{
				MessageBox::Show(pe->Message, "Error");
			}
			catch (System::OverflowException^ pe)
			{
				MessageBox::Show(pe->Message, "Error");
			}
		 }
private: System::Void Marker1Freq_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
};
}