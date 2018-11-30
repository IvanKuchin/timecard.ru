# --- in-source build prohibited

if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt." )
endif()

# ---------------------------

cmake_minimum_required(VERSION 3.5)

project("timecard.su" CXX)
set(My_VERSION_MAJOR 0)
set(My_VERSION_MINOR 1)

set(CMAKE_CXX_STANDARD 14)
# --- changing CXX_FLAGS is _not_ the best practise
# --- cmake 3.12 have -D switch, capable remove definition per-target
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -pedantic")

# --- Doesn't work properly, enable and give it a try
# --- set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "include-what-you-use")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/ )

option(IMAGEMAGICK_DISABLE	"turn off ImageMagick++" OFF)
option(MAXMIND_DISABLE		"turn off GeoIP" OFF)
option(FFPMPEG_DISABLE		"turn off VideoConverter" OFF)

# --- Set required variables
# ---- build variables
set(CRON_PROJECT_DIR		${PROJECT_BINARY_DIR}/share/${PROJECT_NAME}/cron)
set(ARCHIVE_PROJECT_DIR		${PROJECT_BINARY_DIR}/share/${PROJECT_NAME}/archive)
set(MAXMIND_PROJECT_DIR		${PROJECT_BINARY_DIR}/share/${PROJECT_NAME}/maxmind)
set(CI_SCRIPT_PROJECT_DIR	${PROJECT_BINARY_DIR}/share/${PROJECT_NAME}/ci-scripts)

# ---- install variables
set(ROOTDIR		"/home/httpd/dev.${PROJECT_NAME}")
set(IMAGEROOTDIR	"/home/httpd/devimages.${PROJECT_NAME}")
set(LOGDIR		"/home/ikuchin/src_logs/")
set(CGIDIR		${ROOTDIR}/cgi-bin)
set(CGIADMINDIR		${ROOTDIR}/cgi-bin/admin)
set(CRON_INSTALL_DIR	${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/cron)
set(ARCHIVE_INSTALL_DIR	${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/archive)
set(MAXMIND_INSTALL_DIR	${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/maxmind)

add_subdirectory(src/)
add_subdirectory(supplemental/cron/)
add_subdirectory(supplemental/archive/)
add_subdirectory(supplemental/maxmind/)
add_subdirectory(supplemental/ci-scripts/)

include(CTest)
add_subdirectory(tests/)

# --- CPack
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${My_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${My_VERSION_MINOR}")

include(CPack)

