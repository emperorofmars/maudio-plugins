cmake_minimum_required(VERSION 3.0)

file(WRITE plugins.conf "|IMultiStore\n")
file(WRITE maudio.conf "|IKeyValueStore\n")
#file(WRITE maudio.conf "|IKeyValueStore\npluginconf res/config/plugins.conf\n")

add_subdirectory("Mixer")
add_subdirectory("Player")
add_subdirectory("FileWriter")
add_subdirectory("FileReader")

if(PORTABLE EQUAL 1)
	install(FILES maudio.conf plugins.conf DESTINATION "res/config")
endif()