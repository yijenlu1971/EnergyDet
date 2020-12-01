#include <Windows.h>
#include <iostream>
#include "EnergyDet.h"


#define ASR_FRAME_SIZE	800

#define __DEBUG_FRAME__
#ifdef __DEBUG_FRAME__
int		iFrame = 0;
#endif

STN_Config MyConfig =
{
	FALSE,		// noise detection
	40,			// pauseTimeout (unit in frame. 1 frame = 10ms)
	100,		// silenceTimeout (unit in frame. 1 frame = 10ms)
	1300,		// dialtoneTimeout (frames)
	500,		// reorderTimeout (frames)
	IP_TRK_SILENCE_THRESHOLD,	// silence threshold (dbm)
	VOICE_LEVEL
};

//////////////////////////////////////////////////////////////////////////////////////////////////
/// Peter Lu Dec-2020 : energy detection for ASR
/// 
/// void EnergyDetInit(ENRG_Context* ctx) : initialize the context of energyDet
/// void EnergyDetDeInit(ENRG_Context* ctx) : remove all voice buffers from queue
/// 
/// void EnergyDetProcess : 
///		process every ASR frame, calculate the energy of 10-ms, and detect voice/silence
/// 
/// USHORT EnergyDetGetBuf :
///		get buffer at run-time, or dump all buffer when finish
//////////////////////////////////////////////////////////////////////////////////////////////////
void EnergyDetInit(ENRG_Context* ctx)
{
	ctx->uid = 0;
	ctx->bSilenceOn = ctx->bVoiceOn = false;
	BSC_Init(&ctx->bscCtx);
	STN_Init(&ctx->stnCtx, &MyConfig);

	ctx->pVoiceQue = new VoiceBufQue;
	ctx->pVoiceBuf = NULL;
}

void EnergyDetDeInit(ENRG_Context* ctx)
{
	if (ctx->pVoiceQue != NULL)
	{
		VoiceBuffer* pBuf;

//		std::cout << "Queue size: " << ctx->pVoiceQue->Count() << std::endl;

		while (!ctx->pVoiceQue->IsEmpty())
		{
			pBuf = ctx->pVoiceQue->RemoveHead();
			if (pBuf) free(pBuf);
		}

		delete ctx->pVoiceQue;
		ctx->pVoiceQue = NULL;
	}
}

void EnergyDetProcess(ENRG_Context *ctx, SHORT *pVoiceBuf, USHORT frameLen)
{
	VoiceBuffer* pBuf;
	SHORT	tempBuf[BSC_FRAME_SIZE];
	USHORT	i;
	int		ptr = 0;

	for (i = 0; i < frameLen / BSC_FRAME_SIZE; i++)
	{
		BSC_Analyze(&ctx->bscCtx, &pVoiceBuf[ptr], tempBuf);
		STN_Detect(&ctx->stnCtx, ctx->bscCtx.mean);

		/////////////////////////////////////////////////////////
		// the beginning of the stream or silence on (pre-buffer)
		if (!ctx->bVoiceOn)
		{
			pBuf = (VoiceBuffer*)malloc(sizeof(VoiceBuffer));
			if (pBuf != NULL)
			{
				pBuf->m_Id = ctx->uid++;
				CopyMemory(pBuf->m_Buffer, &pVoiceBuf[ptr], sizeof(SHORT) * BSC_FRAME_SIZE);
				ctx->pVoiceQue->InsertTail(pBuf);
			}

			///////////////////////////////////////////////////////
			// remove silence from queue, but reserve some tail
			if (++ctx->prevCount >= MyConfig.pauseTimeout)
			{
				pBuf = ctx->pVoiceBuf;
				if (pBuf)
				{
					pBuf = ctx->pVoiceQue->Next(pBuf);

					if (pBuf)
					{
						pBuf = ctx->pVoiceQue->Remove(pBuf);
						ctx->prevCount--;
					}
				}
				else
				{
					pBuf = ctx->pVoiceQue->RemoveHead();
					if (pBuf) free(pBuf);

					ctx->prevCount--;
				}
			}
		}

		if (ctx->stnCtx.results & STN_SILENCE)
		{
			if (!ctx->bSilenceOn)
			{
				STN_Init(&ctx->stnCtx, &MyConfig);

				///////////////////////////////////////////////////////
				// remove pre-buffer from queue after detecting silence
				for (int n = 0; n < MyConfig.silenceTimeout && ctx->pVoiceQue->Count() > 0; n++)
				{
					pBuf = ctx->pVoiceQue->RemoveTail();
					if (pBuf)
					{
						free(pBuf);
					}
					else
					{
						break;
					}
				}

				ctx->prevCount = 0;
				if (ctx->pVoiceQue->Count() > 0)
				{
					ctx->pVoiceBuf = ctx->pVoiceQue->Tail();
				}
				else
				{
					ctx->pVoiceBuf = NULL;
				}

				ctx->bVoiceOn = false;
				ctx->bSilenceOn = true;

#ifdef __DEBUG_FRAME__
				std::cout << "Sil detected @" << (iFrame * (ASR_FRAME_SIZE / BSC_FRAME_SIZE) + i) * 10 << " ms" << std::endl;
#endif
			}
			else
			{
//				std::cout << "Sil...." << (iFrame * (ASR_FRAME_SIZE / BSC_FRAME_SIZE) + i) * 10 << " ms" << endl;
			}
		}
		else if (ctx->stnCtx.results & STN_VOICE)
		{
			if (!ctx->bVoiceOn)
			{
//				STN_Init(&ctx->stnCtx, &MyConfig);

				ctx->bVoiceOn = true;
				ctx->bSilenceOn = false;
#ifdef __DEBUG_FRAME__
				std::cout << "Voi detected @" << (iFrame * (ASR_FRAME_SIZE / BSC_FRAME_SIZE) + i) * 10 << " ms" << std::endl;
#endif
			}

			///////////////////////////////////////////////////////
			// put voice into the tail of the queue
			// std::cout << "Voice .... @" << (iFrame * (ASR_FRAME_SIZE / BSC_FRAME_SIZE) + i) * 10 << " ms" << endl;
			pBuf = (VoiceBuffer*)malloc(sizeof(VoiceBuffer));
			if (pBuf != NULL)
			{
				pBuf->m_Id = ctx->uid++;
				CopyMemory(pBuf->m_Buffer, &pVoiceBuf[ptr], sizeof(SHORT) * BSC_FRAME_SIZE);
				ctx->pVoiceQue->InsertTail(pBuf);
			}
		}
		
		ptr += BSC_FRAME_SIZE;
	}

#ifdef __DEBUG_FRAME__
	iFrame++;
#endif
}

USHORT EnergyDetGetBuf(ENRG_Context* ctx, SHORT* pVoiceBuf, USHORT maxLen, BOOLEAN bAll = FALSE)
{
	USHORT		rcLen = 0;
	USHORT		dist = ctx->bSilenceOn ? MyConfig.silenceTimeout : MyConfig.pauseTimeout;
	VoiceBuffer	* pBuf;

	if (ctx->pVoiceQue->IsEmpty()) return 0;

	if (bAll) dist = 0;	// dump all buffer

	while ((ctx->pVoiceQue->Count() > dist) && (rcLen < maxLen))
	{
		pBuf = ctx->pVoiceQue->RemoveHead();
		if (pBuf)
		{
			CopyMemory(&pVoiceBuf[rcLen], pBuf->m_Buffer, sizeof(SHORT) * BSC_FRAME_SIZE);
			rcLen += BSC_FRAME_SIZE;
			free(pBuf);
		}
	}

#ifdef __DEBUG_FRAME__
//	std::cout << "GetBuf @" << (iFrame * (ASR_FRAME_SIZE / BSC_FRAME_SIZE)) * 10 << " ms" << std::endl;
#endif

	return rcLen;
}

/// Peter Lu Dec-2020 : energy detection for ASR
//////////////////////////////////////////////////////////////////////////////////////////////////


struct WAVEHEADER
{
	UCHAR	lpszName[4];
	DWORD	dwSize;
	UCHAR	lpszID[4];
};

struct RIFFCHUNK
{
	UCHAR	lpszName[4];
	DWORD	dwSize;
};

#define RECAST(x) reinterpret_cast<x>
#define STCAST(x) static_cast<x>

///////////////////////
// Main function
///////////////////////
int main()
{
	char infile[] = "test.wav";
	char outfile[] = "testOut.wav";
	HANDLE		hWaveFile;
	WAVEHEADER	header;
	RIFFCHUNK	rcChunk;
	WAVEFORMATEX wfInfo;
	PCMWAVEFORMAT	wfInfo2;
	DWORD	dwBytesOut, inLength, outLength = 0;
	UCHAR	*lpAudioData;

	hWaveFile = CreateFile(infile, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hWaveFile == INVALID_HANDLE_VALUE)
	{
		std::cout << "Can't open " << infile << std::endl;
		return -1;
	}

	// (1) read the wav header of the test
	SetFilePointer(hWaveFile, 12, 0, FILE_BEGIN);
	BOOL rc = ReadFile(hWaveFile, RECAST(LPVOID)(&rcChunk), STCAST(DWORD)(sizeof(RIFFCHUNK)), &dwBytesOut, NULL);
	rc = ReadFile(hWaveFile, RECAST(LPVOID)(&wfInfo), STCAST(DWORD)(sizeof(WAVEFORMATEX)), &dwBytesOut, NULL);
	
	if (wfInfo.nSamplesPerSec != 8000 || wfInfo.wBitsPerSample != 16)
	{
		std::cout << "Format NOT supported. Rate:" << wfInfo.nSamplesPerSec << " Bps:" << wfInfo.wBitsPerSample << std::endl;
		return -1;
	}

	if (rcChunk.dwSize != sizeof(WAVEFORMATEX))
	{
		wfInfo.cbSize = 0;
		SetFilePointer(hWaveFile, 12 + sizeof(RIFFCHUNK) + rcChunk.dwSize, 0, FILE_BEGIN);
	}
	else
	{
		SetFilePointer(hWaveFile, wfInfo.cbSize, 0, FILE_CURRENT);
	}

	do
	{
		rc = ReadFile(hWaveFile, RECAST(LPVOID)(&rcChunk), STCAST(DWORD)(sizeof(rcChunk)), &dwBytesOut, NULL);
	} while (memcmp(rcChunk.lpszName, "data", 4) != 0);

	inLength = rcChunk.dwSize;
	std::cout << "Total length = " << inLength/16 << " ms" << std::endl;
	lpAudioData = RECAST(UCHAR*)(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, inLength));

	// (2) read the raw data of the voice file
	rc = ReadFile(hWaveFile, lpAudioData, inLength, &dwBytesOut, NULL);
	CloseHandle(hWaveFile);
	
	// (3) write into a voice file after trimming silence
	hWaveFile = CreateFile(outfile, GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hWaveFile == INVALID_HANDLE_VALUE)
	{
		std::cout << "Can't open " << outfile << std::endl;
		return -1;
	}

	memcpy(header.lpszName, "RIFF", 4);
	memcpy(header.lpszID, "WAVE", 4);
	header.dwSize = 36;
	rc = WriteFile(hWaveFile, RECAST(LPVOID)(&header), STCAST(DWORD)(sizeof(WAVEHEADER)), &dwBytesOut, NULL);

	memcpy(rcChunk.lpszName, "fmt ", 4);
	rcChunk.dwSize = sizeof(wfInfo2);
	rc = WriteFile(hWaveFile, RECAST(LPVOID)(&rcChunk), STCAST(DWORD)(sizeof(RIFFCHUNK)), &dwBytesOut, NULL);

	wfInfo2.wf.wFormatTag = WAVE_FORMAT_PCM;
	wfInfo2.wf.nChannels = 1;
	wfInfo2.wf.nSamplesPerSec = 8000;
	wfInfo2.wf.nAvgBytesPerSec = 16000;
	wfInfo2.wf.nBlockAlign = 2;
	wfInfo2.wBitsPerSample = 16;
	rc = WriteFile(hWaveFile, RECAST(LPVOID)(&wfInfo2), STCAST(DWORD)(sizeof(PCMWAVEFORMAT)), &dwBytesOut, NULL);

	memcpy(rcChunk.lpszName, "data", 4);
	rcChunk.dwSize = 0;
	rc = WriteFile(hWaveFile, RECAST(LPVOID)(&rcChunk), STCAST(DWORD)(sizeof(RIFFCHUNK)), &dwBytesOut, NULL);

	// (4) Process energy detection
	ENRG_Context	energyCtx;
	SHORT* pBuf = (SHORT * )lpAudioData;
	DWORD	i, ptrLen = 0;
	SHORT* pVoiceOutBuf = (SHORT*)malloc(sizeof(SHORT)* ASR_FRAME_SIZE);

	EnergyDetInit(&energyCtx);

	inLength /= 2;
	for (i = 0; i < inLength / ASR_FRAME_SIZE; i++)
	{
		// (5) do process every ASR frame
		EnergyDetProcess(&energyCtx, pBuf+ptrLen, ASR_FRAME_SIZE);
		ptrLen += ASR_FRAME_SIZE;

		// (6) get available buffer at run-time
		while (true)
		{
			USHORT len = EnergyDetGetBuf(&energyCtx, pVoiceOutBuf, ASR_FRAME_SIZE);

			if (len == 0) break;

			rc = WriteFile(hWaveFile, RECAST(LPVOID)(pVoiceOutBuf), STCAST(DWORD)(sizeof(SHORT) * len), &dwBytesOut, NULL);
			if (rc == TRUE)
			{
				outLength += dwBytesOut;
			}
		}
	}

	// dump all buffer
	/*while (true)
	{
		USHORT len = EnergyDetGetBuf(&energyCtx, pVoiceOutBuf, ASR_FRAME_SIZE, TRUE);

		if (len == 0) break;

		rc = WriteFile(hWaveFile, RECAST(LPVOID)(pVoiceOutBuf), STCAST(DWORD)(sizeof(SHORT) * len), &dwBytesOut, NULL);
		if (rc == TRUE)
		{
			outLength += dwBytesOut;
		}
	}*/

	EnergyDetDeInit(&energyCtx);

	SetFilePointer(hWaveFile, 0, 0, FILE_BEGIN);
	header.dwSize = 36 + outLength;
	rc = WriteFile(hWaveFile, RECAST(LPVOID)(&header), STCAST(DWORD)(sizeof(WAVEHEADER)), &dwBytesOut, NULL);

	SetFilePointer(hWaveFile, sizeof(WAVEHEADER) + sizeof(RIFFCHUNK) + sizeof(PCMWAVEFORMAT), 0, FILE_BEGIN);
	memcpy(rcChunk.lpszName, "data", 4);
	rcChunk.dwSize = outLength;
	rc = WriteFile(hWaveFile, RECAST(LPVOID)(&rcChunk), STCAST(DWORD)(sizeof(RIFFCHUNK)), &dwBytesOut, NULL);

	std::cout << "Voice length = " << outLength / 16 << " ms" << std::endl;

	free(pVoiceOutBuf);
	CloseHandle(hWaveFile);
	HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, lpAudioData);
	return 0;
}
