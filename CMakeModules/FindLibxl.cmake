# Locate XL library
# This module defines
# XL_LIBRARY, the name of the library to link against
# XL_FOUND, if false, do not try to link to XL
# XL_INCLUDE_DIR, where to find libxl.h
#

message(STATUS "looking for libxl-dev")

set( XL_FOUND OFF )

find_path( XL_INCLUDE_DIR libxl.h
  HINTS
  $ENV{XLDIR}
  PATH_SUFFIXES include 
  PATHS
  ${CMAKE_SOURCE_DIR}/libxl-3.8.4.0/include_cpp
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local/include
  /usr/include
  /sw/include
  /opt/local/include
  /opt/csw/include 
  /opt/include
  /mingw/include
)

 message( "XL_INCLUDE_DIR is ${XL_INCLUDE_DIR}" )

EXECUTE_PROCESS( COMMAND getconf LONG_BIT COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )

if( ${ARCHITECTURE} STREQUAL "64" )
    set( XL_LIBRARY_TEST ${CMAKE_SOURCE_DIR}/libxl-3.8.4.0/lib64 )
else()
    set( XL_LIBRARY_TEST ${CMAKE_SOURCE_DIR}/libxl-3.8.4.0/lib )
endif()

 message( "XL_LIBRARY_TEST is ${XL_LIBRARY_TEST}" )

find_library( XL_LIBRARY
  NAMES xl
  HINTS
  "${XL_LIBRARY_TEST}"
  $ENV{XLDIR}
  PATH_SUFFIXES lib64 lib bin
  PATHS
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
  /mingw
)

 message( "XL_LIBRARY is ${XL_LIBRARY}" )

if(XL_INCLUDE_DIR)
    if(XL_LIBRARY)
	set( XL_FOUND "YES" )
    endif()
endif()


if(XL_FOUND)
    message(STATUS "looking for libxl-dev - done")
else()
    message(FATAL_ERROR "ERROR: libxl-dev not installed")
endif()
