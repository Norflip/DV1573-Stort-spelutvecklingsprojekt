#include "stdafx.h"
#include "AudioMaster.h"

AudioMaster::AudioMaster()
{
	/* Init Audioengine */
	if (!engine)
		engine = new AudioEngine;
	engine->Initialize();

	//engine->Initialize3DAudio();

	/* Create different soundchannels */
	engine->GetAudioMaster()->CreateSubmixVoice(&soundsSubmix, 1, 44100, 0, 0, 0, 0);
	engine->GetAudioMaster()->CreateSubmixVoice(&musicSubmix, 1, 44100, 0, 0, 0, 0);
	engine->GetAudioMaster()->CreateSubmixVoice(&environmentSubmix, 1, 44100, 0, 0, 0, 0);

	sendSounds = { 0, soundsSubmix };
	soundsSendList = { 1, &sendSounds };
	sendMusic = { 0, musicSubmix };
	musicSendList = { 1, &sendMusic };
	sendEnvironment = { 0, environmentSubmix };
	environmentSendList = { 1, &sendEnvironment };

	/* Volume for both channels */
	soundsSubmix->SetVolume(soundEffectsVolume);
	musicSubmix->SetVolume(musicVolume);
	environmentSubmix->SetVolume(environmentVolume);

	/* Load tracks */
	LoadFile(L"Sounds/jakestuff.mp3", "menusound", menuTest, AudioTypes::Music, true);
	LoadFile(L"Sounds/pickup3.wav", "pickupSound", pickupSound, AudioTypes::Sound, false);
	LoadFile(L"Sounds/fuelPickup.wav", "pickupFuel", pickupFuel, AudioTypes::Sound, false);
	LoadFile(L"Sounds/swinging_axe.mp3", "axeSwing", axeSwingSound, AudioTypes::Sound, false);
	LoadFile(L"Sounds/Punch.wav", "punch", punchSound, AudioTypes::Sound, false);
	LoadFile(L"Sounds/walking.wav", "walk", walkSound, AudioTypes::Sound, true);
	LoadFile(L"Sounds/running.mp3", "run", runSound, AudioTypes::Sound, true);
	LoadFile(L"Sounds/windyLoop.mp3", "wind", windSound, AudioTypes::Environment, true);
	//LoadFile(L"Sounds/choptree.mp3", "choptree", chop, AudioTypes::Sound, false);
	LoadFile(L"Sounds/insideWalk.mp3", "insideWalk", insideWalk, AudioTypes::Sound, true);
	LoadFile(L"Sounds/DarkAmbient.mp3", "ambient", ambientSound, AudioTypes::Music, true);

	SetVolume(AudioTypes::Music, 0.7f);
	SetVolume(AudioTypes::Sound, 0.7f);
	SetVolume(AudioTypes::Environment, 0.7f);
}

AudioMaster::~AudioMaster()
{
	if (engine)
		delete engine;

	soundTracks.clear();
}

void AudioMaster::LoadFile(const std::wstring fileName, std::string name, SoundEvent& soundEvent, const AudioTypes& soundType, bool loop)
{
	//ZeroMemory(&soundEvent, sizeof(SoundEvent));

	/* Load soundfile with engine into a wave and make it playable */	
	engine->LoadFile(fileName, soundEvent.audioData, &waveFormatEx, soundEvent.waveLength);
	soundEvent.waveFormat = *waveFormatEx;


	/* Create source voice */
	HRESULT loadFile;
	if (soundType == AudioTypes::Sound)
		loadFile = engine->GetAudioMaster()->CreateSourceVoice(&soundEvent.sourceVoice, &soundEvent.waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr, &soundsSendList, NULL);
	else if (soundType == AudioTypes::Music)
		loadFile = engine->GetAudioMaster()->CreateSourceVoice(&soundEvent.sourceVoice, &soundEvent.waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr, &musicSendList, NULL);
	else if (soundType == AudioTypes::Environment)
		loadFile = engine->GetAudioMaster()->CreateSourceVoice(&soundEvent.sourceVoice, &soundEvent.waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr, &environmentSendList, NULL);
	if (FAILED(loadFile))
		OutputDebugStringW(L"Critical error: Unable to create source voice!");

	ZeroMemory(&soundEvent.audioBuffer, sizeof(XAUDIO2_BUFFER));
	soundEvent.audioBuffer.AudioBytes = (UINT32)soundEvent.audioData.size();
	soundEvent.audioBuffer.pAudioData = (BYTE* const)&soundEvent.audioData[0];
	soundEvent.audioBuffer.pContext = nullptr;

	if (loop)
	{
		soundEvent.audioBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;
		soundEvent.looping = true;
	}
	else
	{
		soundEvent.audioBuffer.LoopCount = XAUDIO2_NO_LOOP_REGION;
		soundEvent.looping = false;
	}

	soundEvent.playing = false;
	
	soundTracks.insert({ name, soundEvent });
}

void AudioMaster::PlaySoundEvent(std::string soundName)
{
	SoundEvent soundEvent;
	for (auto& i : soundTracks)
	{
		if (i.first == soundName)
		{
			//i.second.playing = true;
			soundEvent = i.second;
		}
	}

	/* Submit the audio buffer from soundevent to the source voice and start it */
	if (soundEvent.looping)
	{	
		if (!soundEvent.playing)
		{
			HRESULT playSound;
			playSound = soundEvent.sourceVoice->SubmitSourceBuffer(&soundEvent.audioBuffer);
			if (FAILED(playSound))
				OutputDebugStringW(L"Critical error: Unable to submit source buffer!");

			soundEvent.sourceVoice->Start();

			for (auto& i : soundTracks)
			{
				if (i.first == soundName)
				{
					i.second.playing = true;
				}
			}
		}
	}
	else
	{
		HRESULT playSound;
		playSound = soundEvent.sourceVoice->SubmitSourceBuffer(&soundEvent.audioBuffer);
		if (FAILED(playSound))
			OutputDebugStringW(L"Critical error: Unable to submit source buffer!");

		soundEvent.sourceVoice->Start();

		/*for (auto& i : soundTracks)
		{
			if (i.first == soundName)
			{
				i.second.playing = true;
			}
		}*/
	}
}

void AudioMaster::StopSoundEvent(std::string name)
{
	SoundEvent soundEvent;
	for (auto& i : soundTracks)
	{
		if (i.first == name)
			soundEvent = i.second;
	}

	if (soundEvent.looping)
	{
		if (soundEvent.playing)
		{
			soundEvent.sourceVoice->Stop();
			for (auto& i : soundTracks)
			{
				if (i.first == name)
				{
					i.second.playing = false;
				}
			}
		}
	}
	else
	{
		soundEvent.sourceVoice->Stop();
	}
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

void AudioMaster::StopAllSoundEffects()
{
	SoundEvent soundEvent;
	for (auto& i : soundTracks)
	{		
		soundEvent = i.second;
		soundEvent.playing = false;
		soundEvent.sourceVoice->Stop();
	}	
}

void AudioMaster::PlayerOutside(bool state)
{
	if(state)
		environmentSubmix->SetVolume(soundEffectsVolume);
	else
		environmentSubmix->SetVolume(0.2f);
}

