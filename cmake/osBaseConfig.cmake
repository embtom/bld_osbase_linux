get_filename_component(osBase_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(CMakeFindDependencyMacro)

list(APPEND CMAKE_MODULE_PATH "${osBase_CMAKE_DIR}/cmake")
find_dependency(Threads REQUIRED)
find_dependency(LibRt REQUIRED)
find_dependency(streamRouter REQUIRED)

if(NOT TARGET EMBTOM::osBase)
    include("${osBase_CMAKE_DIR}/osBaseTargets.cmake")
endif()
