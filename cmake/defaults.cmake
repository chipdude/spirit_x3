# Site defaults for compiling and linking.
# Things you can set:
#    CMAKE_BUILD_TYPE - see cmake docs; default is "Debug"
#    USE_GPROF USE_PPROF - set true or false
#    WITH_PGO - set to "generate" or "use"
# After including this file:
#    (nothing, it's all automatic)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

option(USE_INLINE "Allow function inlining" ON)
option(USE_GPROF "Profile the project using gprof" OFF)
option(USE_PPROF "Profile the project using pprof" OFF)
set(WITH_PGO "" CACHE STRING "Profile-guided optimization: 'generate' or 'use'")

# general flags for any compilation with gcc/g++
# Parsec is all at least Westmere, which is i7 pus AES and PCLMUL, but not AVX
# But the fastest hardware is AVX2 so tune for that
set(GNU_FLAGS "-pthread -march=corei7 -maes -mpclmul -mtune=corei7-avx")
set(GNU_FLAGS "${GNU_FLAGS} -pedantic -Wall -Wextra")

if(NOT USE_INLINE)
    message("Disabling inlining")
    set(GNU_FLAGS "${GNU_FLAGS} -fno-inline")
endif(NOT USE_INLINE)
if(USE_GPROF)
    message("Adding profiling info for gprof")
    set(GNU_FLAGS "${GNU_FLAGS} -pg")
endif(USE_GPROF)
if(USE_PPROF)
    message("Linking with -lprofiler")
    set(LINK_LIBRARIES "${LINK_LIBRARIES} profiler")
endif(USE_PPROF)
if(WITH_PGO)
    message("Will ${WITH_PGO} profile-guided optimization")
    set(GNU_FLAGS "${GNU_FLAGS} -fprofile-${WITH_PGO}")
endif(WITH_PGO)

# glibc and libstdc++ features we're fond of (i.e. need):
add_definitions(-D_REENTRANT -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -D_GNU_SOURCE)
# these are for std::thread:
add_definitions(-D_GLIBCXX_USE_NANOSLEEP -D_GLIBCXX_USE_SCHED_YIELD)
# valgrind awareness is occasionally useful:
include(CheckIncludeFiles)
check_include_files("valgrind/valgrind.h" HAVE_VALGRIND_H)
if (NOT HAVE_VALGRIND_H)
    add_definitions(-DNVALGRIND)
endif (NOT HAVE_VALGRIND_H)

# diverging C and C++
set(GXX_FLAGS "-std=c++1y ${GNU_FLAGS} -Wundef")
set(GCC_FLAGS "-std=c11   ${GNU_FLAGS} -Wstrict-prototypes -Wmissing-prototypes")

# release- and debug-specific flags for gcc
set(GNU_DEBINFO_FLAGS "-ggdb3")
set(GNU_DEBUG_FLAGS   "-O0 ${GNU_DEBINFO_FLAGS} -D_DEBUG -DDEBUG -rdynamic")
set(GNU_RELEASE_FLAGS "-O2 -DNDEBUG")

# now that we know what the flags are, paste them into the cmake macros
set(CMAKE_C_FLAGS                  "${GCC_FLAGS}")
set(CMAKE_CXX_FLAGS                "${GXX_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE          "${GNU_RELEASE_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE        "${GNU_RELEASE_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG            "${GNU_DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG          "${GNU_DEBUG_FLAGS}")
set(CMAKE_C_FLAGS_RELWITHDEBINFO   "${GNU_RELEASE_FLAGS} ${GNU_DEBINFO_FLAGS}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${GNU_RELEASE_FLAGS} ${GNU_DEBINFO_FLAGS}")
