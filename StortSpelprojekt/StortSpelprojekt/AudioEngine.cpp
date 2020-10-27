#include "AudioEngine.h"

AudioEngine::AudioEngine() : audio(nullptr), masterVoice(nullptr), sourceReader(nullptr), sourceReaderConfig(nullptr), nativeMediaType(nullptr), partialType(nullptr), uncompressedAudioFile(nullptr)
{
	
}

AudioEngine::~AudioEngine()
{
	MFShutdown();
	masterVoice->DestroyVoice();
	audio->StopEngine();

	if (uncompressedAudioFile)
		delete uncompressedAudioFile;
	if (nativeMediaType)
		delete nativeMediaType;
	if (sourceReader)
		delete sourceReader;
	if (audio)
		delete audio;
	if(masterVoice)
		delete masterVoice;
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

	startUp = XAudio2Create(&audio);
	if (FAILED(startUp))
		OutputDebugStringW(L"Unable to create XAudio2 engine");

	startUp = audio->CreateMasteringVoice(&masterVoice);
	if (FAILED(startUp))
		OutputDebugStringW(L"Unable to create mastering voice for xAudio");
}

void AudioEngine::LoadFile(const std::wstring& filename, std::vector<BYTE>& audioData, WAVEFORMATEX** waveFormatEx, unsigned int& waveLength)
{
	DWORD streamIndex = (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM;

	HRESULT loadFile = MFCreateSourceReaderFromURL(filename.c_str(), sourceReaderConfig, &sourceReader);
	if (FAILED(loadFile))
		OutputDebugStringW(L"Unable to create source reader from URL");

	loadFile = sourceReader->SetStreamSelection((DWORD)MF_SOURCE_READER_ALL_STREAMS, false);
	if (FAILED(loadFile))
		OutputDebugStringW(L"Unable to disable possible streams");

	loadFile = sourceReader->SetStreamSelection(streamIndex, true);
	if (FAILED(loadFile))
		OutputDebugStringW(L"Unable to enable first audio stream");

	/* Query information about media file and make sure it's an audio file */
	HRESULT queryMediaInfo = sourceReader->GetNativeMediaType(streamIndex, 0, &nativeMediaType);
	if (FAILED(queryMediaInfo))
		OutputDebugStringW(L"Unalbe to query media information");
	
	GUID majorType{};
	queryMediaInfo = nativeMediaType->GetGUID(MF_MT_MAJOR_TYPE, &majorType);
	if (FAILED(queryMediaInfo))
		OutputDebugStringW(L"The requested file is not an audio file");

	/* Check if th audio file is compressed or uncompressed, do different stuff.... */
	GUID subType{};
	queryMediaInfo = nativeMediaType->GetGUID(MF_MT_MAJOR_TYPE, &subType);
	if (FAILED(queryMediaInfo))
		OutputDebugStringW(L"Failed to check if its compressed och uncompressed");

	if (subType == MFAudioFormat_Float || subType == MFAudioFormat_PCM)
	{
		// Uncompressed, ok stuff
	}
	else
	{
		/* If we are working with a compressed format, such as mp3, for example, we have to decode it first */
		HRESULT decompressingFile = MFCreateMediaType(&partialType);
		if (FAILED(decompressingFile))
			OutputDebugStringW(L"Unalbe to create media type");

		decompressingFile = partialType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		if (FAILED(decompressingFile))
			OutputDebugStringW(L"Unable to set media type when decompressing");

		decompressingFile = partialType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
		if (FAILED(decompressingFile))
			OutputDebugStringW(L"Unable to set media type to uncompressed");

		decompressingFile = sourceReader->SetCurrentMediaType(streamIndex, NULL, partialType);
		if (FAILED(decompressingFile))
			OutputDebugStringW(L"Unable to set current media type to source reader");	
	}

	/* Uncompress the data and load it into an XAudio2 Buffer */
	HRESULT createStreamFile = sourceReader->GetCurrentMediaType(streamIndex, &uncompressedAudioFile);
	if (FAILED(createStreamFile))
		OutputDebugStringW(L"Unable to retrieve the current media type!");

	createStreamFile = MFCreateWaveFormatExFromMFMediaType(uncompressedAudioFile, waveFormatEx, &waveLength);
	if (FAILED(createStreamFile))
		OutputDebugStringW(L"Critical error: Unable to create the wave format!");

	/* Ensure the stream is selected */
	createStreamFile = sourceReader->SetStreamSelection(streamIndex, true);
	if (FAILED(createStreamFile))
		OutputDebugStringW(L"Critical error: Unable to select audio stream!");

	/* Copy data into byte vector */
	IMFSample* sample = nullptr;
	IMFMediaBuffer* buffer = nullptr;
	BYTE* localAudioData = NULL;
	DWORD localAudioDataLength = 0;

	while (true)
	{
		DWORD flags = 0;
		createStreamFile = sourceReader->ReadSample(streamIndex, 0, nullptr, &flags, nullptr, &sample);
		if (FAILED(createStreamFile))
			OutputDebugStringW(L"Critical error: Unable to read audio sample!");

		// Check whether the data is still valid
		if (flags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
			break;

		// Check for end of stream
		if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
			break;

		if (sample == nullptr)
			continue;

		// Convert data to contiguous buffer
		createStreamFile = sample->ConvertToContiguousBuffer(&buffer);
		if (FAILED(createStreamFile))
			OutputDebugStringW(L"Critical error: Unable to convert audio sample to contiguous buffer!");

		// Lock buffer and copy data to local memory
		createStreamFile = buffer->Lock(&localAudioData, nullptr, &localAudioDataLength);
		if (FAILED(createStreamFile))
			OutputDebugStringW(L"Critical error: Unable to lock the audio buffer!");

		for (size_t i = 0; i < localAudioDataLength; i++)
			audioData.push_back(localAudioData[i]);

		// Unlock the buffer
		createStreamFile = buffer->Unlock();
		localAudioData = nullptr;

		if (FAILED(createStreamFile))
			OutputDebugStringW(L"Critical error while unlocking the audio buffer!");
	}
}
