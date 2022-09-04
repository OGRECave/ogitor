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

ogitor_add_library(OFS SHARED ${OFS_SOURCE} ${OFS_HEADERS})
target_compile_definitions(OFS PRIVATE OFS_EXPORT)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

install(TARGETS OFS 
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)
