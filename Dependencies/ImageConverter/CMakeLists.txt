cmake_minimum_required(VERSION 2.8.10)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

# CMake policy introduced in relation to Qt 5.1
if("${CMAKE_VERSION}" VERSION_GREATER 2.8.11)   
	cmake_policy(SET CMP0020 OLD)
endif()

project(ImageConverter)

find_package(Qt5Widgets REQUIRED)

if(NOT Qt5Widgets_FOUND)
    MESSAGE(SEND_ERROR "Failed to find Qt 5.0.0 or greater.")
else(NOT Qt5Widgets_FOUND)
    set(CMAKE_PACKAGE_QTGUI TRUE)
	
    file(GLOB UI_FILES *.ui)

    set(HEADER_FILES
  			./include/imageconverter.hxx
    )

    set(SOURCE_FILES
  			./src/imageconverter.cpp
    )

    SET(MOC_SRCS2
  			./include/imageconverter.hxx
    )

    qt5_wrap_cpp(MOC_SRCS ${MOC_SRCS2})
    qt5_wrap_ui(UI_SRCS ${UI_FILES})

    source_group("Moc Files" FILES ${MOC_SRCS})
    source_group("Ui Files" FILES ${UI_FILES})

    # if we don't include this CMake will not include ui headers properly:
    include_directories(${CMAKE_CURRENT_BINARY_DIR})

    set(SRCS ${HEADER_FILES} ${SOURCE_FILES} ${MOC_SRCS} ${UI_FILES})

    set(ImageConverter_VERSION 0)
    set(ImageConverter_VERSION_MAJOR 1)

    include_directories(include)

    include_directories(.)
    include_directories(${OGITOR_INCLUDES})

    link_libraries(${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} Ogitor)
    ogitor_add_library(ImageConverter SHARED ${SRCS})
    set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
	
	qt5_use_modules(ImageConverter Widgets)

    set_target_properties(ImageConverter PROPERTIES COMPILE_DEFINITIONS IMAGECONVERTER_EXPORT)
	install(TARGETS ImageConverter
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            RUNTIME DESTINATION bin)
endif(NOT Qt5Widgets_FOUND)

# vim: set sw=2 ts=2 noet:
