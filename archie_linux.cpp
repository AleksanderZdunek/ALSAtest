#include "archie_linux_aux.h"
#include <alsa/asoundlib.h>
#include <functional>
#include <stdexcept>
#include <string>

#include <cstdio>
#include <iostream>
#include "ALSAconfigspaceToString.h"

namespace Archie{
enum { BLOCKING_MODE = 0 };
const char defaultDeviceName[]{"default"};

snd_pcm_t* device = nullptr;
snd_pcm_hw_params_t* hwConfig = nullptr;

WAVEFORMATEXTENSIBLE configFormat{0};
std::function<DWORD(UINT32, BYTE*)> LoadData = [](UINT32, BYTE*){return AUDCLNT_BUFFERFLAGS_SILENT;};

void CleanUp()
{
	device = nullptr;
	snd_pcm_hw_params_free(hwConfig);
	hwConfig = nullptr;
}

struct throwOnError_struct
{
	const char* message;
};
inline throwOnError_struct ThrowOnError(const char* msg)
{
	return {msg};
}
inline int operator>>(int errorcode, throwOnError_struct s)
{
	if (0 > errorcode)
	{
		CleanUp();
		throw std::runtime_error("Archie: " + std::string(s.message) + "\nErrorcode: " + std::to_string(errorcode));
	}
}

void SetConfig()
{
	snd_pcm_hw_params_set_access(device, hwConfig, SND_PCM_ACCESS_RW_INTERLEAVED) >> ThrowOnError("snd_pcm_hw_params_set_access");
	snd_pcm_hw_params_set_format(device, hwConfig, SND_PCM_FORMAT_FLOAT_LE) >> ThrowOnError("snd_pcm_hw_params_set_format");
	snd_pcm_hw_params_set_channels(device, hwConfig, 2) >> ThrowOnError("snd_pcm_hw_params_set_channels");
	snd_pcm_hw_params_set_rate(device, hwConfig, 48000, 0) >> ThrowOnError("snd_pcm_hw_params_set_rate");
}

bool Init()
{
	snd_pcm_hw_params_malloc(&hwConfig) >> ThrowOnError("snd_pcm_hw_params_malloc");
	snd_pcm_open(&device, defaultDeviceName, SND_PCM_STREAM_PLAYBACK, BLOCKING_MODE) >> ThrowOnError("snd_pcm_open");
	snd_pcm_hw_params_any(device, hwConfig) >> ThrowOnError("snd_pcm_hw_params_any");
	SetConfig();
	snd_pcm_hw_params(device, hwConfig) >> ThrowOnError("snd_pcm_hw_params");
	std::cerr << config2string(hwConfig);
	return true;
}

void UnInit()
{
	//TODO!
}

void Play()
{
	snd_pcm_uframes_t bufferSize_frames;
	snd_pcm_hw_params_get_buffer_size(hwConfig, &bufferSize_frames) >> ThrowOnError("snd_pcm_hw_params_get_buffer_size");
	unsigned int nrChannels;
	snd_pcm_hw_params_get_channels(hwConfig, &nrChannels) >> ThrowOnError("snd_pcm_hw_params_get_channels");
	unsigned int sampleSize = 4; //assume a sensible sample format, for now.
	UINT32 bufferSize_bytes = bufferSize_frames * sampleSize * nrChannels;

	DWORD flag = 0;
	//BYTE* intermediatePCMBuffer = (BYTE*)malloc(bufferSize_bytes); //FIX ME
	BYTE* intermediatePCMBuffer = new BYTE[bufferSize_bytes];
	int i(0);
	do
	{	
		//std::cout << bufferSize_bytes << " " << bufferSize_frames << std::endl;
		//std::cout << -EINTR << "" <<-EBADFD << " " << -EPIPE << " " << -ESTRPIPE <<std::endl;
		//fprintf(stderr, "Traceprint1\n");	
		flag = LoadData(bufferSize_bytes, intermediatePCMBuffer);
		//fprintf(stderr, "Traceprint2\n");	
		std::cerr << snd_pcm_writei(device,intermediatePCMBuffer, bufferSize_frames);
		//fprintf(stderr, "Traceprint3 %d\n",i++);
	} while (AUDCLNT_BUFFERFLAGS_SILENT != flag);
}

}//!Archie
