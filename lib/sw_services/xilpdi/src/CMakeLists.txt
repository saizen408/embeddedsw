# Copyright (C) 2023 Advanced Micro Devices, Inc.  All rights reserved.
# Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.14.7)
project(xilpdi)

find_package(common)
collector_create (PROJECT_LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}")
collector_create (PROJECT_LIB_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${CMAKE_BINARY_DIR}/include)

add_subdirectory(common)
if("${CMAKE_MACHINE}" STREQUAL "Versal")
    add_subdirectory(versal)
endif()
collector_list (_sources PROJECT_LIB_SOURCES)
collector_list (_headers PROJECT_LIB_HEADERS)
file(COPY ${_headers} DESTINATION ${CMAKE_BINARY_DIR}/include)
add_library(xilpdi STATIC ${_sources})
set_target_properties(xilpdi PROPERTIES LINKER_LANGUAGE C)
get_headers(${_headers})
set_target_properties(xilpdi PROPERTIES ADDITIONAL_CLEAN_FILES "${CMAKE_LIBRARY_PATH}/libxilpdi.a;${clean_headers}")
install(TARGETS xilpdi LIBRARY DESTINATION ${CMAKE_LIBRARY_PATH} ARCHIVE DESTINATION ${CMAKE_LIBRARY_PATH})
install(DIRECTORY ${CMAKE_BINARY_DIR}/include DESTINATION ${CMAKE_INCLUDE_PATH}/..) 
