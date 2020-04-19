cmake_minimum_required(VERSION 2.8.10)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

# CMake policy introduced in relation to Qt 5.1
if("${CMAKE_VERSION}" VERSION_GREATER 2.8.11)   
	cmake_policy(SET CMP0020 OLD)
endif()

project(GenericTextEditor)

if(NOT Qt5Widgets_FOUND)
    MESSAGE(SEND_ERROR "Failed to find Qt 5.0.0 or greater.")
else(NOT Qt5Widgets_FOUND)
    set(CMAKE_PACKAGE_QTGUI TRUE)

    file(GLOB UI_FILES *.ui)

    set(HEADER_FILES        
        ./include/generictexteditor.hxx
        ./include/generictexteditorcodec.hxx
        ./include/generictexteditordocument.hxx
        ./include/itexteditorcodec.hxx        
	    ./include/xmltexteditorcodec.hxx
    )

    set(SOURCE_FILES
        ./src/generictexteditor.cpp
        ./src/generictexteditorcodec.cpp
        ./src/generictexteditordocument.cpp        
	    ./src/xmltexteditorcodec.cpp
    )

    SET(MOC_SRCS2
        ./include/generictexteditor.hxx
        ./include/generictexteditorcodec.hxx
        ./include/generictexteditordocument.hxx
        ./include/itexteditorcodec.hxx        
	    ./include/xmltexteditorcodec.hxx
    )

    qt5_wrap_cpp(MOC_SRCS ${MOC_SRCS2})
    qt5_wrap_ui(UI_SRCS ${UI_FILES})

    source_group("Moc Files" FILES ${MOC_SRCS})
    source_group("Ui Files" FILES ${UI_FILES})

    # if we don't include this CMake will not include ui headers properly:
    include_directories(${CMAKE_CURRENT_BINARY_DIR})

    set(SRCS ${HEADER_FILES} ${SOURCE_FILES} ${MOC_SRCS} ${UI_FILES})

    set(GenericTextEditor_VERSION 0)
    set(GenericTextEditor_VERSION_MAJOR 1)

    include_directories("include")

    include_directories(.)
    include_directories(${OGITOR_INCLUDES})
    include_directories(../OFS/include)

    link_libraries(${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} OFS Ogitor)
    ogitor_add_library(GenericTextEditor SHARED ${SRCS})
    set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
	
	qt5_use_modules(GenericTextEditor Widgets)

    add_dependencies(GenericTextEditor OFS)
    set_target_properties(GenericTextEditor PROPERTIES COMPILE_DEFINITIONS GENERICTEXTEDITOR_EXPORT)
	install(TARGETS GenericTextEditor
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            RUNTIME DESTINATION bin)
endif(NOT Qt5Widgets_FOUND)

# vim: set sw=2 ts=2 noet:
