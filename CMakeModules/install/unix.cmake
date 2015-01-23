# SDK : Ogitor includes
file(GLOB ogitor_headers "${CMAKE_CURRENT_SOURCE_DIR}/Ogitor/include/*.h")
install(FILES ${ogitor_headers}
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/include/ogitor
    CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
)

# SDK: OFS headers
file(GLOB ofs_headers "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/OFS/include/*.h")
install(FILES ${ofs_headers}
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/include/OFS
    CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
)

# SDK: ImageConverter headers
file(GLOB ImageConverter_headers "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/ImageConverter/include/*.hxx")
install(FILES ${ImageConverter_headers}
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/include/ogitor/dependencies/ImageConverter
    CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
)

# SDK: GenericImageEditor headers
file(GLOB GenericImageEditor_headers "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/GenericImageEditor/include/*.hxx")
install(FILES ${GenericImageEditor_headers}
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/include/ogitor/dependencies/GenericImageEditor
    CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
)

# SDK: GenericTextEditor headers
file(GLOB GenericTextEditor_headers "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/GenericTextEditor/include/*.hxx")
install(FILES ${GenericTextEditor_headers}
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/include/ogitor/dependencies/GenericTextEditor
    CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
)

# SDK: Hydrax headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Hydrax/src
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ogitor/dependencies
    CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
    PATTERN "*.cpp" EXCLUDE
    PATTERN "CMakeLists.txt" EXCLUDE
)

# SDK: SkyX headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/SkyX
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ogitor/dependencies
    CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
    PATTERN "*.cpp" EXCLUDE
    PATTERN "CMakeLists.txt" EXCLUDE
)

# SDK: PagedGeometry headers
file(GLOB pg_headers "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/PagedGeometry/include/*.h")
install(FILES ${pg_headers}
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/include/ogitor/dependencies/PagedGeometry
    CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
)

# SDK: Angelscript headers
file(GLOB angelscript_headers "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/include/*.h")
install(FILES ${angelscript_headers}
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/include/ogitor/dependencies/Angelscript
    CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Angelscript/add_on
    DESTINATION
    ${CMAKE_INSTALL_PREFIX}/include/ogitor/dependencies/Angelscript
    CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
    PATTERN "*.cpp" EXCLUDE
    PATTERN "*.dsp" EXCLUDE
    PATTERN "*.dsw" EXCLUDE
    PATTERN "*.asm" EXCLUDE
    PATTERN "*.txt" EXCLUDE
    PATTERN "CMakeLists.txt" EXCLUDE
)

# SDK: Caelum headers
file(GLOB caelum_headers "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Caelum-0.5.0/main/include/*.h")
install(FILES ${caelum_headers}
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/include/ogitor/dependencies/Caelum-0.5.0
    CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
)

# TODO: MeshMagick
# TODO: lua
# TODO: PythonQt
