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

	}
	else
	{
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

	HRESULT createStreamFile = sourceReader->GetCurrentMediaType(streamIndex, &uncompressedAudioFile);
	if (FAILED(createStreamFile))
		OutputDebugStringW(L"");



	
}
