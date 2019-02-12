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
    admin.cpp
    )

# --- Libraries
# ---- big joint librabry
set(JOINT_LIBRARY_SRCs
    utilities_common.cpp
    utilities_timecard1.cpp
    utilities2.cpp
    utilities1.cpp
    )

# ---- list of independent libraries
set(INDEPENDENT_LIBRARIES_SRC
    cuser.cpp
    cactivator.cpp
    ccgi.cpp
    ccookie.cpp
    csession.cpp
    crequest.cpp
    chtml.cpp
    cmail.cpp
    cvars.cpp
    cmysql.cpp
    cexception.cpp
    ctemplate.cpp
    cfiles.cpp
    cstatistics.cpp
    c_bt.cpp
    c_float.cpp
    c_invoice_service.cpp
    cvideoconverter.cpp
    c_timecard_to_print.cpp
    c_print_timecard.cpp
    )

# --- MACROSes start ---

macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")

    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()

    message(STATUS "print_all_variables------------------------------------------}")
endmacro()

macro(set_lib_properties target_lib proj_src proj_inc_list proj_lib_list)
    # message(STATUS "set_lib_properties ${target_lib}")

    add_library(		${target_lib}			${proj_src})
    target_include_directories(	${target_lib}		PUBLIC	${proj_inc_list})
    target_link_libraries(	${target_lib}			${proj_lib_list})

    set(			PROJECT_LIBS		${PROJECT_LIBS} ${target_lib})
endmacro()

# --- MACROSes finish ---


# --- Packages and libraries control
include(FindPkgConfig)

pkg_check_modules(LIBMYSQLCLIENT REQUIRED mysqlclient)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
find_package(LibWebSockets REQUIRED)
find_package(Libhpdf REQUIRED)
find_package(Libxl REQUIRED)

if(IMAGEMAGICK_DISABLE)
    message("Magick++ turned off")
else(IMAGEMAGICK_DISABLE)
#    find_package(ImageMagick COMPONENTS Magick++)
    find_package(ImageMagick REQUIRED)
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

# --- install option
# --- this setting allows to dynalic linker find .so library in custom location
# --- otherwise it have to be added to ld.so.conf or environment variable
# --- actual linker options are -Wl,-rpath,/path/to/dynamic/library
# --- https://gitlab.kitware.com/cmake/community/wikis/doc/cmake/RPATH-handling
set(CMAKE_INSTALL_RPATH		${XL_LIBRARY_DIR})

set(project_include_directories
    ${PROJECT_SOURCE_DIR}/src/include/
    ${PROJECT_BINARY_DIR}/src/include/      # --- rendered config.h, generated table.h
    ${PROJECT_SOURCE_DIR}/cyrillic-facets/  # --- mbstowcs 
    ${LIBMYSQLCLIENT_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    ${IMAGEMAGICK_INCLUDE_DIR}
    ${CURL_INCLUDE_DIR}
    ${AVUTIL_INCLUDE_DIR}
    ${AVFORMAT_INCLUDE_DIR}
    ${HPDF_INCLUDE_DIR}
    ${XL_INCLUDE_DIR}
    )

set(project_link_libraries
    ${LIBMYSQLCLIENT_LIBRARIES}
    ${OPENSSL_CRYPTO_LIBRARIES}
    ${IMAGEMAGICK_LIBRARY}
    ${CURL_LIBRARIES}
    ${AVUTIL_LIBRARY}
    ${AVFORMAT_LIBRARY}
    ${MAXMINDDB_LIBRARY}
    ${HPDF_LIBRARY}
    ${XL_LIBRARY}
    )

# --- does this system provide the exp function ?
include(CheckFunctionExists)
check_function_exists(exp HAVE_EXP)

configure_file(
    "${PROJECT_SOURCE_DIR}/src/include/localy.h.in"
    "${PROJECT_BINARY_DIR}/src/include/localy.h"
    )


# ---- big joint librabry
set_lib_properties(__joint_lib "${JOINT_LIBRARY_SRCs}" "${project_include_directories}" "${project_link_libraries}")

# ---- list of independent libraries
foreach(INDEPENDENT_LIBRARY_SRC ${INDEPENDENT_LIBRARIES_SRC})
    string(REGEX REPLACE "(^.*/|\\.[^.]*$)" "" file_without_ext ${INDEPENDENT_LIBRARY_SRC})

    set_lib_properties(${file_without_ext} "${file_without_ext}.cpp" "${project_include_directories}" "${project_link_libraries}")
endforeach()

# --- MSVC grouping
source_group("Headers Private"  FILES ${HDR_PRIVATE})
source_group("Headers Public"   FILES ${HDR_PUBLIC})
source_group("Sources"          FILES ${SOURCES})
source_group("Resources"        FILES ${RESOURCES})

# --- Executables sources
foreach(EXECUTABLE_CGI ${EXECUTABLE_CGIs})
    string(REGEX REPLACE "(^.*/|\\.[^.]*$)" "" file_without_ext ${EXECUTABLE_CGI})

    add_executable(		${file_without_ext}.cgi		${file_without_ext}.cpp	${SOURCES})
    target_link_libraries(	${file_without_ext}.cgi		${PROJECT_LIBS})
    target_compile_features(${file_without_ext}.cgi
        PUBLIC
            cxx_strong_enums
        PRIVATE
            cxx_range_for
    )
    # message("=== ${file_without_ext}")
endforeach()

add_executable(cron_daily			cron_daily.cpp		${SOURCES})
add_executable(cron_minute			cron_minute.cpp		${SOURCES})

target_link_libraries(cron_daily		${PROJECT_LIBS})
target_link_libraries(cron_minute		${PROJECT_LIBS})

if(MSVC)
    target_compile_options(cron_daily		PRIVATE	/W4 /WX)
else(MSVC)
    target_compile_options(cron_minute 		PRIVATE	-Wall -Wextra -pedantic)
endif(MSVC)


# --- Install settings
#set(CMAKE_INSTALL_RPATH		${XL_LIBRARY_DIR})

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
#install(TARGETS admin.cgi			DESTINATION		${CGIADMINDIR})

install(TARGETS cron_daily			DESTINATION		${CRON_INSTALL_DIR})
install(TARGETS cron_minute			DESTINATION		${CRON_INSTALL_DIR})



# print_all_variables()

