
cmake_minimum_required( VERSION 2.8 )

project( visualinfo )

include( ExternalProject )

if( ${CMAKE_TOOLCHAIN_FILE} )
    set( TOOL_FILE ${CMAKE_TOOLCHAIN_FILE} )
endif()

set( DOWNLOAD_DIR ".downloads-by-cmake" )

ExternalProject_Add( libglew
    DOWNLOAD_DIR ${DOWNLOAD_DIR}
    URL http://www.valvers.com/files/glew-1.10.0-src.tar.bz2
    URL_MD5 2c38d9306a81df0bb282510954f9b9f6
    CONFIGURE_COMMAND
        ${CMAKE_COMMAND}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_TOOLCHAIN_FILE=${TOOL_FILE}
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            <SOURCE_DIR>

    BUILD_IN_SOURCE 1
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
    INSTALL_DIR "${PROJECT_BINARY_DIR}/glew"
    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install )

add_library( glews STATIC IMPORTED )
add_dependencies( glews libglew )

if( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )
    set_property( TARGET glews PROPERTY IMPORTED_LOCATION "${PROJECT_BINARY_DIR}/glew/lib/libglews.a" )
elseif( MSVC )
    set_property( TARGET glews PROPERTY IMPORTED_LOCATION "${PROJECT_BINARY_DIR}/glew/lib/libglews.lib" )
endif()

if( WIN32 )
    list( APPEND MLIBS opengl32 )
    list( APPEND MLIBS gdi32 )
    list( APPEND MLIBS user32 )
    list( APPEND MLIBS kernel32 )
endif()

add_executable( visualinfo
    ${PROJECT_SOURCE_DIR}/src/visualinfo.c )

target_link_libraries( visualinfo glews ${MLIBS} )
