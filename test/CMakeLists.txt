project(osbaseTest)

find_package(streamRouter REQUIRED)	

set(TEST_CASES_BASE DEP_EMBTOM_OSBASE)
set(LIB_MENU_BASE   DEP_EMBTOM_OSBASE)
set(APP_TEST_BASE   DEP_EMBTOM_OSBASE)

add_subdirectory(lib_test_framework_EMBUNIT)
add_subdirectory(lib_test_cases)
add_subdirectory(lib_menu)
add_subdirectory(app_test)
add_subdirectory(TST_lib_thread)
add_subdirectory(TST_lib_list)
add_subdirectory(TST_lib_timer)

set(SOURCES main.c )

add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(${PROJECT_NAME} app_test osBase EMBTOM::streamRouter)
