cmake_minimum_required(VERSION 2.8)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(OFS)

set(OFS_SOURCE
        source/file_ops.cpp
        source/ofs.cpp
	source/ofs13.cpp
	source/ofs14.cpp
	source/ofs_converter.cpp
        source/ofs_rfs.cpp
)

set(OFS_HEADERS
        include/file_ops.h
	include/ofs.h
	include/ofs_base.h
	include/ofs13.h
	include/ofs14.h
	include/ofs_converter.h
        include/ofs_rfs.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
)

find_package(Boost REQUIRED filesystem)
include_directories(${Boost_INCLUDE_DIRS})

ogitor_add_library(OFS SHARED ${OFS_SOURCE} ${OFS_HEADERS})
target_link_libraries(OFS ${Boost_LIBRARIES})

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set_target_properties(OFS PROPERTIES COMPILE_DEFINITIONS "OFS_EXPORT")
install(TARGETS OFS LIBRARY DESTINATION lib)
