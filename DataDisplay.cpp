
//	DataDisplay.cpp -- Data display for single-frequency points;
//	used for debugging.

#include "stdafx.h"
#using <mscorlib.dll>
#include "DataDisplay.h"

DataDisplay::DataDisplay(VNA_RXBUFFER *RxData)
	{
	// Initialize the box
	Text = S"Data Point Display";
	FormBorderStyle = FormBorderStyle::Fixed3D;
	Size = System::Drawing::Size(500,600);

	// Set up the labels
	REVMQ = new Label();
	REVMQ->Size = System::Drawing::Size(300, 20);
	REVMQ->Location = Point(20, 30);
	REVMQ->Text = "Reflected Mag Q-ref ";
	d1 = new Label();
	d1->Size = System::Drawing::Size(80, 20);
	d1->Location = Point(320, 30);
	d1->Text = Convert::ToString(RxData->ReflMQ);

	REVPQ = new Label();
	REVPQ->Size = System::Drawing::Size(300, 20);
	REVPQ->Location = Point(20, 60);
	REVPQ->Text = "Reflected Phs Q-ref ";
	d2 = new Label();
	d2->Size = System::Drawing::Size(80, 20);
	d2->Location = Point(320, 60);
	d2->Text = Convert::ToString(RxData->ReflPQ);

	VREF1 = new Label();
	VREF1->Size = System::Drawing::Size(300, 20);
	VREF1->Location = Point(20, 90);
	VREF1->Text = "Reference Voltage 1 ";
	d3 = new Label();
	d3->Size = System::Drawing::Size(80, 20);
	d3->Location = Point(320, 90);
	d3->Text = Convert::ToString(RxData->Vref1);

	FWDMQ = new Label();
	FWDMQ->Size = System::Drawing::Size(300, 20);
	FWDMQ->Location = Point(20, 120);
	FWDMQ->Text = "Forward Mag Q-ref   ";
	d4 = new Label();
	d4->Size = System::Drawing::Size(80, 20);
	d4->Location = Point(320, 120);
	d4->Text = Convert::ToString(RxData->FwdMQ);

	FWDPQ = new Label();
	FWDPQ->Size = System::Drawing::Size(300, 20);
	FWDPQ->Location = Point(20, 150);
	FWDPQ->Text = "Forward Phs Q-ref   ";
	d5 = new Label();
	d5->Size = System::Drawing::Size(80, 20);
	d5->Location = Point(320, 150);
	d5->Text = Convert::ToString(RxData->FwdPQ);

	REVMI = new Label();
	REVMI->Size = System::Drawing::Size(300, 20);
	REVMI->Location = Point(20, 180);
	REVMI->Text = "Reflected Mag I-ref ";
	d6 = new Label();
	d6->Size = System::Drawing::Size(80, 20);
	d6->Location = Point(320, 180);
	d6->Text = Convert::ToString(RxData->ReflMI);

	REVPI = new Label();
	REVPI->Size = System::Drawing::Size(300, 20);
	REVPI->Location = Point(20, 210);
	REVPI->Text = "Reflected Phs I-ref ";
	d7 = new Label();
	d7->Size = System::Drawing::Size(80, 20);
	d7->Location = Point(320, 210);
	d7->Text = Convert::ToString(RxData->ReflPI);

	FWDMI = new Label();
	FWDMI->Size = System::Drawing::Size(300, 20);
	FWDMI->Location = Point(20, 240);
	FWDMI->Text = "Forward Mag I-ref   ";
	d8 = new Label();
	d8->Size = System::Drawing::Size(80, 20);
	d8->Location = Point(320, 240);
	d8->Text = Convert::ToString(RxData->FwdMI);

	FWDPI = new Label();
	FWDPI->Size = System::Drawing::Size(300, 20);
	FWDPI->Location = Point(20, 270);
	FWDPI->Text = "Forward Phs I-ref   ";
	d9 = new Label();
	d9->Size = System::Drawing::Size(80, 20);
	d9->Location = Point(320, 270);
	d9->Text = Convert::ToString(RxData->FwdPI);

	HEAD = new Label();
	HEAD->Size = System::Drawing::Size(300, 20);
	HEAD->Location = Point(20, 300);
	HEAD->Text = "Header for buffer   ";
	d0 = new Label();
	d0->Size = System::Drawing::Size(80, 20);
	d0->Location = Point(320, 300);
	d0->Text = Convert::ToString(RxData->Header);

	VREF2 = new Label();
	VREF2->Size = System::Drawing::Size(300, 20);
	VREF2->Location = Point(20, 330);
	VREF2->Text = "Reference Voltage 2 ";
	d11 = new Label();
	d11->Size = System::Drawing::Size(100, 20);
	d11->Location = Point(320, 330);
	d11->Text = Convert::ToString(RxData->Vref2);

	FREQ = new Label();
	FREQ->Size = System::Drawing::Size(300, 20);
	FREQ->Location = Point(20, 360);
	FREQ->Text = "Frequency word      ";
	d10 = new Label();
	d10->Size = System::Drawing::Size(100, 20);
	d10->Location = Point(320, 360);
	d10->Text = Convert::ToString(RxData->Freq);


	OKButton = new Button();
	OKButton->Text = S"OK";
	OKButton->Name = "OKButton";
	OKButton->Location = Point(200,500);
	OKButton->DialogResult = DialogResult::OK;

	AcceptButton = OKButton;
	Controls->Add(REVMQ);
	Controls->Add(REVPQ);
	Controls->Add(VREF1);
	Controls->Add(FWDMQ);
	Controls->Add(FWDPQ);
	Controls->Add(REVMI);
	Controls->Add(REVPI);
	Controls->Add(FWDMI);
	Controls->Add(FWDPI);
	Controls->Add(HEAD);
	Controls->Add(FREQ);
	Controls->Add(VREF2);
	Controls->Add(d1);
	Controls->Add(d2);
	Controls->Add(d3);
	Controls->Add(d4);
	Controls->Add(d5);
	Controls->Add(d6);
	Controls->Add(d7);
	Controls->Add(d8);
	Controls->Add(d9);
	Controls->Add(d0);
	Controls->Add(d10);
	Controls->Add(d11);
	Controls->Add(OKButton);

	}

