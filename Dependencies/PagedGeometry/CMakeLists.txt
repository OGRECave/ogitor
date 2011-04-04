cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(PagedGeometry)

set(HEADER_FILES
	include/BatchedGeometry.h
	include/BatchPage.h
	include/GrassLoader.h
	include/ImpostorPage.h
	include/MersenneTwister.h
	include/PagedGeometry.h
	include/PagedGeometryConfig.h
	include/PropertyMaps.h
	include/RandomTable.h
	include/StaticBillboardSet.h
	include/TreeLoader2D.h
	include/TreeLoader3D.h
	include/WindBatchedGeometry.h
	include/WindBatchPage.h
)

set(SOURCE_FILES
	source/BatchedGeometry.cpp
	source/BatchPage.cpp
	source/GrassLoader.cpp 
	source/ImpostorPage.cpp
	source/PagedGeometry.cpp
	source/PropertyMaps.cpp
	source/StaticBillboardSet.cpp
	source/TreeLoader2D.cpp
	source/TreeLoader3D.cpp
	source/WindBatchedGeometry.cpp
	source/WindBatchPage.cpp
)

set(PG_VERSION 0)
set(PG_VERSION_MAJOR 1)

include_directories(include)

if(UNIX)
	set(CMAKE_CXX_FLAGS -fPIC)
endif(UNIX)

add_library(PagedGeometry ${HEADER_FILES} ${SOURCE_FILES}  )
set(LIBRARY_OUTPUT_PATH  ../lib)

set_target_properties(PagedGeometry PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
set_target_properties(PagedGeometry PROPERTIES VERSION ${PG_VERSION} SOVERSION ${PG_VERSION_MAJOR})
set_target_properties(PagedGeometry PROPERTIES COMPILE_DEFINITIONS PG_LIB)
set_target_properties(PagedGeometry PROPERTIES SOLUTION_FOLDER Dependencies) 

target_link_libraries(PagedGeometry ${OGRE_LIBRARIES})

#IF(UNIX AND NOT APPLE)
#install(TARGETS Hydrax
#		 LIBRARY DESTINATION Plugins/dependencies
#		 CONFIGURATIONS Release)
#install(TARGETS Hydrax
#		 LIBRARY DESTINATION Plugins/dependencies
#		 CONFIGURATIONS Debug)
#ENDIF(UNIX AND NOT APPLE)

# vim: set sw=2 ts=2 noet:
