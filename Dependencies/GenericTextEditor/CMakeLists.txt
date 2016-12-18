cmake_minimum_required(VERSION 2.8.10)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

# CMake policy introduced in relation to Qt 5.1
if("${CMAKE_VERSION}" VERSION_GREATER 2.8.11)   
	cmake_policy(SET CMP0020 OLD)
endif()

project(GenericTextEditor)

find_package(Qt5Widgets REQUIRED)

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

    if(NOT OGITOR_DIST)
        set_target_properties(GenericTextEditor PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
    endif(NOT OGITOR_DIST)

    set_target_properties(GenericTextEditor PROPERTIES VERSION ${GenericTextEditor_VERSION} SOVERSION ${GenericTextEditor_VERSION_MAJOR})
    set_target_properties(GenericTextEditor PROPERTIES COMPILE_DEFINITIONS GENERICTEXTEDITOR_EXPORT)

    if(UNIX AND NOT APPLE)
        add_custom_command(TARGET GenericTextEditor PRE_BUILD
            COMMAND mkdir -p ../../RunPath/lib)
        add_custom_command(TARGET GenericTextEditor POST_BUILD
            COMMAND cp -r ../lib/libGenericTextEditor_Ogitor.* ../../RunPath/lib/)
    endif(UNIX AND NOT APPLE)

    if(MSVC_IDE)
        add_custom_command(TARGET GenericTextEditor PRE_BUILD
            COMMAND if not exist ..\\..\\RunPath\\bin mkdir ..\\..\\RunPath\\bin)
        add_custom_command(TARGET GenericTextEditor POST_BUILD
            COMMAND copy \"$(TargetPath)\" ..\\..\\RunPath\\bin)
    endif(MSVC_IDE)
    if(WIN32)
        install(TARGETS GenericTextEditor
            RUNTIME DESTINATION bin
            CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel)
    endif(WIN32)

    IF(UNIX AND NOT APPLE)

        set_target_properties( GenericTextEditor PROPERTIES OUTPUT_NAME "GenericTextEditor_Ogitor" VERSION
        ${OGITOR_VERSION_STRING} SOVERSION ${abi_version} )

        install(TARGETS GenericTextEditor
            LIBRARY DESTINATION lib
            CONFIGURATIONS Release RelWithDebInfo MinSizeRel)

    ENDIF(UNIX AND NOT APPLE)

    IF (APPLE)
        set_target_properties(GenericTextEditor PROPERTIES OUTPUT_NAME GenericTextEditor)
        # Set the INSTALL_PATH so that Frameworks can be local
        set_target_properties(GenericTextEditor PROPERTIES BUILD_WITH_INSTALL_RPATH 1 INSTALL_NAME_DIR "@executable_path/../Plugins/Ogitor")
    ENDIF(APPLE)

endif(NOT Qt5Widgets_FOUND)

# vim: set sw=2 ts=2 noet:
