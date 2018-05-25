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

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;



namespace VNAR3
{
	/// <summary> 
	/// Summary for About
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>

	/// About Status Box
	public ref class About : public System::Windows::Forms::Form
	{
	public: 
		About(String^ vers)		
		{
			InitializeComponent();
			VersionLabel->Text = vers;		
		}
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Button^  LaunchBrowserButton;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  VersionLabel;
	private: System::Windows::Forms::Button^  button1;

	public: String^ vers;			///< VNAR software build number
	
	protected: 
		~About()
		{
			if(components)
				delete(components);
		};

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(About::typeid));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->VersionLabel = (gcnew System::Windows::Forms::Label());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->LaunchBrowserButton = (gcnew System::Windows::Forms::Button());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(40, 16);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(208, 24);
			this->label1->TabIndex = 0;
			this->label1->Text = L"TAPR Vector Network Analyzer";
			this->label1->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label2
			// 
			this->label2->BackColor = System::Drawing::Color::Transparent;
			this->label2->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"label2.Image")));
			this->label2->Location = System::Drawing::Point(112, 40);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(72, 72);
			this->label2->TabIndex = 1;
			// 
			// label3
			// 
			this->label3->BackColor = System::Drawing::Color::Transparent;
			this->label3->Location = System::Drawing::Point(24, 193);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(256, 32);
			this->label3->TabIndex = 2;
			this->label3->Text = L"Software Licensed Under GNU General Public License (GPL). See gpl.txt for details" 
				L".";
			this->label3->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// VersionLabel
			// 
			this->VersionLabel->BackColor = System::Drawing::Color::Transparent;
			this->VersionLabel->Location = System::Drawing::Point(24, 124);
			this->VersionLabel->Name = L"VersionLabel";
			this->VersionLabel->Size = System::Drawing::Size(256, 55);
			this->VersionLabel->TabIndex = 3;
			this->VersionLabel->Text = L"Version: ";
			this->VersionLabel->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// button1
			// 
			this->button1->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->button1->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->button1->Location = System::Drawing::Point(112, 234);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 4;
			this->button1->Text = L"OK";
			// 
			// label4
			// 
			this->label4->BackColor = System::Drawing::Color::Transparent;
			this->label4->Location = System::Drawing::Point(56, 312);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(192, 16);
			this->label4->TabIndex = 5;
			this->label4->Text = L"(Check TAPR website for updates)";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// LaunchBrowserButton
			// 
			this->LaunchBrowserButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->LaunchBrowserButton->Location = System::Drawing::Point(92, 280);
			this->LaunchBrowserButton->Name = L"LaunchBrowserButton";
			this->LaunchBrowserButton->Size = System::Drawing::Size(112, 24);
			this->LaunchBrowserButton->TabIndex = 6;
			this->LaunchBrowserButton->Text = L"Launch Browser";
			this->LaunchBrowserButton->Click += gcnew System::EventHandler(this, &About::LaunchBrowserButton_Click);
			// 
			// panel1
			// 
			this->panel1->BackColor = System::Drawing::Color::Transparent;
			this->panel1->Location = System::Drawing::Point(32, 272);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(248, 64);
			this->panel1->TabIndex = 7;
			// 
			// About
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"$this.BackgroundImage")));
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->ClientSize = System::Drawing::Size(304, 344);
			this->Controls->Add(this->LaunchBrowserButton);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->VersionLabel);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->panel1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->Name = L"About";
			this->Text = L"About";
			this->Load += gcnew System::EventHandler(this, &About::About_Load);
			this->ResumeLayout(false);

		}		


	private: System::Void LaunchBrowserButton_Click(System::Object^  sender, System::EventArgs^  e)
			 {
				 System::Diagnostics::Process::Start("http://www.tapr.org/~n5eg/index_files/vna_library/current.htm");
			 }

private: System::Void About_Load(System::Object^  sender, System::EventArgs^  e) {
		 }
};
}