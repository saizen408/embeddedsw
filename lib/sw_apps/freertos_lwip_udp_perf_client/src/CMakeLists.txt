# Copyright (C) 2023 Advanced Micro Devices, Inc.  All rights reserved.
# Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.14.7)

include(${CMAKE_CURRENT_SOURCE_DIR}/Freertos_lwip_udp_perf_clientExample.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/UserConfig.cmake)
set(APP_NAME freertos_lwip_udp_perf_client)
project(${APP_NAME})

find_package(common)
include(${CMAKE_SOURCE_DIR}/freertos_lwip_udp_perf_client.cmake NO_POLICY_SCOPE)
enable_language(C ASM)
collect(PROJECT_LIB_DEPS xilstandalone)
collect(PROJECT_LIB_DEPS xil)
collect(PROJECT_LIB_DEPS xiltimer)
collect(PROJECT_LIB_DEPS lwip213)
collect(PROJECT_LIB_DEPS freertos)
collect(PROJECT_LIB_DEPS gcc)
collect(PROJECT_LIB_DEPS c)
collector_list (_deps PROJECT_LIB_DEPS)
list (APPEND _deps ${USER_LINK_LIBRARIES})

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
    set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})
endif()
collect (PROJECT_LIB_SOURCES udp_perf_client.c)
collect (PROJECT_LIB_SOURCES main.c)
collect (PROJECT_LIB_SOURCES iic_phyreset.c)
collector_list (_sources PROJECT_LIB_SOURCES)
linker_gen("${CMAKE_CURRENT_SOURCE_DIR}/linker_files/")
string(APPEND CMAKE_C_FLAGS ${USER_COMPILE_OPTIONS})
string(APPEND CMAKE_C_LINK_FLAGS ${USER_LINK_OPTIONS})
add_executable(${APP_NAME}.elf ${_sources})
target_link_libraries(${APP_NAME}.elf -Wl,-T -Wl,\"${CMAKE_SOURCE_DIR}/lscript.ld\" -L\"${CMAKE_SOURCE_DIR}/\" -L\"${CMAKE_LIBRARY_PATH}/\" -L\"${USER_LINK_DIRECTORIES}/\" -Wl,--start-group,-l${_deps} -Wl,--end-group)
target_compile_definitions(${APP_NAME}.elf PUBLIC ${USER_COMPILE_DEFINITIONS})
if(DEFINED CMAKE_SIZE)
add_custom_command(
	TARGET ${APP_NAME}.elf POST_BUILD
	COMMAND ${CMAKE_SIZE} --format=berkeley ${APP_NAME}.elf)
endif()
target_include_directories(${APP_NAME}.elf PUBLIC "${CMAKE_BINARY_DIR}/include")
if (${NON_YOCTO})
   set (INCLUDE_DIRS
       "${CMAKE_INCLUDE_PATH}/include/"
       "${CMAKE_BINARY_DIR}/include")
   target_include_directories(${APP_NAME}.elf PUBLIC ${INCLUDE_DIRS} ${USER_INCLUDE_DIRECTORIES})
endif()
