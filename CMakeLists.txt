cmake_minimum_required (VERSION 2.8.9)
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(PROJECT_NAME Domino)
set(EXE_NAME domino)
set(SRC_DIR "src")
set(BIN_DIR "bin") 
set(INCLUDE_DIR "include")
set(SHADER_DIR "shaders")
set(TEXTURE_DIR "textures")
set(MESH_DIR "meshes")
set(FONT_DIR "fonts")
set(EXT_DIR "ext")
set(SCRIPT_DIR "scripts")
set(GLM_DIR "glm")
set(BULLET_DIR "bullet")
set(LUA_DIR "lua")
set(LUA_LIB "Lua")
set(DOMINO_SETUP_LUA "domino_setup.lua")

# Set project name.
project(PROJECT_NAME)

# Full paths.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)
set(SRC_PATH ${PROJECT_SOURCE_DIR}/${SRC_DIR})
set(INCLUDE_PATH ${PROJECT_SOURCE_DIR}/${INCLUDE_DIR}) 
set(SHADER_PATH ${PROJECT_SOURCE_DIR}/${SHADER_DIR}/)
set(TEXTURE_PATH ${PROJECT_SOURCE_DIR}/${TEXTURE_DIR}/)
set(FONT_PATH ${PROJECT_SOURCE_DIR}/${FONT_DIR}/)
set(MESH_PATH ${PROJECT_SOURCE_DIR}/${MESH_DIR}/)
set(EXT_PATH ${PROJECT_SOURCE_DIR}/${EXT_DIR}/)
set(SCRIPT_PATH ${PROJECT_SOURCE_DIR}/${SCRIPT_DIR}/)
set(BULLET_PATH ${EXT_PATH}/${BULLET_DIR}/)
set(LUA_PATH ${EXT_PATH}/${LUA_DIR}/)

# Files.
file(GLOB SRC_FILES_LIST "${SRC_PATH}/*.cpp")

# Requird packages.
find_package(OpenGL)
find_package(GLEW)
find_package(SDL2)

# Include files.
# Include directories.
configure_file(${CMAKE_SOURCE_DIR}/${INCLUDE_DIR}/SysDefines.h.cmake 
               ${CMAKE_BINARY_DIR}/${INCLUDE_DIR}/SysDefines.h)
set(BULLET_INCLUDE_DIR "${BULLET_PATH}/src")
set(LUA_INCLUDE_DIR "${LUA_PATH}/src")
set(FREETYPE_INCLUDE_DIR "/usr/include/freetype2")
include_directories(${INCLUDE_PATH} "${CMAKE_BINARY_DIR}/${INCLUDE_DIR}")
# Use system flag so to disable warnings when compiling using -Wall.
include_directories(SYSTEM ${EXT_PATH} ${BULLET_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIR} ${LUA_INCLUDE_DIR})

if( "${CMAKE_BUILD_TYPE}" STREQUAL "" )
  set(CMAKE_BUILD_TYPE "Debug")
endif("${CMAKE_BUILD_TYPE}" STREQUAL "")

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  add_definitions("-DDEBUG")  
endif("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

message(STATUS "CMake build type: " ${CMAKE_BUILD_TYPE})

# Set compilation options.
if (UNIX)
  add_definitions("-DUNIX")

  if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions("-g")  
  endif("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -finline-functions")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -std=c++14 -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -pedantic")
elseif (WIN32)
  add_definitions("-DWINDOWS")
else(UNIX)
  message(FATAL "Unsupported OS")
endif(UNIX)

# This is to remove GLM warning.
add_definitions("-DGLM_FORCE_RADIANS")

# Add sources to executable.
add_executable(${EXE_NAME} ${SRC_FILES_LIST})

# ------------------------------------------------------------------------------
find_library(SDL2_IMAGE_LIB_PATH SDL2_image
             DOC "The SDL2 Image library")
if(EXISTS ${SDL2_IMAGE_LIB_PATH})
  message(STATUS "Looking for SDL2 Image: found")
else(EXISTS ${SDL2_IMAGE_LIB_PATH})
  message(FATAL_ERROR "Looking for SDL2 Image: not found")
endif(EXISTS ${SDL2_IMAGE_LIB_PATH})

find_library(FREETYPE_LIB_PATH freetype
             DOC "The freetype library")
if(EXISTS ${FREETYPE_LIB_PATH})
  message(STATUS "Looking for freetype: found")
else(EXISTS ${FREETYPE_LIB_PATH})
  message(FATAL_ERROR "Looking for freetype: not found")
endif(EXISTS ${FREETYPE_LIB_PATH})

target_link_libraries(${EXE_NAME} ${SDL2_LIBRARY}
                                  ${GLEW_LIBRARY} 
                                  ${OPENGL_LIBRARIES}
                                  ${SDL2_IMAGE_LIB_PATH} 
                                  ${FREETYPE_LIB_PATH}) 

add_subdirectory(${BULLET_PATH})

set_property(TARGET BulletDynamics APPEND_STRING PROPERTY COMPILE_FLAGS " -w")
set_property(TARGET BulletCollision APPEND_STRING PROPERTY COMPILE_FLAGS " -w")
set_property(TARGET LinearMath APPEND_STRING PROPERTY COMPILE_FLAGS " -w")

# Warning! Preserve this ordering for these static libraries.
# Dynamics depends on Collision and they both depend on LinearMath.
target_link_libraries(${EXE_NAME} BulletDynamics BulletCollision LinearMath)

add_subdirectory(${LUA_PATH})
target_link_libraries(${EXE_NAME} ${LUA_LIB}
                                  ${DL_LIB_PATH}
                                  ${READ_LINE_LIB_PATH})

# ------------------------------------------------------------------------------
# Copy lua config file.
add_custom_command(TARGET ${EXE_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy ${SCRIPT_PATH}/${DOMINO_SETUP_LUA} 
                   ${CMAKE_BINARY_DIR}/${DOMINO_SETUP_LUA})
