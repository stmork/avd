#
#  SPDX-License-Identifier: MIT
#  SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
#

cmake_minimum_required(VERSION 3.28)
project(AVD
	DESCRIPTION "Amiga Video Digitizer"
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -fno-rtti -fno-exceptions")

add_compile_definitions(_DEBUG)

include_directories(${GLOBAL_INCLUDE})

add_executable(${PROJECT_NAME}
	avd.cc
	avddeviceinfo.cc      avddeviceinfo.h
	i2cdevice.cc          i2cdevice.h
	dummyi2cdevice.cc     dummyi2cdevice.h
	amigavideodevice.cc   amigavideodevice.h
	videodevicefactory.cc videodevicefactory.h
)

add_custom_target(astyle
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	COMMAND astyle
	*.cc *.h
)

add_custom_target(cppcheck
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	COMMAND cppcheck
	-D_DEBUG
	--inline-suppr
	--suppress=unusedStructMember
	--enable=style,warning,performance,portability
	--language=c++ --std=c++20
	--library=qt
	--xml-version=2 --force -q
	*.cc *.h
	2>cppcheck.xml
)

set_property(
	TARGET cppcheck
	APPEND
	PROPERTY ADDITIONAL_CLEAN_FILES ${CMAKE_SOURCE_DIR}/cppcheck.xml
)
