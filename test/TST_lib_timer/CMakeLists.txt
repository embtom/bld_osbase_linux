
add_executable(TST_lib_timer "main.c")
set_target_properties(TST_lib_timer PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(TST_lib_timer test_cases 
                                     osBase 
                                     EMBTOM::streamRouter)

add_test(NAME TST_lib_timer COMMAND TST_lib_timer)                                     