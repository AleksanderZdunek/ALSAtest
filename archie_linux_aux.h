#pragma once
#ifndef ARCHIE_LINUX_AUX_H
#define ARCHIE_LINUX_AUX_H
#include <cstdint>

typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint32_t UINT32;
typedef unsigned char BYTE;

enum _AUDCLNT_BUFFERFLAGS
{
	AUDCLNT_BUFFERFLAGS_SILENT = 0x2
};

struct GUID
{
	unsigned long Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char Data4[8];
};

struct WAVEFORMATEX
{
	WORD wFormatTag;
	WORD nChannels;
	DWORD nSamplesPerSec;
	DWORD nAvgBytesPerSec;
	WORD nBlockAlign;
	WORD wBitsPerSample;
	WORD cbSize;
};

struct WAVEFORMATEXTENSIBLE
{
	WAVEFORMATEX Format;
	union
	{
		WORD wValidBitsPerSample;
		WORD wSamplesPerBlock;
		WORD wReserved;
	} Samples;
	DWORD dwChannelMask;
	GUID SubFormat;
};

#endif // !ARCHIE_LINUX_AUX_H