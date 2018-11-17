set(EXECUTABLE_CGIs
    index.cpp
    approver.cpp
    agency.cpp
    subcontractor.cpp
    account.cpp
    avataruploader.cpp
    generalimageuploader.cpp
    imageuploader.cpp
    system.cpp
    ajax_anyrole_1.cpp
#    admin.cpp
    )

# --- Libraries
# ---- big joint librabry
set(JOINT_LIBRARY_SRCs
    ccgi.cpp
    crequest.cpp
    ccookie.cpp
    cmail.cpp
    cactivator.cpp
    csession.cpp
    chtml.cpp
    utilities_common.cpp
    utilities_timecard1.cpp
    utilities2.cpp
    utilities1.cpp
    cvideoconverter.cpp
    c_bt.cpp
    cuser.cpp
    )

# ---- list of independent libraries
set(INDEPENDENT_LIBRARIES_SRC
    cvars.cpp
    cmysql.cpp
    cexception.cpp
    ctemplate.cpp
    cfiles.cpp
    cstatistics.cpp
    c_float.cpp
    )


# --- Packages and libraries control
include(FindPkgConfig)

pkg_check_modules(LIBMYSQLCLIENT REQUIRED mysqlclient)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
find_package(LibWebSockets REQUIRED)

if(IMAGEMAGICK_DISABLE)
    message("Magick++ turned off")
else(IMAGEMAGICK_DISABLE)
    find_package(ImageMagick COMPONENTS Magick++)
endif(IMAGEMAGICK_DISABLE)

if(FFPMPEG_DISABLE)
    message("Videoconverter/ffmpeg turned off")
else(FFPMPEG_DISABLE)
    find_package(avutil REQUIRED)
    find_package(avcodec REQUIRED)
    find_package(avformat REQUIRED)
endif(FFPMPEG_DISABLE)

if(MAXMIND_DISABLE)
    message("GeoIP turned off")
else(MAXMIND_DISABLE)
    find_package(maxminddb REQUIRED)
endif(MAXMIND_DISABLE)

include_directories(
    ${PROJECT_SOURCE_DIR}/src/include/
    ${PROJECT_BINARY_DIR}/src/include/ # --- rendered config.h, generated table.h
    ${LIBMYSQLCLIENT_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    ${ImageMagick_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIR}
    ${AVUTIL_INCLUDE_DIR}
    ${AVFORMAT_INCLUDE_DIR}
    )

link_directories(
    ${LIBMYSQLCLIENT_LIBDIR}
    )

link_libraries(
    ${LIBMYSQLCLIENT_LIBRARIES}
    ${OPENSSL_CRYPTO_LIBRARIES}
    ${ImageMagick_LIBRARIES}
    ${CURL_LIBRARIES}
    ${AVUTIL_LIBRARY}
    ${AVFORMAT_LIBRARY}
    ${MAXMINDDB_LIBRARY}
    )

# --- does this system provide the exp function ?
include(CheckFunctionExists)
check_function_exists(exp HAVE_EXP)

configure_file(
    "${PROJECT_SOURCE_DIR}/src/include/localy.h.in"
    "${PROJECT_BINARY_DIR}/src/include/localy.h"
    )


# ---- big joint librabry
add_library(__joint_lib		${JOINT_LIBRARY_SRCs})
set(EXTRA_LIBS			${EXTRA_LIBS} __joint_lib)

# ---- list of independent libraries
foreach(INDEPENDENT_LIBRARY_SRC ${INDEPENDENT_LIBRARIES_SRC})
    string(REGEX REPLACE "(^.*/|\\.[^.]*$)" "" file_without_ext ${INDEPENDENT_LIBRARY_SRC})
    add_library(${file_without_ext}		${file_without_ext}.cpp)
    set(EXTRA_LIBS				${EXTRA_LIBS} ${file_without_ext})
#    message("--- ${file_without_ext}")
endforeach()

# --- MSVC grouping
source_group("Headers Private"  FILES ${HDR_PRIVATE})
source_group("Headers Public"   FILES ${HDR_PUBLIC})
source_group("Sources"          FILES ${SOURCES})
source_group("Resources"        FILES ${RESOURCES})

# --- Executables sources

foreach(EXECUTABLE_CGI ${EXECUTABLE_CGIs})
    string(REGEX REPLACE "(^.*/|\\.[^.]*$)" "" file_without_ext ${EXECUTABLE_CGI})
    add_executable(${file_without_ext}.cgi		${file_without_ext}.cpp	${SOURCES})
    target_link_libraries(${file_without_ext}.cgi	${EXTRA_LIBS})
    # message("=== ${file_without_ext}")
endforeach()

add_executable(cron_daily			cron_daily.cpp			${SOURCES})
add_executable(cron_minute			cron_minute.cpp			${SOURCES})

target_link_libraries(cron_daily		${EXTRA_LIBS})
target_link_libraries(cron_minute		${EXTRA_LIBS})

# --- Install part
install(TARGETS index.cgi			DESTINATION		${CGIDIR})
install(TARGETS approver.cgi			DESTINATION		${CGIDIR})
install(TARGETS agency.cgi			DESTINATION		${CGIDIR})
install(TARGETS subcontractor.cgi		DESTINATION		${CGIDIR})
install(TARGETS account.cgi			DESTINATION		${CGIDIR})
install(TARGETS avataruploader.cgi		DESTINATION		${CGIDIR})
install(TARGETS generalimageuploader.cgi	DESTINATION		${CGIDIR})
install(TARGETS imageuploader.cgi		DESTINATION		${CGIDIR})
install(TARGETS system.cgi			DESTINATION		${CGIDIR})
install(TARGETS ajax_anyrole_1.cgi		DESTINATION		${CGIDIR})
install(TARGETS admin.cgi			DESTINATION		${CGIADMINDIR})

install(TARGETS cron_daily			DESTINATION		${CRON_INSTALL_DIR})
install(TARGETS cron_minute			DESTINATION		${CRON_INSTALL_DIR})

# --- MACROSes start ---

macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()

# --- MACROSes finish ---


#print_all_variables()

