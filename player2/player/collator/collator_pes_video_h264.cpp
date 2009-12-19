/************************************************************************
COPYRIGHT (C) STMicroelectronics 2007

Source file name : collator_pes_video_h264.cpp
Author :           Julian

Implementation of the pes collator class for player 2.


Date        Modification                                    Name
----        ------------                                    --------
26-Jul-07   Created                                         Nick

************************************************************************/

////////////////////////////////////////////////////////////////////////////
/// \class Collator_PesVideoH264_c
///
/// Implements initialisation of collator video class for H264
///

// /////////////////////////////////////////////////////////////////////
//
//      Include any component headers

#include "collator_pes_video_h264.h"
#include "h264.h"

// /////////////////////////////////////////////////////////////////////////
//
// Locally defined constants
//

// /////////////////////////////////////////////////////////////////////////
//
// Locally defined structures
//

////////////////////////////////////////////////////////////////////////////
///
/// Initialize the class by resetting it.
///
/// During a constructor calls to virtual methods resolve to the current class (because
/// the vtable is still for the class being constructed). This means we need to call
/// ::Reset again because the calls made by the sub-constructors will not have called
/// our reset method.
///
Collator_PesVideoH264_c::Collator_PesVideoH264_c( void )
{
    if( InitializationStatus != CollatorNoError )
	return;

    Collator_PesVideoH264_c::Reset();
}

////////////////////////////////////////////////////////////////////////////
///
/// Resets and configures according to the requirements of this stream content
///
/// \return void
///
CollatorStatus_t Collator_PesVideoH264_c::Reset( void )
{
CollatorStatus_t Status;

//

    COLLATOR_DEBUG(">><<\n");

    Status = Collator_PesVideo_c::Reset();
    if( Status != CollatorNoError )
	return Status;

    Configuration.GenerateStartCodeList      = true;
    Configuration.MaxStartCodes              = 300;					// If someone inserts 32 SPS and 256 PPS  

    Configuration.StreamIdentifierMask       = PES_START_CODE_MASK;
    Configuration.StreamIdentifierCode       = PES_START_CODE_VIDEO;
    Configuration.BlockTerminateMask         = 0x9b;					// Slice normal or IDR
    Configuration.BlockTerminateCode         = 0x01;
    Configuration.IgnoreCodesRangeStart      = 0xff;					// Ignore nothing
    Configuration.IgnoreCodesRangeEnd        = 0x00;	
    Configuration.InsertFrameTerminateCode   = true;					// Insert a filler data code, to guarantee thatNo terminal code
    Configuration.TerminalCode               = 0x0C;					// picture parameter sets will always be followed by a zero byte 
											// (makes the MoreRsbpData implementation a lot simpler).
    Configuration.ExtendedHeaderLength       = 0;
    Configuration.DeferredTerminateFlag      = false;

    return CollatorNoError;
}
