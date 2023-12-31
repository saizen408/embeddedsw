# Copyright (C) 2023 Advanced Micro Devices, Inc.  All rights reserved.
# Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.15)
project(sdps)

find_package(common)
collector_create (PROJECT_LIB_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}")
collector_create (PROJECT_LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${CMAKE_BINARY_DIR}/include)
collect (PROJECT_LIB_HEADERS xsdps_core.h)
collect (PROJECT_LIB_SOURCES xsdps_host.c)
collect (PROJECT_LIB_SOURCES xsdps_options.c)
collect (PROJECT_LIB_SOURCES xsdps_card.c)
collect (PROJECT_LIB_SOURCES xsdps_sinit.c)
collect (PROJECT_LIB_SOURCES xsdps.c)
collect (PROJECT_LIB_HEADERS xsdps.h)
collect (PROJECT_LIB_HEADERS xsdps_hw.h)
collect (PROJECT_LIB_SOURCES xsdps_g.c)
collector_list (_sources PROJECT_LIB_SOURCES)
collector_list (_headers PROJECT_LIB_HEADERS)
file(COPY ${_headers} DESTINATION ${CMAKE_BINARY_DIR}/include)
add_library(sdps STATIC ${_sources})
set_target_properties(sdps PROPERTIES LINKER_LANGUAGE C)
