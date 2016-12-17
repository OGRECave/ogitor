cmake_minimum_required(VERSION 2.8.4)

project(Dependencies)

SET(CMAKE_DEBUG_POSTFIX "_d")
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/lib")

# External Projects
# include(ExternalProject)

# Caelum
#ExternalProject_Add(Caelum
#  PREFIX ${CMAKE_CURRENT_BINARY_DIR}
#  HG_REPOSITORY https://bitbucket.org/ogreaddons/caelum
#  CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/RunPath -DCMAKE_DEBUG_POSTFIX=${CMAKE_DEBUG_POSTFIX}
#  BUILD_IN_SOURCE 1
#)
#set_target_properties(Caelum PROPERTIES FOLDER Dependencies)

# Hydrax
#ExternalProject_Add(Hydrax
#  PREFIX ${CMAKE_CURRENT_BINARY_DIR}
#  HG_REPOSITORY https://bitbucket.org/ogreaddons/hydrax
#  CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/RunPath -DCMAKE_DEBUG_POSTFIX=${CMAKE_DEBUG_POSTFIX}
#  BUILD_IN_SOURCE 1
#)
#set_target_properties(Hydrax PROPERTIES FOLDER Dependencies)

# Pagedgeometry
add_subdirectory(pagedgeometry)

# Skyx
#ExternalProject_Add(Skyx
#  PREFIX ${CMAKE_CURRENT_BINARY_DIR}
#  HG_REPOSITORY https://bitbucket.org/ogreaddons/skyx
#  CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/RunPath -DCMAKE_DEBUG_POSTFIX=${CMAKE_DEBUG_POSTFIX}
#  BUILD_IN_SOURCE 1
#)
#set_target_properties(Skyx PROPERTIES FOLDER Dependencies)
