# Copyright (C) 2023 Advanced Micro Devices, Inc.  All rights reserved.
# Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.14.7)
project(xilsecure)

include(${CMAKE_CURRENT_SOURCE_DIR}/xilsecure.cmake NO_POLICY_SCOPE)
find_package(common)
collector_create (PROJECT_LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}")
collector_create (PROJECT_LIB_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${CMAKE_BINARY_DIR}/include)

enable_language(C ASM)

add_subdirectory(common)
if("${CMAKE_MACHINE}" STREQUAL "Versal")
add_subdirectory(versal)
else()
add_subdirectory(zynqmp)
endif()

foreach(lang ASM C CXX)
  # GNU ar always updates the index: no need for CMAKE_RANLIB
  SET(CMAKE_${lang}_CREATE_STATIC_LIBRARY
      "<CMAKE_AR> rc <TARGET> <LINK_FLAGS> <OBJECTS>")
endforeach()

collector_list (_sources PROJECT_LIB_SOURCES)
collector_list (_headers PROJECT_LIB_HEADERS)
file(COPY ${_headers} DESTINATION ${CMAKE_BINARY_DIR}/include)
if (${NON_YOCTO})
file(COPY ${CMAKE_BINARY_DIR}/include/xsecure_config.h DESTINATION ${CMAKE_INCLUDE_PATH}/)
endif()
add_library(xilsecure STATIC ${_sources})
set_target_properties(xilsecure PROPERTIES LINKER_LANGUAGE C)
get_headers(${_headers})
set_target_properties(xilsecure PROPERTIES ADDITIONAL_CLEAN_FILES "${CMAKE_LIBRARY_PATH}/libxilsecure.a;${CMAKE_INCLUDE_PATH}/xsecure_config.h;${clean_headers}")
install(TARGETS xilsecure LIBRARY DESTINATION ${CMAKE_LIBRARY_PATH} ARCHIVE DESTINATION ${CMAKE_LIBRARY_PATH})
install(DIRECTORY ${CMAKE_BINARY_DIR}/include DESTINATION ${CMAKE_INCLUDE_PATH}/..) 
