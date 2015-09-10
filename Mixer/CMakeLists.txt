cmake_minimum_required(VERSION 3.0)
project(Mixer.mplug)

set(MAUDIO_PORTABLE 0 CACHE BOOL "set portable install")

if(MAUDIO_PORTABLE EQUAL 1)
	add_definitions("-DMAUDIO_PORTABLE=1")
else()
	add_definitions("-DMAUDIO_PORTABLE=0")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wl,--no-as-needed")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "./res/lib:.")

set(MAUDIO_MIXER_SOURCE_FILES
	${PROJECT_SOURCE_DIR}/Mixer.cpp
)

#maudio library
add_library(mixer SHARED ${MAUDIO_MIXER_SOURCE_FILES})
target_link_libraries(mixer -pthread)
target_link_libraries(mixer maudio)

set_target_properties(mixer PROPERTIES PREFIX "" OUTPUT_NAME "Mixer" SUFFIX ".mplug")

if(MAUDIO_PORTABLE EQUAL 1)
	install(TARGETS mixer DESTINATION "res/plugins")
	file(APPEND ../plugins.conf "plugin Mixer.mplug\n")
else()
	install(TARGETS mixer LIBRARY DESTINATION "plugins")
endif()
