cmake_minimum_required(VERSION 2.6)

project(Skyx)

set(HEADER_FILES
	AtmosphereManager.h
	BasicController.h
	CloudsManager.h
	ColorGradient.h
	Controller.h
	GPUManager.h
	MeshManager.h
	MoonManager.h
	Prerequisites.h
	SkyX.h
	VCloudsManager.h
	VClouds/DataManager.h
	VClouds/Ellipsoid.h
	VClouds/FastFakeRandom.h
	VClouds/GeometryBlock.h
	VClouds/GeometryManager.h
	VClouds/Lightning.h
	VClouds/LightningManager.h
	VClouds/VClouds.h
)

set(SOURCE_FILES
	AtmosphereManager.cpp
	BasicController.cpp
	CloudsManager.cpp
	ColorGradient.cpp
	GPUManager.cpp
	MeshManager.cpp
	MoonManager.cpp
	Prerequisites.cpp
	SkyX.cpp
	VCloudsManager.cpp
	VClouds/DataManager.cpp
	VClouds/Ellipsoid.cpp
	VClouds/FastFakeRandom.cpp
	VClouds/GeometryBlock.cpp
	VClouds/GeometryManager.cpp
	VClouds/Lightning.cpp
	VClouds/LightningManager.cpp
	VClouds/VClouds.cpp
)

set(SKYX_VERSION_MAJOR "0")
set(SKYX_VERSION_MINOR "3")
set(SKYX_VERSION_PATCH "0")
set(SKYX_VERSION "${SKYX_VERSION_MAJOR}.${SKYX_VERSION_MINOR}.${SKYX_VERSION_PATCH}")

ogitor_add_library(Skyx SHARED ${HEADER_FILES} ${SOURCE_FILES})
set(LIBRARY_OUTPUT_PATH  ../lib)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/VClouds")

set_target_properties(Skyx PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
set_target_properties(Skyx PROPERTIES VERSION ${SKYX_VERSION} SOVERSION ${SKYX_VERSION_MAJOR})
set_target_properties(Skyx PROPERTIES COMPILE_DEFINITIONS SKYX_LIB)
set_target_properties(Skyx PROPERTIES SOLUTION_FOLDER Dependencies)

target_link_libraries(Skyx ${OGRE_LIBRARIES} ${Boost_LIBRARIES})

# specify a precompiled header to use
use_precompiled_header(Skyx
  "${CMAKE_CURRENT_SOURCE_DIR}/Prerequisites.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/Prerequisites.cpp"
)

if(WIN32)
	add_custom_command(TARGET Skyx PRE_BUILD
		COMMAND if not exist ..\\..\\RunPath\\bin mkdir ..\\..\\RunPath\\bin)
	add_custom_command(TARGET Skyx POST_BUILD
		COMMAND copy \"$(TargetPath)\" ..\\..\\RunPath\\bin)

	install(TARGETS Skyx
		RUNTIME DESTINATION bin
		CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
endif(WIN32)

IF(UNIX AND NOT APPLE)

    set_target_properties( Skyx PROPERTIES OUTPUT_NAME "Skyx_Ogitor" VERSION
    ${OGITOR_VERSION_STRING} SOVERSION ${abi_version} )

    if(OGITOR_DIST)
        install(TARGETS Skyx
            LIBRARY DESTINATION ${OGRE_PLUGIN_DIR_LINUX}
            CONFIGURATIONS Release RelWithDebInfo MinSizeRel)

        install(TARGETS Skyx
            LIBRARY DESTINATION lib
            CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    endif(OGITOR_DIST)

ENDIF(UNIX AND NOT APPLE)

IF (APPLE)
        set_target_properties(Skyx PROPERTIES OUTPUT_NAME Skyx)
        # Set the INSTALL_PATH so that Frameworks can be local
        set_target_properties(Skyx PROPERTIES BUILD_WITH_INSTALL_RPATH 1 INSTALL_NAME_DIR "@executable_path/../Plugins/Ogitor")
   ENDIF(APPLE)

# vim: set sw=2 ts=2 noet:
