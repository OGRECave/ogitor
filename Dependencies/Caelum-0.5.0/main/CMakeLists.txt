cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(Caelum)

set(HEADER_FILES
	include/Astronomy.h
	include/Caelum.h
	include/CaelumExceptions.h
	include/CaelumPlugin.h
	include/CaelumSystem.h
	include/CameraBoundElement.h
	include/CaelumPrecompiled.h
	include/CaelumPrerequisites.h
	include/CaelumScriptTranslator.h
	include/CloudSystem.h
	include/DepthComposer.h
	include/FastGpuParamRef.h
	include/FlatCloudLayer.h
	include/GeometryFactory.h
	include/GroundFog.h
	include/ImageHelper.h
	include/ImageStarfield.h
	include/InternalUtilities.h
	include/Moon.h
	include/OwnedPtr.h
	include/PointStarfield.h
	include/PrecipitationController.h
	include/SkyDome.h
	include/SkyLight.h
	include/Sun.h
	include/TypeDescriptor.h
	include/UniversalClock.h
)

set(SOURCE_FILES
	src/Astronomy.cpp
	src/BrightStarCatalogue.cpp
	src/CaelumDefaultTypeDescriptorData.cpp
	src/CaelumPlugin.cpp
	src/CaelumPrecompiled.cpp
	src/CaelumScriptTranslator.cpp
	src/CaelumSystem.cpp
	src/CameraBoundElement.cpp
	src/CloudSystem.cpp
	src/DepthComposer.cpp
	src/FlatCloudLayer.cpp
	src/FastGpuParamRef.cpp
	src/GeometryFactory.cpp
	src/GroundFog.cpp
	src/ImageHelper.cpp
	src/InternalUtilities.cpp
	src/ImageStarfield.cpp
	src/Moon.cpp
	src/PointStarfield.cpp
	src/PrecipitationController.cpp
	src/SkyDome.cpp
	src/SkyLight.cpp
	src/Sun.cpp
	src/TypeDescriptor.cpp
	src/UniversalClock.cpp
)

set(Caelum_VERSION 0)
set(Caelum_VERSION_MAJOR 1)

include_directories(include)
# Terrain Scene Manager
#include_directories ( ${OGITOR_SM_INCLUDES} )
link_libraries(Caelum  ${OGRE_LIBRARIES} ${Boost_LIBRARIES})

add_library(Caelum SHARED ${HEADER_FILES} ${SOURCE_FILES})
set(LIBRARY_OUTPUT_PATH  ../../lib)

set_target_properties(Caelum PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
set_target_properties(Caelum PROPERTIES VERSION ${Caelum_VERSION} SOVERSION ${Caelum_VERSION_MAJOR})
set_target_properties(Caelum PROPERTIES SOLUTION_FOLDER Dependencies)
set_target_properties(Caelum PROPERTIES COMPILE_DEFINITIONS CAELUM_LIB)

if(WIN32)
	add_custom_command(TARGET Caelum POST_BUILD
		COMMAND copy \"$(TargetPath)\" ..\\..\\..\\RunPath\\bin)

	install(TARGETS Caelum
		RUNTIME DESTINATION bin
		CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
endif(WIN32)

IF(UNIX AND NOT APPLE)

set_target_properties( Caelum PROPERTIES OUTPUT_NAME "Caelum_Ogitor" VERSION
${OGITOR_VERSION_STRING} SOVERSION ${abi_version} )

install(TARGETS Caelum
    LIBRARY DESTINATION ${OGRE_PLUGIN_DIR_LINUX}
    CONFIGURATIONS Release RelWithDebInfo MinSizeRel)

install(TARGETS Caelum
    LIBRARY DESTINATION lib
    CONFIGURATIONS Release RelWithDebInfo MinSizeRel)

ENDIF(UNIX AND NOT APPLE)

# vim: set sw=2 ts=2 noet:
