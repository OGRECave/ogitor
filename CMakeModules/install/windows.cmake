# SDK : main libs - debug
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib/Debug
	DESTINATION ${PREFIX}/sdk/lib
	CONFIGURATIONS Debug
)
# SDK : main libs - release
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib/Release
	DESTINATION ${PREFIX}/sdk/lib
	CONFIGURATIONS Release RelWithDebInfo MinSizeRel
)
# SDK : Ogitor includes
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Ogitor/include
	DESTINATION ${PREFIX}/sdk
	CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
)
# SDK : Dependencies libs
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Dependencies/lib
	DESTINATION ${PREFIX}/sdk/dependencies
	CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
)
# SDK: OFS headers
FILE(GLOB ofs_headers "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/OFS/include/*.h")
install(FILES ${ofs_headers}
	DESTINATION ${PREFIX}/sdk/dependencies/include/OFS
	CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
)
# SDK: ImageConverter headers
FILE(GLOB ImageConverter_headers "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/ImageConverter/include/*.hxx")
install(FILES ${ImageConverter_headers}
	DESTINATION ${PREFIX}/sdk/dependencies/include/ImageConverter
	CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
)
# SDK: GenericImageEditor headers
FILE(GLOB GenericImageEditor_headers "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/GenericImageEditor/include/*.hxx")
install(FILES ${GenericImageEditor_headers}
	DESTINATION ${PREFIX}/sdk/dependencies/include/GenericImageEditor
	CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
)
# SDK: GenericTextEditor headers
FILE(GLOB GenericTextEditor_headers "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/GenericTextEditor/include/*.hxx")
install(FILES ${GenericTextEditor_headers}
	DESTINATION ${PREFIX}/sdk/dependencies/include/GenericTextEditor
	CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
)
# SDK: Hydrax headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/HYDRAX/Hydrax/src/Hydrax
	DESTINATION ${PREFIX}/sdk/dependencies/include
	CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
	PATTERN "*.cpp" EXCLUDE
	PATTERN "CMakeLists.txt" EXCLUDE
)
# SDK: SkyX headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/SkyX
	DESTINATION ${PREFIX}/sdk/dependencies/include
	CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
	PATTERN "*.cpp" EXCLUDE
	PATTERN "CMakeLists.txt" EXCLUDE
)
# SDK: PagedGeometry headers
FILE(GLOB pg_headers "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/PagedGeometry/include/*.h")
install(FILES ${pg_headers}
	DESTINATION ${PREFIX}/sdk/dependencies/include/OFS
	CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
)
# SDK: Angelscript headers
FILE(GLOB angelscript_headers "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/include/*.h")
install(FILES ${angelscript_headers}
	DESTINATION ${PREFIX}/sdk/dependencies/include/Angelscript
	CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/add_on
	DESTINATION ${PREFIX}/sdk/dependencies/include/Angelscript
	CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
	PATTERN "*.cpp" EXCLUDE
	PATTERN "*.dsp" EXCLUDE
	PATTERN "*.dsw" EXCLUDE
	PATTERN "*.asm" EXCLUDE
	PATTERN "*.txt" EXCLUDE
	PATTERN "CMakeLists.txt" EXCLUDE
)
# SDK: Caelum headers
FILE(GLOB caelum_headers "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Caelum-0.5.0/main/include/*.h")
install(FILES ${caelum_headers}
	DESTINATION ${PREFIX}/sdk/dependencies/include/Caelum-0.5.0
	CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
)

# TODO: MeshMagick
# TODO: lua
# TODO: PythonQt

# Ogre DLLs
if(EXISTS ${OGRE_Overlay_BINARY_REL})
    install(FILES ${OGRE_Overlay_BINARY_REL} DESTINATION bin CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
endif()
if(EXISTS ${OGRE_RenderSystem_Direct3D11_REL})
    install(FILES ${OGRE_RenderSystem_Direct3D11_REL} DESTINATION bin CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
else()
    set(OGRE_COMMENT_RENDERSYSTEM_D3D11_REL "#")
endif()
if(EXISTS ${OGRE_RenderSystem_Direct3D9_REL})
    install(FILES ${OGRE_RenderSystem_Direct3D9_REL} DESTINATION bin CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
else()
    set(OGRE_COMMENT_RENDERSYSTEM_D3D9_REL "#")
endif()
if(EXISTS ${OGRE_RenderSystem_GL3Plus_REL})
    install(FILES ${OGRE_RenderSystem_GL3Plus_REL} DESTINATION bin CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
else()
    set(OGRE_COMMENT_RENDERSYSTEM_GL3PLUS_REL "#")
endif()
if(EXISTS ${OGRE_RenderSystem_GL_REL})
    install(FILES ${OGRE_RenderSystem_GL_REL} DESTINATION bin CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
else()
    set(OGRE_COMMENT_RENDERSYSTEM_GL_REL "#")
endif()
install(FILES ${OGRE_BINARY_REL}
    ${OGRE_Plugin_OctreeSceneManager_REL}
    ${OGRE_PLUGIN_DIR_REL}/cg.dll
    ${OGRE_Plugin_CgProgramManager_REL}
    ${OGRE_Plugin_ParticleFX_REL}
    ${OGRE_Terrain_BINARY_REL}
    ${OGRE_Paging_BINARY_REL}
    DESTINATION bin
    CONFIGURATIONS Release RelWithDebInfo MinSizeRel)

if(EXISTS ${OGRE_Overlay_BINARY_DBG})
    install(FILES ${OGRE_Overlay_BINARY_DBG} DESTINATION bin CONFIGURATIONS Debug)
endif()
if(EXISTS ${OGRE_RenderSystem_Direct3D11_DBG})
    install(FILES ${OGRE_RenderSystem_Direct3D11_DBG} DESTINATION bin CONFIGURATIONS Debug)
else()
    set(OGRE_COMMENT_RENDERSYSTEM_D3D11_DBG "#")
endif()
if(EXISTS ${OGRE_RenderSystem_Direct3D9_DBG})
    install(FILES ${OGRE_RenderSystem_Direct3D9_DBG} DESTINATION bin CONFIGURATIONS Debug)
else()
    set(OGRE_COMMENT_RENDERSYSTEM_D3D9_DBG "#")
endif()
if(EXISTS ${OGRE_RenderSystem_GL3Plus_DBG})
    install(FILES ${OGRE_RenderSystem_GL3Plus_DBG} DESTINATION bin CONFIGURATIONS Debug)
else()
    set(OGRE_COMMENT_RENDERSYSTEM_GL3PLUS_REL "#")
endif()
if(EXISTS ${OGRE_RenderSystem_GL_DBG})
    install(FILES ${OGRE_RenderSystem_GL_DBG} DESTINATION bin CONFIGURATIONS Debug)
else()
    set(OGRE_COMMENT_RENDERSYSTEM_GL_DBG "#")
endif()
install(FILES ${OGRE_BINARY_DBG}
    ${OGRE_Plugin_OctreeSceneManager_DBG}
    ${OGRE_PLUGIN_DIR_DBG}/cg.dll
    ${OGRE_Plugin_CgProgramManager_DBG}
    ${OGRE_Plugin_ParticleFX_DBG}
    ${OGRE_Terrain_BINARY_DBG}
    ${OGRE_Paging_BINARY_DBG}
    DESTINATION bin
    CONFIGURATIONS Debug)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/Templates/plugins.wincfg.in ${CMAKE_CURRENT_SOURCE_DIR}/RunPath/bin/plugins.cfg)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/Templates/plugins_d.wincfg.in ${CMAKE_CURRENT_SOURCE_DIR}/RunPath/bin/plugins_debug.cfg)

IF(OGITOR_DIST)
	if(EXISTS redist/dxwebsetup.exe)
		install(FILES redist/dxwebsetup.exe
			DESTINATION redist
			CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
	endif()
	if(EXISTS redist/msvcr100.dll)
		install(FILES redist/msvcr100.dll
			redist/msvcp100.dll
			DESTINATION bin
			CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
	endif()
ENDIF(OGITOR_DIST)
