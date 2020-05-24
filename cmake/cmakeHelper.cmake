FUNCTION (write_git_version)
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
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/osBaseVersion.c.in" "${CMAKE_CURRENT_BINARY_DIR}/osBaseVersion.c" @ONLY)
ENDFUNCTION()

FUNCTION (target_include_public_headers_to_directories _target)
    get_property(EXPORT_OSBASE_HEADERS  TARGET ${_target}  PROPERTY PUBLIC_HEADER)
    set(_tmplist "")
    foreach (_parts ${EXPORT_OSBASE_HEADERS})
        set(_tmpvar)
        get_filename_component(_tmpvar "${_parts}" ABSOLUTE DIRECTORY)
        get_filename_component(_tmpvar "${_tmpvar}" DIRECTORY)
        list(APPEND _tmplist $<BUILD_INTERFACE:${_tmpvar}>)
    endforeach()
    set(EXPORT_HEADERS_DIR ${_tmplist})

    list(REMOVE_DUPLICATES EXPORT_HEADERS_DIR)
    target_include_directories(${_target} PUBLIC ${EXPORT_HEADERS_DIR})
ENDFUNCTION()