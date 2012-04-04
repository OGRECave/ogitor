prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=${CMAKE_INSTALL_PREFIX}
libdir=${CMAKE_INSTALL_PREFIX}/lib
includedir=${CMAKE_INSTALL_PREFIX}/include/meshmagick

Name: meshmagick
Description: ogre mesh manipulation utility
Requires: OGRE >= 1.6
Version: ${MESHMAGICK_MAJOR_VERSION}.${MESHMAGICK_MINOR_VERSION}.${MESHMAGICK_PATCH_VERSION}
Libs: -L${CMAKE_INSTALL_PREFIX}/lib -lmeshmagick
Cflags: -I${CMAKE_INSTALL_PREFIX}/include/meshmagick
