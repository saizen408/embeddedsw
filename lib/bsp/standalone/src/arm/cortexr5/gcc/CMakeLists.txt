# SPDX-License-Identifier: MIT
# Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "FreeRTOS")
collect (PROJECT_LIB_SOURCES  asm_vectors.S)
endif()
collect (PROJECT_LIB_SOURCES boot.S)
collect (PROJECT_LIB_SOURCES cpu_init.S)
collect (PROJECT_LIB_SOURCES xil-crt0.S)
