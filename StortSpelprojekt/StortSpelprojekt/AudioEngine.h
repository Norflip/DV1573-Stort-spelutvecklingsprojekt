#pragma once
// Windows Media Foundation
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "XAudio2.lib")


//#include <x3daudio.h>
#include <xaudio2.h>
#include <string>
#include <vector>

class AudioEngine
{
public:
	AudioEngine();
	virtual ~AudioEngine();	
	IXAudio2* GetAudioMaster() { return this->audioMaster; }
	void LoadFile(const std::wstring& filename, std::vector<BYTE>& audioData, WAVEFORMATEX** waveFormatEx, unsigned int& waveLength);
	void Initialize();	
	void Initialize3DAudio();

private:
	IXAudio2* audioMaster;
	IXAudio2MasteringVoice* masterVoice;

	IMFSourceReader* sourceReader;			// Set stream
	IMFMediaType* nativeMediaType;			// Query mediainfo
	IMFMediaType* partialType;				// For creating mediafile/type/stuff
	IMFMediaType* uncompressedAudioFile;	// Working audiofile
	IMFAttributes* sourceReaderConfig;
	IMFSample* sample;
	IMFMediaBuffer* buffer;
	BYTE* localAudioData;
	
	//X3DAUDIO_LISTENER listener;	// when player is
	//X3DAUDIO_EMITTER emitter;	// where the sound comes from
	//X3DAUDIO_DSP_SETTINGS dspSettings;
};
