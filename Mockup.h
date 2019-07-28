#pragma once
#include "Constants.h" 

#include "USB_EZ_interface.h"
//#include <Windows.h>

#define M_OPEN 1
#define M_SHORT 2
#define M_LOAD 3
#define M_THROUGH 4
#define M_ATTN 5
#include <math.h> 

namespace VNAR3 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Mockup
	/// </summary>
	public ref class Mockup : public System::Windows::Forms::Form
	{
	public:

	int mode;					///< maximum magnitude reading
	int cable_before;
	private: System::Windows::Forms::TextBox^  RText;
	public: 
	private: System::Windows::Forms::TextBox^  CText;
	private: System::Windows::Forms::TextBox^  LText;
	private: System::Windows::Forms::TrackBar^  noiseLevel;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::TextBox^  NText;
	private: System::Windows::Forms::Label^  label11;
	private: System::Windows::Forms::Label^  label12;
	private: System::Windows::Forms::Label^  label13;
	private: System::Windows::Forms::TextBox^  frequencyBox;
	private: System::Windows::Forms::Label^  label14;
	private: System::Windows::Forms::Button^  matchButton;
	private: System::Windows::Forms::Label^  label15;
	private: System::Windows::Forms::TextBox^  SCText;
	private: System::Windows::Forms::Label^  label16;
	private: System::Windows::Forms::Label^  label17;
	private: System::Windows::Forms::Label^  label18;
	private: System::Windows::Forms::TrackBar^  SourceCapacitance;
	private: System::Windows::Forms::Label^  label19;
	private: System::Windows::Forms::Label^  label20;
	public: 
			 int cable_after;
	



		Mockup(VNADevice^ VNADev)		
		{
			InitializeComponent();
			VNA = VNADev;
		}
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Mockup()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::RadioButton^  radioButton1;
	protected: 
	private: System::Windows::Forms::RadioButton^  radioButton2;
	private: System::Windows::Forms::RadioButton^  radioButton3;
	private: System::Windows::Forms::RadioButton^  radioButton4;
	private: System::Windows::Forms::RadioButton^  radioButton5;
	private: System::Windows::Forms::TrackBar^  CableBefore;
	private: System::Windows::Forms::TrackBar^  CableAfter;
	private: System::Windows::Forms::TrackBar^  Resistance;
	private: System::Windows::Forms::TrackBar^  Capacitance;
	private: System::Windows::Forms::TrackBar^  Inductance;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  label9;
	private: VNADevice^ VNA;		///< Vector Network Analyzer hardware object

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
			this->radioButton1 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton2 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton3 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton4 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton5 = (gcnew System::Windows::Forms::RadioButton());
			this->CableBefore = (gcnew System::Windows::Forms::TrackBar());
			this->CableAfter = (gcnew System::Windows::Forms::TrackBar());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->Resistance = (gcnew System::Windows::Forms::TrackBar());
			this->Capacitance = (gcnew System::Windows::Forms::TrackBar());
			this->Inductance = (gcnew System::Windows::Forms::TrackBar());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->RText = (gcnew System::Windows::Forms::TextBox());
			this->CText = (gcnew System::Windows::Forms::TextBox());
			this->LText = (gcnew System::Windows::Forms::TextBox());
			this->noiseLevel = (gcnew System::Windows::Forms::TrackBar());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->NText = (gcnew System::Windows::Forms::TextBox());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->frequencyBox = (gcnew System::Windows::Forms::TextBox());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->matchButton = (gcnew System::Windows::Forms::Button());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->SCText = (gcnew System::Windows::Forms::TextBox());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->label18 = (gcnew System::Windows::Forms::Label());
			this->SourceCapacitance = (gcnew System::Windows::Forms::TrackBar());
			this->label19 = (gcnew System::Windows::Forms::Label());
			this->label20 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CableBefore))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CableAfter))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Resistance))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Capacitance))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Inductance))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->noiseLevel))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SourceCapacitance))->BeginInit();
			this->SuspendLayout();
			// 
			// radioButton1
			// 
			this->radioButton1->AutoSize = true;
			this->radioButton1->Location = System::Drawing::Point(21, 17);
			this->radioButton1->Name = L"radioButton1";
			this->radioButton1->Size = System::Drawing::Size(51, 17);
			this->radioButton1->TabIndex = 0;
			this->radioButton1->TabStop = true;
			this->radioButton1->Text = L"Open";
			this->radioButton1->UseVisualStyleBackColor = true;
			this->radioButton1->CheckedChanged += gcnew System::EventHandler(this, &Mockup::radioButton1_CheckedChanged);
			this->radioButton1->Click += gcnew System::EventHandler(this, &Mockup::radioButton1_Click);
			// 
			// radioButton2
			// 
			this->radioButton2->AutoSize = true;
			this->radioButton2->Location = System::Drawing::Point(21, 40);
			this->radioButton2->Name = L"radioButton2";
			this->radioButton2->Size = System::Drawing::Size(50, 17);
			this->radioButton2->TabIndex = 1;
			this->radioButton2->TabStop = true;
			this->radioButton2->Text = L"Short";
			this->radioButton2->UseVisualStyleBackColor = true;
			this->radioButton2->Click += gcnew System::EventHandler(this, &Mockup::radioButton2_Click);
			// 
			// radioButton3
			// 
			this->radioButton3->AutoSize = true;
			this->radioButton3->Location = System::Drawing::Point(21, 63);
			this->radioButton3->Name = L"radioButton3";
			this->radioButton3->Size = System::Drawing::Size(49, 17);
			this->radioButton3->TabIndex = 2;
			this->radioButton3->TabStop = true;
			this->radioButton3->Text = L"Load";
			this->radioButton3->UseVisualStyleBackColor = true;
			this->radioButton3->Click += gcnew System::EventHandler(this, &Mockup::radioButton3_Click);
			// 
			// radioButton4
			// 
			this->radioButton4->AutoSize = true;
			this->radioButton4->Location = System::Drawing::Point(21, 86);
			this->radioButton4->Name = L"radioButton4";
			this->radioButton4->Size = System::Drawing::Size(65, 17);
			this->radioButton4->TabIndex = 3;
			this->radioButton4->TabStop = true;
			this->radioButton4->Text = L"Through";
			this->radioButton4->UseVisualStyleBackColor = true;
			this->radioButton4->Click += gcnew System::EventHandler(this, &Mockup::radioButton4_Click);
			// 
			// radioButton5
			// 
			this->radioButton5->AutoSize = true;
			this->radioButton5->Location = System::Drawing::Point(21, 109);
			this->radioButton5->Name = L"radioButton5";
			this->radioButton5->Size = System::Drawing::Size(75, 17);
			this->radioButton5->TabIndex = 4;
			this->radioButton5->TabStop = true;
			this->radioButton5->Text = L"Attn 40 dB";
			this->radioButton5->UseVisualStyleBackColor = true;
			this->radioButton5->CheckedChanged += gcnew System::EventHandler(this, &Mockup::radioButton5_CheckedChanged);
			// 
			// CableBefore
			// 
			this->CableBefore->Location = System::Drawing::Point(12, 267);
			this->CableBefore->Maximum = 100;
			this->CableBefore->Name = L"CableBefore";
			this->CableBefore->Size = System::Drawing::Size(394, 45);
			this->CableBefore->TabIndex = 5;
			this->CableBefore->Scroll += gcnew System::EventHandler(this, &Mockup::CableBefore_Scroll);
			this->CableBefore->ValueChanged += gcnew System::EventHandler(this, &Mockup::CableBefore_ValueChanged);
			// 
			// CableAfter
			// 
			this->CableAfter->Location = System::Drawing::Point(12, 309);
			this->CableAfter->Maximum = 100;
			this->CableAfter->Name = L"CableAfter";
			this->CableAfter->Size = System::Drawing::Size(394, 45);
			this->CableAfter->TabIndex = 6;
			this->CableAfter->ValueChanged += gcnew System::EventHandler(this, &Mockup::CableAfter_ValueChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(172, 259);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(74, 13);
			this->label1->TabIndex = 7;
			this->label1->Text = L"Lenght Before";
			this->label1->Click += gcnew System::EventHandler(this, &Mockup::label1_Click);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(172, 299);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(65, 13);
			this->label2->TabIndex = 8;
			this->label2->Text = L"Lenght After";
			// 
			// Resistance
			// 
			this->Resistance->Location = System::Drawing::Point(229, 12);
			this->Resistance->Maximum = 100;
			this->Resistance->Name = L"Resistance";
			this->Resistance->Size = System::Drawing::Size(177, 45);
			this->Resistance->TabIndex = 9;
			this->Resistance->ValueChanged += gcnew System::EventHandler(this, &Mockup::Resistance_ValueChanged);
			// 
			// Capacitance
			// 
			this->Capacitance->Location = System::Drawing::Point(229, 53);
			this->Capacitance->Maximum = 100;
			this->Capacitance->Name = L"Capacitance";
			this->Capacitance->Size = System::Drawing::Size(177, 45);
			this->Capacitance->TabIndex = 10;
			this->Capacitance->ValueChanged += gcnew System::EventHandler(this, &Mockup::Capacitance_ValueChanged);
			// 
			// Inductance
			// 
			this->Inductance->Location = System::Drawing::Point(229, 142);
			this->Inductance->Maximum = 100;
			this->Inductance->Name = L"Inductance";
			this->Inductance->Size = System::Drawing::Size(177, 45);
			this->Inductance->TabIndex = 11;
			this->Inductance->ValueChanged += gcnew System::EventHandler(this, &Mockup::Inductance_ValueChanged);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(141, 12);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(60, 13);
			this->label3->TabIndex = 12;
			this->label3->Text = L"Resistance";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(141, 53);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(67, 13);
			this->label4->TabIndex = 13;
			this->label4->Text = L"Capacitance";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(141, 142);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(61, 13);
			this->label5->TabIndex = 14;
			this->label5->Text = L"Inductance";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(226, 79);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(31, 13);
			this->label6->TabIndex = 15;
			this->label6->Text = L"0.1pf";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(373, 79);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(31, 13);
			this->label7->TabIndex = 16;
			this->label7->Text = L"10nF";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(226, 166);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(36, 13);
			this->label8->TabIndex = 17;
			this->label8->Text = L"0.1nH";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(373, 166);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(33, 13);
			this->label9->TabIndex = 18;
			this->label9->Text = L"10uH";
			// 
			// RText
			// 
			this->RText->Location = System::Drawing::Point(289, 32);
			this->RText->Name = L"RText";
			this->RText->Size = System::Drawing::Size(56, 20);
			this->RText->TabIndex = 19;
			// 
			// CText
			// 
			this->CText->Location = System::Drawing::Point(289, 76);
			this->CText->Name = L"CText";
			this->CText->Size = System::Drawing::Size(56, 20);
			this->CText->TabIndex = 20;
			// 
			// LText
			// 
			this->LText->Location = System::Drawing::Point(289, 163);
			this->LText->Name = L"LText";
			this->LText->Size = System::Drawing::Size(56, 20);
			this->LText->TabIndex = 21;
			// 
			// noiseLevel
			// 
			this->noiseLevel->Location = System::Drawing::Point(229, 186);
			this->noiseLevel->Maximum = 0;
			this->noiseLevel->Minimum = -100;
			this->noiseLevel->Name = L"noiseLevel";
			this->noiseLevel->Size = System::Drawing::Size(177, 45);
			this->noiseLevel->TabIndex = 22;
			this->noiseLevel->Value = -60;
			this->noiseLevel->Scroll += gcnew System::EventHandler(this, &Mockup::noiseLevel_Scroll);
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(141, 192);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(34, 13);
			this->label10->TabIndex = 23;
			this->label10->Text = L"Noise";
			// 
			// NText
			// 
			this->NText->Location = System::Drawing::Point(289, 211);
			this->NText->Name = L"NText";
			this->NText->Size = System::Drawing::Size(56, 20);
			this->NText->TabIndex = 24;
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(141, 66);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(67, 13);
			this->label11->TabIndex = 25;
			this->label11->Text = L"Load parallel";
			this->label11->Click += gcnew System::EventHandler(this, &Mockup::label11_Click);
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Location = System::Drawing::Point(141, 155);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(61, 13);
			this->label12->TabIndex = 26;
			this->label12->Text = L"Load series";
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Location = System::Drawing::Point(141, 25);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(31, 13);
			this->label13->TabIndex = 27;
			this->label13->Text = L"Load";
			// 
			// frequencyBox
			// 
			this->frequencyBox->Location = System::Drawing::Point(229, 237);
			this->frequencyBox->Name = L"frequencyBox";
			this->frequencyBox->Size = System::Drawing::Size(177, 20);
			this->frequencyBox->TabIndex = 28;
			this->frequencyBox->Text = L"10.0";
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Location = System::Drawing::Point(141, 240);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(56, 13);
			this->label14->TabIndex = 29;
			this->label14->Text = L"Freq(MHz)";
			// 
			// matchButton
			// 
			this->matchButton->Location = System::Drawing::Point(21, 237);
			this->matchButton->Name = L"matchButton";
			this->matchButton->Size = System::Drawing::Size(81, 20);
			this->matchButton->TabIndex = 30;
			this->matchButton->Text = L"Match";
			this->matchButton->UseVisualStyleBackColor = true;
			this->matchButton->Click += gcnew System::EventHandler(this, &Mockup::matchButton_Click);
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->Location = System::Drawing::Point(141, 112);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(77, 13);
			this->label15->TabIndex = 36;
			this->label15->Text = L"Source parallel";
			// 
			// SCText
			// 
			this->SCText->Location = System::Drawing::Point(289, 122);
			this->SCText->Name = L"SCText";
			this->SCText->Size = System::Drawing::Size(56, 20);
			this->SCText->TabIndex = 35;
			// 
			// label16
			// 
			this->label16->AutoSize = true;
			this->label16->Location = System::Drawing::Point(373, 125);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(31, 13);
			this->label16->TabIndex = 34;
			this->label16->Text = L"10nF";
			// 
			// label17
			// 
			this->label17->AutoSize = true;
			this->label17->Location = System::Drawing::Point(226, 125);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(31, 13);
			this->label17->TabIndex = 33;
			this->label17->Text = L"0.1pf";
			// 
			// label18
			// 
			this->label18->AutoSize = true;
			this->label18->Location = System::Drawing::Point(141, 99);
			this->label18->Name = L"label18";
			this->label18->Size = System::Drawing::Size(67, 13);
			this->label18->TabIndex = 32;
			this->label18->Text = L"Capacitance";
			// 
			// SourceCapacitance
			// 
			this->SourceCapacitance->Location = System::Drawing::Point(229, 99);
			this->SourceCapacitance->Maximum = 100;
			this->SourceCapacitance->Name = L"SourceCapacitance";
			this->SourceCapacitance->Size = System::Drawing::Size(177, 45);
			this->SourceCapacitance->TabIndex = 31;
			this->SourceCapacitance->ValueChanged += gcnew System::EventHandler(this, &Mockup::SourceCapacitance_ValueChanged);
			// 
			// label19
			// 
			this->label19->AutoSize = true;
			this->label19->Location = System::Drawing::Point(373, 214);
			this->label19->Name = L"label19";
			this->label19->Size = System::Drawing::Size(26, 13);
			this->label19->TabIndex = 37;
			this->label19->Text = L"0dB";
			// 
			// label20
			// 
			this->label20->AutoSize = true;
			this->label20->Location = System::Drawing::Point(226, 218);
			this->label20->Name = L"label20";
			this->label20->Size = System::Drawing::Size(41, 13);
			this->label20->TabIndex = 38;
			this->label20->Text = L"-100dB";
			// 
			// Mockup
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoValidate = System::Windows::Forms::AutoValidate::EnableAllowFocusChange;
			this->ClientSize = System::Drawing::Size(434, 339);
			this->Controls->Add(this->label20);
			this->Controls->Add(this->label19);
			this->Controls->Add(this->label15);
			this->Controls->Add(this->SCText);
			this->Controls->Add(this->label16);
			this->Controls->Add(this->label17);
			this->Controls->Add(this->label18);
			this->Controls->Add(this->SourceCapacitance);
			this->Controls->Add(this->matchButton);
			this->Controls->Add(this->label14);
			this->Controls->Add(this->frequencyBox);
			this->Controls->Add(this->label13);
			this->Controls->Add(this->label12);
			this->Controls->Add(this->label11);
			this->Controls->Add(this->NText);
			this->Controls->Add(this->label10);
			this->Controls->Add(this->noiseLevel);
			this->Controls->Add(this->LText);
			this->Controls->Add(this->CText);
			this->Controls->Add(this->RText);
			this->Controls->Add(this->label9);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->Inductance);
			this->Controls->Add(this->Capacitance);
			this->Controls->Add(this->Resistance);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->CableAfter);
			this->Controls->Add(this->CableBefore);
			this->Controls->Add(this->radioButton5);
			this->Controls->Add(this->radioButton4);
			this->Controls->Add(this->radioButton3);
			this->Controls->Add(this->radioButton2);
			this->Controls->Add(this->radioButton1);
			this->Name = L"Mockup";
			this->Text = L"Mockup";
			this->Load += gcnew System::EventHandler(this, &Mockup::Mockup_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CableBefore))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CableAfter))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Resistance))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Capacitance))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Inductance))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->noiseLevel))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SourceCapacitance))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void Mockup_Load(System::Object^  sender, System::EventArgs^  e) {
			 }
	private: System::Void Mockup_FormClosed(System::Object^  sender, System::EventArgs^  e) {
			 VNA->SetMode(0);
			 }
private: System::Void label1_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void CableBefore_Scroll(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void radioButton1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void radioButton1_Click(System::Object^  sender, System::EventArgs^  e) {
			 VNA->SetMode(M_OPEN);
			 Resistance->Value = 100;
		 }
private: System::Void radioButton2_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->VNA->SetMode(M_SHORT);
			 Resistance->Value = 0;
		 }
private: System::Void radioButton3_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->VNA->SetMode(M_LOAD);
			 Resistance->Value = 50;
		 }
private: System::Void radioButton4_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->VNA->SetMode(M_THROUGH);
			 Resistance->Value = 50;
		 }
private: System::Void radioButton5_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 this->VNA->SetMode(M_ATTN);
			 Resistance->Value = 50;
		 }
private: System::Void CableBefore_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 this->VNA->SetBefore(CableBefore->Value);
		 }
private: System::Void CableAfter_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 this->VNA->SetAfter(CableAfter->Value);
		 }

private: System::Void Resistance_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			this->VNA->SetResistance(Resistance->Value);
			RText->Text = String::Format("{0}",(pow(10, (Resistance->Value - 50.0)/20.0)*50).ToString("G5"));
		 }
private: System::Void Capacitance_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			CText->Text = String::Format("{0}pF",(pow(10,Capacitance->Value/20.0)/10).ToString("G5"));
			this->VNA->SetCapacitance(Capacitance->Value);
		 }
private: System::Void SourceCapacitance_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			SCText->Text = String::Format("{0}pF",(pow(10,SourceCapacitance->Value/20.0)/10).ToString("G5"));
			this->VNA->SetSourceCapacitance(SourceCapacitance->Value);
		 }
private: System::Void Inductance_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			LText->Text = String::Format("{0}nH",(pow(10,Inductance->Value/20.0)/10).ToString("G5"));
			this->VNA->SetInductance(Inductance->Value);
		 }

private: System::Void noiseLevel_Scroll(System::Object^  sender, System::EventArgs^  e) {
//			float n = toLin(noiseLevel->Value);
			NText->Text = String::Format("{0}dB",noiseLevel->Value.ToString("G5"));
			this->VNA->SetNoise(toLin(noiseLevel->Value));
		 }
private: System::Void label11_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void matchButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 double freq; // in GHz
			 double rL; // Load resistance in ohm
			 double rS = 50.0; // Source resistance
			 double Q; // Q factor
			 double L;
			 double C;
			 try
			{
				freq = Convert::ToDouble(frequencyBox->Text)/1000.0;
			}
			catch (System::FormatException^ pe)
			{
				MessageBox::Show(pe->Message, "Error");
			}
			catch (System::OverflowException^ pe)
			{
				MessageBox::Show(pe->Message, "Error");
			}
			rL = pow(10, (Resistance->Value - 50.0)/20.0)*50;
			if (rL>50.0) {
				Q = sqrt((rL/rS)-1);
				L = 0.159*rS*Q/freq; // in nH
				C = 159*Q/freq/rL;    // in pF
				LText->Text = String::Format("{0}nH",L.ToString("G5"));
				CText->Text = String::Format("{0}pF",C.ToString("G5"));
				SCText->Text = String::Format("0pF");

			} else {
				rS = rL;
				rL = 50.0;
				Q = sqrt((rL/rS)-1);
				L = 0.159*rS*Q/freq; // in nH
				C = 159*Q/freq/rL;    // in pF
				LText->Text = String::Format("{0}nH",L.ToString("G5"));
				SCText->Text = String::Format("{0}pF",C.ToString("G5"));
				CText->Text = String::Format("0pF");
			}

		 }
};
}
