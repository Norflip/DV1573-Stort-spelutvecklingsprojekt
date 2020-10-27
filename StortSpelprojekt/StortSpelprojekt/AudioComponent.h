#pragma once
#include "AudioEngine.h"

enum AudioTypes { Music, Sound };

struct SoundEvent
{
	IXAudio2SourceVoice* sourceVoice;	// the XAudio2 source voice
	WAVEFORMATEX waveFormat;			// the format of the audio file
	unsigned int waveLength;			// the length of the wave
	std::vector<BYTE> audioData;		// the audio data
	XAUDIO2_BUFFER audioBuffer;			// the actual buffer with the audio data

	float fallof;						// falloff distance
	unsigned int priority;				// music priority

	unsigned int index;					// the index of the actual sound to play

	SoundEvent() {};
	~SoundEvent() {};

	//friend class AudioComponent;
};

class AudioComponent
{
private:
	// the main audio engine
	AudioEngine* engine;				// the main audio engine: XAudio2 with Windows Media Component
	HRESULT hr;

	// Expand this shit
	XAUDIO2_VOICE_SENDS soundsSendList;
	XAUDIO2_VOICE_SENDS musicSendList;

	XAUDIO2_SEND_DESCRIPTOR sendSounds;
	XAUDIO2_SEND_DESCRIPTOR sendMusic;

	// submix voices
	IXAudio2SubmixVoice* soundsSubmix;	// collection of all sound effects
	IXAudio2SubmixVoice* musicSubmix;	// collection of all music files

	float soundEffectsVolume = 1.0f;
	float musicVolume = 1.0f;

public:
	AudioComponent();
	~AudioComponent();

	// Load files from disk
	void LoadFile(const std::wstring fileName, SoundEvent& soundEvent, const AudioTypes& soundType);

	// Play sound
	void PlaySoundEvent(const SoundEvent& soundEvent);

	// Stop Sound
	void StopSoundEvent(const SoundEvent& soundEvent);

	void SetVolume(const AudioTypes& audioType, const float volume);
	float GetVolume(const AudioTypes& audioType) const;
};