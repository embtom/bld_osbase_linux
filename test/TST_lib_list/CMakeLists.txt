
add_executable(TST_lib_list "main.c")
set_target_properties(TST_lib_list PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(TST_lib_list test_cases 
                                     osBase 
                                     EMBTOM::streamRouter)

add_test(NAME TST_lib_list COMMAND TST_lib_list)                                     