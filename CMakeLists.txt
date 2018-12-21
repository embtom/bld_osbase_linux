project(osbase)
cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(Threads REQUIRED)
find_package(iotty REQUIRED)
find_package(LibRt REQUIRED)

set(CMAKE_DEBUG_POSTFIX _g)

######################################################
#Compile Git Version into the library
######################################################
SET (GIT_EXECUTABLE git)
# the commit's SHA1, and whether the building workspace was dirty or not
execute_process(COMMAND 
  "${GIT_EXECUTABLE}" describe --match=NeVeRmAtCh --always --abbrev=40 --dirty
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_SHA1
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# the date of the commit
execute_process(COMMAND 
  "${GIT_EXECUTABLE}" log -1 --format=%ad --date=local
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_DATE
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# the subject of the commit
execute_process(COMMAND
  "${GIT_EXECUTABLE}" log -1 --format=%s
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_COMMIT_SUBJECT
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# generate version.cc
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/osbase_version.c.in" "${CMAKE_CURRENT_BINARY_DIR}/osbase_version.c" @ONLY)

######################################################
#Configure flags
######################################################
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(LIB_LIST_LOCK lock_cas)
SET(LIB_THREAD_EXTERNAL_IOTTY true)
SET(LIB_THREAD_ARCHITECTURE posix)
SET(LIB_TIMER_EXTERNAL_IOTTY true)
SET(LIB_TIMER_ARCHITECTURE posix)
SET(LIB_CLOCK_ARCHITECTURE posix)
SET(TREADING_LIBS pthread rt)  

######################################################
#Subdirectoris 
######################################################
add_subdirectory(lib_convention)
add_subdirectory(lib_list)
add_subdirectory(lib_clock)
add_subdirectory(lib_mini_printf)
add_subdirectory(lib_thread)
add_subdirectory(lib_timer)
    
######################################################
#Get interface headers of the sublibraries
######################################################
get_property(HEADERS_LIB_CLOCK TARGET lib_clock PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
get_property(HEADERS_LIB_CONVENTION TARGET lib_convention PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
get_property(HEADERS_LIB_LIST TARGET lib_list PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
get_property(COMPILE_DEF_LIB_LIST TARGET lib_list PROPERTY COMPILE_DEFINITIONS)
get_property(HEADERS_LIB_MINI_PRINTF TARGET lib_mini_printf PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
get_property(HEADERS_LIB_THREAD TARGET lib_thread PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
get_property(HEADERS_LIB_TIMER TARGET lib_timer PROPERTY INTERFACE_INCLUDE_DIRECTORIES)

SET(MY_PUBLIC_HEADERS_DIR ${HEADERS_LIB_LIST} ${HEADERS_LIB_MINI_PRINTF} ${HEADERS_LIB_THREAD} ${HEADERS_LIB_CLOCK} 
                           ${HEADERS_LIB_TIMER} ${HEADERS_LIB_CONVENTION} ./)

FOREACH(item ${MY_PUBLIC_HEADERS_DIR})
  file(GLOB item_headers "${item}/*.h")
  LIST(APPEND MY_PUBLIC_HEADERS ${item_headers})
ENDFOREACH(item ${HEADERS_OS_BASE_SUB_LIBS})
  
######################################################
#Build target settings
######################################################
add_library(${PROJECT_NAME} SHARED ${CMAKE_CURRENT_BINARY_DIR}/osbase_version.c ) # $<TARGET_OBJECTS:lib_thread> 
target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads LIBRT::LIBRT EMBTOM::iotty)
target_link_libraries(${PROJECT_NAME} PRIVATE lib_convention  
      "-Wl,--whole-archive" lib_list lib_clock lib_thread lib_timer lib_mini_printf "-Wl,--no-whole-archive")
target_include_directories(${PROJECT_NAME} PRIVATE ./)
target_include_directories(${PROJECT_NAME} PUBLIC  $<INSTALL_INTERFACE:include/embtom/osbase> )
target_compile_definitions(${PROJECT_NAME} PUBLIC ${COMPILE_DEF_LIB_LIST})

######################################################
#Install
######################################################
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${MY_PUBLIC_HEADERS}")

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    EXPORT osbase-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/embtom/osbase
)
install(FILES cmake/osbaseConfig.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/osbase)
install(FILES cmake/FindLibRt.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/osbase/cmake)


install(EXPORT osbase-targets
  FILE
    osbaseTargets.cmake
  NAMESPACE
    EMBTOM::
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/osbase
)

######################################################
#CPack
######################################################
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "embtom-osbase-${CMAKE_BUILD_TYPE}")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_PACKAGE_DESCRIPTION "EMBTOM::osbase")
set(CPACK_PACKAGE_CONTACT "Thomas Willetal t.willetal@googlemail.com")
set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
#set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/Debian/postinst")

include(CPack)
