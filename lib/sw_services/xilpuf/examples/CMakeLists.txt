# Copyright (C) 2023 Advanced Micro Devices, Inc.  All rights reserved.
# Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.14.7)
include(${CMAKE_CURRENT_SOURCE_DIR}/XilpufExample.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../src/xilpuf.cmake)
project(xilpuf)

find_package(common)
enable_language(C ASM)
collect(PROJECT_LIB_DEPS xilstandalone)
collect(PROJECT_LIB_DEPS xil)
collect(PROJECT_LIB_DEPS xiltimer)
collect(PROJECT_LIB_DEPS xilsecure)
collect(PROJECT_LIB_DEPS xilpuf)
collect(PROJECT_LIB_DEPS xilmailbox)
collect(PROJECT_LIB_DEPS gcc)
collect(PROJECT_LIB_DEPS c)
collector_list (_deps PROJECT_LIB_DEPS)

if (${XILPUF_Mode})
		SET(EXAMPLES xilpuf_regeneration_example.c; xilpuf_enc_dec_data_example.c;)
	else()
		SET(EXAMPLES xilpuf_regeneration_client_example.c; xilpuf_enc_dec_data_client_example.c;)
	endif()
linker_gen("${CMAKE_CURRENT_SOURCE_DIR}/../../../../scripts/linker_files/")

foreach(EXAMPLE ${EXAMPLES})
    string(REGEX REPLACE "\\.[^.]*$" "" EXAMPLE ${EXAMPLE})
    add_executable(${EXAMPLE}.elf ${EXAMPLE})
    target_link_libraries(${EXAMPLE}.elf -Wl,--gc-sections -T\"${CMAKE_SOURCE_DIR}/lscript.ld\" -L\"${CMAKE_SOURCE_DIR}/\" -Wl,--start-group ${_deps} -Wl,--end-group)
endforeach()
