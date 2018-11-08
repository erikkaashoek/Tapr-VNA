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

// DataDisplay.h -- contains the VNA BUFFER typedefs.
// Defines the format of the USB packets to and from the VNA
//
// Update: 06-04-04	TCM		Add new RX buffer type (for fast reads) and REPLYTYPE defines
// Update: 10-22-04 TCM		Add support for extended dynamic range of S21 magnitude.
//							Break QDAC level into Hi and Lo, add S21M HI and S21M Lo types to
//							both FAST and FULL responses.  Reduce FAST response from 5 to 4
//							frequencies per packet.
// Update: 03-18-06 TCM     Change Receive slow buffer to include MID-Level amplitude reading
//							and Transmit slow buffer to add value of Mid level. Change
//							IntegrationCount to MeasurementDelayTime. (Integration is done
//							by host in later versions).  Target code needs to change to
//							accomodate these buffer revisions.
//         10-14-2007 TCM   Define MeasurementSet to encalsulate one set of raw measurements at one
//							point (one discrete frequency).


#include "stdafx.h"

// FORMAT of the FULL Response:
//
//  Offset  Word (LSB first, MSB second)		Condition
//  ------  -------------------------------     --------------
//	0000    0100 								Valid frame
//  0002    S11MI S11PI VREF1 S21MIHi S21PI VREF2 	Ref=I, TX=REFL
//  000E    S11MI S11PI							Ref=I, TX=FWD
//  0012    S11MQ S11PQ VREF1 S21MQHi S21PQ VREF2	Ref=Q, TX=REFL
//  001E    S11MQ S11PQ							Ref=Q, TX=FWD
//  0022    S21MQLo 
//  0024    S21MQMid
//  0026	remainder of frame all zeros

#define	VNA_REPLYTYPE_FULL	0x0					// Full response buffer  (complete data set)
#define VNA_REPLYTYPE_FAST	0x1					// Fast response buffer (5 readings)

	/// Format of Received USB Packet from VNA for Slow mode 
typedef struct _VNA_RXBUFFER {
	///\struct Received USB Packet from VNA for slow mode
	unsigned short Header;		///< Received Packet Type and Status
	unsigned short ReflMI;		///< Reflection Magnitude I
	unsigned short ReflPI;		///< Reflection Phase I
	unsigned short Vref1;		///< Voltage Reference 1
	unsigned short TranMI;		///< Transmission Magnitude I
	unsigned short TranPI;		///< Transmission Phase I
	unsigned short Vref2;		///< Voltage Reference 2
	unsigned short FwdMI;		///< Forward Magnitude I
	unsigned short FwdPI;		///< Forward Phase I
	unsigned short ReflMQ;		///< Reflection Magnitude Q
	unsigned short ReflPQ;		///< Reflection Phase Q
	unsigned short Vref1a;		///< Voltage Reference 1
	unsigned short TranMQHi;	///< Transmission Magnitude Q Hi count received
	unsigned short TranPQ;		///< Transmission Phase Q
	unsigned short Vref2a;		///< Voltage Reference 2
	unsigned short FwdMQ;		///< Forward Magnitude Q
	unsigned short FwdPQ;		///< Forward Phase Q
	unsigned short TranMQLo;	///< Transmission Magnitude Q Lo count received
	unsigned short TranMQMid;	///< Transmission Magnitude Q Mid count received - only used for SLOW reads
	unsigned short TranPILow;	///< Transmission Phase I with low reference level
	unsigned short TranPQLow;	///< Transmission Phase Q with low reference level
	unsigned long Freq;
	unsigned short fill1, fill2, fill3, fill4, fill5; ///< filler (unused locations in buffer)
	unsigned short fill6, fill7, fill8, fill9, fill10; ///< filler
	unsigned short check;		///< filler
} VNA_RXBUFFER;

	/// Format of Transmit USB Packet sent to VNA
typedef struct _VNA_TXBUFFER {
	__int64 TxAccum;			///< 8 byte transmit DDS accumulator value (we use 6 LSBytes)
	unsigned short IDAClevelHi;	///< 2 byte I-DAC (Transmit Hi) level. Changed name in VNAR3
	unsigned short QDAClevel;	///< 2 byte Q-DAC (Reference) level
	unsigned short ReplyType;	///< Rx buffer reply type requested (VNA_REPLYTYPE_FULL or FAST)
	unsigned short MeasureDelay;	///< Instrument measurement settling time (VNAR 1.5)
	__int64 Freq2;				///< TxAccumulator values for multi-freq read frequency #2 (ReplyType > 0)
	__int64 Freq3;				///< TxAccumulator values for multi-freq read frequency #3 (ReplyType > 0)
	__int64 Freq4;				///< TxAccumulator values for multi-freq read frequency #4 (ReplyType > 0)
	__int64 dummy;				// placeholder for Freq5 (removed to provide room for 3-Mag levels).

	unsigned short IDAClevelLo; ///< 2 byte I-DAC (Transmit Lo) level. New for VNAR3
	unsigned short IDAClevelMid; ///< 2 byte I-DAC (Transmit Mid) level. New for VNAR 1.5, only SLOW reads
	unsigned short IDAClevelPhLow;  //< 2-byte I-DAC level for lower level phase measurements
	unsigned short fill1;
	unsigned short dummya, dummy0, dummy1, dummy2;
} VNA_TXBUFFER;


	/// Format of Received USB Packet from VNA for Fast mode 
typedef struct _VNA_RXBUFF_FAST {	// Gives 4 reading sets per USB packet
	unsigned short Header;
	unsigned short ReflMQ1, ReflPI1, ReflPQ1, TranMQ1Hi, TranPI1, TranPQ1, TranMQ1Lo, TranMQ1Mid;
	unsigned short ReflMQ2, ReflPI2, ReflPQ2, TranMQ2Hi, TranPI2, TranPQ2, TranMQ2Lo, TranMQ2Mid;
	unsigned short ReflMQ3, ReflPI3, ReflPQ3, TranMQ3Hi, TranPI3, TranPQ3, TranMQ3Lo, TranMQ3Mid;
	unsigned short ReflMQ4, ReflPI4, ReflPQ4, TranMQ4Hi, TranPI4Mix, TranPQ4Mix, TranMQ4LoMix;
			// The 'Mix' form of the last 3 parameters uses the highest nibble of each to Hold TranMQ4Mid
			// which has to be bit-mask removed and reassembled from the words.  The actual value
			// of TranPI4, TranPQ4, and TranMQ4Lo are the lower 12 bits of each variable respectively
			// This is to enable room for one more 12-bit reply than could otherwise fit in the 64-byte
			// reply packet. Pretty messy.
} VNA_RXBUFF_FAST;


// TraceSet for one complete data point. The old code defined them this way: 
				//trace1[m] = RxBuf->ReflMI;
				//trace2[m] = RxBuf->ReflMQ;
				//trace3[m] = RxBuf->ReflPI;
				//trace4[m] = RxBuf->ReflPQ;

				//trace5[m] = RxBuf->Vref1;

				//trace6[m] = RxBuf->TranMI;
				//trace7[m] = RxBuf->TranMQHi;
				//trace8[m] = RxBuf->TranPI;
				//trace9[m] = RxBuf->TranPQ;

				//trace10[m] = RxBuf->Vref2;
				//trace11[m] = RxBuf->TranMQLo;
				//trace12[m] = RxBuf->TranMQMid;

				//trace13[m] = RxBuf->TranPILow;
				//trace14[m] = RxBuf->TranPQLow;

/// Set of raw instrument readings for a measurement point at one frequency
public ref struct MeasurementSet {
	unsigned short ReflMI;				///< Reflection Magnitude I (trace1)
	unsigned short ReflMQ;				///< Reflection Magnitude Q (trace2)
	unsigned short ReflPI;				///< Reflection Phase I (trace3)
	unsigned short ReflPQ;				///< Reflection Phase Q (trace4)
	unsigned short Vref1;				///< Reference voltage 1 (trace5)
	unsigned short TranMI;				///< Transmission Magnitude I (trace6)
	unsigned short TranMQHi;			///< Transmission Magnitude Q Hi (trace 7)
	unsigned short TranPI;				///< Transmission Phase I (trace 8)
	unsigned short TranPQ;				///< Transmission Phase Q (trace 9)
	unsigned short Vref2;				///< Reference voltage 2 (trace10)
	unsigned short TranMQLo;			///< Transmission Magnitude Q Low (trace 11)
	unsigned short TranMQMid;			///< Transmission Magnitude Q Mid (trace 12)
	unsigned short TranPILow;			///< Transmission Phase I Low ref level (trace 13)
	unsigned short TranPQLow;			///< Transmission Phase Q Low ref level (trace 14)
};

