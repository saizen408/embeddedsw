# SPDX-License-Identifier: MIT
# Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "cortexa53-32")
add_subdirectory(32bit)
else()
add_subdirectory(64bit)
endif()
if(("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "cortexa53-32") OR ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "cortexa53") OR ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "aarch64") OR ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "cortexa72"))
add_subdirectory(includes_ps)
endif()
collect (PROJECT_LIB_HEADERS xarmv8.h)
collect (PROJECT_LIB_HEADERS xarmv8_config.h)
