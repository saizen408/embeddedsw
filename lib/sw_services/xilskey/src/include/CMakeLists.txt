# Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
collect (PROJECT_LIB_HEADERS xilskey_eps_zynqmp.h)
collect (PROJECT_LIB_HEADERS xilskey_eps_zynqmp_puf.h)
collect (PROJECT_LIB_HEADERS xilskey_utils.h)
if(NOT("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "pmu_microblaze"))
collect (PROJECT_LIB_HEADERS xilskey_bbram.h)
endif()
if((NOT("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "cortexa53")) AND (NOT("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "pmu_microblaze")) )
collect (PROJECT_LIB_HEADERS xilskey_epl.h)
collect (PROJECT_LIB_HEADERS xilskey_eps.h)
endif()

