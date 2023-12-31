# Copyright (C) 2023 Advanced Micro Devices, Inc.  All rights reserved.
# Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.14.7)
project(common)

find_package(common)
if (${NON_YOCTO})
set(INTC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../intc/src/")
set(SCUGIC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../scugic/src/")
else()
set(INTC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SCUGIC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
endif()
find_package(INTC QUIET)
list(APPEND defs "#ifndef XCOMMON_DRV_CONFIG_H_\n")
list(APPEND defs "#define XCOMMON_DRV_CONFIG_H_\n\n")
if ((EXISTS "${INTC_DIR}/INTCConfig.cmake") AND
    DRIVER_INSTANCES)
    list(APPEND defs "#define AXI_INTC\n")
endif()

find_package(SCUGIC QUIET)
if ((EXISTS "${SCUGIC_DIR}/SCUGICConfig.cmake") AND
     DRIVER_INSTANCES)
    list(APPEND defs "#define XPAR_SCUGIC\n")
endif()
list(APPEND defs "\n#endif")
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/xcommon_drv_config.h ${defs})

collector_create (PROJECT_LIB_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}")
collector_create (PROJECT_LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${CMAKE_BINARY_DIR}/include)
add_subdirectory(clocking)
collect (PROJECT_LIB_HEADERS xinterrupt_wrap.h)
collect (PROJECT_LIB_HEADERS xcommon_drv_config.h)
collect (PROJECT_LIB_SOURCES xinterrupt_wrap.c)
collector_list (_sources PROJECT_LIB_SOURCES)
collector_list (_headers PROJECT_LIB_HEADERS)
file(COPY ${_headers} DESTINATION ${CMAKE_BINARY_DIR}/include)
add_library(common STATIC ${_sources})
set_target_properties(common PROPERTIES LINKER_LANGUAGE C)
