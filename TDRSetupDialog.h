//
//    Copyright 2005-2010 Thomas C. McDermott, N5EG
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
	/// Summary for TDRSetupDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class TDRSetupDialog : public System::Windows::Forms::Form
	{
	public: 
		TDRSetupDialog(void)
		{
			InitializeComponent();
		}
        
	protected: 
		~TDRSetupDialog()
		{
			if(components)
				delete(components);
		}

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  TDRStartTextBox;
	private: System::Windows::Forms::TextBox^  TDRStopTextBox;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Button^  OKbutton;
	private: System::Windows::Forms::Button^  Cancelbutton;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::TextBox^  TDRVFTextBox;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::RadioButton^  MetersRadioButton;
	private: System::Windows::Forms::RadioButton^  FeetRadioButton;

	public:
		float TDRStart, TDRStop;



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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(TDRSetupDialog::typeid));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->TDRStartTextBox = (gcnew System::Windows::Forms::TextBox());
			this->TDRStopTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->OKbutton = (gcnew System::Windows::Forms::Button());
			this->Cancelbutton = (gcnew System::Windows::Forms::Button());
			this->TDRVFTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->FeetRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->MetersRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Font = (gcnew System::Drawing::Font(L"Verdana", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(56, 24);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(192, 23);
			this->label1->TabIndex = 0;
			this->label1->Text = L"TDR Display Control";
			this->label1->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// TDRStartTextBox
			// 
			this->TDRStartTextBox->Location = System::Drawing::Point(200, 72);
			this->TDRStartTextBox->Name = L"TDRStartTextBox";
			this->TDRStartTextBox->Size = System::Drawing::Size(80, 20);
			this->TDRStartTextBox->TabIndex = 1;
			this->TDRStartTextBox->Text = L"0";
			// 
			// TDRStopTextBox
			// 
			this->TDRStopTextBox->Location = System::Drawing::Point(200, 112);
			this->TDRStopTextBox->Name = L"TDRStopTextBox";
			this->TDRStopTextBox->Size = System::Drawing::Size(80, 20);
			this->TDRStopTextBox->TabIndex = 2;
			this->TDRStopTextBox->Text = L"4350";
			// 
			// label2
			// 
			this->label2->BackColor = System::Drawing::Color::Transparent;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->label2->Location = System::Drawing::Point(16, 72);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(152, 23);
			this->label2->TabIndex = 3;
			this->label2->Text = L"TDR Display Start Time, nsec.";
			// 
			// label3
			// 
			this->label3->BackColor = System::Drawing::Color::Transparent;
			this->label3->Location = System::Drawing::Point(16, 112);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(152, 23);
			this->label3->TabIndex = 4;
			this->label3->Text = L"TDR Display Stop time, nsec.";
			// 
			// OKbutton
			// 
			this->OKbutton->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->OKbutton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->OKbutton->Location = System::Drawing::Point(64, 312);
			this->OKbutton->Name = L"OKbutton";
			this->OKbutton->Size = System::Drawing::Size(72, 24);
			this->OKbutton->TabIndex = 5;
			this->OKbutton->Text = L"OK";
			this->OKbutton->Click += gcnew System::EventHandler(this, &TDRSetupDialog::OKbutton_Click);
			// 
			// Cancelbutton
			// 
			this->Cancelbutton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->Cancelbutton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->Cancelbutton->Location = System::Drawing::Point(168, 312);
			this->Cancelbutton->Name = L"Cancelbutton";
			this->Cancelbutton->Size = System::Drawing::Size(72, 24);
			this->Cancelbutton->TabIndex = 6;
			this->Cancelbutton->Text = L"Cancel";
			// 
			// TDRVFTextBox
			// 
			this->TDRVFTextBox->Location = System::Drawing::Point(200, 160);
			this->TDRVFTextBox->Name = L"TDRVFTextBox";
			this->TDRVFTextBox->Size = System::Drawing::Size(80, 20);
			this->TDRVFTextBox->TabIndex = 7;
			this->TDRVFTextBox->Text = L"0.68";
			// 
			// label4
			// 
			this->label4->BackColor = System::Drawing::Color::Transparent;
			this->label4->Location = System::Drawing::Point(16, 160);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(168, 32);
			this->label4->TabIndex = 8;
			this->label4->Text = L"Enter the Estimated Cable Velocity Factor (0.4 to 0.99)";
			// 
			// groupBox1
			// 
			this->groupBox1->BackColor = System::Drawing::Color::Transparent;
			this->groupBox1->Controls->Add(this->FeetRadioButton);
			this->groupBox1->Controls->Add(this->MetersRadioButton);
			this->groupBox1->Location = System::Drawing::Point(56, 208);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(200, 80);
			this->groupBox1->TabIndex = 9;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"View Distance to marker as:";
			// 
			// FeetRadioButton
			// 
			this->FeetRadioButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->FeetRadioButton->Location = System::Drawing::Point(16, 48);
			this->FeetRadioButton->Name = L"FeetRadioButton";
			this->FeetRadioButton->Size = System::Drawing::Size(64, 24);
			this->FeetRadioButton->TabIndex = 1;
			this->FeetRadioButton->Text = L"Feet";
			// 
			// MetersRadioButton
			// 
			this->MetersRadioButton->Checked = true;
			this->MetersRadioButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->MetersRadioButton->Location = System::Drawing::Point(16, 24);
			this->MetersRadioButton->Name = L"MetersRadioButton";
			this->MetersRadioButton->Size = System::Drawing::Size(72, 24);
			this->MetersRadioButton->TabIndex = 0;
			this->MetersRadioButton->TabStop = true;
			this->MetersRadioButton->Text = L"Meters";
			// 
			// TDRSetupDialog
			// 
			this->AcceptButton = this->OKbutton;
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"$this.BackgroundImage")));
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->ClientSize = System::Drawing::Size(314, 368);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->TDRVFTextBox);
			this->Controls->Add(this->TDRStopTextBox);
			this->Controls->Add(this->TDRStartTextBox);
			this->Controls->Add(this->Cancelbutton);
			this->Controls->Add(this->OKbutton);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->Name = L"TDRSetupDialog";
			this->Text = L"TDRSetupDialog";
			this->groupBox1->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}		




	public:
	    void set_Start(float number)   { TDRStartTextBox->Text = number.ToString(); }
		float get_Start()
		{ 
			float n;
			try
			{
				n = Convert::ToSingle(TDRStartTextBox->Text);
			}
			catch (System::FormatException^ pe)
			{
				MessageBox::Show(pe->Message, "Error");
			}
			catch (System::OverflowException^ pe)
			{
				MessageBox::Show(pe->Message, "Error");
			}
			return n;
		}
	    void set_Stop(float number)   { TDRStopTextBox->Text = number.ToString(); }
		float get_Stop()
		{ 
			float n;
			try
			{
				n = Convert::ToSingle(TDRStopTextBox->Text);
			}
			catch (System::FormatException^ pe)
			{
				MessageBox::Show(pe->Message, "Error");
			}
			catch (System::OverflowException^ pe)
			{
				MessageBox::Show(pe->Message, "Error");
			}
			return n;
		}
		void set_VF(float number)	{ TDRVFTextBox->Text = number.ToString(); }
		float get_VF()
		{ 
			float n;
			try
			{
				n = Convert::ToSingle(TDRVFTextBox->Text);
			}
			catch (System::FormatException^ pe)
			{
				MessageBox::Show(pe->Message, "Error");
			}
			catch (System::OverflowException^ pe)
			{
				MessageBox::Show(pe->Message, "Error");
			}
			return n;
		}
		bool get_MetricMode() { return MetersRadioButton->Checked; }
		void set_MetricMode(bool mode)
		{
			if(mode)
			{
				MetersRadioButton->Checked = true;
				FeetRadioButton->Checked = false;
			}
			else
			{
				MetersRadioButton->Checked = false;
				FeetRadioButton->Checked = true;
			}
		}
	private: System::Void OKbutton_Click(System::Object^  sender, System::EventArgs^  e)
		{
			try
			{
//					VNAR3::Form1::TDRStartTime = Convert::ToSingle(TDRStartTextBox->Text);
//					VNAR3::Form1::TDRStopTime = Convert::ToSingle(TDRStopTextBox->Text);
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

};
}