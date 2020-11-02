#pragma once
#include "AudioEngine.h"

enum AudioTypes { Music, Sound };

struct SoundEvent
{
	IXAudio2SourceVoice* sourceVoice;	/* XAudio2 source voice */
	WAVEFORMATEX waveFormat;			/* Format of the audio file */
	unsigned int waveLength;			/* Length of the wave */
	std::vector<BYTE> audioData;		/* Audio data */
	XAUDIO2_BUFFER audioBuffer;			/* The actual buffer with the audio data */

	float fallof;						/* Falloff distance */
	unsigned int priority;				/* Music priority */
	unsigned int index;					/* the index of the actual sound to play */

	SoundEvent() {};
	virtual ~SoundEvent() {};
};

class AudioMaster
{
private:
	// The main audio engine ( XAudio2 with Windows Media Component ) 
	AudioEngine* engine;				
	WAVEFORMATEX* waveFormatEx;
	
	// Expand this shit
	XAUDIO2_VOICE_SENDS soundsSendList;
	XAUDIO2_VOICE_SENDS musicSendList;
	XAUDIO2_SEND_DESCRIPTOR sendSounds;
	XAUDIO2_SEND_DESCRIPTOR sendMusic;

	// Submix voices
	IXAudio2SubmixVoice* soundsSubmix;	
	IXAudio2SubmixVoice* musicSubmix;

	float soundEffectsVolume = 1.0f;
	float musicVolume = 1.0f;

public:
	AudioMaster();
	virtual ~AudioMaster();

	/* Load sound (not streaming atm) */
	void LoadFile(const std::wstring fileName, std::string name, SoundEvent& soundEvent, const AudioTypes& soundType, bool loop);

	void PlaySoundEvent(std::string soundName);
	void StopSoundEvent(std::string name);

	void SetVolume(const AudioTypes& audioType, const float volume);
	float GetVolume(const AudioTypes& audioType) const;

	static AudioMaster& Instance()
	{
		static AudioMaster instance;
		return instance;
	}

	AudioMaster(AudioMaster const&) = delete;
	void operator = (AudioMaster const& ) = delete;

	/* Store shiet */
	std::unordered_map<std::string, SoundEvent> soundTracks;
};