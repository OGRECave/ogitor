cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(OgPythonqtScript)

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

    set(OgPythonqtScript_VERSION 0)
    set(OgPythonqtScript_VERSION_MAJOR 1)

    include_directories(include)
    include_directories(${DEPENDENCIES_INCLUDES})
    include_directories(${OGITOR_INCLUDES})
    include_directories(${PYTHON_INCLUDE_DIR})
    message(STATUS "${OGITOR_INCLUDES}")

    include_directories(.)
    link_directories(${OGITOR_LIBPATH})
    link_directories(${DEPENDENCIES_LIBPATH})

    link_libraries( ${OGRE_LIBRARIES} Ogitor PythonQt)
    add_library(OgPythonqtScript SHARED ${HEADER_FILES} ${SOURCE_FILES})
    set(LIBRARY_OUTPUT_PATH ../lib)

    add_dependencies(OgPythonqtScript Ogitor)
    add_dependencies(OgPythonqtScript PythonQt)

    if(NOT OGITOR_DIST)
        set_target_properties(OgPythonqtScript PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
    endif(NOT OGITOR_DIST)

    set_target_properties(OgPythonqtScript PROPERTIES VERSION ${OgPythonqtScript_VERSION} SOVERSION ${OgPythonqtScript_VERSION_MAJOR})
    set_target_properties(OgPythonqtScript PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)
    set_target_properties(OgPythonqtScript PROPERTIES FOLDER Plugins) 

    if(UNIX AND NOT APPLE)
        add_custom_command(TARGET OgPythonqtScript PRE_BUILD
            COMMAND mkdir -p ../../RunPath/Plugins)
        add_custom_command(TARGET OgPythonqtScript POST_BUILD
            COMMAND cp -r ../lib/libOgPythonqtScript.* ../../RunPath/Plugins/)
    endif(UNIX AND NOT APPLE)

    if(WIN32)
        add_custom_command(TARGET OgPythonqtScript PRE_BUILD
            COMMAND if not exist ..\\..\\RunPath\\Plugins mkdir ..\\..\\RunPath\\Plugins)
        add_custom_command(TARGET OgPythonqtScript POST_BUILD
            COMMAND copy \"$(TargetPath)\" ..\\..\\RunPath\\Plugins & if exist ..\\..\\RunPath\\Plugins\\OgPythonqtScript_d.dll move ..\\..\\RunPath\\Plugins\\OgPythonqtScript_d.dll ..\\..\\RunPath\\Plugins\\debug)

        install(TARGETS OgPythonqtScript
            RUNTIME DESTINATION Plugins
            CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    endif(WIN32)

    IF(UNIX AND NOT APPLE)
        if(OGITOR_DIST)
            install(TARGETS OgPythonqtScript
                LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ogitor
                CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
        endif(OGITOR_DIST)
    ENDIF(UNIX AND NOT APPLE)
endif(NOT QT4_FOUND)

# vim: set sw=2 ts=2 noet:
