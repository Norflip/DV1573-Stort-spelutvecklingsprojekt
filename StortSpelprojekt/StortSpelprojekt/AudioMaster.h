#pragma once
#include "AudioEngine.h"

enum AudioTypes { Music, Sound, Environment };

struct SoundEvent
{
	IXAudio2SourceVoice* sourceVoice;	/* XAudio2 source voice */
	WAVEFORMATEX waveFormat;			/* Format of the audio file */
	unsigned int waveLength;			/* Length of the wave */
	std::vector<BYTE> audioData;		/* Audio data */
	XAUDIO2_BUFFER audioBuffer;			/* The actual buffer with the audio data */
		
	bool playing;
	bool looping;

	SoundEvent() {};
	virtual ~SoundEvent() {};
};

class AudioMaster
{
private:
	// The main audio engine 
	AudioEngine* engine;				
	WAVEFORMATEX* waveFormatEx;
	
	// Expand this shit
	XAUDIO2_VOICE_SENDS soundsSendList;
	XAUDIO2_VOICE_SENDS musicSendList;
	XAUDIO2_VOICE_SENDS environmentSendList;
	XAUDIO2_SEND_DESCRIPTOR sendSounds;
	XAUDIO2_SEND_DESCRIPTOR sendMusic;
	XAUDIO2_SEND_DESCRIPTOR sendEnvironment;

	// Submix voices
	IXAudio2SubmixVoice* soundsSubmix;	
	IXAudio2SubmixVoice* musicSubmix;
	IXAudio2SubmixVoice* environmentSubmix;

	float soundEffectsVolume = 1.0f;
	float musicVolume = 1.0f;
	float environmentVolume = 1.0f;

	SoundEvent menuTest, pickupSound, pickupFuel, axeSwingSound, punchSound, walkSound, runSound, windSound, insideWalk;

public:
	AudioMaster();
	virtual ~AudioMaster();

	/* Load sound (not streaming atm) */
	void LoadFile(const std::wstring fileName, std::string name, SoundEvent& soundEvent, const AudioTypes& soundType, bool loop);

	void PlaySoundEvent(std::string soundName);
	void StopSoundEvent(std::string name);

	void SetVolume(const AudioTypes& audioType, const float volume);
	float GetVolume(const AudioTypes& audioType) const;

	void StopAllSoundEffects();
	void PlayerOutside(bool state);
		
	static AudioMaster& Instance()
	{
		static AudioMaster instance;
		return instance;
	}

	AudioMaster(AudioMaster const&) = delete;
	void operator = (AudioMaster const& ) = delete;

	/* Store sounds */
	std::unordered_map<std::string, SoundEvent> soundTracks;
};