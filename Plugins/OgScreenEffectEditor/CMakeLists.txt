cmake_minimum_required(VERSION 2.8.10)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

# CMake policy introduced in relation to Qt 5.1
if("${CMAKE_VERSION}" VERSION_GREATER 2.8.11)   
	cmake_policy(SET CMP0020 OLD)
endif()

project(OgScreenEffectEditor)

find_package(Qt5Widgets REQUIRED)

if(NOT Qt5Widgets_FOUND)
	message(SEND_ERROR "Failed to find Qt 5.0.0 or greater.")
else(NOT Qt5Widgets_FOUND)
	set(CMAKE_PACKAGE_QTGUI TRUE)
	
	file(GLOB UI_FILES *.ui)

	set(HEADER_FILES
		screeneffectview.hxx
		ScreenEffectEditor.h
		Plugin.h
	)

	set(SOURCE_FILES
		screeneffectview.cpp
		ScreenEffectEditor.cpp
		Plugin.cpp
	)

	set(MOC_SRCS2
		screeneffectview.hxx
	)

	qt5_wrap_cpp(MOC_SRCS ${MOC_SRCS2} OPTIONS ${OGITOR_QT_PREPROCESSOR})
	qt5_wrap_ui(UI_SRCS ${UI_FILES})

	source_group("Moc Files" FILES ${MOC_SRCS})
	source_group("Ui Files" FILES ${UI_FILES})

	qt5_add_resources(RESOURCE_SRCS ./res/resource.qrc)
	source_group("Qrc Files" FILES ${RESOURCE_SRCS})

	# if we don't include this CMake will not include ui headers properly:
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	#qt5_add_resources(RESOURCE_SRCS ./res/qtOgitor.qrc)
	#source_group("Qrc Files" FILES ${RESOURCE_SRCS})

	set(SRCS ${HEADER_FILES} ${SOURCE_FILES} ${MOC_SRCS} ${UI_FILES} ${RESOURCE_SRCS})

	set(OgScreenEffectEditor_VERSION 1)
	set(OgScreenEffectEditor_VERSION_MAJOR 0)

	include_directories(include)
	include_directories(${DEPENDENCIES_INCLUDES})
	include_directories(${OGITOR_INCLUDES})
	message(STATUS "${OGITOR_INCLUDES}")

	include_directories(.)
	link_directories(${OGITOR_LIBPATH})
	link_directories(${DEPENDENCIES_LIBPATH})

	link_libraries(${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} ${OGRE_LIBRARIES} Ogitor TinyXML)
	ogitor_add_library(OgScreenEffectEditor SHARED ${SRCS})
	set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
	
	qt5_use_modules(OgScreenEffectEditor Widgets)

	if(NOT OGITOR_DIST)
		set_target_properties(OgScreenEffectEditor PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
	endif(NOT OGITOR_DIST)

	set_target_properties(OgScreenEffectEditor PROPERTIES VERSION ${OgScreenEffectEditor_VERSION} SOVERSION ${OgScreenEffectEditor_VERSION_MAJOR})
	set_target_properties(OgScreenEffectEditor PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)
	set_target_properties(OgScreenEffectEditor PROPERTIES SOLUTION_FOLDER Plugins)
		
	install(TARGETS OgScreenEffectEditor
            LIBRARY DESTINATION ${OGITOR_PLUGIN_INSTALL_PATH}
            ARCHIVE DESTINATION ${OGITOR_PLUGIN_INSTALL_PATH}
            RUNTIME DESTINATION ${OGITOR_PLUGIN_INSTALL_PATH})
endif(NOT Qt5Widgets_FOUND)

# vim: set sw=2 ts=2 noet:
