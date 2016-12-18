project(OgreOfsPlugin)

include_directories("${CMAKE_SOURCE_DIR}/Dependencies/OFS/include")
include_directories(include)

set(OgreOfsPlugin_hdrs
	./include/OfsFileSystem.h
)

set(OgreOfsPlugin_srcs
	./src/OfsFileSystem.cpp
	./src/Plugin.cpp
)

ogitor_add_library(OgreOfsPlugin SHARED ${OgreOfsPlugin_hdrs} ${OgreOfsPlugin_srcs})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set_target_properties(OgreOfsPlugin PROPERTIES COMPILE_DEFINITIONS "PLUGIN_EXPORT")

target_link_libraries(OgreOfsPlugin ${OGRE_LIBRARIES} OFS ${Boost_regex_LIBRARY})

install(TARGETS OgreOfsPlugin LIBRARY DESTINATION ${OGITOR_LIBOGREOFSPLUGIN_PATH})
# vim: set sw=2 ts=2 noet:
