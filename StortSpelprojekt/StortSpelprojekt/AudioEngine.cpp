#include "stdafx.h"
#include "AudioEngine.h"

AudioEngine::AudioEngine() : audioMaster(nullptr), masterVoice(nullptr), sourceReader(nullptr), nativeMediaType(nullptr), partialType(nullptr), uncompressedAudioFile(nullptr), sourceReaderConfig(nullptr), sample(nullptr), buffer(nullptr), localAudioData(NULL)
{
	
}

AudioEngine::~AudioEngine()
{
	MFShutdown();
	masterVoice->DestroyVoice();
	audioMaster->StopEngine();

	/*if (localAudioData)
		delete localAudioData;
	if (sample)
		delete sample;
	if (partialType)
		delete partialType;
	if (uncompressedAudioFile)
		delete uncompressedAudioFile;
	if (nativeMediaType)
		delete nativeMediaType;
	if (sourceReader)
		delete sourceReader;*/
	/*if (audioMaster)
		delete audioMaster;
	if(masterVoice)
		delete masterVoice;*/
	/*if(!audio && !masterVoice)
		OutputDebugStringW(L"Ok delete motherfakka");*/
}

void AudioEngine::Initialize()
{
	HRESULT startUp = MFStartup(MF_VERSION);
	if (FAILED(startUp))
		OutputDebugStringW(L"Critical: Unable to start");

	startUp = MFCreateAttributes(&sourceReaderConfig, 1);
	if(FAILED(startUp))
		OutputDebugStringW(L"Unable to create config");

	startUp = sourceReaderConfig->SetUINT32(MF_LOW_LATENCY, true);
	if (FAILED(startUp))
		OutputDebugStringW(L"Unable to set WMF config");

	startUp = XAudio2Create(&audioMaster);
	if (FAILED(startUp))
		OutputDebugStringW(L"Unable to create XAudio2 engine");

	startUp = audioMaster->CreateMasteringVoice(&masterVoice);
	if (FAILED(startUp))
		OutputDebugStringW(L"Unable to create mastering voice for xAudio");
}

void AudioEngine::LoadFile(const std::wstring& filename, std::vector<BYTE>& audioData, WAVEFORMATEX** waveFormatEx, unsigned int& waveLength)
{	
	DWORD streamIndex = (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM;

	/* Create the source reader */
	HRESULT hr = MFCreateSourceReaderFromURL(filename.c_str(), sourceReaderConfig, &sourceReader);
	if (FAILED(hr))
		OutputDebugStringW(L"Unable to create source reader from URL!");

	/* Select the first audio stream, and deselect all other streams */
	hr = sourceReader->SetStreamSelection((DWORD)MF_SOURCE_READER_ALL_STREAMS, false);
	if (FAILED(hr))
		OutputDebugStringW(L"Unable to disable streams!");

	hr = sourceReader->SetStreamSelection(streamIndex, true);
	if (FAILED(hr))
		OutputDebugStringW(L"Unable to enable first audio stream!");


	/* 
		1. Query information about the current media file 
		2. Make sure it is an audio file with 
		3. Check if the audio file is compressed or uncompressed
		4. Do stuff based on pnt 3. ( Nothing or decompressing to playable format ) 
	*/

	hr = sourceReader->GetNativeMediaType(streamIndex, 0, &nativeMediaType);
	if (FAILED(hr))
		OutputDebugStringW(L"Unable to query media information!");

	GUID majorType{};
	hr = nativeMediaType->GetGUID(MF_MT_MAJOR_TYPE, &majorType);
	if (majorType != MFMediaType_Audio)
		OutputDebugStringW(L"Critical error: the requested file is not an audio file!");

	GUID subType{};
	hr = nativeMediaType->GetGUID(MF_MT_SUBTYPE, &subType);
	if (subType == MFAudioFormat_Float || subType == MFAudioFormat_PCM)
	{
		// Audio file is uncompressed -> nothing to do here
	}
	else
	{		
		hr = MFCreateMediaType(&partialType);
		if (FAILED(hr))
			OutputDebugStringW(L"Critical error: Unable create media type!");

		/* Set the media type to "audio" */
		hr = partialType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		if (FAILED(hr))
			OutputDebugStringW(L"Critical error: Unable to set media type to audio!");

		/* Set to uncompressed media type */
		hr = partialType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
		if (FAILED(hr))
			OutputDebugStringW(L"Critical error: Unable to set guid of media type to uncompressed!");

		hr = sourceReader->SetCurrentMediaType(streamIndex, NULL, partialType);
		if (FAILED(hr))
			OutputDebugStringW(L"Critical error: Unable to set current media type!");
	}


	/* 
		1. Create audiofile out of uncompressed format 
		2. Load to xAudio Buffer 
	*/ 

	hr = sourceReader->GetCurrentMediaType(streamIndex, &uncompressedAudioFile);
	if (FAILED(hr))
		OutputDebugStringW(L"Critical error: Unable to retrieve the current media type!");

	hr = MFCreateWaveFormatExFromMFMediaType(uncompressedAudioFile, waveFormatEx, &waveLength);
	if (FAILED(hr))
		OutputDebugStringW(L"Critical error: Unable to create the wave format!");

	hr = sourceReader->SetStreamSelection(streamIndex, true);
	if (FAILED(hr))
		OutputDebugStringW(L"Critical error: Unable to select audio stream!");


	DWORD localAudioDataLength = 0;
	while (true)
	{
		DWORD flags = 0;
		hr = sourceReader->ReadSample(streamIndex, 0, nullptr, &flags, nullptr, &sample);
		if (FAILED(hr))
			OutputDebugStringW(L"Critical error: Unable to read audio sample!");

		/* Check if the data is still valid */
		if (flags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
			break;

		/* Check for end of stream */
		if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
			break;

		if (sample == nullptr)
			continue;

		hr = sample->ConvertToContiguousBuffer(&buffer);
		if (FAILED(hr))
			OutputDebugStringW(L"Critical error: Unable to convert audio sample to contiguous buffer!");

		/* Lock buffer and copy data to local memory, then unlock it */
		hr = buffer->Lock(&localAudioData, nullptr, &localAudioDataLength);
		if (FAILED(hr))
			OutputDebugStringW(L"Critical error: Unable to lock the audio buffer!");

		for (size_t i = 0; i < localAudioDataLength; i++)
			audioData.push_back(localAudioData[i]);

		hr = buffer->Unlock();
		localAudioData = nullptr;

		if (FAILED(hr))
			OutputDebugStringW(L"Critical error while unlocking the audio buffer!");
	}
}