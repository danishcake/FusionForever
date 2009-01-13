#include "StdAfx.h"
#include "SoundManager.h"
#include <SDL_mixer.h>


SoundManager::SoundManager(void)
{
	int audio_rate = 22050;
	Uint16 audio_format = AUDIO_S16; 
	int audio_channels = 2;
	int audio_buffers = 4096;

	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers))
	{
		Logger::LogError("Unable to open audio!\n");
		status_ = SoundStatus::ErrorInitialising;
	} else
	{
		Mix_AllocateChannels(32);
		Logger::Log("Sound initialised");
		status_ = SoundStatus::OK;
	}

}

SoundManager::~SoundManager(void)
{
}

SoundManager& SoundManager::Instance()
{
	static SoundManager* instance = NULL;
	if(!instance)
		instance = new SoundManager();
	return *instance;
}

void SoundManager::PlaySample(std::string _filename)
{
	//Look the sound up, it's probably already loaded. If not then load it.
	if(status_ != SoundStatus::OK)
		return;

	Mix_Chunk* sample = NULL;
	if(samples_.find(_filename) != samples_.end())
	{
		sample = samples_[_filename];
	} else
	{
		sample = Mix_LoadWAV((std::string("Sounds/") + _filename).c_str());
		
		if(sample != NULL)
			samples_[_filename] = sample;
		else
			Logger::LogError("Unable to load sound:" + _filename);
	}

	if(sample)
	{
		//Play the sound
		int channel = Mix_PlayChannel(-1, sample, 0);
		if(channel < 0)
			Logger::Instance() << Mix_GetError() << "\n";
		else
			Logger::Instance() << "Playing on" << channel << "\n";
	}
	
}