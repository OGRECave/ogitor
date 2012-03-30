######################################################################
# OGITOR BUILD SYSTEM
# Welcome to the CMake build system for OGITOR.
# This is the main file where we prepare the general build environment
# and provide build configuration options.
######################################################################

cmake_minimum_required(VERSION 2.6)

# Works around warnings libraries linked against that don't
# have absolute paths (e.g. -lOgreMain)
cmake_policy(SET CMP0003 NEW)

# Works around warnings about escaped quotes in ADD_DEFINITIONS
# statements.
cmake_policy(SET CMP0005 NEW)

project(Ogitor)

set(OGITOR_VERSION_STRING "0.5.4")
set(OGITOR_VERSION_STRING_MAJOR "0")
set(OGITOR_VERSION_STRING_MINOR "5")
set(OGITOR_VERSION_STRING_PATCH "4")

set(abi_version ${OGITOR_VERSION_STRING})

# OGITOR_DIST (global install) is TRUE by default for Linux
if(UNIX AND NOT APPLE)
    option(OGITOR_DIST "Ogitor Installer Installation" TRUE)
else(UNIX AND NOT APPLE)
    option(OGITOR_DIST "Ogitor Installer Installation" FALSE)
endif(UNIX AND NOT APPLE)

# Handle install prefix
if(WIN32)
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/RunPath" CACHE PATH "Ogitor install prefix" FORCE)
    endif (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
else(WIN32)
    if(NOT OGITOR_DIST)
        set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/RunPath" CACHE PATH "Ogitor install prefix" FORCE)
    endif(NOT OGITOR_DIST)
endif(WIN32)

option(OGITOR_MESHMAGICK_INTERFACE "Use MeshMagick widget" FALSE)

set(OGITOR_PREPROCESSOR "OGITOR_EXPORT")

set(OGITOR_QT_PREPROCESSOR "")


# Find Boost
# Prefer static linking in all cases
if (NOT OGRE_BUILD_PLATFORM_IPHONE)
    if (WIN32 OR APPLE)
        set(Boost_USE_STATIC_LIBS TRUE)
    else ()
        # Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
        set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
    endif ()
    set(Boost_ADDITIONAL_VERSIONS "1.38.0" "1.38" "1.39.0" "1.39" "1.40.0" "1.40" "1.44.0" "1.44" "1.45" "1.46" "1.47" "1.48" "1.49" "1.49.0")
    # Components that need linking (NB does not include header-only components like bind)
    set(OGRE_BOOST_COMPONENTS thread date_time regex)
    find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
    if (NOT Boost_FOUND)
        # Try again with the other type of libs
        set(Boost_USE_STATIC_LIBS NOT ${Boost_USE_STATIC_LIBS})
        find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
    endif()
    find_package(Boost QUIET)
endif(NOT OGRE_BUILD_PLATFORM_IPHONE)

# Somehow, relative paths doesn't work on Linux when installing files..
if(UNIX)
    set(PREFIX ${CMAKE_INSTALL_PREFIX})
else()
    set(PREFIX "./")
endif()

set(OGITOR_WORK_DIR ${CMAKE_CURRENT_BINARY_DIR})

if(CMAKE_BUILD_TYPE STREQUAL "")
    # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
    # differentiation between debug and release builds.
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

if(WIN32)
set(CMAKE_DEBUG_POSTFIX "_d")
endif(WIN32)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/;${CMAKE_MODULE_PATH}")

if(UNIX AND NOT OGITOR_DIST)
 set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
 set(CMAKE_INSTALL_RPATH ../lib ../Plugins)
endif(UNIX AND NOT OGITOR_DIST)

find_package(OGRE REQUIRED)

if(UNIX)
    # Newer ld versions need this linked explicitly
    find_package(Threads REQUIRED)
    add_definitions(${CMAKE_THREAD_LIBS_INIT})
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}-Wl,--no-as-needed ${CMAKE_THREAD_LIBS_INIT}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}-Wl,--no-as-needed ${CMAKE_THREAD_LIBS_INIT}")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS}-Wl,--no-as-needed ${CMAKE_THREAD_LIBS_INIT}")
endif(UNIX)


if(APPLE)
    find_package(CG REQUIRED)
endif(APPLE)

if((NOT "${OGRE_VERSION_NAME}" STREQUAL "Byatis") AND (NOT "${OGRE_VERSION_NAME}" STREQUAL "Ghadamon"))
    message(SEND_ERROR "You need Ogre 1.8 Byatis or later to build this.")
endif()

if(UNIX)
    get_filename_component(OGRE_PLUGIN_DIR_LINUX ${OGRE_Plugin_OctreeSceneManager_LIBRARY_REL} PATH)
    set (OGRE_PLUGIN_DIR_LINUX ${OGRE_PLUGIN_DIR_LINUX} CACHE STRING "Ogre plugin dir (release)")
endif(UNIX)

include(CheckTypeSize)
CHECK_TYPE_SIZE("void*" OGITOR_PTR_SIZE BUILTIN_TYPES_ONLY)
if (OGITOR_PTR_SIZE EQUAL 8)
set(OGITOR_PLATFORM_X64 TRUE)
else ()
set(OGITOR_PLATFORM_X64 FALSE)
endif ()

if(UNIX)
  # Fix x64 issues on Linux
  if(OGITOR_PLATFORM_X64 AND NOT APPLE)
      add_definitions(-fPIC)
      set(LIB_SUFFIX 64)
  else(OGITOR_PLATFORM_X64 AND NOT APPLE)
      set(LIB_SUFFIX "")
  endif(OGITOR_PLATFORM_X64 AND NOT APPLE)
endif(UNIX)

include_directories(${OGRE_INCLUDE_DIRS})
include_directories(${OGRE_Terrain_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIR})
#library_directories(${Boost_LIBRARY_DIR})

# Ogitor
set(OGITOR_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/Ogitor/include")
set(OGITOR_LIBPATH	"${CMAKE_CURRENT_SOURCE_DIR}/Ogitor/lib")

# Dependencies
set(DEPENDENCIES_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/PagedGeometry/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Caelum-0.5.0/main/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/HYDRAX/Hydrax/src/Hydrax"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/SkyX"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/MeshMagick/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/include"
    #"${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lua/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/add_on/autowrapper"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/add_on/scriptbuilder"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/add_on/scriptstdstring"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/add_on/scriptmath"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/add_on/scriptarray"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/add_on/quaternion"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/add_on/vector3"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/add_on/debugger"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/OFS/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/GenericTextEditor/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/GenericImageEditor/include"
)
set(DEPENDENCIES_LIBPATH "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lib")

# Resources (Only used by OSX)
set(MAC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Mac")
set(RUNPATH "${CMAKE_CURRENT_SOURCE_DIR}/RunPath")

#set_target_properties(ALL_BUILD PROPERTIES SOLUTION_FOLDER CMake)
#set_target_properties(ZERO_CHECK PROPERTIES SOLUTION_FOLDER CMake)
#set_target_properties(INSTALL PROPERTIES SOLUTION_FOLDER CMake)
#set_target_properties(PACKAGE PROPERTIES SOLUTION_FOLDER CMake)

file(MAKE_DIRECTORY ${OGITOR_WORK_DIR}/RunPath/Plugins/debug)
file(MAKE_DIRECTORY ${OGITOR_WORK_DIR}/RunPath/bin)

# configure global paths for OgitorsGlobals
if(UNIX)
    set(DBG_POSTFIX "")
    if(OGITOR_DIST)
        set(OGITOR_BIN_PATH "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Ogitor bin path")
        set(OGITOR_RUN_PATH "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Ogitor run path")
        set(OGITOR_RESOURCE_PATH "${CMAKE_INSTALL_PREFIX}/share/ogitor" CACHE PATH "Ogitor resource path")
        set(OGITOR_LANGUAGE_PATH "${CMAKE_INSTALL_PREFIX}/share/ogitor/languages" CACHE PATH "Ogitor languages path")
        set(OGITOR_MEDIA_PATH "${CMAKE_INSTALL_PREFIX}/share/ogitor/Media" CACHE PATH "Ogitor media path")
        set(OGITOR_PROJECTS_PATH "${CMAKE_INSTALL_PREFIX}/Projects" CACHE PATH "Ogitor Projects path")
        set(OGITOR_SCRIPTS_PATH "${CMAKE_INSTALL_PREFIX}/share/ogitor/Scripts" CACHE PATH "Ogitor scripts path")
        set(OGITOR_PLUGIN_PATH "${CMAKE_INSTALL_PREFIX}/lib/ogitor" CACHE PATH "Ogitor plugins path")
        set(OGITOR_PLUGIN_ICON_PATH "${CMAKE_INSTALL_PREFIX}/share/ogitor/Plugins/Icons" CACHE PATH "Ogitor plugins icon path")
        set(OGITOR_OGRE_PLUGIN_PATH "${OGRE_PLUGIN_DIR_LINUX}" CACHE PATH "Ogre plugins path")
        set(OGITOR_LIBOGREOFSPLUGIN_PATH "${OGRE_PLUGIN_DIR_LINUX}" CACHE PATH "libOgreOfsPlugin path")
    else(OGITOR_DIST)
        # Doing local install
        set(OGITOR_BIN_PATH "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Ogitor bin path")
        set(OGITOR_RUN_PATH "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Ogitor run path")
        set(OGITOR_RESOURCE_PATH "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Ogitor resource path")
        set(OGITOR_LANGUAGE_PATH "${CMAKE_INSTALL_PREFIX}/share/ogitor/languages" CACHE PATH "Ogitor languages path")
        set(OGITOR_MEDIA_PATH "${CMAKE_INSTALL_PREFIX}/share/ogitor/Media" CACHE PATH "Ogitor media path")
        set(OGITOR_PROJECTS_PATH "${CMAKE_INSTALL_PREFIX}/Projects" CACHE PATH "Ogitor Projects path")
        set(OGITOR_SCRIPTS_PATH "${CMAKE_INSTALL_PREFIX}/share/ogitor/Scripts" CACHE PATH "Ogitor scripts path")
        set(OGITOR_PLUGIN_PATH "${CMAKE_INSTALL_PREFIX}/Plugins" CACHE PATH "Ogitor plugins path")
        set(OGITOR_PLUGIN_ICON_PATH "${CMAKE_INSTALL_PREFIX}/share/ogitor/Plugins/Icons" CACHE PATH "Ogitor plugins icon path")
        set(OGITOR_OGRE_PLUGIN_PATH "${OGRE_PLUGIN_DIR_LINUX}" CACHE PATH "Ogre plugins path")
        set(OGITOR_LIBOGREOFSPLUGIN_PATH "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "libOgreOfsPlugin path")
    endif(OGITOR_DIST)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/Templates/plugins.cfg.in ${OGITOR_WORK_DIR}/RunPath/bin/plugins.cfg)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/Templates/resources.cfg.in ${OGITOR_WORK_DIR}/RunPath/bin/resources.cfg)
else(UNIX)
    # We are on Windows
    set(OGITOR_BIN_PATH "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Ogitor bin path")
    set(OGITOR_RUN_PATH "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Ogitor run path")
    set(OGITOR_RESOURCE_PATH "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Ogitor resource path")
    set(OGITOR_LANGUAGE_PATH "${CMAKE_INSTALL_PREFIX}/languages" CACHE PATH "Ogitor languages path")
    set(OGITOR_MEDIA_PATH "${CMAKE_INSTALL_PREFIX}/Media" CACHE PATH "Ogitor media path")
    set(OGITOR_PROJECTS_PATH "${CMAKE_INSTALL_PREFIX}/Projects" CACHE PATH "Ogitor Projects path")
    set(OGITOR_SCRIPTS_PATH "${CMAKE_INSTALL_PREFIX}/Scripts" CACHE PATH "Ogitor scripts path")
    set(OGITOR_PLUGIN_PATH "${CMAKE_INSTALL_PREFIX}/Plugins" CACHE PATH "Ogitor plugins path")
    set(OGITOR_PLUGIN_ICON_PATH "${CMAKE_INSTALL_PREFIX}/Plugins/Icons" CACHE PATH "Ogitor plugins icon path")
    set(OGITOR_OGRE_PLUGIN_PATH "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Ogre plugins path")
    set(OGITOR_LIBOGREOFSPLUGIN_PATH "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "libOgreOfsPlugin path")
endif(UNIX)

# hide advanced options
mark_as_advanced(
    OGITOR_BIN_PATH
    OGITOR_RUN_PATH
    OGITOR_RESOURCE_PATH
    OGITOR_LANGUAGE_PATH
    OGITOR_MEDIA_PATH
    OGITOR_PROJECTS_PATH
    OGITOR_SCRIPTS_PATH
    OGITOR_PLUGIN_PATH
    OGITOR_PLUGIN_ICON_PATH
    OGITOR_OGRE_PLUGIN_PATH
    OGITOR_LIBOGREOFSPLUGIN_PATH
)

# Error out if Source_Dir/RunPath/Media and Source_Dir/RunPath/Projects isn't present on the users system
if(NOT IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/RunPath/Media)
  message(SEND_ERROR "Failed to find ${CMAKE_CURRENT_SOURCE_DIR}/RunPath/Media.\nMake sure that you've downloaded 'media.zip' from Ogitor.org and extracted it into ${CMAKE_CURRENT_SOURCE_DIR}/RunPath as 'Media'.")
endif(NOT IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/RunPath/Media)
if(NOT IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/RunPath/Projects)
  message(SEND_ERROR "Failed to find ${CMAKE_CURRENT_SOURCE_DIR}/RunPath/Projects.\nMake sure that you've downloaded 'projects.zip' from Ogitor.org and extracted it into ${CMAKE_CURRENT_SOURCE_DIR}/RunPath as 'Projects'.")
endif(NOT IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/RunPath/Projects)

# Generate OgitorsGlobals.cpp
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/Templates/OgitorsGlobals.cpp.in ${OGITOR_WORK_DIR}/Ogitor/src/OgitorsGlobals.cpp)

add_subdirectory(Ogitor)

# Setup Dependencies
add_subdirectory(Dependencies/HYDRAX/Hydrax/src/Hydrax)
add_subdirectory(Dependencies/Caelum-0.5.0/main)
add_subdirectory(Dependencies/PagedGeometry)
add_subdirectory(Dependencies/Angelscript)
#add_subdirectory(Dependencies/lua)
add_subdirectory(Dependencies/SkyX)
add_subdirectory(Dependencies/OFS)
add_subdirectory(Dependencies/GenericTextEditor)
add_subdirectory(Dependencies/GenericImageEditor)
IF(OGITOR_MESHMAGICK_INTERFACE)
    add_subdirectory(Dependencies/MeshMagick)
ENDIF(OGITOR_MESHMAGICK_INTERFACE)


# plugins
add_subdirectory(Plugins/OgCaelum)
add_subdirectory(Plugins/OgHydrax)
add_subdirectory(Plugins/OgSkyx)
add_subdirectory(Plugins/DotSceneSerializer)
add_subdirectory(Plugins/OgModularZone)
add_subdirectory(Plugins/OgMaterialEditor)
add_subdirectory(Plugins/OgAngelScript)
add_subdirectory(Plugins/OgScriptEditor)
add_subdirectory(Plugins/OgAngelDialogManager)

add_subdirectory(OgreOfsPlugin)

add_subdirectory(qtOgitor/qtpropertybrowser-2.5)

add_subdirectory(qtOgitor)

add_subdirectory(qtOfs)

add_subdirectory(SceneLoader)

add_subdirectory(RunPath)

#add_subdirectory(Help)

add_custom_target(lupdate
    COMMAND ${PROJECT_SOURCE_DIR}/gen-qt-projectfile ${PROJECT_SOURCE_DIR} > ${PROJECT_SOURCE_DIR}/qtOgitor/languages/translate.pro
    COMMAND lupdate -noobsolete ${PROJECT_SOURCE_DIR}/qtOgitor/languages/translate.pro
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/qtOgitor/languages
)


if(WIN32)
    install(FILES ${OGRE_PLUGIN_DIR_REL}/OgreMain.dll
        ${OGRE_PLUGIN_DIR_REL}/RenderSystem_Direct3D9.dll
        ${OGRE_PLUGIN_DIR_REL}/RenderSystem_GL.dll
        ${OGRE_PLUGIN_DIR_REL}/Plugin_OctreeSceneManager.dll
        ${OGRE_PLUGIN_DIR_REL}/cg.dll
        ${OGRE_PLUGIN_DIR_REL}/Plugin_CgProgramManager.dll
        ${OGRE_PLUGIN_DIR_REL}/Plugin_ParticleFX.dll
        ${OGRE_PLUGIN_DIR_REL}/OgreTerrain.dll
        ${OGRE_PLUGIN_DIR_REL}/OgrePaging.dll
        DESTINATION bin
        CONFIGURATIONS Release)

    install(FILES ${OGRE_PLUGIN_DIR_DBG}/OgreMain_d.dll
        ${OGRE_PLUGIN_DIR_DBG}/RenderSystem_Direct3D9_d.dll
        ${OGRE_PLUGIN_DIR_DBG}/RenderSystem_GL_d.dll
        ${OGRE_PLUGIN_DIR_DBG}/Plugin_OctreeSceneManager_d.dll
        ${OGRE_PLUGIN_DIR_DBG}/cg.dll
        ${OGRE_PLUGIN_DIR_DBG}/Plugin_CgProgramManager_d.dll
        ${OGRE_PLUGIN_DIR_DBG}/Plugin_ParticleFX_d.dll
        ${OGRE_PLUGIN_DIR_DBG}/OgreTerrain_d.dll
        ${OGRE_PLUGIN_DIR_DBG}/OgrePaging_d.dll
        #${OGRE_PLUGIN_DIR_DBG}/OgreMain_d.pdb
        #${OGRE_PLUGIN_DIR_DBG}/RenderSystem_Direct3D9_d.pdb
        #${OGRE_PLUGIN_DIR_DBG}/RenderSystem_GL_d.pdb
        #${OGRE_PLUGIN_DIR_DBG}/Plugin_OctreeSceneManager_d.pdb
        #${OGRE_PLUGIN_DIR_DBG}/Plugin_CgProgramManager_d.pdb
        #${OGRE_PLUGIN_DIR_DBG}/Plugin_BSPSceneManager_d.pdb
        #${OGRE_PLUGIN_DIR_DBG}/Plugin_ParticleFX_d.pdb
        #${OGRE_PLUGIN_DIR_DBG}/OgreTerrain_d.pdb
        #${OGRE_PLUGIN_DIR_DBG}/OgrePaging_d.pdb
        DESTINATION bin
        CONFIGURATIONS Debug)

    IF(OGITOR_DIST)
        install(FILES redist/dxwebsetup.exe
            DESTINATION redist
            CONFIGURATIONS Release)
        install(FILES redist/msvcr100.dll
            redist/msvcp100.dll
            DESTINATION bin
            CONFIGURATIONS Release)
    ENDIF(OGITOR_DIST)

endif(WIN32)

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

    set(CPACK_INSTALL_CMAKE_PROJECTS "${OGITOR_WORK_DIR};Ogitor;ALL;/")
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
        CopyFiles \\\"$INSTDIR\\\\bin\\\\plugins.cfg\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
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
