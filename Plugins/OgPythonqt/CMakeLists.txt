cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(OgPythonqt)

set(QT_MIN_VERSION "4.5.0")
find_package(Qt4 REQUIRED)

if(NOT QT4_FOUND)
    message(SEND_ERROR "Failed to find Qt 4.5 or greater.")
else(NOT QT4_FOUND)

    set(HEADER_FILES
        PythonqtInterpreter.h
    )

    set(SOURCE_FILES
        PythonqtInterpreter.cpp
    )

    set(OgPythonqt_VERSION 0)
    set(OgPythonqt_VERSION_MAJOR 1)

    include_directories(include)
    include_directories(${DEPENDENCIES_INCLUDES})
    include_directories(${OGITOR_INCLUDES})
    include_directories(${PYTHON_INCLUDE_DIR})
    message(STATUS "${OGITOR_INCLUDES}")

    include_directories(.)
    link_directories(${OGITOR_LIBPATH})
    link_directories(${DEPENDENCIES_LIBPATH})

    link_libraries( ${OGRE_LIBRARIES} Ogitor PythonQt)
    add_library(OgPythonqt SHARED ${HEADER_FILES} ${SOURCE_FILES})
    set(LIBRARY_OUTPUT_PATH ../lib)

    add_dependencies(OgPythonqt Ogitor)
    add_dependencies(OgPythonqt PythonQt)

    if(NOT OGITOR_DIST)
        set_target_properties(OgPythonqt PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
    endif(NOT OGITOR_DIST)

    set_target_properties(OgPythonqt PROPERTIES VERSION ${OgPythonqt_VERSION} SOVERSION ${OgPythonqt_VERSION_MAJOR})
    set_target_properties(OgPythonqt PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)
    set_target_properties(OgPythonqt PROPERTIES FOLDER Plugins) 

    if(UNIX AND NOT APPLE)
        add_custom_command(TARGET OgPythonqt PRE_BUILD
            COMMAND mkdir -p ../../RunPath/Plugins)
        add_custom_command(TARGET OgPythonqt POST_BUILD
            COMMAND cp -r ../lib/libOgPythonqt.* ../../RunPath/Plugins/)
    endif(UNIX AND NOT APPLE)

    if(WIN32)
        add_custom_command(TARGET OgPythonqt PRE_BUILD
            COMMAND if not exist ..\\..\\RunPath\\Plugins mkdir ..\\..\\RunPath\\Plugins)
        add_custom_command(TARGET OgPythonqt POST_BUILD
            COMMAND copy \"$(TargetPath)\" ..\\..\\RunPath\\Plugins & if exist ..\\..\\RunPath\\Plugins\\OgPythonqt_d.dll move ..\\..\\RunPath\\Plugins\\OgPythonqt_d.dll ..\\..\\RunPath\\Plugins\\debug)

        install(TARGETS OgPythonqt
            RUNTIME DESTINATION Plugins
            CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    endif(WIN32)

    IF(UNIX AND NOT APPLE)
        if(OGITOR_DIST)
            install(TARGETS OgPythonqt
                LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ogitor
                CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
        endif(OGITOR_DIST)
    ENDIF(UNIX AND NOT APPLE)
endif(NOT QT4_FOUND)

# vim: set sw=2 ts=2 noet:
