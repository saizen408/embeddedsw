# Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
collect (PROJECT_LIB_SOURCES event_groups.c)
collect (PROJECT_LIB_SOURCES list.c)
collect (PROJECT_LIB_SOURCES queue.c)
collect (PROJECT_LIB_SOURCES stream_buffer.c)
collect (PROJECT_LIB_SOURCES timers.c)
collect (PROJECT_LIB_SOURCES croutine.c)
collect (PROJECT_LIB_SOURCES tasks.c)

add_subdirectory(portable)
add_subdirectory(include)
