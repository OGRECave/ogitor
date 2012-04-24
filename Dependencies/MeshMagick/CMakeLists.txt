project(meshmagick)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0003 NEW)
cmake_policy(SET CMP0005 OLD)

include(FeatureSummary)

set(MESHMAGICK_MAJOR_VERSION 0)
set(MESHMAGICK_MINOR_VERSION 5)
set(MESHMAGICK_PATCH_VERSION 2)

# include our custom cmake scripts
#set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# dependencies
#find_package(PkgConfig)
#find_package(OGRE 1.6 REQUIRED)

set(MESHMAGICK_SOURCE
	src/MeshMagick.cpp
	src/MmEditableBone.cpp
	src/MmEditableMesh.cpp
	src/MmEditableSkeleton.cpp
	src/MmInfoTool.cpp
	src/MmInfoToolFactory.cpp
	src/MmMeshMergeTool.cpp
	src/MmMeshMergeToolFactory.cpp
	src/MmMeshUtils.cpp
	src/MmOgreEnvironment.cpp
	src/MmOptimiseTool.cpp
	src/MmOptimiseToolFactory.cpp
	src/MmOptionsParser.cpp
	src/MmRenameTool.cpp
	src/MmRenameToolFactory.cpp
	src/MmStatefulMeshSerializer.cpp
	src/MmStatefulSkeletonSerializer.cpp
	src/MmTool.cpp
	src/MmToolManager.cpp
	src/MmToolsUtils.cpp
	src/MmTransformTool.cpp
	src/MmTransformToolFactory.cpp
)

set(MESHMAGICK_HEADERS
	include/MeshMagick.h
	include/MeshMagickPrerequisites.h
	include/MmEditableBone.h
	include/MmEditableMesh.h
	include/MmEditableSkeleton.h
	include/MmInfoToolFactory.h
	include/MmInfoTool.h
	include/MmMeshMergeToolFactory.h
	include/MmMeshMergeTool.h
	include/MmMeshUtils.h
	include/MmOgreEnvironment.h
	include/MmOptimiseToolFactory.h
	include/MmOptimiseTool.h
	include/MmOptionsParser.h
	include/MmRenameToolFactory.h
	include/MmRenameTool.h
	include/MmStatefulMeshSerializer.h
	include/MmStatefulSkeletonSerializer.h
	include/MmToolFactory.h
	include/MmTool.h
	include/MmToolManager.h
	include/MmToolUtils.h
	include/MmTransformToolFactory.h
	include/MmTransformTool.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${OGRE_INCLUDE_DIRS})

ogitor_add_library(meshmagick_shared_lib SHARED ${MESHMAGICK_SOURCE} ${MESHMAGICK_HEADERS})
set(LIBRARY_OUTPUT_PATH  ../lib)
set_target_properties(meshmagick_shared_lib PROPERTIES
	OUTPUT_NAME meshmagick
	VERSION ${MESHMAGICK_MAJOR_VERSION}.${MESHMAGICK_MINOR_VERSION}.${MESHMAGICK_PATCH_VERSION}
	SOVERSION ${MESHMAGICK_MAJOR_VERSION}.${MESHMAGICK_MINOR_VERSION}
	DEFINE_SYMBOL MESHMAGICK_EXPORTS)
target_link_libraries(meshmagick_shared_lib ${OGRE_LIBRARIES})

if(NOT APPLE)
	add_executable(meshmagick_bin src/main.cpp)
else()
	set(COPY_FRAMEWORKS ${OGRE_LIBRARIES})

	add_executable(meshmagick_bin src/main.cpp ${COPY_FRAMEWORKS})

	set_source_files_properties(${COPY_FRAMEWORKS} PROPERTIES MACOSX_PACKAGE_LOCATION Frameworks)
endif(NOT APPLE)

set_target_properties(meshmagick_bin PROPERTIES
	OUTPUT_NAME meshmagick
	DEFINE_SYMBOL MESHMAGICK_IMPORTS)
target_link_libraries(meshmagick_bin meshmagick_shared_lib ${OGRE_LIBRARIES})

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/meshmagick.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/meshmagick.pc)

if(WIN32)
	add_custom_command(TARGET meshmagick_shared_lib PRE_BUILD
		COMMAND if not exist ..\\..\\RunPath\\bin mkdir ..\\..\\RunPath\\bin)
	add_custom_command(TARGET meshmagick_shared_lib POST_BUILD
		COMMAND copy \"$(TargetPath)\" ..\\..\\RunPath\\bin)

	install(TARGETS meshmagick_bin meshmagick_shared_lib
		RUNTIME DESTINATION bin
		CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
endif(WIN32)

#install(TARGETS meshmagick_bin meshmagick_shared_lib
#	RUNTIME DESTINATION bin
#	LIBRARY DESTINATION lib)

#if(NOT WIN32)
#	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/meshmagick.pc
#					DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
#endif(NOT WIN32)

#install(FILES
#include/MeshMagick.h
#include/MeshMagickPrerequisites.h
#include/MmEditableBone.h
#include/MmEditableMesh.h
#include/MmEditableSkeleton.h
#include/MmInfoToolFactory.h
#include/MmInfoTool.h
#include/MmMeshMergeToolFactory.h
#include/MmMeshMergeTool.h
#include/MmMeshUtils.h
#include/MmOgreEnvironment.h
#include/MmOptimiseToolFactory.h
#include/MmOptimiseTool.h
#include/MmOptionsParser.h
#include/MmRenameToolFactory.h
#include/MmRenameTool.h
#include/MmStatefulMeshSerializer.h
#include/MmStatefulSkeletonSerializer.h
#include/MmToolFactory.h
#include/MmTool.h
#include/MmToolManager.h
#include/MmToolUtils.h
#include/MmTransformToolFactory.h
#include/MmTransformTool.h
#DESTINATION ${CMAKE_INSTALL_PREFIX}/include/meshmagick)

#include(CPack)

# vim: set sw=2 ts=2 noet:
