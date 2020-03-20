cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(OgHydrax)

set(HEADER_FILES
	HydraxWaterEditor.h
)

set(SOURCE_FILES
	HydraxWaterEditor.cpp
)

set(OgHydrax_VERSION 0)
set(OgHydrax_VERSION_MAJOR 1)

include_directories(include)
# Terrain Scene Manager
#include_directories(${OGITOR_SM_INCLUDES})
include_directories(${DEPENDENCIES_INCLUDES})
include_directories(${OGITOR_INCLUDES})
message(STATUS "${OGITOR_INCLUDES}")

include_directories(.)
link_directories(${OGITOR_LIBPATH})
link_directories(${DEPENDENCIES_LIBPATH})

link_libraries(${OGRE_LIBRARIES} Ogitor)
ogitor_add_library(OgHydrax SHARED ${HEADER_FILES} ${SOURCE_FILES})
if(WIN32)
    target_link_libraries(OgHydrax debug ${DEPENDENCIES_LIBPATH}/Hydrax_d.lib optimized ${DEPENDENCIES_LIBPATH}/Hydrax.lib)
else()
    target_link_libraries(OgHydrax debug ${DEPENDENCIES_LIBPATH}/Hydrax_d.so optimized ${DEPENDENCIES_LIBPATH}/Hydrax.so)
endif()
#target_link_libraries(OgreMain Ogitor)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

if(NOT OGITOR_DIST)
    set_target_properties(OgHydrax PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
endif(NOT OGITOR_DIST)

set_target_properties(OgHydrax PROPERTIES VERSION ${OgHydrax_VERSION} SOVERSION ${OgHydrax_VERSION_MAJOR})
set_target_properties(OgHydrax PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)
set_target_properties(OgHydrax PROPERTIES SOLUTION_FOLDER Plugins)

install(TARGETS OgHydrax
        LIBRARY DESTINATION ${OGITOR_PLUGIN_INSTALL_PATH}
        ARCHIVE DESTINATION ${OGITOR_PLUGIN_INSTALL_PATH}
        RUNTIME DESTINATION ${OGITOR_PLUGIN_INSTALL_PATH})

# vim: set sw=2 ts=2 noet:
