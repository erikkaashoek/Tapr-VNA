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
	/// Summary for NumEntBox
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>

	/// Numeric Entry dialogBox
public ref class NumEntBox : public System::Windows::Forms::Form
	{
	public: 
		NumEntBox(void)
		{
			InitializeComponent();
		}
        
	protected: 
		~NumEntBox()
		{
			if(components)
				delete(components);
		}

	private: System::Windows::Forms::Button^  OKButton;
	private: System::Windows::Forms::Button^  CancelButton;
	private: System::Windows::Forms::TextBox^  NumberBox;
	private: System::Windows::Forms::Label^  label1;

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(NumEntBox::typeid));
			this->OKButton = (gcnew System::Windows::Forms::Button());
			this->CancelButton = (gcnew System::Windows::Forms::Button());
			this->NumberBox = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// OKButton
			// 
			this->OKButton->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->OKButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->OKButton->Location = System::Drawing::Point(48, 112);
			this->OKButton->Name = L"OKButton";
			this->OKButton->Size = System::Drawing::Size(75, 23);
			this->OKButton->TabIndex = 1;
			this->OKButton->Text = L"OK";
			// 
			// CancelButton
			// 
			this->CancelButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->CancelButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->CancelButton->Location = System::Drawing::Point(168, 112);
			this->CancelButton->Name = L"CancelButton";
			this->CancelButton->Size = System::Drawing::Size(75, 23);
			this->CancelButton->TabIndex = 2;
			this->CancelButton->Text = L"Cancel";
			// 
			// NumberBox
			// 
			this->NumberBox->Location = System::Drawing::Point(72, 64);
			this->NumberBox->Name = L"NumberBox";
			this->NumberBox->Size = System::Drawing::Size(144, 20);
			this->NumberBox->TabIndex = 0;
			this->NumberBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// label1
			// 
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Location = System::Drawing::Point(96, 32);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(88, 16);
			this->label1->TabIndex = 3;
			this->label1->Text = L"Enter new value";
			// 
			// NumEntBox
			// 
			this->AcceptButton = this->OKButton;
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"$this.BackgroundImage")));
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->ClientSize = System::Drawing::Size(292, 164);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->NumberBox);
			this->Controls->Add(this->CancelButton);
			this->Controls->Add(this->OKButton);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"NumEntBox";
			this->Text = L"Direct Value Entry";
			this->ResumeLayout(false);
			this->PerformLayout();

		}		
	
	
	public:
	    void set_NumericValue(int number)   { NumberBox->Text = number.ToString(); } // display existing value

		int get_NumericValue()						// retrieve value that user entered
		{ 
			int n;
			try										// make sure it's an integer number
			{
				n = Convert::ToInt32(NumberBox->Text);	
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

	};

}