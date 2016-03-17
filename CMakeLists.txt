cmake_minimum_required(VERSION 2.6)

PROJECT(Gino)
SET (AUTHOR "Fabio Di Matteo" INTERNAL "Author")
SET (VERSION "0.1.0")

INCLUDE (FindPkgConfig)

IF (NOT PKG_CONFIG_FOUND)
	MESSAGE (FATAL_ERROR "pkg-config not found...")
ENDIF (NOT PKG_CONFIG_FOUND)

pkg_check_modules (GIO REQUIRED gio-2.0>=2.0)

IF (NOT GIO_FOUND)
	MESSAGE(FATAL_ERROR "You don't seem to have GIO-2 >= 2.0 development libraries installed...")
ENDIF (NOT GIO_FOUND)

ADD_DEFINITIONS()


#Include
INCLUDE_DIRECTORIES (. ${GIO_INCLUDE_DIRS})


LINK_DIRECTORIES (${GIO_LIBRARY_DIRS} )
LINK_LIBRARIES (${GIO_LIBRARIES} )


ADD_EXECUTABLE(gino src/utils.c src/main.c)

# for uninstall use: 'xargs rm < install_manifest.txt'
SET(CMAKE_INSTALL_PREFIX /usr)
INSTALL (TARGETS gino DESTINATION bin)

