/*
 * Project Maudio
 * Copyright (C) 2015 Martin Schwarz
 * See LICENSE.txt for the full license
 */

#ifndef MAUDIO_FILEREADER
#define MAUDIO_FILEREADER

#include "maudio/action/BaseAction.hpp"
#include "maudio/audiodata/AudioBuffer.hpp"
#include "maudio/audiodata/AudioInfo.hpp"
#include "sndfile.h"
#include <memory>
#include <string>
#include <mutex>

namespace maudio{

class FileReader : public BaseAction{
public:
	FileReader();
	virtual ~FileReader();

	virtual IAudioBuffer *get(unsigned long pos, unsigned int length) noexcept;
	virtual IAudioInfo *getInfo() noexcept;

	virtual int MaxInputs() const;
	virtual bool HasOutputs() const;

	virtual void readConfig(const IKeyValueStore *conf);

	virtual IControl *getControl();

	virtual bool checkCompatible(IAudioInfo *info);

	virtual void serialize(IMultiLevelStore *data) const;
	virtual void deserialize(const IMultiLevelStore *data);

	bool setFileName(const char *path);
	const char *getFileName();

private:
	class Control : public IControl{
	public:
		Control(FileReader *data);
		virtual ~Control();

		virtual unsigned int getNumFunctions();
		virtual const char *getFunctionName(unsigned int num);
		virtual const char *getFunctionParam(unsigned int num);
		virtual const char *callFunction(unsigned int num, const char *param = NULL);
		virtual const char *callFunction(const char *name, const char *param = NULL);
		virtual void stop();

	private:
		FileReader *mData;
	};
	std::shared_ptr<Control> mControl = std::make_shared<Control>(this);
	
	std::string mFileName;
	SNDFILE *mFile = NULL;
	AudioInfo mAudioInfo;
	mutable std::recursive_mutex mMutex;
};

} // maudio


extern "C" void* create();
extern "C" void destroy(void *data);
extern "C" const char *getName();

#endif // MAUDIO_FILEREADER


