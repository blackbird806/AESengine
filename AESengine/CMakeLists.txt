## This file is a quick tutorial on writing CMakeLists for targeting the Vita
cmake_minimum_required(VERSION 2.8)

## This includes the Vita toolchain, must go before project definition
# It is a convenience so you do not have to type
# -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake for cmake. It is
# highly recommended that you include this block for all projects.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

## Define project parameters here
# Name of the project
project(aes)
# This line adds Vita helper macros, must go after project definition in order
# to build Vita specific artifacts (self/vpk).
include("${VITASDK}/share/vita.cmake" REQUIRED)

## Configuration options for this app
# Display name (under bubble in LiveArea)
set(VITA_APP_NAME "AES app")
# Unique ID must be exactly 9 characters. Recommended: XXXXYYYYY where X =
# unique string of developer and Y = a unique number for this app
set(VITA_TITLEID  "BIRD00002")
# Optional version string to show in LiveArea's more info screen
set(VITA_VERSION  "00.01")

## Flags and includes for building
# Note that we make sure not to overwrite previous flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g -mfpu=neon")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
# Optional. You can specify more param.sfo flags this way.
set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")

add_definitions(-DAES_DEBUG )
add_definitions(-DGLM_FORCE_CTOR_INIT)

# Add any additional include paths here
include_directories(
  src/
  thirdParty/
  thirdParty/fmt/include
  thirdParty/glm
)

# Add any additional library paths here
# ${CMAKE_CURRENT_BINARY_DIR} lets you use any library currently being built
link_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

## Build and link
# Add all the files needed to compile here
file(

  GLOB

  source_files 
  
  src/engine.cpp
  src/main.cpp
  src/core/*.cpp
  src/test/*.cpp
  src/spatial/*.cpp
  src/UI/*.cpp
  src/renderer/camera.cpp
  src/renderer/model.cpp
  src/renderer/draw2d.cpp
  src/renderer/material.cpp
  src/renderer/textureUtility.cpp
  src/renderer/fontRenderer.cpp
  src/renderer/RHI/*.cpp
  src/renderer/RHI/SceGxm/*.cpp

  thirdParty/fmt/src/*
  thirdParty/stb/*
  thirdParty/psvDebugScreen/debugScreen.c
)

add_executable(${PROJECT_NAME}
  ${source_files}
)

# Library to link to (drop the -l prefix). This will mostly be stubs.
target_link_libraries(${PROJECT_NAME}
  stdc++
  pthread
  SceDisplay_stub
  SceGxm_stub
  SceNet_stub
  SceCtrl_stub
  SceRazorCapture_stub 
  SceRazorHud_stub 
  SceSysmodule_stub 
)

## Create Vita files
vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})
# The FILE directive lets you add additional files to the VPK, the syntax is
# FILE src_path dst_path_in_vpk. In this case, we add the LiveArea paths.
vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE sce_sys/icon0.png sce_sys/icon0.png
  FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
  FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
  FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
  FILE assets/shaders/vita/bin/clear_fs.gxp assets/shaders/vita/clear_fs.gxp
  FILE assets/shaders/vita/bin/clear_vs.gxp assets/shaders/vita/clear_vs.gxp
  FILE assets/shaders/vita/bin/basic2d_vs.gxp assets/shaders/vita/basic2d_vs.gxp
  FILE assets/shaders/vita/bin/basic2d_fs.gxp assets/shaders/vita/basic2d_fs.gxp
  FILE assets/shaders/vita/bin/basic3d_vs.gxp assets/shaders/vita/basic3d_vs.gxp
  FILE assets/shaders/vita/bin/basic3d_fs.gxp assets/shaders/vita/basic3d_fs.gxp
)
