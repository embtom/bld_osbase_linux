project(test_desgin_pattern)

#However, the file(GLOB...) allows for wildcard additions:
set(SOURCES main.cpp )

#file(GLOB SOURCES "*.c")
add_definitions(-DTRACE)
 
#link_directories(${lib_log_BINARY_DIR})
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} os_basic )
#target_include_directories(${PROJECT_NAME} PRIVATE ${lib_log_SOURCE_DIR} ${itf_lib_thread_SOURCE_DIR} ${lib_convention_SOURCE_DIR}) 