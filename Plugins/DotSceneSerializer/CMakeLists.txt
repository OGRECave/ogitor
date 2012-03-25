cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(DotSceneSerializer)

set(HEADER_FILES
	DotSceneSerializer.h
)

set(SOURCE_FILES
	DotSceneSerializer.cpp
	DotSceneSerializerExport.cpp
)

set(DotSceneSerializer_VERSION 0)
set(DotSceneSerializer_VERSION_MAJOR 1)

include_directories(include)
# Terrain Scene Manager
#include_directories (${OGITOR_SM_INCLUDES})
include_directories(${DEPENDENCIES_INCLUDES})
include_directories(${OGITOR_INCLUDES})
message(STATUS "${OGITOR_INCLUDES}")

include_directories(.)
link_directories(${OGITOR_LIBPATH})
link_directories(${DEPENDENCIES_LIBPATH})

link_libraries(${OGRE_LIBRARIES} Ogitor Hydrax)
add_library(DotSceneSerializer SHARED ${HEADER_FILES} ${SOURCE_FILES})
#target_link_libraries(OgreMain Ogitor)
set(LIBRARY_OUTPUT_PATH ../lib)

#set_target_properties(DotSceneSerializer PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
set_target_properties(DotSceneSerializer PROPERTIES VERSION ${DotSceneSerializer_VERSION} SOVERSION ${DotSceneSerializer_VERSION_MAJOR})
set_target_properties(DotSceneSerializer PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)
set_target_properties(DotSceneSerializer PROPERTIES SOLUTION_FOLDER Plugins)

if(UNIX AND NOT APPLE)
	add_custom_command(TARGET DotSceneSerializer PRE_BUILD
		COMMAND mkdir -p ../../RunPath/Plugins)
	add_custom_command(TARGET DotSceneSerializer POST_BUILD
		COMMAND cp -r ../lib/libDotSceneSerializer.* ../../RunPath/Plugins/)
endif(UNIX AND NOT APPLE)

if(WIN32)
	add_custom_command(TARGET DotSceneSerializer PRE_BUILD
		COMMAND if not exist ..\\..\\RunPath\\Plugins mkdir ..\\..\\RunPath\\Plugins)
	add_custom_command(TARGET DotSceneSerializer POST_BUILD
		COMMAND copy \"$(TargetPath)\" ..\\..\\RunPath\\Plugins)
	add_custom_command(TARGET DotSceneSerializer POST_BUILD
		COMMAND if exist ..\\..\\RunPath\\Plugins\\DotSceneSerializer_d.dll move ..\\..\\RunPath\\Plugins\\DotSceneSerializer_d.dll ..\\..\\RunPath\\Plugins\\debug)

	install(TARGETS DotSceneSerializer
		RUNTIME DESTINATION Plugins
		CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
endif(WIN32)

IF(UNIX AND NOT APPLE)
    if(OGITOR_DIST)
        install(TARGETS DotSceneSerializer
            LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/share/qtOgitor/Plugins
            CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    endif(OGITOR_DIST)
ENDIF(UNIX AND NOT APPLE)

# vim: set sw=2 ts=2 noet:
