cmake_minimum_required(VERSION 2.8.10)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

# CMake policy introduced in relation to Qt 5.1
if("${CMAKE_VERSION}" VERSION_GREATER 2.8.11)   
	cmake_policy(SET CMP0020 OLD)
endif()

project(GenericImageEditor)

find_package(Qt5Widgets REQUIRED)

if(NOT Qt5Widgets_FOUND)
    MESSAGE(SEND_ERROR "Failed to find Qt 5.0.0 or greater.")
else(NOT Qt5Widgets_FOUND)
    set(CMAKE_PACKAGE_QTGUI TRUE)

    file(GLOB UI_FILES *.ui)

    set(HEADER_FILES
        ./include/genericimageeditor.hxx       
        ./include/genericimageeditorcodec.hxx
        ./include/genericimageeditordocument.hxx
        ./include/heightimageeditorcodec.hxx
        ./include/iimageeditorcodec.hxx
    )

    set(SOURCE_FILES
        ./src/genericimageeditor.cpp        
        ./src/genericimageeditorcodec.cpp
        ./src/genericimageeditordocument.cpp
        ./src/heightimageeditorcodec.cpp
    )

    SET(MOC_SRCS2
        ./include/genericimageeditor.hxx       
        ./include/genericimageeditorcodec.hxx
        ./include/genericimageeditordocument.hxx
        ./include/heightimageeditorcodec.hxx
        ./include/iimageeditorcodec.hxx
    )

    qt5_wrap_cpp(MOC_SRCS ${MOC_SRCS2})
    qt5_wrap_ui(UI_SRCS ${UI_FILES})

    source_group("Moc Files" FILES ${MOC_SRCS})
    source_group("Ui Files" FILES ${UI_FILES})

    # if we don't include this CMake will not include ui headers properly:
    include_directories(${CMAKE_CURRENT_BINARY_DIR})

    set(SRCS ${HEADER_FILES} ${SOURCE_FILES} ${MOC_SRCS} ${UI_FILES})

    set(GenericImageEditor_VERSION 0)
    set(GenericImageEditor_VERSION_MAJOR 1)

    include_directories(include)

    include_directories(.)
    include_directories(${OGITOR_INCLUDES})
    include_directories(../OFS/include)
    include_directories(../ImageConverter/include)

    link_libraries(${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} OFS Ogitor ImageConverter)
    ogitor_add_library(GenericImageEditor SHARED ${SRCS})
    set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)	

	add_dependencies(GenericImageEditor OFS)
	add_dependencies(GenericImageEditor ImageConverter)
    
    set_target_properties(GenericImageEditor PROPERTIES VERSION ${GenericImageEditor_VERSION} SOVERSION ${GenericImageEditor_VERSION_MAJOR})
    set_target_properties(GenericImageEditor PROPERTIES COMPILE_DEFINITIONS GENERICIMAGEEDITOR_EXPORT)

	qt5_use_modules(GenericImageEditor Widgets)
	install(TARGETS GenericImageEditor
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            RUNTIME DESTINATION bin)
endif(NOT Qt5Widgets_FOUND)

# vim: set sw=2 ts=2 noet:
