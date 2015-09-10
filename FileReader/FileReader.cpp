/*
 * Project Maudio
 * Copyright (C) 2015 Martin Schwarz
 * See LICENSE.txt for the full license
 */

#include "FileReader.hpp"
#include "maudio/util/sptr.hpp"
#include "maudio/util/Util.hpp"
#include "maudio/util/AudioException.hpp"

namespace maudio{

FileReader::FileReader(){
}

FileReader::~FileReader(){
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	if(mFile) sf_close(mFile);
	mFile = NULL;
}

IAudioBuffer *FileReader::get(unsigned long pos, unsigned int length) noexcept{
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	AudioInfo bufInfo(mAudioInfo);
	bufInfo.setOffset(pos);
	bufInfo.setSamples(length);
	AudioBuffer *buf = new AudioBuffer(bufInfo);
	
	sf_seek(mFile, pos, SEEK_SET);
	
	long long len = sf_read_float(mFile, buf->getRaw(),
			buf->getInfo()->getSamples() * buf->getInfo()->getChannels());
	
	if(len <= 0){
		//error
	}
	
	return buf;
}

IAudioInfo *FileReader::getInfo() noexcept{
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	AudioInfo *ret = new AudioInfo();
	*ret = mAudioInfo;
	return ret;
}

int FileReader::MaxInputs() const{
	return 0;
}

bool FileReader::HasOutputs() const{
	return true;
}

void FileReader::readConfig(const IKeyValueStore *conf){
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	try{
	}
	catch(std::exception &e){
	}
    return;
}

IControl *FileReader::getControl(){
	return mControl.get();
}

bool FileReader::checkCompatible(IAudioInfo *info){
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	if(info) return true;
	return false;
}

void FileReader::serialize(IMultiLevelStore *data) const{
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	if(!data) return;
	data->add("name", getName());
	data->add("file", mFileName.c_str());
	return;
}

void FileReader::deserialize(const IMultiLevelStore *data){
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	if(!data) return;
	try{
		setName(data->get("name"));
		setFileName(data->get("file"));
	}
	catch(std::exception &e){
	}
	return;
}

bool FileReader::setFileName(const char *path){
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	if(mFile) sf_close(mFile);
	mFile = NULL;
	mAudioInfo = AudioInfo();
	if(!path) return false;
	
	SF_INFO info;
	mFile = sf_open(path, SFM_READ, &info);
	if(!mFile) return false;
	if(info.frames * info.channels <= 0) return false;
	
	mFileName = path;
	mAudioInfo.setSamplerate(info.samplerate);
	mAudioInfo.setSamples(info.frames * info.channels);
	mAudioInfo.setChannels(info.channels);
	
	/*
	std::cerr << "FILEREADER: " << std::endl;
	std::cerr << mAudioInfo.getSamplerate() << std::endl;
	std::cerr << mAudioInfo.getSamples() << std::endl;
	std::cerr << mAudioInfo.getChannels() << std::endl;
	*/
	return true;
}

const char *FileReader::getFileName(){
	return mFileName.c_str();
}


FileReader::Control::Control(FileReader *data){
	mData = data;
}

FileReader::Control::~Control(){
}

unsigned int FileReader::Control::getNumFunctions(){
	return 2;
}

const char *FileReader::Control::getFunctionName(unsigned int num){
	if(num == 0) return "setFileName";
	return NULL;
}

const char *FileReader::Control::getFunctionParam(unsigned int num){
	if(num == 0) return "s";
	return NULL;
}

const char *FileReader::Control::callFunction(unsigned int num, const char *param){
	if(num == 0){
		if(!param) return heapLiteral("param is NULL!");
		if(!mData->setFileName(param)){
			return heapLiteral("failed to set file");
		}
		return NULL;
	}
	return heapLiteral("invalid function number!");
}

const char *FileReader::Control::callFunction(const char *name, const char *param){
	if(std::string("setFileName") == std::string(name)){
		return callFunction((unsigned int)0, param);
	}
	return heapLiteral("invalid function name!");
}

void FileReader::Control::stop(){
	return;
}

} // maudio


extern "C" void* create(){
	return new maudio::FileReader();
}

extern "C" void destroy(void *data){
	delete (maudio::FileReader *)data;
}

extern "C" const char *getName(){
	static const char *ret = "FileReader";
	return ret;
}






