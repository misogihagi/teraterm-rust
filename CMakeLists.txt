﻿
cmake_minimum_required(VERSION 3.11)

project(teraterm_all)

option(ENABLE_TTXSAMPLES "TTXSamples" ON)
option(MORE_WARNING "stronger warning" OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# _WIN32_WINNT_WIN10 0x0a00
set(_WIN32_WINNT "0x0a00")
if(MSVC)
  # https://www.leeholmes.com/blog/2017/02/27/differences-between-visual-studio-2012-2013-and-2015/
  if(${CMAKE_GENERATOR} MATCHES "Visual Studio 8 2005" OR ${CMAKE_GENERATOR} MATCHES "Visual Studio 9 2008")
    set(_WIN32_WINNT "0x0501")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_WIN32_WINNT=${_WIN32_WINNT}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D_WIN32_WINNT=${_WIN32_WINNT}")
  string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
  string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
  string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
  string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
  if (MORE_WARNING)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
    if(${MSVC_VERSION} GREATER_EQUAL 1910) # Visual Studio 2017以上の時
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /analyze")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /analyze")
    endif()
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_CRT_SECURE_NO_DEPRECATE")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D_CRT_SECURE_NO_DEPRECATE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3")
  endif()
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GF /Gy")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /GF /Gy")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
  if(${MSVC_VERSION} GREATER_EQUAL 1910)
    # Visual Studio 2017以上の時
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /ZI")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
  endif()
elseif(MINGW)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_WIN32_WINNT=${_WIN32_WINNT}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=${_WIN32_WINNT}")
  set(CMAKE_C_CXX_WARNING_FLAGS "-Wall -Wextra -Wno-unused-parameter -Wno-sign-compare")
  set(CMAKE_C_WARNING_FLAGS "${CMAKE_C_CXX_WARNING_FLAGS}  -Wno-pointer-sign")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_WARNING_FLAGS} -static -ffunction-sections -fdata-sections ${C_ONLY_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_CXX_WARNING_FLAGS} -static -ffunction-sections -fdata-sections")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
  if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc -static-libstdc++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
  endif()
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s -Wl,--gc-sections")
  set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -s -Wl,--gc-sections")
endif()

### create version_info.h
if(EXISTS "${CMAKE_SOURCE_DIR}/.git/svn")
  # git-svn
  find_package(Git)
  if(Git_FOUND)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} svn log --oneline -1
      RESULT_VARIABLE rv
      OUTPUT_VARIABLE ov
      ERROR_QUIET)
    if(${rv})
      message("git svn error? please check git svn")
    else()
      string(REGEX MATCH "[0-9]+" SVNVERSION "${ov}")
    endif()
  endif()
elseif(EXISTS "${CMAKE_SOURCE_DIR}/.svn")
  # svn
  find_package(Subversion)
  if(SUBVERSION_FOUND)
    if("CMAKE_COMMAND=${CMAKE_COMMAND}" MATCHES "msys")
      # svn on msys
      file(TO_NATIVE_PATH ${CMAKE_SOURCE_DIR} MSYS_CMAKE_SOURCE_DIR)
      string(REGEX REPLACE "^([a-zA-Z]):/" "/\\1/" MSYS_CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR})
      Subversion_WC_INFO(${MSYS_CMAKE_SOURCE_DIR} TT)
    else()
      Subversion_WC_INFO(${CMAKE_SOURCE_DIR} TT)
    endif()
    set(SVNVERSION "${TT_WC_REVISION}")
  endif()
else()
  # build without svn,git?
  #set(SVNVERSION "0000")
endif()
configure_file(
  version_info.h.in
  version_info.h
  @ONLY)
message("revision=\"${SVNVERSION}\"")

### tool
# perl
if (MSVC OR ("$ENV{MSYSTEM}" MATCHES "MINGW"))
  find_program(
    PERL perl.exe
    HINTS ${CMAKE_CURRENT_SOURCE_DIR}/libs/perl/perl/bin
    HINTS c:/Strawberry/perl/bin
    HINTS c:/Perl64/bin
    HINTS c:/Perl/bin
    HINTS c:/cygwin/usr/bin
    HINTS c:/cygwin64/usr/bin
    )
else()
  set(PERL "perl")
endif()
message("perl=${PERL}")

if(NOT $ENV{USER})
  set(USER $ENV{USER})
elseif(NOT $ENV{USERNAME})
  set(USER $ENV{USERNAME})
else()
  set(USER "NONAME")
endif()

if(NOT DEFINED SNAPSHOT_DIR)
  if(DEFINED SVNVERSION)
    string(TIMESTAMP SNAPSHOT_DIR "snapshot-r${SVNVERSION}-%Y%m%d_%H%M%S-${USER}")
  else()
    string(TIMESTAMP SNAPSHOT_DIR "snapshot-unknown-%Y%m%d_%H%M%S-${USER}")
  endif()
endif()
set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/${SNAPSHOT_DIR}")

### subdirectories
add_subdirectory(svnrev_perl)
add_subdirectory(teraterm)
add_subdirectory(ttpmenu)
add_subdirectory(TTProxy)
add_subdirectory(ttssh2)
add_subdirectory(TTXKanjiMenu)
if(${ENABLE_TTXSAMPLES})
  add_subdirectory(TTXSamples)
endif()
add_subdirectory(installer)
add_subdirectory(doc)
