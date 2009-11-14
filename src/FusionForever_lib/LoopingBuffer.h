#pragma once

/**
  * Represents a looping sound. Once playing it will loop indefinately until stopped
  * Good for 'Chaingun' style noises
  */
class LoopingBuffer
{
private:
	std::string filename_;
	int channel_;
	float volume_;
public:
	LoopingBuffer();
	explicit LoopingBuffer(std::string _filename);
	//TODO explicit LoopingBuffer(std::string _rampup, std::string _looping, std::string _rampdown);
	~LoopingBuffer(void);

	bool GetPlaying();
	void Start();
	void Stop();
	void SetVolume(float _level);
};
