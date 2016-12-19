cmake_minimum_required(VERSION 2.8)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(OgreTerrainConverter)

set(OgreTerrainConverter_SOURCE
        source/OgreTerrainConverter.cpp
)

set(OgreTerrainConverter_HEADERS
        include/OgreTerrainConverter.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
)

ogitor_add_library(OgreTerrainConverter SHARED ${OgreTerrainConverter_SOURCE} ${OgreTerrainConverter_HEADERS})

target_link_libraries(OgreTerrainConverter ${OGRE_LIBRARIES})

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set_target_properties(OgreTerrainConverter PROPERTIES COMPILE_DEFINITIONS "OgreTerrainConverter_EXPORT")

install(TARGETS OgreTerrainConverter LIBRARY DESTINATION lib)
