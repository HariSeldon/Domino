cmake_minimum_required (VERSION 2.8.9)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

set(PROJECT_NAME Domino)
set(EXE_NAME domino)
set(SRC_DIR "src")
set(BIN_DIR "bin") 
set(INCLUDE_DIR "include")
set(SHADER_DIR "shaders")
set(TEXTURE_DIR "textures")
set(MESH_DIR "meshes")
set(FONT_DIR "fonts")

# Set project name.
project(PROJECT_NAME)

# Full paths.
set(SRC_PATH ${PROJECT_SOURCE_DIR}/${SRC_DIR})
set(INCLUDE_PATH ${PROJECT_SOURCE_DIR}/${INCLUDE_DIR}) 
set(SHADER_PATH ${PROJECT_SOURCE_DIR}/${SHADER_DIR}/)
set(TEXTURE_PATH ${PROJECT_SOURCE_DIR}/${TEXTURE_DIR}/)
set(FONT_PATH ${PROJECT_SOURCE_DIR}/${FONT_DIR}/)
set(MESH_PATH ${PROJECT_SOURCE_DIR}/${MESH_DIR}/)

# Files.
file(GLOB SRC_FILES_LIST "${SRC_PATH}/*.cpp")

# Include files.
# Include directories.
configure_file(${CMAKE_SOURCE_DIR}/${INCLUDE_DIR}/SysDefines.h.cmake 
               ${CMAKE_BINARY_DIR}/${INCLUDE_DIR}/SysDefines.h)
set(BULLET_INCLUDE_DIR "/usr/include/bullet")
set(FREETYPE_INCLUDE_DIR "/usr/include/freetype2")
include_directories(${INCLUDE_PATH} "${CMAKE_BINARY_DIR}/${INCLUDE_DIR}")
# Use system flag so to disable warnings when compiling using -Wall.
include_directories(SYSTEM ${BULLET_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIR})

# Set compilation options.
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions("-g")

# Add sources to executable.
add_executable(${EXE_NAME} ${SRC_FILES_LIST})

# Add install command.
install_targets("/${BIN_DIR}/" ${EXE_NAME})

# ------------------------------------------------------------------------------
# GL
find_library(GL_LIB_PATH GL
             DOC "The GL library")
if(EXISTS ${GL_LIB_PATH})
  message(STATUS "Looking for GL: found")
else(EXISTS ${GL_LIB_PATH})
  message(FATAL_ERROR "Looking for GL: not found")
endif(EXISTS ${GL_LIB_PATH})

find_library(GLU_LIB_PATH GLU
             DOC "The GLU library")
if(EXISTS ${GLU_LIB_PATH})
  message(STATUS "Looking for GLU: found")
else(EXISTS ${GLU_LIB_PATH})
  message(FATAL_ERROR "Looking for GLU: not found")
endif(EXISTS ${GLU_LIB_PATH})

find_library(GLEW_LIB_PATH GLEW
             DOC "The GLEW library")
if(EXISTS ${GLEW_LIB_PATH})
  message(STATUS "Looking for GLEW: found")
else(EXISTS ${GLEW_LIB_PATH})
  message(FATAL_ERROR "Looking for GLEW: not found")
endif(EXISTS ${GLEW_LIB_PATH})

find_library(SDL2_LIB_PATH SDL2
             DOC "The SDL2 library")
if(EXISTS ${SDL2_LIB_PATH})
  message(STATUS "Looking for SDL2: found")
else(EXISTS ${SDL2_LIB_PATH})
  message(FATAL_ERROR "Looking for SDL2: not found")
endif(EXISTS ${SDL2_LIB_PATH})


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

target_link_libraries(${EXE_NAME} ${SDL2_LIB_PATH} ${SDL2_IMAGE_LIB_PATH} ${GLEW_LIB_PATH} ${GLU_LIB_PATH} ${GL_LIB_PATH} ${FREETYPE_LIB_PATH})

# BulletCollision
find_library(BULLET_COLLISION_LIB_PATH BulletCollision
             DOC "The BulletCollision library")
if(EXISTS ${BULLET_COLLISION_LIB_PATH})
  message(STATUS "Looking for BulletCollision: found")
else(EXISTS ${BULLET_COLLISION_LIB_PATH})
  message(FATAL_ERROR "Looking for BulletCollision: not found")
endif(EXISTS ${BULLET_COLLISION_LIB_PATH})

# BulletDynamics
find_library(BULLET_DYNAMICS_LIB_PATH BulletDynamics
             DOC "The BulleetDynamics library")
if(EXISTS ${BULLET_DYNAMICS_LIB_PATH})
  message(STATUS "Looking for BulletDynamics: found")
else(EXISTS ${BULLET_DYNAMICS_LIB_PATH})
  message(FATAL_ERROR "Looking for BulletDynamics: not found")
endif(EXISTS ${BULLET_DYNAMICS_LIB_PATH})

# LinearMath
find_library(LINEAR_MATH_LIB_PATH LinearMath
             DOC "The LinearMath library")
if(EXISTS ${LINEAR_MATH_LIB_PATH})
  message(STATUS "Looking for LinearMath: found")
else(EXISTS ${LINEAR_MATH_LIB_PATH})
  message(FATAL_ERROR "Looking for LinearMath: not found")
endif(EXISTS ${LINEAR_MATH_LIB_PATH})

target_link_libraries(${EXE_NAME} ${BULLET_DYNAMICS_LIB_PATH} ${BULLET_COLLISION_LIB_PATH} ${LINEAR_MATH_LIB_PATH})
