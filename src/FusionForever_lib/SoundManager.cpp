#include "StdAfx.h"
#include "SoundManager.h"
#include <SDL_mixer.h>


SoundManager::SoundManager(void)
{
	int audio_rate = 44100;
	Uint16 audio_format = AUDIO_S16; 
	int audio_channels = 2;
	int audio_buffers = 1024;

	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers))
	{
		Logger::ErrorOut() << "Unable to open audio!\n";
		status_ = SoundStatus::ErrorInitialising;
	} else
	{
		Mix_AllocateChannels(32);
		Logger::ErrorOut() << "Sound initialised\n";
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
	sample_queue_[_filename]++;
}

void SoundManager::PlayEnqueuedSample(std::string _filename, unsigned char _distance)
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
			Logger::ErrorOut() << "Unable to load sound:" << _filename << "\n";
	}

	if(sample)
	{
		//Play the sound
		int channel = Mix_PlayChannel(-1, sample, 0);
		
		if(channel < 0)
		{
		//	Logger::ErrorOut() << Mix_GetError() << "\n";
		}
		else
		{
			Mix_SetDistance(channel, _distance);
		//	Logger::DiagnosticOut() << "Playing on" << channel << "\n";
		}
	}
}

void SoundManager::Tick()
{
	std::pair<std::string, int> sample_request;
	
	BOOST_FOREACH(sample_request, sample_queue_)
	{
		if(sample_request.second < 5)
		{
			PlayEnqueuedSample(sample_request.first, 128);
		} else
		{
			PlayEnqueuedSample(sample_request.first, 0);
		}
	}
	sample_queue_.clear();
}

