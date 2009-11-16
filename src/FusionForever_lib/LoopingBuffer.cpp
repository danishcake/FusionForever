#include "StdAfx.h"
#include "LoopingBuffer.h"

int LoopingBuffer::running_count_ = 0;

LoopingBuffer::LoopingBuffer(std::string _filename) :
filename_(_filename), channel_(-1), volume_(1.0f)
{
}

LoopingBuffer::LoopingBuffer() :
filename_(""), channel_(-1)
{
}

LoopingBuffer::~LoopingBuffer(void)
{
	Stop();
}

bool LoopingBuffer::GetPlaying()
{
	return channel_ >= 0;
}

void LoopingBuffer::Start()
{
	if(filename_.length() == 0)
		return;
	if(!GetPlaying())
	{
		channel_ = SoundManager::Instance().PlayLoopingSample(filename_);
		SoundManager::Instance().SetVolume(channel_, volume_);
		running_count_++;
		Logger::DiagnosticOut() << "Active loops++=" << running_count_ << "\n";
	}
}

void LoopingBuffer::Stop()
{
	if(GetPlaying())
	{
		SoundManager::Instance().StopChannel(channel_);
		channel_ = -1;
		running_count_--;
		Logger::DiagnosticOut() << "Active loops--= " << running_count_ << "\n";
	}
}

void LoopingBuffer::SetVolume(float _level)
{
	volume_ = _level < 0 ? 0 : _level > 1 ? 1 : _level;
	if(GetPlaying())
	{
		SoundManager::Instance().SetVolume(channel_, volume_);
	}
}
