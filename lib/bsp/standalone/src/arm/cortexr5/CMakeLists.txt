# SPDX-License-Identifier: MIT
# Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
add_subdirectory(gcc)
add_subdirectory(platform)
#collect (PROJECT_LIB_SOURCES xtime_l.c)
collect (PROJECT_LIB_SOURCES xil_mpu.c)
collect (PROJECT_LIB_HEADERS xil_mmu.h)
#collect (PROJECT_LIB_SOURCES xpm_counter.c)
collect (PROJECT_LIB_HEADERS xpseudo_asm.h)
collect (PROJECT_LIB_HEADERS xil_cache.h)
#collect (PROJECT_LIB_HEADERS xtime_l.h)
collect (PROJECT_LIB_HEADERS xpm_counter.h)
collect (PROJECT_LIB_SOURCES xil_cache.c)
#collect (PROJECT_LIB_SOURCES usleep.c)
collect (PROJECT_LIB_HEADERS xreg_cortexr5.h)
#collect (PROJECT_LIB_SOURCES sleep.c)
collect (PROJECT_LIB_HEADERS xil_mpu.h)
collect (PROJECT_LIB_HEADERS xcortexr5.h)
collect (PROJECT_LIB_HEADERS xcortexr5_config.h)
