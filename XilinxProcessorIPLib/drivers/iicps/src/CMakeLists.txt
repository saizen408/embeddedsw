# Copyright (C) 2023 Advanced Micro Devices, Inc.  All rights reserved.
# Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.15)
project(iicps)

find_package(common)
collector_create (PROJECT_LIB_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}")
collector_create (PROJECT_LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${CMAKE_BINARY_DIR}/include)
collect (PROJECT_LIB_SOURCES xiicps_g.c)
collect (PROJECT_LIB_SOURCES xiicps_selftest.c)
collect (PROJECT_LIB_SOURCES xiicps.c)
collect (PROJECT_LIB_HEADERS xiicps.h)
collect (PROJECT_LIB_HEADERS xiicps_xfer.h)
collect (PROJECT_LIB_SOURCES xiicps_slave.c)
collect (PROJECT_LIB_SOURCES xiicps_hw.c)
collect (PROJECT_LIB_SOURCES xiicps_intr.c)
collect (PROJECT_LIB_SOURCES xiicps_master.c)
collect (PROJECT_LIB_HEADERS xiicps_hw.h)
collect (PROJECT_LIB_SOURCES xiicps_options.c)
collect (PROJECT_LIB_SOURCES xiicps_xfer.c)
collect (PROJECT_LIB_SOURCES xiicps_sinit.c)
collector_list (_sources PROJECT_LIB_SOURCES)
collector_list (_headers PROJECT_LIB_HEADERS)
file(COPY ${_headers} DESTINATION ${CMAKE_BINARY_DIR}/include)
add_library(iicps STATIC ${_sources})
set_target_properties(iicps PROPERTIES LINKER_LANGUAGE C)
