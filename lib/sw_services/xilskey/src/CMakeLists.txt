# Copyright (C) 2023 Advanced Micro Devices, Inc.  All rights reserved.
# Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.14.7)
project(xilskey)

find_package(common)
include(${CMAKE_CURRENT_SOURCE_DIR}/xilskey.cmake NO_POLICY_SCOPE)
collector_create (PROJECT_LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}")
collector_create (PROJECT_LIB_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${CMAKE_BINARY_DIR}/include)

enable_language(C ASM)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include)

collect (PROJECT_LIB_SOURCES xilskey_eps_zynqmp.c)
collect (PROJECT_LIB_SOURCES xilskey_eps_zynqmp_puf.c)
collect (PROJECT_LIB_SOURCES xilskey_utils.c)
collect (PROJECT_LIB_HEADERS xilskey_bbramps_zynqmp_hw.h)
collect (PROJECT_LIB_HEADERS xilskey_eps_zynqmp_hw.h)


if(("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "cortexa53"))
collect (PROJECT_LIB_SOURCES xilskey_bbramps_zynqmp.c)
elseif((NOT("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "cortexa53")) AND (NOT("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "pmu_microblaze")))
collect (PROJECT_LIB_SOURCES xilskey_bbram.c)
collect (PROJECT_LIB_SOURCES xilskey_epl.c)
collect (PROJECT_LIB_HEADERS xilskey_eps.c)
collect (PROJECT_LIB_HEADERS xilskey_epshw.h)
collect (PROJECT_LIB_HEADERS xilskey_jscmd.c)
collect (PROJECT_LIB_HEADERS xilskey_jscmd.h)
collect (PROJECT_LIB_HEADERS xilskey_js.h)
collect (PROJECT_LIB_HEADERS xilskey_jslib.c)
collect (PROJECT_LIB_HEADERS xilskey_jslib.h)
collect (PROJECT_LIB_HEADERS xilskey_jtag.h)
collect (PROJECT_LIB_SOURCES xilskey_bbramps_zynqmp.c)
endif()



collector_list (_sources PROJECT_LIB_SOURCES)
collector_list (_headers PROJECT_LIB_HEADERS)
file(COPY ${_headers} DESTINATION ${CMAKE_BINARY_DIR}/include)
if (${NON_YOCTO})
file(COPY ${CMAKE_BINARY_DIR}/include/xilskey_config.h DESTINATION ${CMAKE_INCLUDE_PATH}/)
endif()
add_library(xilskey STATIC ${_sources})
set_target_properties(xilskey PROPERTIES LINKER_LANGUAGE C)
get_headers(${_headers})
set_target_properties(xilskey PROPERTIES ADDITIONAL_CLEAN_FILES "${CMAKE_LIBRARY_PATH}/libxilskey.a;${CMAKE_INCLUDE_PATH}/xilskey_config.h;${clean_headers}")
install(TARGETS xilskey LIBRARY DESTINATION ${CMAKE_LIBRARY_PATH} ARCHIVE DESTINATION ${CMAKE_LIBRARY_PATH})
install(DIRECTORY ${CMAKE_BINARY_DIR}/include DESTINATION ${CMAKE_INCLUDE_PATH}/..) 
