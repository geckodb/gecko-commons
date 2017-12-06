# this will make sure that one is using the clang compiler
set(CMAKE_CXX_COMPILER  "/usr/bin/clang++")
set(CMAKE_C_COMPILER "/usr/bin/clang")

project(gecko-commons)

set(CMAKE_BINARY_DIR          ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH    ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_TEMP_OUTPUT_DIR     ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -std=c11 -Wall -Werror")

cmake_minimum_required(VERSION 3.5)

include(
    "cmake/FindAPR.cmake"
)

include_directories(
        "include"
        ${APR_INCLUDE_DIR}
)

option(BUILD_DOCUMENTATION "API documentation requires Doxygen" ${DOXYGEN_FOUND})

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR})

set(SOURCE_FILES
        include/gecko-commons/c11threads.h
        include/gecko-commons/containers/gs_freelist.h
        include/gecko-commons/containers/gs_hash.h
        include/gecko-commons/containers/gs_hashset.h
        include/gecko-commons/containers/gs_list.h
        include/gecko-commons/containers/gs_vec.h
        include/gecko-commons/gs_async.h
        include/gecko-commons/gs_debug.h
        include/gecko-commons/gs_error.h
        include/gecko-commons/gs_hash.h
        include/gecko-commons/gs_interval.h
        include/gecko-commons/gs_msg.h
        include/gecko-commons/gs_require.h
        include/gecko-commons/gs_spinlock.h
        include/gecko-commons/gs_timer.h
        include/gecko-commons/gs_utils.h

        src/gecko-commons/containers/gs_freelist.c
        src/gecko-commons/containers/gs_hash.c
        src/gecko-commons/containers/gs_hashset.c
        src/gecko-commons/containers/gs_list.c
        src/gecko-commons/containers/gs_vec.c
        src/gecko-commons/gs_async.c
        src/gecko-commons/gs_error.c
        src/gecko-commons/gs_hash.c
        src/gecko-commons/gs_interval.c
        src/gecko-commons/gs_spinlock.c
        src/gecko-commons/gs_timer.c
        src/gecko-commons/gs_utils.c

        include/gecko-commons/gecko-commons.h
        include/gecko-commons/stdinc.h include/gecko-commons/gs_string.h src/gecko-commons/gs_string.c include/gecko-commons/relative_ptr.h)

add_executable(
    gecko
    test.c
    ${SOURCE_FILES}
)

target_link_libraries(
        gecko
        ${APR_LIBRARIES}
        ${APRUTIL_LIBRARIES}
)

if(DOXYGEN_FOUND)
    add_custom_target(
        doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif(DOXYGEN_FOUND)