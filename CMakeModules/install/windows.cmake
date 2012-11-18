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
