#include "stdafx.h"
#include "AudioMaster.h"

AudioMaster::AudioMaster()
{
	/* Init Audioengine */
	if (!engine)
		engine = new AudioEngine;
	engine->Initialize();

	/* Create different soundchannels */
	engine->GetAudioMaster()->CreateSubmixVoice(&soundsSubmix, 1, 44100, 0, 0, 0, 0);
	engine->GetAudioMaster()->CreateSubmixVoice(&musicSubmix, 1, 44100, 0, 0, 0, 0);

	sendSounds = { 0, soundsSubmix };
	soundsSendList = { 1, &sendSounds };
	sendMusic = { 0, musicSubmix };
	musicSendList = { 1, &sendMusic };

	/* Volume for both channels */
	soundsSubmix->SetVolume(soundEffectsVolume);
	musicSubmix->SetVolume(musicVolume);
}

AudioMaster::~AudioMaster()
{
	if (engine)
		delete engine;
}

void AudioMaster::LoadFile(const std::wstring fileName, SoundEvent& soundEvent, const AudioTypes& soundType)
{
	/* Load soundfile with engine into a wave and make it playable */
	engine->LoadFile(fileName, soundEvent.audioData, &waveFormatEx, soundEvent.waveLength);
	soundEvent.waveFormat = *waveFormatEx;

	/* Create source voice */
	HRESULT loadFile;
	if (soundType == AudioTypes::Sound)
		loadFile = engine->GetAudioMaster()->CreateSourceVoice(&soundEvent.sourceVoice, &soundEvent.waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr, &soundsSendList, NULL);
	else if (soundType == AudioTypes::Music)
		loadFile = engine->GetAudioMaster()->CreateSourceVoice(&soundEvent.sourceVoice, &soundEvent.waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr, &musicSendList, NULL);
	if (FAILED(loadFile))
		OutputDebugStringW(L"Critical error: Unable to create source voice!");

	ZeroMemory(&soundEvent.audioBuffer, sizeof(XAUDIO2_BUFFER));
	soundEvent.audioBuffer.AudioBytes = (UINT32)soundEvent.audioData.size();
	soundEvent.audioBuffer.pAudioData = (BYTE* const)&soundEvent.audioData[0];
	soundEvent.audioBuffer.pContext = nullptr;
	soundEvent.audioBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;
}

void AudioMaster::PlaySoundEvent(const SoundEvent& soundEvent)
{
	/* Submit the audio buffer from soundevent to the source voice and start it */
	HRESULT playSound;
	playSound = soundEvent.sourceVoice->SubmitSourceBuffer(&soundEvent.audioBuffer);
	if (FAILED(playSound))
		OutputDebugStringW(L"Critical error: Unable to submit source buffer!");
	
	soundEvent.sourceVoice->Start();
}

void AudioMaster::StopSoundEvent(const SoundEvent& soundEvent)
{
	soundEvent.sourceVoice->Stop();
}

void AudioMaster::SetVolume(const AudioTypes& audioType, const float volume)
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

float AudioMaster::GetVolume(const AudioTypes& audioType) const
{
	if (audioType == AudioTypes::Music)
		return musicVolume;
	else if (audioType == AudioTypes::Sound)
		return soundEffectsVolume;
	return 1.0f;
}
