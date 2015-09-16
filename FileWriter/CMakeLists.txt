cmake_minimum_required(VERSION 3.0)
project(FileWriter.mplug)

if(NOT DEFINED MAUDIO_PARENT_PROJECT)
	set(MAUDIO_PARENT_PROJECT 0)
endif()

set(PORTABLE 0 CACHE BOOL "set portable install")
set(DEBIAN 0 CACHE BOOL "create Debian package")

if(PORTABLE EQUAL 1)
	add_definitions("-DMAUDIO_PORTABLE=1")
else()
	add_definitions("-DMAUDIO_PORTABLE=0")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wl,--no-as-needed")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "./res/lib:.")

set(MAUDIO_FILEWRITER_SOURCE_FILES
	${PROJECT_SOURCE_DIR}/FileWriter.cpp
)

#maudio library
add_library(filewriter SHARED ${MAUDIO_FILEWRITER_SOURCE_FILES})
target_link_libraries(filewriter -lsndfile)
target_link_libraries(filewriter -pthread)

if(${MAUDIO_PARENT_PROJECT} EQUAL 0)
	target_link_libraries(filewriter -lmaudio)
else()
	target_link_libraries(filewriter maudio)
endif()

set_target_properties(filewriter PROPERTIES PREFIX "" OUTPUT_NAME "FileWriter" SUFFIX ".mplug")

if(PORTABLE EQUAL 1)
	install(TARGETS filewriter DESTINATION "res/plugins")
	file(APPEND ../plugins.conf "plugin FileWriter.mplug\n")
else()
	install(TARGETS filewriter LIBRARY DESTINATION "plugins")
endif()
