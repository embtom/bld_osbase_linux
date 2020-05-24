
add_executable(TST_lib_thread "main.c")
set_target_properties(TST_lib_thread PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(TST_lib_thread test_cases 
                                     osBase 
                                     EMBTOM::streamRouter)

add_test(NAME TST_lib_thread COMMAND TST_lib_thread)                                     