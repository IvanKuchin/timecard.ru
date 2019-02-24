set(EXECUTABLE_CGIs
    index.cgi
    approver.cgi
    agency.cgi
    subcontractor.cgi
    ajax_anyrole_1.cgi
    admin.cgi
    system.cgi
    )

# --- Libraries
# ---- list of independent libraries
set(INDEPENDENT_LIBRARIES_SRC
    c_bt.cpp
    c_invoice_service.cpp
    c_timecard_to_print.cpp
    c_print_timecard.cpp
    utilities_timecard1.cpp     # --- order is important, must follow c_bt
    )

# --- MACROSes start ---

macro(set_pd_lib_properties target_lib proj_src proj_inc_list proj_lib_list)
    # message(STATUS "set_pd_lib_properties ${target_lib}")

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

# ---- list of independent libraries
foreach(INDEPENDENT_LIBRARY_SRC ${INDEPENDENT_LIBRARIES_SRC})
    string(REGEX REPLACE "(^.*/|\\.[^.]*$)" "" file_without_ext ${INDEPENDENT_LIBRARY_SRC})

    set_pd_lib_properties(${file_without_ext} "${file_without_ext}.cpp" "${PROJECT_ALL_INCLUDE_DIRECTORIES}" "${project_link_libraries}")
endforeach()

# --- MSVC grouping
source_group("Headers Private"  FILES ${HDR_PRIVATE})
source_group("Headers Public"   FILES ${HDR_PUBLIC})
source_group("Sources"          FILES ${SOURCES})
source_group("Resources"        FILES ${RESOURCES})

# --- Link part (2 parts: custom build, normal build)
# ---- 1) Custom build speeding up linking process by linking only required libraries (.o)
# add_executable(company.cgi  company.cpp ${SOURCES})
# target_link_libraries(company.cgi __joint_lib cmysql c_float ccgi cuser cmail ccookie cexception cactivator cvars csession cfiles cstatistics crequest ctemplate)
# target_compile_features(company.cgi PUBLIC cxx_strong_enums PRIVATE cxx_range_for )
# list(REMOVE_ITEM EXECUTABLE_CGIs company.cpp)

# ---- 2) Normal build link with _all_ libraries (.o), therefore change in any .src trigger re-link for all executables
foreach(EXECUTABLE_CGI ${EXECUTABLE_CGIs})
    __add_executable_3libs(${EXECUTABLE_CGI} "${PROJECT_ALL_INCLUDE_DIRECTORIES}" "${PROJECT_PD_LIBS}" "${PROJECT_PI_LIBS}" "${PROJECT_EXTERNAL_LIBS}")
endforeach()


# --- cron building
__add_executable_3libs(cron_minute  "${PROJECT_ALL_INCLUDE_DIRECTORIES}" "${PROJECT_PI_LIBS}" "${PROJECT_PI_LIBS}" "${PROJECT_EXTERNAL_LIBS}")
__add_executable_3libs(cron_daily   "${PROJECT_ALL_INCLUDE_DIRECTORIES}" "${PROJECT_PI_LIBS}" "${PROJECT_PI_LIBS}" "${PROJECT_EXTERNAL_LIBS}")

# --- Install settings
# --- this setting allows to dynalic linker find .so library in custom location
# --- otherwise it have to be added to ld.so.conf or environment variable
# --- actual linker options are -Wl,-rpath,/path/to/dynamic/library
# --- https://gitlab.kitware.com/cmake/community/wikis/doc/cmake/RPATH-handling
# set(CMAKE_INSTALL_RPATH                 ${XL_LIBRARY_DIR})
set(CMAKE_INSTALL_RPATH             ${PROJECT_SOURCE_DIR}/libxl-3.8.4.0/lib64)
set_target_properties(agency.cgi    PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

# set(CMAKE_INSTALL_RPATH_USE_LINK_PATH   TRUE)

message(STATUS "PD: configure RPATH to ${CMAKE_INSTALL_RPATH}")


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
