/*
 * Project Maudio
 * Copyright (C) 2015 Martin Schwarz
 * See LICENSE.txt for the full license
 */

#include "FileWriter.hpp"
#include "maudio/util/sptr.hpp"
#include "maudio/util/Util.hpp"
#include "maudio/util/AudioException.hpp"

namespace maudio{

FileWriter::FileWriter(){
}

FileWriter::~FileWriter(){
	stopWriting();
}

void FileWriter::write(unsigned long pos, unsigned long length){
	stopWriting();
	
	mThreadWorking = true;
	mThread.reset(new std::thread(asyncWrite, this, pos, length));
	return;
}

void FileWriter::stopWriting(){
	if(mThreadWorking){
		mThreadWorking = false;
		if(mThread->joinable()) mThread->join();
		else mThread.reset();
	}
	return;
}

void FileWriter::asyncWrite(FileWriter *writer, unsigned long pos, unsigned long length){
	writer->mSuccess = 1;
	if(!writer->InputOk(0)){
		writer->mSuccess = -1;
		return;
	}
	auto buf = writer->getFromSlot(0, pos, length);
	auto minfo = writer->getInfoFromSlot(0);
	if(!buf || !minfo){
		writer->mSuccess = -1;
		return;
	}
	
	SNDFILE* ofile = NULL;
	SF_INFO info;
	
	info.samplerate = minfo->getSamplerate();
	info.channels = minfo->getChannels();
	info.format = SF_FORMAT_OGG | SF_FORMAT_VORBIS; //TODO set this function

	if(!(ofile = sf_open(writer->mFile.c_str(), SFM_WRITE, &info))){
		sf_close(ofile);
		writer->mSuccess = -1;
		return;
	}
	
	unsigned long len = buf->getInfo()->getSamples() * buf->getInfo()->getChannels();
	
	if(sf_write_float(ofile, buf->getRaw(), (sf_count_t)len) != (sf_count_t)len){
		sf_close(ofile);
		writer->mSuccess = -1;
		return;
	}

	sf_write_sync(ofile);
	sf_close(ofile);
	
	writer->mSuccess = 2;
	return;
}

bool FileWriter::setFileName(const char *path){
	if(!path) return false;
	mFile = path;
	return true;
}

const char *FileWriter::getFileName(){
	return mFile.c_str();
}

IAudioBuffer *FileWriter::get(unsigned long pos, unsigned int length) noexcept{
	return NULL;
}

IAudioInfo *FileWriter::getInfo() noexcept{
	return NULL;
}

int FileWriter::MaxInputs() const{
	return 1;
}

bool FileWriter::HasOutputs() const{
	return false;
}

void FileWriter::readConfig(const IKeyValueStore *conf){
	try{
	}
	catch(std::exception &e){
	}
    return;
}

IControl *FileWriter::getControl(){
	return mControl.get();
}

bool FileWriter::checkCompatible(IAudioInfo *info){
	if(info) return true;
	return false;
}

void FileWriter::serialize(IMultiLevelStore *data) const{
	if(!data) return;
	data->add("name", getName());
	data->add("file", mFile.c_str());
	return;
}

void FileWriter::deserialize(const IMultiLevelStore *data){
	if(!data) return;
	try{
		setName(data->get("name"));
		mFile = data->get("file");
	}
	catch(std::exception &e){
	}
	return;
}


FileWriter::Control::Control(FileWriter *data){
	mData = data;
}

FileWriter::Control::~Control(){
}

unsigned int FileWriter::Control::getNumFunctions(){
	return 2;
}

const char *FileWriter::Control::getFunctionName(unsigned int num){
	if(num == 0) return "setFileName";
	if(num == 1) return "write";
	if(num == 2) return "getResult";
	return NULL;
}

const char *FileWriter::Control::getFunctionParam(unsigned int num){
	if(num == 0) return "s";
	if(num == 1) return "ul;ul=0";
	if(num == 2) return "void";
	return NULL;
}

const char *FileWriter::Control::callFunction(unsigned int num, const char *param){
	if(num == 0){
		if(!param) return heapLiteral("param is NULL!");
		std::vector<std::string> params = split(std::string(param), ';');
		if(params.size() < 1) return heapLiteral("invalid number of params!");
		
		mData->setFileName(params[0].c_str());
		return NULL;
	}
	if(num == 1){
		if(!param) return heapLiteral("param is NULL!");
		std::vector<std::string> params = split(std::string(param), ';');
		if(params.size() < 1) return heapLiteral("invalid number of params!");
		try{
			unsigned long param1 = string_to<unsigned long>(params[0]);
			unsigned long param2 = 0;
			if(params.size() >= 2) param2 = string_to<unsigned long>(params[1]);
				mData->write(param1, param2);
			}
		catch(std::exception &e){
			return heapLiteral("invalid param!");
		}
		return NULL;
	}
	if(num == 2){
		if(mData->mSuccess == 0) return heapLiteral("not running");
		if(mData->mSuccess == 1) return heapLiteral("working");
		if(mData->mSuccess == 2) return heapLiteral("success");
		else return heapLiteral("failure");
	}
	return heapLiteral("invalid function number!");
}

const char *FileWriter::Control::callFunction(const char *name, const char *param){
	if(std::string("setFileName") == std::string(name)){
		return callFunction((unsigned int)0, param);
	}
	if(std::string("write") == std::string(name)){
		return callFunction((unsigned int)1, param);
	}
	if(std::string("getResult") == std::string(name)){
		return callFunction((unsigned int)2, param);
	}
	return heapLiteral("invalid function name!");
}

void FileWriter::Control::stop(){
	mData->stopWriting();
	return;
}

} // maudio


extern "C" void* create(){
	return new maudio::FileWriter();
}

extern "C" void destroy(void *data){
	delete (maudio::FileWriter *)data;
}

extern "C" const char *getName(){
	static const char *ret = "FileWriter";
	return ret;
}






