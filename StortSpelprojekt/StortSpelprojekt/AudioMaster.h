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
	~SoundEvent() {};
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
	~AudioMaster();

	/* Load sound (not streaming atm) */
	void LoadFile(const std::wstring fileName, SoundEvent& soundEvent, const AudioTypes& soundType);

	void PlaySoundEvent(const SoundEvent& soundEvent);
	void StopSoundEvent(const SoundEvent& soundEvent);

	void SetVolume(const AudioTypes& audioType, const float volume);
	float GetVolume(const AudioTypes& audioType) const;
};