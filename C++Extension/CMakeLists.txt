cmake_minimum_required(VERSION 3.20)
project(msp_lib VERSION 2.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------
# Ruby paths — passed in from the workflow
# -----------------------------------------------------------------------
set(RUBY_INCLUDE_DIR      "" CACHE PATH   "Ruby main include dir")
set(RUBY_INCLUDE_ARCH_DIR "" CACHE PATH   "Ruby arch include dir")
set(RUBY_LIB_DIR          "" CACHE PATH   "Ruby lib dir")
set(RUBY_LIB              "" CACHE STRING "Ruby lib filename")

if(NOT RUBY_INCLUDE_DIR)
  message(FATAL_ERROR "Set -DRUBY_INCLUDE_DIR")
endif()

# -----------------------------------------------------------------------
# Source directories
# -----------------------------------------------------------------------
set(SRC_MAIN    ${CMAKE_CURRENT_SOURCE_DIR}/Source/main)
set(SRC_WIN     ${CMAKE_CURRENT_SOURCE_DIR}/Source/win)
set(NEWTON_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/NewtonDynamics)
set(SDL2_ROOT   ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/SDL2)
set(SDL2MX_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/SDL2_mixer)

# -----------------------------------------------------------------------
# Newton Dynamics — static library
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

target_compile_definitions(Newton PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_options(Newton PRIVATE -w -O2)

# -----------------------------------------------------------------------
# msp_lib — Ruby C extension (compiled with MinGW g++)
# -----------------------------------------------------------------------
file(GLOB MSP_SOURCES
  ${SRC_MAIN}/*.cpp
  ${SRC_WIN}/*.cpp
)

add_library(msp_lib SHARED ${MSP_SOURCES})

set_target_properties(msp_lib PROPERTIES
  PREFIX ""
  SUFFIX ".dll"
)

target_include_directories(msp_lib PRIVATE
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
  WIN32 _WIN64 _WINDOWS _USRDLL
)

target_compile_options(msp_lib PRIVATE
  -O2
  -std=c++17
  -Wall
  -Wno-unknown-pragmas
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
  ws2_32
  winmm
)
