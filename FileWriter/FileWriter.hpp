/*
 * Project Maudio
 * Copyright (C) 2015 Martin Schwarz
 * See LICENSE.txt for the full license
 */

#ifndef MAUDIO_PLAYER
#define MAUDIO_PLAYER

#include "maudio/action/BaseAction.hpp"
#include "maudio/audiodata/AudioQueue.hpp"
#include "sndfile.h"
#include <memory>
#include <thread>
#include <string>

namespace maudio{

class FileWriter : public BaseAction{
public:
	FileWriter();
	virtual ~FileWriter();

	void write(unsigned long pos, unsigned long length = 0);
	void stopWriting();
	bool setFileName(const char *path);
	const char *getFileName();

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
	static void asyncWrite(FileWriter *writer, unsigned long pos, unsigned long length);

	class Control : public IControl{
	public:
		Control(FileWriter *data);
		virtual ~Control();

		virtual unsigned int getNumFunctions();
		virtual const char *getFunctionName(unsigned int num);
		virtual const char *getFunctionParam(unsigned int num);
		virtual const char *callFunction(unsigned int num, const char *param = NULL);
		virtual const char *callFunction(const char *name, const char *param = NULL);
		virtual void stop();

	private:
		FileWriter *mData;
	};
	std::shared_ptr<Control> mControl = std::make_shared<Control>(this);
	
	std::string mFile;
	std::shared_ptr<std::thread> mThread;
	bool mThreadWorking = false;
	int mSuccess = 0;
};

} // maudio


extern "C" void* create();
extern "C" void destroy(void *data);
extern "C" const char *getName();

#endif // MAUDIO_PLAYER


