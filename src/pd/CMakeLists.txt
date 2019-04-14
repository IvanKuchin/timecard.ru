set(EXECUTABLE_CGIs_with_PD_PI_LIBS
    index.cgi
    approver.cgi
    agency.cgi
    subcontractor.cgi
    ajax_anyrole_1.cgi
    admin.cgi
    system.cgi
    )
set(EXECUTABLE_CGIs_with_PI_LIBS
    cron_minute
    cron_daily
    )

# --- Libraries
# ---- list of independent libraries
set(INVOICE_LIB_SRCs
    c_pdf.cpp
    c_invoice_service.cpp
    c_invoicing_vars.cpp
    c_print_timecard.cpp
    c_print_invoice_service.cpp
    c_print_vat_service.cpp
    c_timecard_to_print.cpp
    )

set(INDEPENDENT_LIBRARIES_SRCs
    )

set(JOINT_LIBRARY_SRCs
    c_bt.cpp
    c_company.cpp
    utilities_timecard1.cpp     # --- order is important, must follow c_bt
    utilities_timecard2.cpp
    )


# --- MACROSes start ---

macro(set_pd_lib_properties target_lib proj_src proj_inc_list proj_lib_list)
    add_library                 (${target_lib}      ${proj_src})
    target_include_directories  (${target_lib}      PUBLIC  ${proj_inc_list})
    target_link_libraries       (${target_lib}      ${proj_lib_list})

    set(PROJECT_PD_LIBS         ${PROJECT_PD_LIBS}  ${target_lib})
endmacro()

# --- MACROSes finish ---
GET_DIRECTORY_PROPERTY  (XL_LIBRARY_DIR DIRECTORY       ${PROJECT_SOURCE_DIR}/src/pi/ DEFINITION XL_LIBRARY_DIR)

GET_DIRECTORY_PROPERTY  (PROJECT_PI_INCLUDE_DIRECTORIES DIRECTORY ${PROJECT_SOURCE_DIR}/src/pi/ DEFINITION PROJECT_PI_INCLUDE_DIRECTORIES)
GET_DIRECTORY_PROPERTY  (PROJECT_PI_LIBS                DIRECTORY ${PROJECT_SOURCE_DIR}/src/pi/ DEFINITION PROJECT_PI_LIBS)
GET_DIRECTORY_PROPERTY  (PROJECT_EXTERNAL_LIBS          DIRECTORY ${PROJECT_SOURCE_DIR}/src/pi/ DEFINITION PROJECT_EXTERNAL_LIBS)

set(PROJECT_PD_INCLUDE_DIRECTORIES
    ${PROJECT_SOURCE_DIR}/src/pd/include/
    ${PROJECT_SOURCE_DIR}/cyrillic-facets/      # --- mbstowcs 
    )

SET(PROJECT_ALL_INCLUDE_DIRECTORIES ${PROJECT_PI_INCLUDE_DIRECTORIES} ${PROJECT_PD_INCLUDE_DIRECTORIES})

# --- Build libraries
# ---- list of independent libraries
foreach(INDEPENDENT_LIBRARY_SRC ${INDEPENDENT_LIBRARIES_SRCs})
    string(REGEX REPLACE "(^.*/|\\.[^.]*$)" "" file_without_ext ${INDEPENDENT_LIBRARY_SRC})

    set_pd_lib_properties(${file_without_ext} "${file_without_ext}.cpp" "${PROJECT_ALL_INCLUDE_DIRECTORIES}" "${project_link_libraries}")
endforeach()

# ---- joint lib
set_pd_lib_properties(__joint_pd "${JOINT_LIBRARY_SRCs}" "${PROJECT_ALL_INCLUDE_DIRECTORIES}" "${project_link_libraries}")


# ---- invoice lib
set_pd_lib_properties(__invoice "${INVOICE_LIB_SRCs}" "${PROJECT_ALL_INCLUDE_DIRECTORIES}" "${project_link_libraries}")
list(REMOVE_ITEM    PROJECT_PD_LIBS     __invoice)  # --- remove it from PD_LIBS to speed-up linking process

# --- MSVC grouping
source_group("Headers Private"  FILES ${HDR_PRIVATE})
source_group("Headers Public"   FILES ${HDR_PUBLIC})
source_group("Sources"          FILES ${SOURCES})
source_group("Resources"        FILES ${RESOURCES})

# --- Link part (2 parts: custom build, normal build)
# ---- 1) Custom build speeding up linking process by linking only required libraries (.o)

__add_executable_4libs(agency.cgi "${PROJECT_ALL_INCLUDE_DIRECTORIES}" "__invoice" "${PROJECT_PD_LIBS}" "${PROJECT_PI_LIBS}" "${PROJECT_EXTERNAL_LIBS}")
list(REMOVE_ITEM EXECUTABLE_CGIs_with_PD_PI_LIBS agency.cgi)

# ---- 2) Normal build link with _all_ libraries (.o), therefore change in any .src trigger re-link for all executables
foreach(EXECUTABLE_CGI ${EXECUTABLE_CGIs_with_PI_LIBS})
    __add_executable_2libs(${EXECUTABLE_CGI} "${PROJECT_ALL_INCLUDE_DIRECTORIES}" "${PROJECT_PI_LIBS}" "${PROJECT_EXTERNAL_LIBS}")
endforeach()
foreach(EXECUTABLE_CGI ${EXECUTABLE_CGIs_with_PD_PI_LIBS})
    __add_executable_3libs(${EXECUTABLE_CGI} "${PROJECT_ALL_INCLUDE_DIRECTORIES}" "${PROJECT_PD_LIBS}" "${PROJECT_PI_LIBS}" "${PROJECT_EXTERNAL_LIBS}")
endforeach()


# --- Install settings
# --- RPATH customization used only for libxl. LibXL didn't installed like any other OS-libraries
# --- it is downloaded from web-site and placed to src-tree. 
# --- During "cmake .." it is copied to /usr/local/share/... and then RPATH customized.
# ---"
# --- this setting allows to dynalic linker find .so library in custom location
# --- otherwise it have to be added to ld.so.conf or environment variable
# --- actual linker options are -Wl,-rpath,/path/to/dynamic/library
# --- https://gitlab.kitware.com/cmake/community/wikis/doc/cmake/RPATH-handling
# set         (CMAKE_INSTALL_RPATH    ${XL_LIBRARY_DIR})
set_target_properties(agency.cgi    PROPERTIES
                                    BUILD_WITH_INSTALL_RPATH TRUE
                                    INSTALL_RPATH_USE_LINK_PATH TRUE
                                    INSTALL_RPATH "${XL_LIBRARY_DIR}"
                                    )
message(STATUS  "configure libxl RPATH to ${XL_LIBRARY_DIR}}")

# --- Install part

install(TARGETS index.cgi			DESTINATION		${CGIDIR})
install(TARGETS approver.cgi		DESTINATION		${CGIDIR})
install(TARGETS agency.cgi			DESTINATION		${CGIDIR})
install(TARGETS subcontractor.cgi	DESTINATION		${CGIDIR})
install(TARGETS ajax_anyrole_1.cgi	DESTINATION		${CGIDIR})
install(TARGETS system.cgi          DESTINATION     ${CGIDIR})

install(TARGETS admin.cgi           DESTINATION     ${CGIADMINDIR})

install(TARGETS cron_daily			DESTINATION		${CRON_INSTALL_DIR})
install(TARGETS cron_minute			DESTINATION		${CRON_INSTALL_DIR})

# print_all_variables()
