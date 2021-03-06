# This module tries to find libraries and include files
#
# <PREFIX>_FOUND                    do not use library, if false
# <PREFIX>_INCLUDE_DIRS             path where to find headers
# <PREFIX>_LIBRARY_DIRS             path where to find dynamic lib
# <PREFIX>_LIBRARIES                dynamic libraries to link
# <PREFIX>_STATIC_INCLUDE_DIRS      path where to find static headers
# <PREFIX>_STATIC_LIBRARY_DIRS      path where to find static lib
# <PREFIX>_STATIC_LIBRARIES         static libraries to link
#
# This works only on Linux

message(STATUS "looking for libhpdf-dev")

find_path( HPDF_INCLUDE_DIR hpdf.h
  HINTS
  $ENV{HPDFDIR}
  PATH_SUFFIXES include 
  PATHS
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

find_path( HPDF_STATIC_INCLUDE_DIR hpdf.h
  HINTS
  $ENV{HPDFDIR}
  PATH_SUFFIXES include 
  PATHS
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

#message( "HPDF_INCLUDE_DIR is ${HPDF_INCLUDE_DIR}" )

find_library( HPDF_LIBRARY
  NAMES hpdf
  HINTS
  $ENV{HPDFDIR}
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

#message( "HPDF_LIBRARY is ${HPDF_LIBRARY}" )

#find_library( HPDF_STATIC_LIBRARY
#  NAMES libhpdf.a
#  HINTS
#  $ENV{HPDFDIR}
#  PATH_SUFFIXES lib64 lib bin
#  PATHS
#  /usr/local
#  /usr
#  /sw
#  /opt/local
#  /opt/csw
#  /opt
#  /mingw
#)

SET ( HPDF_FOUND OFF )
IF ( HPDF_INCLUDE_DIR )
    IF ( HPDF_STATIC_INCLUDE_DIR )
        IF ( HPDF_LIBRARY )
#            IF ( HPDF_STATIC_LIBRARY )
                SET ( HPDF_FOUND "YES" )
#            ELSE ()
#                message(STATUS "static library not found")
#            ENDIF ()
        ELSE ()
            message(STATUS "dynamic library not found")
        ENDIF ()
    ELSE ()
        message(STATUS "include static dirs not found")
    ENDIF ()
ELSE ()
    message(STATUS "include dirs not found")
ENDIF ()


if(HPDF_FOUND)
    message(STATUS "looking for libhpdf-dev - done")
else()
    message(FATAL_ERROR "ERROR: libhpdf-dev not installed")
endif()
