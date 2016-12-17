cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(ogQtExample)

set(QT_MIN_VERSION "4.5.0")
find_package(Qt4 COMPONENTS QtSvg REQUIRED)

find_package(OGITOR REQUIRED)

if(NOT QT4_FOUND)
	MESSAGE(SEND_ERROR "Failed to find Qt 4.5 or greater.")
else(NOT QT4_FOUND)
	include(${QT_USE_FILE})
	set(CMAKE_PACKAGE_QTGUI TRUE)

	file(GLOB UI_FILES *.ui)

	set(HEADER_FILES
		qtExample.h
	)

	set(SOURCE_FILES
		qtExample.cpp
	)

	SET(MOC_SRCS2
		qtExample.h
	)

	qt4_wrap_cpp(MOC_SRCS ${MOC_SRCS2})
	qt4_wrap_ui(UI_SRCS ${UI_FILES})

	source_group("Moc Files" FILES ${MOC_SRCS})
	source_group("Ui Files" FILES ${UI_FILES})

	# if we don't include this CMake will not include ui headers properly:
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	#qt4_add_resources(RESOURCE_SRCS ./res/qtOgitor.qrc)
	#SOURCE_GROUP("Qrc Files" FILES ${RESOURCE_SRCS})

	set(SRCS ${HEADER_FILES} ${SOURCE_FILES} ${MOC_SRCS} ${UI_FILES})

	set(ogQtExample_VERSION 0)
	set(ogQtExample_VERSION_MAJOR 1)

	include_directories(include)
	include_directories(${DEPENDENCIES_INCLUDES})
	include_directories(${OGITOR_INCLUDES})
	message(STATUS "${OGITOR_INCLUDES}")

	include_directories(.)
	link_directories(${OGITOR_LIBPATH})
	link_directories(${DEPENDENCIES_LIBPATH})

	link_libraries(${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} ${OGRE_LIBRARIES} Ogitor)
	add_library(ogQtExample SHARED ${SRCS})
	set(LIBRARY_OUTPUT_PATH ../lib)

    if(NOT OGITOR_DIST)
        set_target_properties(ogQtExample PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
    endif(NOT OGITOR_DIST)
	
	set_target_properties(ogQtExample PROPERTIES VERSION ${ogQtExample_VERSION} SOVERSION ${ogQtExample_VERSION_MAJOR})
	set_target_properties(ogQtExample PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)
	set_target_properties(ogQtExample PROPERTIES SOLUTION_FOLDER Plugins)
		
	install(TARGETS ogQtExample LIBRARY DESTINATION ${OGITOR_PLUGIN_PATH})
endif(NOT QT4_FOUND)

# vim: set sw=2 ts=2 noet:
