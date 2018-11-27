message("-- buid tests/auto/")

#include_directories(
#    ${PROJECT_SOURCE_DIR}/src/
#    ${PROJECT_SOURCE_DIR}/src/include/
#)

GET_DIRECTORY_PROPERTY(project_include_directories	DIRECTORY ${PROJECT_SOURCE_DIR}/src/ DEFINITION project_include_directories)
GET_DIRECTORY_PROPERTY(PROJECT_LIBS			DIRECTORY ${PROJECT_SOURCE_DIR}/src/ DEFINITION PROJECT_LIBS)

set(CTEST_BINARY_DIRECTORY ${PROJECT_BINARY_DIR}/tests)

file(GLOB files "autotest_*.cpp")

foreach(file ${files})
    string(REGEX REPLACE "(^.*/|\\.[^.]*$)" "" file_without_ext ${file})
    add_executable(		${file_without_ext} 		${file})
    target_include_directories(	${file_without_ext} PRIVATE	${project_include_directories})
    target_link_libraries(	${file_without_ext} 		${PROJECT_LIBS})

    # message(STATUS "add test ${file_without_ext} include_var ${project_include_directories}")

    add_test(${file_without_ext} ${file_without_ext})
    set_tests_properties(${file_without_ext}
	PROPERTIES
	PASS_REGULAR_EXPRESSION "Test passed")
    set_tests_properties(${file_without_ext}
	PROPERTIES
	FAIL_REGULAR_EXPRESSION "(Exception|Test failed)")
    set_tests_properties(${file_without_ext}
	PROPERTIES
	TIMEOUT 120)
endforeach()

#print_all_variables()