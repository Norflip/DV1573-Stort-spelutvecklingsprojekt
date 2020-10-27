#pragma once
// Windows Media Foundation
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid")

#include <xaudio2.h>
#include <string>
#include <vector>

class AudioEngine
{
public:
	AudioEngine();
	~AudioEngine();

private:
	IXAudio2* audio;
	IXAudio2MasteringVoice* masterVoice;

	IMFSourceReader* sourceReader;			// Set stream
	IMFMediaType* nativeMediaType;			// Query mediainfo
	IMFMediaType* partialType;				// For creating mediafile/type/stuff
	IMFMediaType* uncompressedAudioFile;	// Working audiofile

	IMFAttributes* sourceReaderConfig;

	void Initialize();
	void LoadFile(const std::wstring& filename, std::vector<BYTE>& audioData, WAVEFORMATEX** waveFormatEx, unsigned int& waveLength);
};
