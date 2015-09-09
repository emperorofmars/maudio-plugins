/*
 * Project Maudio
 * Copyright (C) 2015 Martin Schwarz
 * See LICENSE.txt for the full license
 */

#ifndef MAUDIO_PLAYER
#define MAUDIO_PLAYER

#include "AudioDevice.hpp"
#include "maudio/action/BaseAction.hpp"
#include "maudio/audiodata/AudioQueue.hpp"
#include <thread>

namespace maudio{

class Player : public BaseAction{
public:
	Player();
	Player(int device);
	Player(std::string &device);
	virtual ~Player();

	void open();
	void open(int device);
	void open(std::string &device);
	void close();

	std::vector<std::string> listDevices();

	void play();
	void pause();
	void unpause();
	void stop();

	void setPosition(unsigned long samples);
	unsigned long getPosition();
	void setPosition(float seconds);
	float getPosition_sek();

	std::string getStatus();
	bool getOpened();
	bool playing();

	virtual IAudioBuffer *get(unsigned long pos, unsigned int length) noexcept;
	virtual IAudioInfo *getInfo() noexcept;

	virtual int MaxInputs() const;
	virtual bool HasOutputs() const;

	virtual void readConfig(const IKeyValueStore *conf);

	virtual IControl *getControl();

	virtual bool checkCompatible(IAudioInfo *info);

	virtual void serialize(IMultiLevelStore *data) const;
	virtual void deserialize(const IMultiLevelStore *data);

private:
	void feed();
	void startFeed();
	void stopFeed();
	static void asyncFeed(Player *player);

	class Control : public IControl{
	public:
		Control(Player *data);
		virtual ~Control();

		virtual unsigned int getNumFunctions();
		virtual const char *getFunctionName(unsigned int num);
		virtual const char *getFunctionParam(unsigned int num);
		virtual const char *callFunction(unsigned int num, const char *param = NULL);
		virtual const char *callFunction(const char *name, const char *param = NULL);
		virtual void stop();

	private:
		Player *mData;
	};
	std::shared_ptr<Control> mControl = std::make_shared<Control>(this);

	AudioDevice *mDevice = NULL;
	std::string mDeviceName;
	std::shared_ptr<AudioQueue> mQueue;
	std::shared_ptr<std::thread> mThread;
	bool mFeederRun = false;
	unsigned long mPosition = 0;

	unsigned int mQueueSize = 1024 * 8;
};

} // maudio


extern "C" void* create();
extern "C" void destroy(void *data);
extern "C" const char *getName();

#endif // MAUDIO_PLAYER


