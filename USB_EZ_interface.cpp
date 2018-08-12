//
//    Copyright 2004-2007, Thomas C. McDermott, N5EG
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
/// \author Thomas C. McDermott

#pragma once
// Encapsulate the EZUSBSYS device as a .NET managed object
//
// Updated: 4-4-03 to use pinned pointer for code download
//
// Updated: 7-8-03 for the .NET 2003 compiler to avoid exposing
// any methods or variables to the caller via the header file.
// It ends up being messy as a result...
//
// 2-22-04 MS VC++ posting says that using command switch
// /d1PrivateNativeTypes  will revert to 2002 behavior of not making
// the native types (unmanaged types) public. This might avoid the
// problem. KB article #822330
//
// Updated: 3-10-04 to use Helper class and forward reference in the .H
// file to sidestep the problem, pin the entire Helper object with one pointer
//


#include "stdafx.h"
#include "objbase.h"
#include "DataDisplay.h"
#include "Mockup.h"
#include "Constants.h"
#include "AudioInput.h"
#using <mscorlib.dll>
#include <math.h>

using namespace System;
	using namespace System::Windows::Forms;
//volatile float magSig;
//volatile float phaseSig;


extern "C"
{
    // Declare USB constants and structures
	#include "winioctl.h"				/// IOCTL definitions
//	#include "c:\ntddk\inc\usb100.h"	/// general USB
#pragma warning(disable: 4200)			// Supress C4200 warning in Microsoft-supplied headers
//	#include "c:\ntddk\inc\usbdi.h"		// USB Device Interface
#pragma warning(default: 4200)
//	#include "c:\ntddk\inc\devioctl.h"	/// Device IOCTL definitions
//	#include "C:\Cypress\USB\Drivers\ezusbdrv\ezusbsys.h"  /// Ezusb IOCTL codes
}
#include "USB_EZ_interface.h"
#define USB_STRING			char[256]
#define USB_CONFIG_DESCR	unsigned short[1024]


#define DDSToFreq(X) (long)(((double)(X)  * 288000000.0 / (4294967296.0 * 65536.0 ) + 0.5 ) )

/// Holds the USB device state while hiding it from other code
/// Helper is not on the managed heap, and requires explicit lifetime control.
/// Neither it nor it's contents get moved in memory by the garbage collector
/*__nogc*/ class Helper				
#define 	BULK_TRANSFER_CONTROL int			
#define USB_DEVICE_DESCRIPTOR int
#define EZUSB_DRIVER_VERSION int
#define GET_STRING_DESCRIPTOR_IN char
#define VENDOR_REQUEST_IN int
{
public:
	BULK_TRANSFER_CONTROL * pInPipe;	///< USB Bulk Transfer Input Pipe
	BULK_TRANSFER_CONTROL * pOutPipe;	///< USB Bulk Transfer Output Pipe
	USB_DEVICE_DESCRIPTOR * pDevDescr;	///< USB Device Descriptor
	EZUSB_DRIVER_VERSION * pDrvVers;	///< EZUSB Driver Version
	GET_STRING_DESCRIPTOR_IN * pStrDescr;	///< VNA Device String Descriptor
	UCHAR * pInterfaceInfo;				///< Interface Information Structure
	unsigned short * pConfigDescr;		///< Configuration Descriptor
	char * pDevString;					///< USB Device Identifier, string version
	HANDLE DevDrvHandle;				///< Device Handle
	LPDWORD pBytesReturned;				///< Size of return buffer from EzUSB calls

	Helper()
	{
		pInPipe = new BULK_TRANSFER_CONTROL;
		pOutPipe = new BULK_TRANSFER_CONTROL;
		pDevDescr = new USB_DEVICE_DESCRIPTOR;
		pDrvVers = new EZUSB_DRIVER_VERSION;
		pStrDescr = new GET_STRING_DESCRIPTOR_IN;
		pInterfaceInfo = new UCHAR[1024];
		pDevString = new USB_STRING;
		pConfigDescr = new USB_CONFIG_DESCR;
		pBytesReturned = new DWORD;
	}
	~Helper()
	{
		delete pInPipe;
		delete pOutPipe;
		delete pDevDescr;
		delete pDrvVers;
		delete pStrDescr;
		delete pInterfaceInfo;
		delete pDevString;
		delete pConfigDescr;
		delete pBytesReturned;
	}
};

	/// Find USB device, acquire handle
void VNADevice::GetHandle(void)
	{
//		HANDLE hDevice;

//		char __pin * usbdev0 = "\\\\.\\ezusb-0";		// device 0
//		char __pin * usbdev1 = "\\\\.\\ezusb-1";		// device 1

		pin_ptr<char> usbdev0 = "\\\\.\\ezusb-0";		// device 0
		pin_ptr<char> usbdev1 = "\\\\.\\ezusb-1";		// device 1

#if 0
		hDevice = CreateFile(usbdev0,		// try device 0
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

		if (hDevice==INVALID_HANDLE_VALUE)
		{
			hDevice = CreateFile(usbdev1,	// try device 1
				GENERIC_WRITE,
				FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				0,
				NULL);
		}

		if (hDevice==INVALID_HANDLE_VALUE)			
			state = -1;						// open failed
		else
#endif
			state = 1;						// open succeded

//		d->DevDrvHandle = hDevice;
};

	/// Release handle to USB device
void VNADevice::ReleaseHandle(void)
	{
#if 0
		if (state == 1)
			CloseHandle(d->DevDrvHandle);
#endif
};

	/// Set or Release RESET on the Ez-USB processor
bool VNADevice::ToggleReset(bool hold)
	{
		// use the vendor request type to set/release the reset register in the 8051

//		VENDOR_REQUEST_IN __pin * pRequest = new VENDOR_REQUEST_IN;

		pin_ptr<VENDOR_REQUEST_IN> pRequest = new VENDOR_REQUEST_IN;
#if 0
		pRequest->bRequest = 0xA0;			// Anchorchips Vendor Request Type
		pRequest->wValue = CPUCS_REG_EZUSB;		// 8051 Control / Status Register
		pRequest->wIndex = 0x00;
		pRequest->wLength = 0x01;
		pRequest->bData = (hold) ? 1 : 0;	// 1 holds 8051 in reset, 0 starts 8051 (at 0x0000)
		pRequest->direction = 0x00;

		GetHandle();

		Result = (DeviceIoControl((HANDLE)d->DevDrvHandle,
			IOCTL_Ezusb_VENDOR_REQUEST,
			pRequest,
			sizeof(VENDOR_REQUEST_IN),
			NULL,
			0,
			d->pBytesReturned,
			NULL) !=0);	// casts BOOL (which is typedef'd to int) to bool

		ReleaseHandle();
#endif
//		return(Result);
	return(true);
};

	/// Construct the VNADevice
VNADevice::VNADevice(System::IO::Ports::SerialPort^  port)							
{
	d = new Helper;					// Allocate a Helper to the VNADevice
	GetHandle();
	serialPort = port;
	ReleaseHandle();
	mode = 0;
	lastFreq = 0;
	lastDir = -1;
};
	/// Destructor for VNADevice
VNADevice::~VNADevice()
{
	delete d;						// since d is on the unmanaged heap
}
	/// Build Device Descriptors and Pipes for USB device
bool VNADevice::Init(void)						
{
	GetHandle();

// Code between these two markers is new experimental code

	//Result = DeviceIoControl((HANDLE)d->DevDrvHandle,
	//	IOCTL_Ezusb_GET_DEVICE_DESCRIPTOR,
	//	NULL,
	//	0,
	//	d->pDevDescr,
	//	sizeof(USB_DEVICE_DESCRIPTOR),
	//	d->pBytesReturned,
	//	NULL);

	//Result = DeviceIoControl((HANDLE)d->DevDrvHandle,
	//	IOCTL_EZUSB_GET_DRIVER_VERSION,
	//	NULL,
	//	0,
	//	d->pDrvVers,
	//	sizeof(EZUSB_DRIVER_VERSION),
	//	d->pBytesReturned,
	//	NULL);

	//Result = DeviceIoControl((HANDLE)d->DevDrvHandle,
	//	IOCTL_Ezusb_GET_CONFIGURATION_DESCRIPTOR,
	//	NULL,
	//	0,
	//	d->pConfigDescr,
	//	sizeof(USB_CONFIG_DESCR),
	//	d->pBytesReturned,
	//	NULL);


	//d->pStrDescr->Index = 1;
	//d->pStrDescr->LanguageId = 27;

	//Result = DeviceIoControl((HANDLE)d->DevDrvHandle,	// gets a UNICODE string
	//	IOCTL_Ezusb_GET_STRING_DESCRIPTOR,
	//	d->pStrDescr,
	//	sizeof(GET_STRING_DESCRIPTOR_IN),
	//	d->pDevString,
	//	sizeof(USB_STRING),
	//	d->pBytesReturned,
	//	NULL);

	//Result = DeviceIoControl((HANDLE)d->DevDrvHandle,
	//	IOCTL_Ezusb_GET_PIPE_INFO,
	//	NULL,
	//	0,
	//	d->pInterfaceInfo,
	//	sizeof(d->pInterfaceInfo),
	//	d->pBytesReturned,
	//	NULL);

	// BUGBUG Something before this point corrupts memory and crashes Windows - thus it's removed

	//// The following code has not been modified to the pinned object style of coding...
	////pPipeInfo = (PUSBD_INTERFACE_INFORMATION) pInterfaceInfo;

	////pInPipe->pipeNum = 0;			// most likely
	////pOutPipe->pipeNum = 1;		// most likely

	////for (int i=0; i<static_cast<int>(pPipeInfo->NumberOfPipes); i++)		// Identify pipes we need
	////{
	////	if (pPipeInfo->Pipes[i].EndpointAddress == 0x02) //EP2-Out
	////		pOutPipe->pipeNum = i;
	////	if (pPipeInfo->Pipes[i].EndpointAddress == 0x82) //EP2-In
	////		pInPipe->pipeNum = i;
	////}

// End experimental code marker

	ReleaseHandle();

	//return(i!=0);					// success if #pipes > zero
	return(true); // guarantee success
};
	/// State of USB device
int VNADevice::State() {return state;};

	/// Release reset on the 8051 processor
bool VNADevice::Start() { return(ToggleReset(0)); };		
	/// Halt the 8051 processor
bool VNADevice::Stop()	 { return(ToggleReset(1)); };		
	/// Download code to 8051 at Address
bool VNADevice::Download(array<System::Byte> ^Codebuffer, int CodeSize, unsigned short Address )	
{
//	ANCHOR_DOWNLOAD_CONTROL __pin * pDownAddr = new ANCHOR_DOWNLOAD_CONTROL;


//	unsigned char __pin *pB = &Codebuffer[0];	// pB is a pinned pointer (doesn't get moved in memory).
												// It's needed when passing a pointer to an unmanaged external DLL.
												// Codebuffer gets unpinned when pB goes out of scope, or
												// gets assigned a value of zero.

#if 0	
	pin_ptr<ANCHOR_DOWNLOAD_CONTROL> pDownAddr = new ANCHOR_DOWNLOAD_CONTROL;
	pin_ptr<System::Byte> pB = &Codebuffer[0];

	pDownAddr->Offset = Address;

	GetHandle();

	Result = (DeviceIoControl((HANDLE)d->DevDrvHandle,
		IOCTL_EZUSB_ANCHOR_DOWNLOAD,
		pDownAddr,
		sizeof(ANCHOR_DOWNLOAD_CONTROL),
		(LPVOID)pB,								
		CodeSize,										
		d->pBytesReturned,
		NULL) !=0);	// casts BOOL (which is typedef'd to int) to bool

	ReleaseHandle();
#endif
	return(true);
//	return(Result);
};
	/// Read data from BULK endpoint
bool VNADevice::Read(VNA_RXBUFFER * readbuf)				
{
//	void __pin * rb = readbuf;			// pin the readbuf in memory
#if 0	
	pin_ptr<VNA_RXBUFFER> rb = readbuf;			// pin the readbuf in memory

	d->pInPipe->pipeNum = 0;			// most likely

	GetHandle();

	Result = (DeviceIoControl((HANDLE)d->DevDrvHandle,
		IOCTL_EZUSB_BULK_READ,
		d->pInPipe,
		sizeof(BULK_TRANSFER_CONTROL),
		rb,								// readbuf
		64,
		d->pBytesReturned,
		NULL) !=0);	// casts BOOL (which is typedef'd to int) to bool

	ReleaseHandle();
#endif
//	return(Result);
	return(true);
};
	/// Write data to BULK endpoint
bool VNADevice::Write(VNA_TXBUFFER * writebuf)					
{
//	void __pin * wb = writebuf;				// pin the writebuf in memory

#if 0	
	pin_ptr<VNA_TXBUFFER> wb = writebuf;	// pin the writebuf in memory

	d->pOutPipe->pipeNum = 1;				// most likely

	GetHandle();

	Result = (DeviceIoControl((HANDLE)d->DevDrvHandle,
		IOCTL_EZUSB_BULK_WRITE,
		d->pOutPipe,
		sizeof(BULK_TRANSFER_CONTROL),
		wb,									// writebuf
		64,
		d->pBytesReturned,
		NULL) !=0);	// casts BOOL (which is typedef'd to int) to bool

	ReleaseHandle();
#endif
//	return(Result);
	return(true);
};

void VNADevice::Sweep(long startF, long stepF, int numPoints, int duration)
{
	Sweep(startF, stepF, numPoints, duration, false);
}

bool VNADevice::Sweep(long startF, long stepF, int numPoints, int duration, int power)
{
//	String ^ t;
	ArmAudio(numPoints);
	dur = duration+2; // Add 2 ms for start and stop of audio
	SetAudioPower(power);
	if (! mode){

		try {
//		t = String::Format("0 {0} {1} {2} ", startF, numPoints+5, stepF);
//		serialPort->WriteLine(String::Format("0 1000000 1 0 ", startF, numPoints, stepF));
//		Sleep(200);
		serialPort->WriteLine(String::Format("0 {0} {1} {2} {3}", startF, numPoints + 10, stepF, dur));
		Sleep(100);
		}
		catch (System::Exception^ e) {
			     MessageBox::Show(e->Message, "Error");
				 return(false);
		}

	} else {
		StartAudioSimulation(mode, numPoints + 10, dur, startF, stepF, cable_before, cable_after, 0);
	}
	return(true);
	// mp = 0;
}

void VNADevice::SetFreq(long startF, int direction)
{
	if (! mode){
		serialPort->WriteLine(String::Format("{1} {0} 1 0 5", startF, direction));
	} else {
		StartAudioSimulation(mode, 1, 5, startF, 0, cable_before, cable_after, direction);
	}
}




#define PhaseToQ(X) (short)( sin((X) * DEGR2RAD) * 1800 + 1850 )
#define PhaseToI(X) (short)( cos((X) * DEGR2RAD) * 1800 + 1850 )
#define Noise(X, Y) (short)( X * (1.0 +  Y * ((1.0 * rand() / RAND_MAX) - 0.5) ));
#define NoiseLevel 0.005
#define MaxLevel +0
#define MinLevel -57
#define MaxDAC	3500
#define MagDac(X)	((short)(MaxDAC * (( X ) - MinLevel) / (MaxLevel - MinLevel) ) )

#define MAXAVERAGE	300

static float sumreflphase[MAXAVERAGE];
static float sumtranphase[MAXAVERAGE];
static float sumreflmag[MAXAVERAGE];
static float sumtranmag[MAXAVERAGE];
static float sumreflevel[MAXAVERAGE];


static float Median(float *data, int size)
{
	int i, j;
	float swap;

	// bubble sort the elements
	for (i=size-1; i>=0; i--) {
        for (j=1; j<=i; j++) {
            if (data[j-1] > data[j])
			{
				swap = data[j-1];
				data[j-1] = data[j];
				data[j] = swap;
			}
		}
	}
	return(data[size/2]);	// the median value
}


	/// Write TxBuffer to VNA (command),  readback the result to RxBuffer (response)
bool VNADevice::WriteRead(VNA_TXBUFFER * TxBuffer, VNA_RXBUFFER * RxBuffer, int direction)
{
	// modifications to test Proto8 target changes
	bool rxsuccess = true;
//	long freq;
	float reflphase,tranphase;
	float reflmag,tranmag;
	float reflevel;
	int i;

//	int reply = TxBuffer->ReplyType;
	int retries=0;
//	return true;
	
	for (i=0; i<dur-2; i++) {
		while (!RetreiveData((int)TxBuffer->TxAccum, dur, sumreflmag[i], sumreflphase[i], sumtranmag[i], sumtranphase[i], sumreflevel[i]) && retries < 400) {
			Sleep(2);
			retries++;
		}
		if (retries >= 400)
			return (false);
	}
	reflmag = Median(sumreflmag,dur-2);
	tranmag = Median(sumtranmag,dur-2);
	reflphase = Median(sumreflphase,dur-2);
	tranphase = Median(sumtranphase,dur-2);
	reflevel = Median(sumreflevel,dur-2);

		// mp++;
	RxBuffer->Vref1 = DB2SHORT(reflevel);
	
	NormalizePhase(reflphase);
	RxBuffer->ReflPQ = PHASE2SHORT(reflphase) ;
	RxBuffer->ReflMQ = DB2SHORT(reflmag);
	NormalizePhase(tranphase);
	RxBuffer->TranPQ = PHASE2SHORT(tranphase) ;
	RxBuffer->TranMQLo = DB2SHORT(tranmag);

	return(rxsuccess);
}


void VNADevice::SetMode(int m) {
	mode = m;
}

void VNADevice::SetBefore(int l) {
	cable_before = l;
}

void VNADevice::SetAfter(int l) {
	cable_after = l;
}
