cmake_minimum_required(VERSION 3.0)
project(FileReader.mplug)

set(MAUDIO_PORTABLE 0 CACHE BOOL "set portable install")

if(MAUDIO_PORTABLE EQUAL 1)
	add_definitions("-DMAUDIO_PORTABLE=1")
else()
	add_definitions("-DMAUDIO_PORTABLE=0")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wl,--no-as-needed")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "./res/lib:.")

set(MAUDIO_FILEWRITER_SOURCE_FILES
	${PROJECT_SOURCE_DIR}/FileReader.cpp
)

#maudio library
add_library(filereader SHARED ${MAUDIO_FILEWRITER_SOURCE_FILES})
target_link_libraries(filereader -lsndfile)
target_link_libraries(filereader maudio)

set_target_properties(filereader PROPERTIES PREFIX "" OUTPUT_NAME "FileReader" SUFFIX ".mplug")

if(MAUDIO_PORTABLE EQUAL 1)
	install(TARGETS filereader DESTINATION "res/plugins")
	file(APPEND ../plugins.conf "plugin FileReader.mplug\n")
else()
	install(TARGETS filereader LIBRARY DESTINATION "plugins")
endif()
