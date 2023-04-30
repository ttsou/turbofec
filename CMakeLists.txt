cmake_minimum_required(VERSION 3.10)
project(turbofec VERSION 0.1)

include(ExternalProject)
set(turbofec_DESTDIR ${CMAKE_CURRENT_BINARY_DIR}/turbofec_install)
set(turbofec_configure ${CMAKE_CURRENT_SOURCE_DIR}/configure)
if( NOT EXISTS ${turbofec_configure} )
    message( FATAL_ERROR "The ${turbofec_configure}: not found.
    Please run:
        autoreconf -i ${CMAKE_CURRENT_SOURCE_DIR}
    ")
endif()

ExternalProject_Add(turbofec
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
    CONFIGURE_COMMAND ${turbofec_configure} --prefix=${CMAKE_INSTALL_PREFIX}
    BUILD_COMMAND ${MAKE}
    INSTALL_COMMAND     make DESTDIR=${turbofec_DESTDIR} install
)

install(DIRECTORY ${turbofec_DESTDIR}/${CMAKE_INSTALL_PREFIX}/ DESTINATION .)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
file(READ AUTHORS CPACK_DEBIAN_PACKAGE_MAINTAINER)
set(CPACK_GENERATOR DEB;STGZ)
set(CPACK_SOURCE_GENERATOR TXZ;DEB)
set(CPACK_PROJECT_CONFIG_FILE ${CMAKE_SOURCE_DIR}/cmake/package.txt)
set(CPACK_SOURCE_IGNORE_FILES
    autom4te.cache/
    \\.git
    \\.git/
    \\.gitignore
    ".*~$")
set(CPACK_VERBATIM_VARIABLES YES)
include(CPack)
