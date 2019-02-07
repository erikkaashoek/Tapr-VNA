#pragma once
#include "Constants.h"
#include "AudioInput.h"
#include "USB_EZ_interface.h"
 
namespace VNAR3 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for SignalGenerator
	/// </summary>
	public ref class SignalGenerator : public System::Windows::Forms::Form
	{
	public:
		SignalGenerator(VNADevice^ VNADev)
		{
			InitializeComponent();
			VNA = VNADev;
			freq = MINCALFREQ;
			oldChecked = -1;
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~SignalGenerator()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TrackBar^  trackBar1;
	private: System::Windows::Forms::RadioButton^  showRefl;
	private: System::Windows::Forms::RadioButton^  showTrans;
	private: System::Windows::Forms::TextBox^  magnitude;
	private: System::Windows::Forms::TextBox^  phase;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  reference;
	private: System::Windows::Forms::Label^  label3;

	private: System::Windows::Forms::TextBox^  frequency;


	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Timer^  timer1;
	private: System::ComponentModel::IContainer^  components;


	protected: 


	private: __int64 freq;
			 __int64 oldFreq;
			 int oldChecked;
			 int checked;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::RadioButton^  showSpectrum;
			 VNADevice^ VNA;					///< Vector Network Analyzer hardware object

		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->trackBar1 = (gcnew System::Windows::Forms::TrackBar());
			this->showRefl = (gcnew System::Windows::Forms::RadioButton());
			this->showTrans = (gcnew System::Windows::Forms::RadioButton());
			this->magnitude = (gcnew System::Windows::Forms::TextBox());
			this->phase = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->reference = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->frequency = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->showSpectrum = (gcnew System::Windows::Forms::RadioButton());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar1))->BeginInit();
			this->SuspendLayout();
			// 
			// trackBar1
			// 
			this->trackBar1->Location = System::Drawing::Point(19, 107);
			this->trackBar1->Maximum = 10000;
			this->trackBar1->Name = L"trackBar1";
			this->trackBar1->Size = System::Drawing::Size(648, 45);
			this->trackBar1->TabIndex = 0;
			this->trackBar1->Scroll += gcnew System::EventHandler(this, &SignalGenerator::trackBar1_Scroll);
			// 
			// showRefl
			// 
			this->showRefl->AutoSize = true;
			this->showRefl->Checked = true;
			this->showRefl->Location = System::Drawing::Point(404, 24);
			this->showRefl->Name = L"showRefl";
			this->showRefl->Size = System::Drawing::Size(86, 17);
			this->showRefl->TabIndex = 1;
			this->showRefl->TabStop = true;
			this->showRefl->Text = L"Transmission";
			this->showRefl->UseVisualStyleBackColor = true;
			this->showRefl->CheckedChanged += gcnew System::EventHandler(this, &SignalGenerator::radioButton1_CheckedChanged);
			// 
			// showTrans
			// 
			this->showTrans->AutoSize = true;
			this->showTrans->Location = System::Drawing::Point(404, 47);
			this->showTrans->Name = L"showTrans";
			this->showTrans->Size = System::Drawing::Size(72, 17);
			this->showTrans->TabIndex = 2;
			this->showTrans->Text = L"Reflextion";
			this->showTrans->UseVisualStyleBackColor = true;
			// 
			// magnitude
			// 
			this->magnitude->Location = System::Drawing::Point(572, 24);
			this->magnitude->Name = L"magnitude";
			this->magnitude->Size = System::Drawing::Size(95, 20);
			this->magnitude->TabIndex = 3;
			// 
			// phase
			// 
			this->phase->Location = System::Drawing::Point(572, 50);
			this->phase->Name = L"phase";
			this->phase->Size = System::Drawing::Size(95, 20);
			this->phase->TabIndex = 4;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(509, 27);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(57, 13);
			this->label1->TabIndex = 5;
			this->label1->Text = L"Magnitude";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(509, 53);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(37, 13);
			this->label2->TabIndex = 6;
			this->label2->Text = L"Phase";
			this->label2->Click += gcnew System::EventHandler(this, &SignalGenerator::label2_Click);
			// 
			// reference
			// 
			this->reference->Location = System::Drawing::Point(572, 76);
			this->reference->Name = L"reference";
			this->reference->Size = System::Drawing::Size(95, 20);
			this->reference->TabIndex = 7;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(509, 79);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(57, 13);
			this->label3->TabIndex = 8;
			this->label3->Text = L"Reference";
			// 
			// frequency
			// 
			this->frequency->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->frequency->Location = System::Drawing::Point(127, 24);
			this->frequency->Name = L"frequency";
			this->frequency->Size = System::Drawing::Size(255, 29);
			this->frequency->TabIndex = 10;
			this->frequency->TextChanged += gcnew System::EventHandler(this, &SignalGenerator::frequency_TextChanged);
			this->frequency->Enter += gcnew System::EventHandler(this, &SignalGenerator::frequency_Enter);
			this->frequency->Leave += gcnew System::EventHandler(this, &SignalGenerator::frequency_Leave);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label4->Location = System::Drawing::Point(23, 27);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(102, 24);
			this->label4->TabIndex = 11;
			this->label4->Text = L"Frequency";
			this->label4->Click += gcnew System::EventHandler(this, &SignalGenerator::label4_Click);
			// 
			// timer1
			// 
			this->timer1->Enabled = true;
			this->timer1->Tick += gcnew System::EventHandler(this, &SignalGenerator::timer1_Tick);
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(402, 8);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(74, 13);
			this->label5->TabIndex = 12;
			this->label5->Text = L"Measurement:";
			// 
			// showSpectrum
			// 
			this->showSpectrum->AutoSize = true;
			this->showSpectrum->Location = System::Drawing::Point(404, 70);
			this->showSpectrum->Name = L"showSpectrum";
			this->showSpectrum->Size = System::Drawing::Size(70, 17);
			this->showSpectrum->TabIndex = 13;
			this->showSpectrum->Text = L"Spectrum";
			this->showSpectrum->UseVisualStyleBackColor = true;
			// 
			// SignalGenerator
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(686, 148);
			this->Controls->Add(this->showSpectrum);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->frequency);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->reference);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->phase);
			this->Controls->Add(this->magnitude);
			this->Controls->Add(this->showTrans);
			this->Controls->Add(this->showRefl);
			this->Controls->Add(this->trackBar1);
			this->Name = L"SignalGenerator";
			this->Text = L"SignalGenerator";
			this->Load += gcnew System::EventHandler(this, &SignalGenerator::SignalGenerator_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void radioButton1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				// VNA->SetFreq(freq,showTrans->Checked);
			 }
	private: System::Void radioButton1_CheckedChanged_1(System::Object^  sender, System::EventArgs^  e) {
				//VNA->SetFreq(freq,showTrans->Checked);
			 }
	private: System::Void label2_Click(System::Object^  sender, System::EventArgs^  e) {
			 }
private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void label4_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e) {
			magnitude->Text = String::Format("{0}",actualMeasurement.magnitude);
			phase->Text = String::Format("{0}",actualMeasurement.phase);
			reference->Text = String::Format("{0}",actualMeasurement.reference);
			//frequency->Text = String::Format("{0}",(freq/1000000.0).ToString("G6"));
			if (showTrans->Checked)
				checked = 1;
			if (showRefl->Checked)
				checked = 0;
			if (showSpectrum->Checked)
				checked = 2;
			if (freq != oldFreq || oldChecked != checked) {
				oldFreq = freq;
				oldChecked = checked;
				VNA->SetFreq(oldFreq,oldChecked);
			}
		 }
private: System::Void trackBar1_Scroll(System::Object^  sender, System::EventArgs^  e) {
//			 freq = MINCALFREQ + (long long)(MAXCALFREQ - MINCALFREQ) * trackBar1->Value / trackBar1 ->Maximum;
			// freq = (__int64) pow((float)10.0, (float) (log10f( ( (float) (15*(__int64)MAXCALFREQ)) - log10f((float)MINCALFREQ)) * trackBar1->Value / trackBar1 ->Maximum + log10f(MINCALFREQ)));
			 freq = (__int64) pow((float)10.0, (float) ((
				 log10f((float)14.6*(__int64)MAXCALFREQ) - 
				 log10f((float)MINCALFREQ)) 
				 * trackBar1->Value / trackBar1 ->Maximum + log10f((float)MINCALFREQ)));
			frequency->Text = String::Format("{0}",(freq/1000000.0).ToString("G6"));
//			VNA->SetFreq(freq,showRefl->Checked);
		 }
private: System::Void SignalGenerator_Load(System::Object^  sender, System::EventArgs^  e) {
			 oldFreq = 0;
			 frequency->Text = String::Format("{0}",(freq/1000000.0).ToString("G6"));
		 }
private: System::Void frequency_Leave(System::Object^  sender, System::EventArgs^  e) {
			 try										// make sure it's an integer number
			{
				freq = (__int64)(Convert::ToDouble(frequency->Text) * 1000000);
				if (freq < MINCALFREQ) 
					MessageBox::Show("Frequency too low", "Error");
				else if (freq > 70.0*MAXCALFREQ)
					MessageBox::Show("Frequency too high", "Error");
				else {
					// trackBar1->Value = ((long long)(freq - MINCALFREQ)) * trackBar1 ->Maximum / (MAXCALFREQ - MINCALFREQ);

					trackBar1->Value = (int)( (log10((float)freq) - log10f(MINCALFREQ)) * trackBar1 ->Maximum /(log10f(7*MAXCALFREQ) - log10f(MINCALFREQ)) );

					//VNA->SetFreq(freq,showRefl->Checked);

				}
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
private: System::Void frequency_TextChanged(System::Object^  sender, System::EventArgs^  e) {
			return;
			 try										// make sure it's an integer number
			{
				freq = (__int64)(Convert::ToDouble(frequency->Text)*1000000.0);
				if (freq < MINCALFREQ) 
					MessageBox::Show("Frequency too low", "Error");
				else if (freq > 70.0*MAXCALFREQ)
					MessageBox::Show("Frequency too high", "Error");
				else {
					trackBar1->Value = (int) ((log10((float)freq) - log10f(MINCALFREQ)) * trackBar1 ->Maximum /(log10f(7*MAXCALFREQ) - log10f(MINCALFREQ)) );
					//VNA->SetFreq(freq,showRefl->Checked);

				}
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
private: System::Void frequency_Enter(System::Object^  sender, System::EventArgs^  e) {
		 try										// make sure it's an integer number
			{
				freq = (__int64)(Convert::ToDouble(frequency->Text)*1000000.0);
				if (freq < MINCALFREQ) 
					MessageBox::Show("Frequency too low", "Error");
				else if (freq > 70.0*MAXCALFREQ)
					MessageBox::Show("Frequency too high", "Error");
				else {
					trackBar1->Value =(int) ((__int64)(freq - MINCALFREQ)) * trackBar1 ->Maximum / (7*MAXCALFREQ - MINCALFREQ);
					//VNA->SetFreq(freq,showRefl->Checked);

				}
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
