# Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
add_subdirectory(common)
if(("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "cortexa72")
	OR ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "cortexa53")
	OR ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "cortexa53-32")
	OR ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "aarch64"))
add_subdirectory(ARMv8)
else()
add_subdirectory(${CMAKE_SYSTEM_PROCESSOR})
endif()
