cmake_minimum_required(VERSION 3.0)
project(maudio_plugins)

if(NOT DEFINED MAUDIO_PARENT_PROJECT)
	set(MAUDIO_PARENT_PROJECT 0)
endif()

set(PORTABLE 1 CACHE BOOL "set portable install")
set(DEBIAN 0 CACHE BOOL "create Debian package")

if(PORTABLE EQUAL 1)
	message(STATUS "maudio_plugins: using portable install")
	
	set(CMAKE_INSTALL_PREFIX "install")
	add_definitions("-DMAUDIO_PORTABLE=1")
else()
	message(STATUS "maudio_plugins: using system install")
	
	add_definitions("-DMAUDIO_PORTABLE=0")
endif()

file(WRITE plugins.conf "|IMultiStore\n")

add_subdirectory("Mixer")
add_subdirectory("Player")
add_subdirectory("FileWriter")
add_subdirectory("FileReader")

#install lib to prefix
if(PORTABLE EQUAL 1)
	install(FILES plugins.conf DESTINATION "res/config")
else()
	install(FILES plugins.conf DESTINATION "maudio/config")
endif()

#create packages
if(${MAUDIO_PARENT_PROJECT} EQUAL 0)
	if(PORTABLE EQUAL 1)
		message(STATUS "maudio_plugins: using zip package generator")
		set(CPACK_GENERATOR "ZIP")
	elseif(${DEBIAN} EQUAL 1)
		message(STATUS "maudio_plugins: using Debian package generator")
		set(CPACK_GENERATOR "DEB")
	endif()

	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Martin Schwarz")
	set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MAudio-Plugins")
	set(CPACK_PACKAGE_VENDOR "Martin Schwarz")
	set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.txt")
	set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
	set(CPACK_PACKAGE_VERSION_MAJOR "0")
	set(CPACK_PACKAGE_VERSION_MINOR "0")
	set(CPACK_PACKAGE_VERSION_PATCH "0")
	set(CPACK_OUTPUT_FILE_PREFIX "packages")
	INCLUDE(CPack)
endif()