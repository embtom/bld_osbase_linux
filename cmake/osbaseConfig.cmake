get_filename_component(osbase_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(CMakeFindDependencyMacro)

list(APPEND CMAKE_MODULE_PATH "${osbase_CMAKE_DIR}/cmake")
find_dependency(Threads REQUIRED)
find_dependency(LibRt REQUIRED)
find_dependency(iotty REQUIRED)

if(NOT TARGET EMBTOM::osbase)
    include("${osbase_CMAKE_DIR}/osbaseTargets.cmake")
endif()
