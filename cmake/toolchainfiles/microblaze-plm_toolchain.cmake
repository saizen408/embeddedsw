set( CMAKE_EXPORT_COMPILE_COMMANDS ON)
set( CMAKE_TRY_COMPILE_PLATFORM_VARIABLES CMAKE_LIBRARY_PATH)
set( CMAKE_INSTALL_MESSAGE LAZY)
set( CMAKE_C_COMPILER mb-gcc )
set( CMAKE_CXX_COMPILER mb-g++ )
set( CMAKE_C_COMPILER_LAUNCHER  )
set( CMAKE_CXX_COMPILER_LAUNCHER  )
set( CMAKE_ASM_COMPILER mb-gcc )
set( CMAKE_AR mb-ar CACHE FILEPATH "Archiver" )
set( CMAKE_SIZE mb-size CACHE FILEPATH "Size" )
set( CMAKE_SYSTEM_PROCESSOR "plm_microblaze" )
set( CMAKE_MACHINE "Versal" )
set( CMAKE_BUILD_TYPE Release)
set( CMAKE_SYSTEM_NAME "Generic" )
set( CMAKE_SPECS_FILE "$ENV{ESW_REPO}/scripts/specs/microblaze/Xilinx.spec" CACHE STRING "Specs file path for using CMAKE toolchain files" )
set( CMAKE_C_FLAGS " -mlittle-endian -mxl-barrel-shift -mxl-pattern-compare -mxl-reorder -mno-xl-soft-mul -mxl-multiply-high -mno-xl-soft-div -Os -flto -ffat-lto-objects  -DVERSAL_PLM=1 -mcpu=v10.0 -specs=${CMAKE_SPECS_FILE} -I${CMAKE_INCLUDE_PATH}" CACHE STRING "CFLAGS" )
set( CMAKE_CXX_FLAGS " -mlittle-endian -mxl-barrel-shift -mxl-pattern-compare -mxl-reorder -mno-xl-soft-mul -mxl-multiply-high -mno-xl-soft-div -Os -flto -ffat-lto-objects -DVERSAL_PLM=1 -mcpu=v10.0 -specs=${CMAKE_SPECS_FILE}" CACHE STRING "CFLAGS" )
set( CMAKE_ASM_FLAGS " -mlittle-endian -mxl-barrel-shift -mxl-pattern-compare -mxl-reorder -mno-xl-soft-mul -mxl-multiply-high -mno-xl-soft-div -Os -flto -ffat-lto-objects -DVERSAL_PLM=1 -mcpu=v10.0 -specs=${CMAKE_SPECS_FILE} -I${CMAKE_INCLUDE_PATH}" CACHE STRING "ASM FLAGS" )
set( CMAKE_C_LINK_FLAGS " -mlittle-endian -mxl-barrel-shift -mxl-pattern-compare -mxl-reorder -mcpu=v10.0 -mno-xl-soft-mul -mxl-multiply-high -mno-xl-soft-div -Wl,-O1  -Wl,--as-needed -DVERSAL_PLM=1 -L${CMAKE_LIBRARY_PATH}" CACHE STRING "LDFLAGS" )
set( CMAKE_CXX_LINK_FLAGS " -mlittle-endian -mxl-barrel-shift -mxl-pattern-compare -mxl-reorder -mcpu=v10.0 -mno-xl-soft-mul -mxl-multiply-high -mno-xl-soft-div -Wl,-O1  -Wl,--as-needed -DVERSAL_PLM=1 -L${CMAKE_LIBRARY_PATH}" CACHE STRING "LDFLAGS" )
set( CMAKE_C_FLAGS_RELEASE "-DNDEBUG" CACHE STRING "Additional CFLAGS for release" )
set( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG" CACHE STRING "Additional CXXFLAGS for release" )
set( CMAKE_ASM_FLAGS_RELEASE "-DNDEBUG" CACHE STRING "Additional ASM FLAGS for release" )
