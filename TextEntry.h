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
	/// Summary for TextEntry
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>

	/// Text Entry dialogbox
	public ref class TextEntry : public System::Windows::Forms::Form
	{
	public: 
		TextEntry(String^ BoxTitle)
		{
			InitializeComponent();
			label1->Text = BoxTitle;
		}

	public:        
		String^ EnteredText() { return textBox1->Text; };
		void set_DisplayedText(String^ Input) { textBox1->Text = Input; }

	protected: 
		~TextEntry()
		{
			if(components)
				delete(components);
		}

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Button^  OKbutton;
	private: System::Windows::Forms::Button^  CancelButton;

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(TextEntry::typeid));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->OKbutton = (gcnew System::Windows::Forms::Button());
			this->CancelButton = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Font = (gcnew System::Drawing::Font(L"Arial", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(24, 16);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(640, 23);
			this->label1->TabIndex = 0;
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(16, 48);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(648, 20);
			this->textBox1->TabIndex = 1;
			// 
			// OKbutton
			// 
			this->OKbutton->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->OKbutton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->OKbutton->Location = System::Drawing::Point(248, 88);
			this->OKbutton->Name = L"OKbutton";
			this->OKbutton->Size = System::Drawing::Size(72, 24);
			this->OKbutton->TabIndex = 2;
			this->OKbutton->Text = L"OK";
			// 
			// CancelButton
			// 
			this->CancelButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->CancelButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->CancelButton->Location = System::Drawing::Point(352, 88);
			this->CancelButton->Name = L"CancelButton";
			this->CancelButton->Size = System::Drawing::Size(72, 24);
			this->CancelButton->TabIndex = 3;
			this->CancelButton->Text = L"Cancel";
			// 
			// TextEntry
			// 
			this->AcceptButton = this->OKbutton;
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"$this.BackgroundImage")));
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->ClientSize = System::Drawing::Size(680, 126);
			this->Controls->Add(this->CancelButton);
			this->Controls->Add(this->OKbutton);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->label1);
			this->Name = L"TextEntry";
			this->Text = L"TextEntry";
			this->ResumeLayout(false);
			this->PerformLayout();

		}		
	};
}