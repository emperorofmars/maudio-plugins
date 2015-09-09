/*
 * Project Maudio
 * Copyright (C) 2015 Martin Schwarz
 * See LICENSE.txt for the full license
 */

#ifndef MAUDIO_AUDIODEVICE
#define MAUDIO_AUDIODEVICE

#include "maudio/audiodata/AudioQueue.hpp"
#include "portaudio.h"
#include <string>
#include <vector>
#include <memory>

namespace maudio{

class AudioDevice{
public:
	static AudioDevice* open();
	static AudioDevice* open(int device);
	static AudioDevice* open(std::string &device);

	static std::vector<std::string> listDevices();

	const int getID();
	const std::string getName();

	void play(std::shared_ptr<AudioQueue> data);
	void pause();
	void unpause();
	void stop();

	int getStatus();

private:
	AudioDevice(const AudioDevice &) = delete;
	void operator=(const AudioDevice &) = delete;

	AudioDevice(int device);
	virtual ~AudioDevice();

	static void initAPI();
	static void closeAPI();

	static void reset();

	static int AudioCallback(const void *input,
							void *output,
							unsigned long frameCount,
							const PaStreamCallbackTimeInfo* timeInfo,
							PaStreamCallbackFlags statusFlags,
							void *userData);

	struct Deleter{
		void operator()(AudioDevice *d){
			delete d;
		}
	};

	int mID;
	std::string mName;

	PaStream *mStream = NULL;
	std::shared_ptr<AudioQueue> mStreamData;
	bool mPlaying;
	bool mPaused;

	static bool mAPIInited;
	static std::vector<std::unique_ptr<AudioDevice, AudioDevice::Deleter>> mDevices;
};

} // maudio

#endif // MAUDIO_AUDIODEVICE


