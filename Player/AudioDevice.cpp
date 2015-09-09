/*
 * Project Maudio
 * Copyright (C) 2015 Martin Schwarz
 * See LICENSE.txt for the full license
 */

#include "AudioDevice.hpp"
#include "maudio/util/AudioException.hpp"
#include <cmath>

namespace maudio{

std::vector<std::unique_ptr<AudioDevice, AudioDevice::Deleter>> AudioDevice::mDevices;
bool AudioDevice::mAPIInited = false;

AudioDevice::AudioDevice(int device){
	mStream = NULL;
	mID = device;
	mPlaying = false;
	mPaused = false;
	auto devlist = listDevices();
	if((int)devlist.size() > mID) mName = listDevices()[mID];
}

AudioDevice::~AudioDevice(){
	stop();
}

AudioDevice* AudioDevice::open(){
	if(!mAPIInited){
		initAPI();
		mAPIInited = true;
	}
	return open(Pa_GetDefaultOutputDevice());
}

AudioDevice* AudioDevice::open(int device){
	if(!mAPIInited){
		initAPI();
		mAPIInited = true;
	}
	if(device > Pa_GetDeviceCount()) throw InvalidAudioDeviceException();
	if(device < 0) device = Pa_GetDefaultOutputDevice();

	for(unsigned int i = 0; i < mDevices.size(); i++){
		if(!mDevices[i]) mDevices.erase(mDevices.begin() + i);
		if(mDevices[i]->getID() == device) return mDevices[i].get();
	}
	mDevices.push_back(std::unique_ptr<AudioDevice, AudioDevice::Deleter>(new AudioDevice(device)));
	return mDevices[mDevices.size() - 1].get();
}

AudioDevice* AudioDevice::open(std::string &device){
	if(!mAPIInited){
		initAPI();
		mAPIInited = true;
	}
	std::vector<std::string> devlist = listDevices();
	for(unsigned int i = 0; i < devlist.size(); i++){
		if(mDevices[i]->getName() == devlist[i]) return open(i);
	}
	throw InvalidAudioDeviceException();
}

std::vector<std::string> AudioDevice::listDevices(){
	std::vector<std::string> ret;
	int num = Pa_GetDeviceCount();
	if(num < 0) throw InternalException();

	const PaDeviceInfo *info = NULL;
	for(unsigned int i = 0; i < (unsigned int)num; i++){
		info = Pa_GetDeviceInfo(i);
		ret.push_back(info->name);
	}
	return ret;
}

const int AudioDevice::getID(){
	return mID;
}

const std::string AudioDevice::getName(){
	return mName;
}

void AudioDevice::play(std::shared_ptr<AudioQueue> data){
	if(!mAPIInited) throw PlayerException();
	if(!data) throw InvalidDataException();
	if(mStream) stop();
	PaError err;

	mStreamData = data;

	PaStreamParameters params;
	params.channelCount = data->getChannels();
	params.device = mID;
	params.hostApiSpecificStreamInfo = NULL;
	params.sampleFormat = paFloat32;
	params.suggestedLatency = 0;

	err = Pa_OpenStream(&mStream, NULL, &params, data->getAudioInfo().getSamplerate(), 512, paNoFlag, &AudioCallback, data.get());
	if(err != paNoError){
		if(mStream){
			try{stop();}
			catch(...){}
		}
		throw MaudioException(Pa_GetErrorText(err));
	}
	if((err = Pa_StartStream(mStream)) != paNoError){
		if(mStream){
			try{stop();}
			catch(...){}
		}
		throw MaudioException(Pa_GetErrorText(err));
	}
	mPlaying = true;
	return;
}

void AudioDevice::pause(){
	if(!mAPIInited) return;
	if(!mStream) return;
	if(mPaused) return;

	Pa_StopStream(mStream);
	mPaused = true;
	return;
}

void AudioDevice::unpause(){
	if(!mAPIInited) return;
	if(!mStream) return;
	if(!mPaused) return;

	Pa_StopStream(mStream);
	mPaused = false;
	return;
}

void AudioDevice::stop(){
	if(!mAPIInited) return;
	if(!mPaused){
		Pa_StopStream(mStream);
		mPaused = false;
	}
	Pa_CloseStream(mStream);

	mStream = NULL;
	mPlaying = false;
	return;
}

int AudioDevice::getStatus(){
	if(!mAPIInited) return -1;
	if(!mPlaying) return 0;
	if(mPlaying) return 1;
	if(!mPaused) return 2;
	return -1;
}

void AudioDevice::initAPI(){
	if(mAPIInited) return;
	PaError err;
	if((err = Pa_Initialize()) != paNoError) return;
	mAPIInited = true;
	return;
}

void AudioDevice::closeAPI(){
	if(!mAPIInited) return;
	for(unsigned int i = 0; i < mDevices.size(); i++){
		if(mDevices[i]) mDevices[i]->stop();
	}
	mAPIInited = false;
	PaError err;
	if((err = Pa_Terminate()) != paNoError) return;
	return;
}

void AudioDevice::reset(){
	return;
}

int AudioDevice::AudioCallback(const void *input,
							void *output,
							unsigned long frameCount,
							const PaStreamCallbackTimeInfo* timeInfo,
							PaStreamCallbackFlags statusFlags,
							void *userData)
{
	if(!userData) return paComplete;
	AudioQueue *data = (AudioQueue*)userData;
	float *out = (float *)output;

	Sample tmp(data->getChannels());
	for(unsigned int i = 0; i < frameCount; i++){
		tmp = data->pop();
		for(unsigned int j = 0; j < data->getChannels(); j++){
			out[i * data->getChannels() + j] = tmp[j];
			//std::cerr << out[i * data->getChannels() + j] << std::endl;
		}
	}

	return paContinue;
}

} // maudio








