cmake_minimum_required(VERSION 3.0)
project(Player.mplug)

if(NOT DEFINED MAUDIO_PARENT_PROJECT)
	set(MAUDIO_PARENT_PROJECT 0)
endif()

set(PORTABLE 0 CACHE BOOL "set portable install")

if(PORTABLE EQUAL 1)
	add_definitions("-DMAUDIO_PORTABLE=1")
else()
	add_definitions("-DMAUDIO_PORTABLE=0")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wl,--no-as-needed")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "./res/lib:.")

set(MAUDIO_PLAYER_SOURCE_FILES
	${PROJECT_SOURCE_DIR}/Player.cpp
	${PROJECT_SOURCE_DIR}/AudioDevice.cpp
)

#maudio library
add_library(player SHARED ${MAUDIO_PLAYER_SOURCE_FILES})
target_link_libraries(player -lportaudio)
target_link_libraries(player -pthread)

if(${MAUDIO_PARENT_PROJECT} EQUAL 0)
	target_link_libraries(player -lmaudio)
else()
	target_link_libraries(player maudio)
endif()

set_target_properties(player PROPERTIES PREFIX "" OUTPUT_NAME "Player" SUFFIX ".mplug")

if(PORTABLE EQUAL 1)
	install(TARGETS player DESTINATION "res/plugins")
	file(APPEND ../plugins.conf "plugin Player.mplug\n")
else()
	install(TARGETS player DESTINATION "share/maudio/plugins")
	file(APPEND ../plugins.conf "plugin Player.mplug\n")
endif()
