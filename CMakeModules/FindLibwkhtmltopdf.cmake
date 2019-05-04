# Locate HPDF library
# This module defines
# HPDF_LIBRARY, the name of the library to link against
# HPDF_FOUND, if false, do not try to link to HPDF
# HPDF_INCLUDE_DIR, where to find hpdf.h
#

message(STATUS "looking for libwkhtmltopdf-dev")

set( WKHTMLTOPDF_FOUND OFF )

find_path( WKHTMLTOPDF_INCLUDE_DIR wkhtmltox/pdf.h
  HINTS
  $ENV{WKHTMLTOPDFDIR}
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

message( "WKHTMLTOPDF_INCLUDE_DIR is ${WKHTMLTOPDF_INCLUDE_DIR}" )

find_library( WKHTMLTOPDF_LIBRARY
  NAMES wkhtmltox
  HINTS
  $ENV{WKHTMLTOPDFDIR}
  PATH_SUFFIXES lib64 lib bin
  PATHS
  /usr/local/lib
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
  /mingw
)

message( "WKHTMLTOPDF_LIBRARY is ${WKHTMLTOPDF_LIBRARY}" )

if(WKHTMLTOPDF_INCLUDE_DIR)
    if(WKHTMLTOPDF_LIBRARY)
	set( WKHTMLTOPDF_FOUND "YES" )
    endif()
endif()


if(WKHTMLTOPDF_FOUND)
    message(STATUS "looking for libwkhtmltopdf-dev - done")
else()
    message(FATAL_ERROR "ERROR: libwkhtmltopdf-dev not installed")
endif()
