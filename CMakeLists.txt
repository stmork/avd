#
#  SPDX-License-Identifier: MIT
#  SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
#

cmake_minimum_required(VERSION 3.28)
project(AVD
	DESCRIPTION "Amiga Video Digitizer"
	LANGUAGES CXX
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -Wall -Wextra")

include_directories(${GLOBAL_INCLUDE})

add_executable(${PROJECT_NAME}
	avd.cc
	i2cdevice.cc i2cdevice.h
	dummyi2cdevice.h dummyi2cdevice.cc
)

add_custom_target(astyle
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	COMMAND astyle
	*.cc *.h
)
