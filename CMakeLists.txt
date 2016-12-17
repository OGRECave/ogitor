######################################################################
# OGITOR BUILD SYSTEM
# Welcome to the CMake build system for OGITOR.
# This is the main file where we prepare the general build environment
# and provide build configuration options.
######################################################################

cmake_minimum_required(VERSION 2.8.10)

# Works around warnings libraries linked against that don't
# have absolute paths (e.g. -lOgreMain)
cmake_policy(SET CMP0003 NEW)

# Works around warnings about escaped quotes in ADD_DEFINITIONS
# statements.
cmake_policy(SET CMP0005 NEW)

# CMake policy introduced in relation to Qt 5.1
if("${CMAKE_VERSION}" VERSION_GREATER 2.8.11)   
	cmake_policy(SET CMP0020 OLD)
endif()

project(Ogitor)

set(OGITOR_VERSION_STRING "0.5.4")
set(OGITOR_VERSION_STRING_MAJOR "0")
set(OGITOR_VERSION_STRING_MINOR "5")
set(OGITOR_VERSION_STRING_PATCH "4")

set(OGITOR_OGSCENE_FORMAT_VERSION "4")

message(STATUS "Building Ogitor version ${OGITOR_VERSION_STRING}")

set(abi_version ${OGITOR_VERSION_STRING})
option(OGITOR_DIST "Ogitor Installer Installation" FALSE)
message(STATUS "OGITOR_DIST is set to ${OGITOR_DIST}")

# Handle install prefix
if(WIN32)
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/RunPath" CACHE PATH "Ogitor install prefix" FORCE)
    endif (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
else(WIN32)
    if(NOT OGITOR_DIST)
        set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/RunPath" CACHE PATH "Ogitor install prefix" FORCE)
    endif(NOT OGITOR_DIST)
endif(WIN32)

option(OGITOR_MESHMAGICK_INTERFACE "Use MeshMagick widget" FALSE)

option(OGITOR_DOWNLOAD_SAMPLEPROJECT "Download and install sample project" TRUE)
option(OGITOR_DOWNLOAD_SAMPLEMEDIA "Download and install sample media" TRUE)

# Somehow, relative paths doesn't work on Linux when installing files..
if(UNIX)
    set(PREFIX ${CMAKE_INSTALL_PREFIX})
else()
    set(PREFIX "./")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "")
    # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
    # differentiation between debug and release builds.
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

if(WIN32)
	set(CMAKE_DEBUG_POSTFIX "_d")
endif(WIN32)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules/;${CMAKE_MODULE_PATH}")

if(UNIX AND NOT OGITOR_DIST)
 set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
 set(CMAKE_INSTALL_RPATH ../lib ../Plugins)
endif(UNIX AND NOT OGITOR_DIST)

find_package(OGRE 1.10 REQUIRED)
link_directories(${OGRE_LIBRARY_DIRS})

include(CheckTypeSize)
CHECK_TYPE_SIZE("void*" OGITOR_PTR_SIZE BUILTIN_TYPES_ONLY)
if (OGITOR_PTR_SIZE EQUAL 8)
	set(OGITOR_PLATFORM_X64 TRUE)
else ()
	set(OGITOR_PLATFORM_X64 FALSE)
endif ()

if(UNIX)
  if(NOT OGITOR_PLATFORM_X64)
    add_definitions(-D_FILE_OFFSET_BITS=64)
  endif(NOT OGITOR_PLATFORM_X64)

  # Fix x64 issues on Linux
  if(OGITOR_PLATFORM_X64 AND NOT APPLE)
      add_definitions(-fPIC)
      set(LIB_SUFFIX 64)
  else(OGITOR_PLATFORM_X64 AND NOT APPLE)
      set(LIB_SUFFIX "")
  endif(OGITOR_PLATFORM_X64 AND NOT APPLE)
endif(UNIX)

include_directories(${OGRE_INCLUDE_DIRS})

# Ogitor
set(OGITOR_INCLUDES "${CMAKE_SOURCE_DIR}/Ogitor/include")
set(OGITOR_LIBPATH  "${CMAKE_SOURCE_DIR}/Ogitor/lib")

# Dependencies
set(DEPENDENCIES_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/pagedgeometry/include"
    "${CMAKE_BINARY_DIR}/Dependencies/src/Meshmagick/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/add_on/autowrapper"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/add_on/scriptbuilder"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/add_on/scriptstdstring"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/add_on/scriptmath"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/add_on/scriptarray"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/add_on/quaternion"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/add_on/vector3"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/add_on/debugger"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/OFS/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/OgreTerrainConverter/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/ImageConverter/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/GenericTextEditor/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/GenericImageEditor/include"
)
set(DEPENDENCIES_LIBPATH ${CMAKE_BINARY_DIR}/Dependencies/lib)

#set_target_properties(ALL_BUILD PROPERTIES SOLUTION_FOLDER CMake)
#set_target_properties(ZERO_CHECK PROPERTIES SOLUTION_FOLDER CMake)
#set_target_properties(INSTALL PROPERTIES SOLUTION_FOLDER CMake)
#set_target_properties(PACKAGE PROPERTIES SOLUTION_FOLDER CMake)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/RunPath/Plugins/debug)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/RunPath/bin)

# Unity build options
# A Unity build includes all sources files in just a few actual compilation units
# to potentially speed up the compilation.
option(OGITOR_UNITY_BUILD "Enable unity build for Ogitor." FALSE)
set(OGITOR_UNITY_FILES_PER_UNIT "50" CACHE STRING "Number of files per compilation unit in Unity build.")

# configure global paths for OgitorsGlobals
if(UNIX)
    set(DBG_POSTFIX "")
	set(OGITOR_BIN_PATH "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Ogitor bin path" FORCE)
	set(OGITOR_RUN_PATH "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Ogitor run path" FORCE)
	set(OGITOR_RESOURCE_PATH "${CMAKE_INSTALL_PREFIX}/share/ogitor" CACHE PATH "Ogitor resource path" FORCE)
	set(OGITOR_LANGUAGE_PATH "${CMAKE_INSTALL_PREFIX}/share/ogitor/languages" CACHE PATH "Ogitor languages path" FORCE)
	set(OGITOR_MEDIA_PATH "${CMAKE_INSTALL_PREFIX}/share/ogitor/Media" CACHE PATH "Ogitor media path" FORCE)
	set(OGITOR_PROJECTS_PATH "${CMAKE_INSTALL_PREFIX}/Projects" CACHE PATH "Ogitor Projects path" FORCE)
	set(OGITOR_SCRIPTS_PATH "${CMAKE_INSTALL_PREFIX}/share/ogitor/Scripts" CACHE PATH "Ogitor scripts path" FORCE)
	set(OGITOR_PLUGIN_PATH "${CMAKE_INSTALL_PREFIX}/lib/ogitor" CACHE PATH "Ogitor plugins path" FORCE)
    set(OGITOR_PLUGIN_ICON_PATH "${CMAKE_INSTALL_PREFIX}/share/ogitor/Plugins/Icons" CACHE PATH "Ogitor plugins icon path" FORCE)
	
    if(OGITOR_DIST)
        set(OGITOR_LIBOGREOFSPLUGIN_PATH "${OGRE_PLUGIN_DIR}" CACHE PATH "libOgreOfsPlugin path" FORCE)
    else(OGITOR_DIST)
        set(OGITOR_LIBOGREOFSPLUGIN_PATH "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "libOgreOfsPlugin path" FORCE)
    endif(OGITOR_DIST)
	
	if (APPLE)
	set(OGITOR_PLUGIN_ICON_PATH "../Plugins/Icons" CACHE PATH "Ogitor plugins icon path" FORCE)
	endif(APPLE)
	
	configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/Templates/resources.cfg.in ${OGITOR_RESOURCE_PATH}/resources.cfg)
else(UNIX)
    # We are on Windows
    set(OGITOR_BIN_PATH "." CACHE PATH "Ogitor bin path" FORCE)
    set(OGITOR_RUN_PATH "." CACHE PATH "Ogitor run path" FORCE)
    set(OGITOR_RESOURCE_PATH "." CACHE PATH "Ogitor resource path" FORCE)
    set(OGITOR_LANGUAGE_PATH "../languages" CACHE PATH "Ogitor languages path" FORCE)
    set(OGITOR_MEDIA_PATH "../Media" CACHE PATH "Ogitor media path" FORCE)
    set(OGITOR_PROJECTS_PATH "../Projects" CACHE PATH "Ogitor Projects path" FORCE)
    set(OGITOR_SCRIPTS_PATH "../Scripts" CACHE PATH "Ogitor scripts path" FORCE)
    set(OGITOR_PLUGIN_PATH "../Plugins" CACHE PATH "Ogitor plugins path" FORCE)
    set(OGITOR_PLUGIN_ICON_PATH "../Plugins/Icons" CACHE PATH "Ogitor plugins icon path" FORCE)
    set(OGITOR_LIBOGREOFSPLUGIN_PATH "." CACHE PATH "libOgreOfsPlugin path" FORCE)
	configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/Templates/resources.cfg.in ${CMAKE_INSTALL_PREFIX}/bin/resources.cfg)
endif(UNIX)

# hide advanced options
#mark_as_advanced(
#    OGITOR_BIN_PATH
#    OGITOR_RUN_PATH
#    OGITOR_RESOURCE_PATH
#    OGITOR_LANGUAGE_PATH
#    OGITOR_MEDIA_PATH
#    OGITOR_PROJECTS_PATH
#    OGITOR_SCRIPTS_PATH
#    OGITOR_PLUGIN_PATH
#    OGITOR_PLUGIN_ICON_PATH
#    OGITOR_LIBOGREOFSPLUGIN_PATH
#)

# Error out if old Media folder is detected
if(UNIX AND NOT APPLE AND IS_DIRECTORY ${CMAKE_SOURCE_DIR}/RunPath/Media/EditorResources)
  message(SEND_ERROR "An old version of the Media folder was detected in: ${CMAKE_CURRENT_SOURCE_DIR}/RunPath .\nMake sure that you've downloaded the latest version of 'media.zip' from Ogitor.org and extracted it into ${CMAKE_CURRENT_SOURCE_DIR}/RunPath as 'Media'.")
endif()
if(NOT OGITOR_DOWNLOAD_SAMPLEMEDIA)
    # Error out if Source_Dir/RunPath/Media and Source_Dir/RunPath/Projects isn't present on the users system
    if(NOT IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/RunPath/Media)
      message(SEND_ERROR "Failed to find ${CMAKE_CURRENT_SOURCE_DIR}/RunPath/Media.\nMake sure that you've downloaded 'media.zip' from Ogitor.org and extracted it into ${CMAKE_CURRENT_SOURCE_DIR}/RunPath as 'Media'.")
    endif(NOT IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/RunPath/Media)
endif(NOT OGITOR_DOWNLOAD_SAMPLEMEDIA)
if(NOT OGITOR_DOWNLOAD_SAMPLEPROJECT)
    if(NOT IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/RunPath/Projects)
      message(SEND_ERROR "Failed to find ${CMAKE_CURRENT_SOURCE_DIR}/RunPath/Projects.\nMake sure that you've downloaded 'projects.zip' from Ogitor.org and extracted it into ${CMAKE_CURRENT_SOURCE_DIR}/RunPath as 'Projects'.")
    endif(NOT IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/RunPath/Projects)
endif(NOT OGITOR_DOWNLOAD_SAMPLEPROJECT)

# External Projects
include(ExternalProject)

# download, unpack and copy sample project
if(OGITOR_DOWNLOAD_SAMPLEPROJECT)
    ExternalProject_Add(SampleProject
      URL https://bitbucket.org/ogitor/ogitor/downloads/projects.zip
      PREFIX ${CMAKE_CURRENT_BINARY_DIR}
      PATCH_COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_CURRENT_BINARY_DIR}/src/SampleProject" "${OGITOR_PROJECTS_PATH}"
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
    )
    set_target_properties(SampleProject PROPERTIES FOLDER Assets)
endif(OGITOR_DOWNLOAD_SAMPLEPROJECT)

# download, unpack and copy sample media
if(OGITOR_DOWNLOAD_SAMPLEMEDIA)
    ExternalProject_Add(SampleMedia
      URL https://bitbucket.org/ogitor/ogitor/downloads/media.zip
      PREFIX ${CMAKE_CURRENT_BINARY_DIR}
      PATCH_COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_CURRENT_BINARY_DIR}/src/SampleMedia" "${OGITOR_MEDIA_PATH}"
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
    )
    set_target_properties(SampleMedia PROPERTIES FOLDER Assets)
endif(OGITOR_DOWNLOAD_SAMPLEMEDIA)

# Generate OgitorsGlobals.cpp
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/Templates/OgitorsGlobals.cpp.in ${CMAKE_BINARY_DIR}/Ogitor/src/OgitorsGlobals.cpp)

add_subdirectory(Ogitor)

# Setup Dependencies
add_subdirectory(Dependencies)
add_subdirectory(Dependencies/Angelscript)
add_subdirectory(Dependencies/OFS)
add_subdirectory(Dependencies/OgreTerrainConverter)
add_subdirectory(Dependencies/ImageConverter)
add_subdirectory(Dependencies/GenericTextEditor)
add_subdirectory(Dependencies/GenericImageEditor)
#IF(OGITOR_MESHMAGICK_INTERFACE)
#    add_subdirectory(Dependencies/MeshMagick)
#ENDIF(OGITOR_MESHMAGICK_INTERFACE)

# Plugins
add_subdirectory(Plugins/DotSceneSerializer)
#add_subdirectory(Plugins/OgModularZone)
add_subdirectory(Plugins/OgMaterialEditor)
add_subdirectory(Plugins/OgAngelScript)
add_subdirectory(Plugins/OgScriptEditor)
add_subdirectory(Plugins/OgAngelDialogManager)
add_subdirectory(Plugins/OgScreenEffectEditor)
add_subdirectory(Plugins/CustomSerializer)

add_subdirectory(OgreOfsPlugin)

add_subdirectory(qtOgitor/qtpropertybrowser-2.5)

add_subdirectory(qtOgitor)

add_subdirectory(qtOfs)

# New version of OIS broke our SceneLoader..
#if(NOT UNIX)
#    add_subdirectory(SceneLoader)
#endif(NOT UNIX)

add_subdirectory(RunPath)

#add_subdirectory(Help)

add_custom_target(lupdate
    COMMAND ${PROJECT_SOURCE_DIR}/gen-qt-projectfile ${PROJECT_SOURCE_DIR} > ${PROJECT_SOURCE_DIR}/qtOgitor/languages/translate.pro
    COMMAND lupdate -noobsolete ${PROJECT_SOURCE_DIR}/qtOgitor/languages/translate.pro
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/qtOgitor/languages
)

if(WIN32)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/install/windows.cmake)
endif()

if(UNIX)
    # apple and linux
    include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/install/unix.cmake)

    if(OGITOR_DIST)
        FILE(GLOB ogitor_cmake_scripts "${CMAKE_SOURCE_DIR}/CMakeModules/findscripts/*.cmake")
        install(FILES ${ogitor_cmake_scripts}
            DESTINATION lib/ogitor/cmake
            CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
        )
    endif(OGITOR_DIST)
endif(UNIX)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

# no package support for Linux or Mac, yet.
if(WIN32 AND NOT UNIX)

    #include (InstallRequiredSystemLibraries)

    set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};Ogitor;ALL;/")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Ogre Editing Framework")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "Ogitor_${CPACK_PACKAGE_VERSION}")
    set(CPACK_PACKAGE_NAME "Ogitor_${CPACK_PACKAGE_VERSION}")
    set(CPACK_PACKAGE_VENDOR "The Ogitor Team")
    set(CPACK_PACKAGE_VERSION ${OGITOR_VERSION_STRING})
    set(CPACK_PACKAGE_VERSION_MAJOR ${OGITOR_VERSION_STRING_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${OGITOR_VERSION_STRING_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ${OGITOR_VERSION_STRING_PATCH})

    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
    set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\Resources\\\\OgitorInstall.bmp")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\qtOgitor.exe")
    set(CPACK_PACKAGE_EXECUTABLES "qtOgitor" "qtOgitor")
    set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut \\\"$DESKTOP\\\\qtOgitor.lnk\\\" \\\"$INSTDIR\\\\bin\\\\qtOgitor.exe\\\"")

    # Use custom version of NSIS.InstallOptions.ini
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/packaging" ${CMAKE_MODULE_PATH})

    # File types association:
    set(CPACK_NSIS_DEFINES "!include ${PROJECT_SOURCE_DIR}/CMakeModules\\\\packaging\\\\FileAssociation.nsh")

    getenv_path(USERPROFILE)

    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
    ExecWait '$SYSDIR\\\\cmd.exe /Q /C xcopy \\\"$INSTDIR\\\\Projects\\\\SampleDotScene\\\" \\\"$PROFILE\\\\OgitorProjects\\\\SampleDotScene\\\\\\\" /E /Y'
    ExecWait '$SYSDIR\\\\cmd.exe /Q /C xcopy \\\"$INSTDIR\\\\Media\\\" \\\"$PROFILE\\\\OgitorProjects\\\\Media\\\\\\\" /E /Y'
    CreateDirectory \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
    CopyFiles \\\"$INSTDIR\\\\bin\\\\SampleLoadApp.exe\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
    CopyFiles \\\"$INSTDIR\\\\bin\\\\SampleAppConfig_dist.xml\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\\SampleAppConfig.xml\\\"
        ExecWait '$SYSDIR\\\\cmd.exe /Q /C xcopy \\\"$INSTDIR\\\\SampleApp_Source\\\" \\\"$PROFILE\\\\OgitorProjects\\\\SampleApp_Source\\\\\\\" /E /Y'
        CopyFiles \\\"$INSTDIR\\\\bin\\\\Ogre*.dll\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
        CopyFiles \\\"$INSTDIR\\\\bin\\\\Plugin*.dll\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
        CopyFiles \\\"$INSTDIR\\\\bin\\\\RenderSystem*.dll\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
        CopyFiles \\\"$INSTDIR\\\\bin\\\\cg.dll\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
        CopyFiles \\\"$INSTDIR\\\\bin\\\\OIS.dll\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
        CopyFiles \\\"$INSTDIR\\\\bin\\\\Hydrax.dll\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
        CopyFiles \\\"$INSTDIR\\\\bin\\\\msvcr100.dll\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
        CopyFiles \\\"$INSTDIR\\\\bin\\\\msvcp100.dll\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
        CopyFiles \\\"$INSTDIR\\\\bin\\\\Caelum.dll\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
        CopyFiles \\\"$INSTDIR\\\\bin\\\\resources.cfg\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
        ExecWait  \\\"$INSTDIR\\\\redist\\\\dxwebsetup.exe /Q\\\"
        WriteRegStr HKCU \\\"Environment\\\" \\\"OGITOR_HOME\\\" $INSTDIR
    ")

    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS} "
    \\\${registerExtension} \\\"Ogitor Scene File\\\" \\\".ogscene\\\" \\\"\\\$INSTDIR\\\\bin\\\\qtOgitor.exe\\\"
    ")

    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS} 'WriteRegStr HKCU \\\"Environment\\\" \\\"OGITOR_HOME\\\" $INSTDIR\\\\bin\\\"' )

    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
    \\\${unregisterExtension} \\\".ogscene\\\" \\\"Ogitor Scene File\\\"
    DeleteRegKey HKCU \\\"Software\\\\Ogitor_${CPACK_PACKAGE_VERSION}\\\"
    DeleteRegKey HKCU \\\"Environment\\\\OGITOR_HOME\\\"
    Delete \\\"$DESKTOP\\\\qtOgitor.lnk\\\"
    ")

    include(CPack)

#add_custom_target(lupdate
#   COMMAND ${PROJECT_SOURCE_DIR}/gen-qt-projectfile ${PROJECT_SOURCE_DIR} > ${PROJECT_SOURCE_DIR}/qtOgitor/languages/translate.pro
#   COMMAND lupdate -noobsolete ${PROJECT_BINARY_DIR}/mscore.pro
#   WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
#   )

#add_custom_target(lrelease
#   COMMAND ${PROJECT_SOURCE_DIR}/gen-qt-projectfile ${PROJECT_SOURCE_DIR} > mscore.pro
#   COMMAND lrelease ${PROJECT_BINARY_DIR}/mscore.pro
#   WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
#   )

ENDIF(WIN32 AND NOT UNIX)
