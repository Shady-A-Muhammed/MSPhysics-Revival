cmake_minimum_required(VERSION 3.20)
project(msp_lib VERSION 2.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------
# Paths — passed in from the GitHub Actions workflow (or command line)
# -----------------------------------------------------------------------
set(RUBY_INCLUDE_DIR      "" CACHE PATH "Ruby main include dir (has ruby.h)")
set(RUBY_INCLUDE_ARCH_DIR "" CACHE PATH "Ruby arch include dir (has ruby/config.h)")
set(RUBY_LIB_DIR          "" CACHE PATH "Ruby lib dir (has .lib file)")
set(RUBY_LIB              "" CACHE STRING "Ruby lib filename e.g. x64-msvcrt-ruby320.lib")

if(NOT RUBY_INCLUDE_DIR)
  message(FATAL_ERROR "Set -DRUBY_INCLUDE_DIR to the Ruby include path")
endif()

# -----------------------------------------------------------------------
# Source directories (relative to this CMakeLists.txt)
# -----------------------------------------------------------------------
set(SRC_MAIN    ${CMAKE_CURRENT_SOURCE_DIR}/Source/main)
set(SRC_WIN     ${CMAKE_CURRENT_SOURCE_DIR}/Source/win)
set(NEWTON_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/NewtonDynamics)
set(SDL2_ROOT   ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/SDL2)
set(SDL2MX_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/SDL2_mixer)

# -----------------------------------------------------------------------
# Newton Dynamics — compile as a static library
# Avoids distributing a separate newton.dll
# -----------------------------------------------------------------------
file(GLOB_RECURSE NEWTON_SOURCES
  ${NEWTON_ROOT}/dMath/*.cpp
  ${NEWTON_ROOT}/dgCore/*.cpp
  ${NEWTON_ROOT}/dgPhysics/*.cpp
  ${NEWTON_ROOT}/dgMeshUtil/*.cpp
  ${NEWTON_ROOT}/dgNewton/*.cpp
)

add_library(Newton STATIC ${NEWTON_SOURCES})

target_include_directories(Newton PUBLIC
  ${NEWTON_ROOT}/dMath
  ${NEWTON_ROOT}/dgCore
  ${NEWTON_ROOT}/dgPhysics
  ${NEWTON_ROOT}/dgMeshUtil
  ${NEWTON_ROOT}/dgNewton
)

target_compile_definitions(Newton PRIVATE
  _CRT_SECURE_NO_WARNINGS
  WIN32
  _WIN64
  _WINDOWS
)

# Newton uses lots of old C code — suppress noise warnings
if(MSVC)
  target_compile_options(Newton PRIVATE
    /W0          # silence all warnings in Newton (not our code)
    /MD          # dynamic MSVC runtime (matches SketchUp's Ruby)
    /O2          # optimize
  )
endif()

# -----------------------------------------------------------------------
# msp_lib — the actual Ruby C extension
# -----------------------------------------------------------------------
file(GLOB MSP_SOURCES
  ${SRC_MAIN}/*.cpp
  ${SRC_WIN}/*.cpp
)

# Output is a DLL — we rename it to .so after build
add_library(msp_lib SHARED ${MSP_SOURCES})

# Ruby extensions export a single Init_ function
set_target_properties(msp_lib PROPERTIES
  PREFIX   ""        # no lib prefix
  SUFFIX   ".dll"    # workflow renames to .so
)

target_include_directories(msp_lib PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Source/compat
  ${SRC_MAIN}
  ${SRC_WIN}
  ${RUBY_INCLUDE_DIR}
  ${RUBY_INCLUDE_ARCH_DIR}
  ${NEWTON_ROOT}/dMath
  ${NEWTON_ROOT}/dgCore
  ${NEWTON_ROOT}/dgPhysics
  ${NEWTON_ROOT}/dgMeshUtil
  ${NEWTON_ROOT}/dgNewton
  ${SDL2_ROOT}/include
  ${SDL2MX_ROOT}/include
)

target_compile_definitions(msp_lib PRIVATE
  _CRT_SECURE_NO_WARNINGS
  WIN32
  _WIN64
  _WINDOWS
  _USRDLL
  USE_WINSOCK
)

target_link_directories(msp_lib PRIVATE
  ${RUBY_LIB_DIR}
  ${SDL2_ROOT}/lib/x64
  ${SDL2MX_ROOT}/lib/x64
)

target_link_libraries(msp_lib PRIVATE
  Newton
  ${RUBY_LIB}
  SDL2
  SDL2main
  SDL2_mixer
  Ws2_32
  winmm
)

if(MSVC)
  target_compile_options(msp_lib PRIVATE
    /FI "${CMAKE_CURRENT_SOURCE_DIR}/Source/compat/strings.h"
    /W3          # reasonable warnings for our code
    /MD          # must match Ruby's runtime
    /O2
    /EHsc        # C++ exception handling
    /permissive- # strict conformance mode (VS2022)
  )

  # Export the Ruby Init_ symbol
  set_target_properties(msp_lib PROPERTIES
    LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/Projects/VS/RubyExtension.def\""
  )
endif()
