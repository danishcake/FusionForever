#pragma once
#include <string>

struct Mix_Chunk;

namespace SoundStatus
{
	enum Enum
	{
		ErrorInitialising, OK
	};
}

class SoundManager
{
private:
	SoundManager(void);
	~SoundManager(void);
	SoundStatus::Enum status_;

	std::map<std::string, Mix_Chunk*> samples_;
	std::map<std::string, int> sample_queue_;

	void PlayEnqueuedSample(std::string _filename, unsigned char _distance);

public:
	static SoundManager& Instance();
	void PlaySample(std::string _filename);
	void Tick();

};
