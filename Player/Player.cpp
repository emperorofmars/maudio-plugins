/*
 * Project Maudio
 * Copyright (C) 2015 Martin Schwarz
 * See LICENSE.txt for the full license
 */

#include "maudio.hpp"
#include "Player.hpp"
#include "maudio/util/sptr.hpp"
#include "maudio/util/Util.hpp"
#include "maudio/util/AudioException.hpp"

namespace maudio{

Player::Player(){
	open();
}

Player::Player(int device){
	open(device);
}

Player::Player(std::string &device){
	open(device);
}

Player::~Player(){
    close();
}

void Player::open(){
	mDevice = AudioDevice::open();
	if(mDevice){
		mDeviceName = mDevice->getName();
	}
	return;
}

void Player::open(int device){
	mDevice = AudioDevice::open(device);
	if(mDevice){
		mDeviceName = mDevice->getName();
	}
	return;
}

void Player::open(std::string &device){
	mDevice = AudioDevice::open(device);
	if(mDevice){
		mDeviceName = mDevice->getName();
	}
	return;
}

void Player::close(){
    if(mDevice != NULL){
        stop();
        mDevice = NULL;
    }
	return;
}

std::vector<std::string> Player::listDevices(){
	std::vector<std::string> ret;
	if(mDevice) ret = mDevice->listDevices();
	return ret;
}

void Player::play(){
	if(!mDevice) return;
	if(!InputOk(0)) return;

	auto info = getInfoFromSlot(0);
	if(!info) return;
	mQueue.reset(new AudioQueue(*info));
	feed();
	startFeed();
	mDevice->play(mQueue);
	return;
}

void Player::pause(){
	if(!mDevice) return;
	stopFeed();
	mDevice->pause();
	return;
}

void Player::unpause(){
	if(!mDevice) return;
	startFeed();
	mDevice->unpause();
	return;
}

void Player::stop(){
	if(!mDevice) return;
	stopFeed();
	mDevice->stop();
	return;
}

void Player::setPosition(unsigned long samples){
	mPosition = samples;
	return;
}

unsigned long Player::getPosition(){
	return mPosition;
}

void Player::setPosition(float seconds){
	if(InputOk(0) > 0){
		sptr<IAudioInfo> info(getInfo());
		mPosition =  seconds * (float)info->getSamplerate();
	}
	return;
}

float Player::getPosition_sek(){
	if(InputOk(0) > 0){
		sptr<IAudioInfo> info(getInfo());
		return (float)mPosition / (float)info->getSamplerate();
	}
	return 0;
}

std::string Player::getStatus(){
	std::string status = "Player not opened";
	if(mDevice && mDevice->getStatus() == -1) status = "Device error";
	if(mDevice && mDevice->getStatus() == 0) status = "Device opened";
	if(mDevice && mDevice->getStatus() == 1) status = "Device playing";
	if(mDevice && mDevice->getStatus() == 2) status = "Device paused";
	return status;
}

bool Player::getOpened(){
	if(mDevice && mDevice->getStatus() >= 0) return true;
	return false;
}

bool Player::playing(){
	if(mDevice && mDevice->getStatus() >= 1) return true;
	return false;
}

IAudioBuffer *Player::get(unsigned long pos, unsigned int length) noexcept{
	return NULL;
}

IAudioInfo *Player::getInfo() noexcept{
	return NULL;
}

int Player::MaxInputs() const{
	return 1;
}

bool Player::HasOutputs() const{
	return false;
}

void Player::readConfig(const IKeyValueStore *conf){
	try{
	//unsigned int tmpQueueSize = conf.get<unsigned int>("PlayerQueueSize");
	//if(tmpQueueSize >= 1024 && tmpQueueSize <= 1024 * 16)
	//	mQueueSize = tmpQueueSize;
	}
	catch(std::exception &e){
	}
    return;
}

IControl *Player::getControl(){
	return mControl.get();
}

bool Player::checkCompatible(IAudioInfo *info){
	if(info) return true;
	return false;
}

void Player::serialize(IMultiLevelStore *data) const{
	if(!data) return;
	data->add("name", getName());
	data->add("queuesize", std::to_string(mQueueSize).c_str());
	data->add("position", std::to_string(mPosition).c_str());
	data->add("devicename", mDeviceName.c_str());
	return;
}

void Player::deserialize(const IMultiLevelStore *data){
	if(!data) return;
	try{
		setName(data->get("name"));
		mQueueSize = string_to<unsigned int>(std::string(data->get("queuesize")));
		mPosition = string_to<unsigned long>(std::string(data->get("position")));
		mDeviceName = data->get("devicename");
	}
	catch(std::exception &e){
	}
	return;
}

void Player::feed(){
	if(!mQueue) return;
	if(!InputOk(0)) return;
	if(mQueueSize <= mQueue->size()) return;

	for(unsigned int i = 0; i < mQueueSize - mQueue->size(); i++){
		auto tmp = getFromSlot(0, mPosition, mQueueSize - mQueue->size());
		if(!tmp) break;
		for(unsigned int j = 0; j < tmp->getInfo()->getSamples(); j++){
			ISample *tmpSample = tmp->get(j);
			mQueue->push(*tmpSample);
			delete tmpSample;
		}
		mPosition += tmp->getInfo()->getSamples();
	}

	return;
}

void Player::startFeed(){
	mFeederRun = true;
	mThread.reset(new std::thread(asyncFeed, this));
	return;
}

void Player::stopFeed(){
    if(mFeederRun){
        mFeederRun = false;
        if(mThread->joinable()) mThread->join();
        else mThread.reset();
    }
	return;
}

void Player::asyncFeed(Player *player){
	while(player && player->mFeederRun){
		player->feed();
		std::this_thread::sleep_for(std::chrono::milliseconds((player->mQueueSize / 2 * 44100) / 4000000));
	}
	return;
}


Player::Control::Control(Player *data){
	mData = data;
}

Player::Control::~Control(){
}

unsigned int Player::Control::getNumFunctions(){
	return 2;
}

const char *Player::Control::getFunctionName(unsigned int num){
	if(num == 0) return "play";
	if(num == 1) return "stop";
	return NULL;
}

const char *Player::Control::getFunctionParam(unsigned int num){
	if(num == 0) return "void";
	if(num == 1) return "void";
	return NULL;
}

const char *Player::Control::callFunction(unsigned int num, const char *param){
	if(num == 0){
		mData->play();
		return NULL;
	}
	if(num == 1){
		mData->stop();
		return NULL;
	}
	return NULL;
}

const char *Player::Control::callFunction(const char *name, const char *param){
	if(std::string("play") == std::string(name)){
		mData->play();
		return NULL;
	}
	if(std::string("stop") == std::string(name)){
		mData->stop();
		return NULL;
	}
	return NULL;
}

void Player::Control::stop(){
	mData->stop();
	return;
}

} // maudio


extern "C" void* create(){
	return new maudio::Player();
}

extern "C" void destroy(void *data){
	delete (maudio::Player *)data;
}

extern "C" const char *getName(){
	static const char *ret = "Player";
	return ret;
}






