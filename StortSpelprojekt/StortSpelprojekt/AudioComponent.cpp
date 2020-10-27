#include "AudioComponent.h"

AudioComponent::AudioComponent()
{
	if (!engine)
		engine = new AudioEngine;
	engine->Initialize();

	sendSounds = { 0, soundsSubmix };
	soundsSendList = { 1, &sendSounds };
	sendMusic = { 0, musicSubmix };
	musicSendList = { 1, &sendMusic };

	// Volume
	soundsSubmix->SetVolume(soundEffectsVolume);
	musicSubmix->SetVolume(musicVolume);
}

AudioComponent::~AudioComponent()
{
}

void AudioComponent::LoadFile(const std::wstring fileName, SoundEvent& soundEvent, const AudioTypes& soundType)
{
	// load file into wave
	WAVEFORMATEX* waveFormatEx;
	engine->LoadFile(fileName, soundEvent.audioData, &waveFormatEx, soundEvent.waveLength);

	soundEvent.waveFormat = *waveFormatEx;

	// create source voice
	if (soundType == AudioTypes::Sound)
		hr = engine->GetAudio()->CreateSourceVoice(&soundEvent.sourceVoice, &soundEvent.waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr, &soundsSendList, NULL);
	else if (soundType == AudioTypes::Music)
		hr = engine->GetAudio()->CreateSourceVoice(&soundEvent.sourceVoice, &soundEvent.waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr, &musicSendList, NULL);
	if (FAILED(hr))
		OutputDebugStringW(L"Critical error: Unable to create source voice!");

	ZeroMemory(&soundEvent.audioBuffer, sizeof(XAUDIO2_BUFFER));
	soundEvent.audioBuffer.AudioBytes = (UINT32)soundEvent.audioData.size();
	soundEvent.audioBuffer.pAudioData = (BYTE* const)&soundEvent.audioData[0];
	soundEvent.audioBuffer.pContext = nullptr;
}

void AudioComponent::PlaySoundEvent(const SoundEvent& soundEvent)
{
	// submit the audio buffer to the source voice
	hr = soundEvent.sourceVoice->SubmitSourceBuffer(&soundEvent.audioBuffer);
	if (FAILED(hr))
		OutputDebugStringW(L"Critical error: Unable to submit source buffer!");

	// start the source voice
	soundEvent.sourceVoice->Start();
}

void AudioComponent::StopSoundEvent(const SoundEvent& soundEvent)
{
	soundEvent.sourceVoice->Stop();
}

void AudioComponent::SetVolume(const AudioTypes& audioType, const float volume)
{
	if (audioType == AudioTypes::Music)
	{
		musicVolume = volume;
		musicSubmix->SetVolume(volume);
	}
	else if (audioType == AudioTypes::Sound)
	{
		soundEffectsVolume = volume;
		soundsSubmix->SetVolume(volume);
	}
}

float AudioComponent::GetVolume(const AudioTypes& audioType) const
{
	if (audioType == AudioTypes::Music)
		return musicVolume;
	else if (audioType == AudioTypes::Sound)
		return soundEffectsVolume;
	return 1.0f;
}
