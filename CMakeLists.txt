project(osBase)
cmake_minimum_required(VERSION 2.8)
#set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_DEBUG_POSTFIX _g)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Threads REQUIRED)
find_package(streamRouter REQUIRED)
find_package(LibRt REQUIRED)

set(OSBASE_VERSION_MAJOR 2)
set(OSBASE_VERSION_MINOR 0)
set(OSBASE_VERSION_PATCH 0)

set(TREADING_LIBS pthread rt)  
message(STATUS "Selected lib_list lock type is ${LIB_LIST_LOCK}")

######################################################
#Compile Git Version into the library
######################################################
include("cmakeHelper")
write_git_version()

######################################################
#Subdirectoris 
######################################################
add_subdirectory(c)

######################################################
#Get interface headers of the sublibraries
######################################################
get_property(HEADERS_LIB_CLOCK        TARGET lib_clock        PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
get_property(HEADERS_LIB_CONVENTION   TARGET lib_convention   PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
get_property(HEADERS_LIB_LIST         TARGET lib_list         PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
get_property(HEADERS_LIB_MINI_PRINTF  TARGET lib_mini_printf  PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
get_property(HEADERS_LIB_THREAD       TARGET lib_thread       PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
get_property(HEADERS_LIB_TIMER        TARGET lib_timer        PROPERTY INTERFACE_INCLUDE_DIRECTORIES)

get_property(COMPILE_DEF_LIB_LIST     TARGET lib_list         PROPERTY COMPILE_DEFINITIONS)

SET(MY_PUBLIC_HEADERS_DIR ${HEADERS_LIB_LIST} ${HEADERS_LIB_MINI_PRINTF} ${HEADERS_LIB_THREAD} ${HEADERS_LIB_CLOCK} 
                           ${HEADERS_LIB_TIMER} ${HEADERS_LIB_CONVENTION} ./)

FOREACH(item ${MY_PUBLIC_HEADERS_DIR})
  file(GLOB item_headers "${item}/*.h")
  LIST(APPEND MY_PUBLIC_HEADERS ${item_headers})
ENDFOREACH(item ${HEADERS_OS_BASE_SUB_LIBS})
  
######################################################
#Build target settings
######################################################
set(OSBASE_VERSION_STRING ${OSBASE_VERSION_MAJOR}.${OSBASE_VERSION_MINOR}.${OSBASE_VERSION_PATCH})

add_library(${PROJECT_NAME}           SHARED ${CMAKE_CURRENT_BINARY_DIR}/osBaseVersion.c )
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION       ${OSBASE_VERSION_STRING} SOVERSION ${OSBASE_VERSION_MAJOR})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${MY_PUBLIC_HEADERS}")

target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads LIBRT::LIBRT EMBTOM::streamRouter)
target_link_libraries(${PROJECT_NAME} PRIVATE lib_convention  
                              "-Wl,--whole-archive" lib_list 
                                                    lib_clock 
                                                    lib_timer 
                                                    lib_thread 
                                                    lib_mini_printf "-Wl,--no-whole-archive")
target_include_directories(${PROJECT_NAME} PRIVATE ./)
target_include_directories(${PROJECT_NAME} PUBLIC  $<INSTALL_INTERFACE:include/embtom/osBase> )
target_compile_definitions(${PROJECT_NAME} PUBLIC ${COMPILE_DEF_LIB_LIST})
target_include_public_headers_to_directories(${PROJECT_NAME})

######################################################
#Install
######################################################

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    EXPORT osBase-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/embtom/osBase
)
install(FILES cmake/osBaseConfig.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/osBase)
install(FILES cmake/FindLibRt.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/osBase/cmake)

install(EXPORT osBase-targets
  FILE
    osBaseTargets.cmake
  NAMESPACE
    EMBTOM::
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/osBase
)

install(FILES config/osBase.conf DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/security/limits.d)

######################################################
# Test APP
######################################################

include(CTest)
option (OSBASE_BUILD_TEST "build tests of the osBase" OFF)
if (${BUILD_TESTING} AND ${OSBASE_BUILD_TEST})
  message(STATUS "Build with test application")
  enable_testing()
  add_subdirectory(test)
endif()

######################################################
#CPack
######################################################
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "osBase-${CMAKE_BUILD_TYPE}")
set(CPACK_PACKAGE_VERSION_MAJOR "2")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_PACKAGE_DESCRIPTION "EMBTOM::osBase")
set(CPACK_PACKAGE_CONTACT "Thomas Willetal t.willetal@googlemail.com")
set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GIT_SHA1: ${GIT_SHA1}")                                      
include(CPack)
