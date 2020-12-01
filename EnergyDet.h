#pragma once
#include <Windows.h>
#include "klist.h"

extern "C"
{
#include "BSC.H"
#include "STN.H"
}


typedef struct _VoiceBuffer
{
	DWORD		m_Id;
	SHORT		m_Buffer[BSC_FRAME_SIZE];	// 80 linear 16-bit buffer @ 10ms 
} VoiceBuffer;

typedef KInterlockedList<VoiceBuffer> VoiceBufQue;

typedef struct
{
	DWORD			uid;
	DWORD			prevCount;
	VoiceBuffer		*pVoiceBuf;

	bool			bSilenceOn;
	bool			bVoiceOn;
	BSC_Context     bscCtx;
	STN_Context     stnCtx;

	VoiceBufQue*	pVoiceQue;

} ENRG_Context;
