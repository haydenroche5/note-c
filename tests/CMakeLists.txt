cmake_minimum_required(VERSION 3.9)

if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}") 
    message(FATAL_ERROR "In-source builds are disabled.
    Please create a build directory and use `cmake ..` inside it.
    NOTE: cmake will now create CMakeCache.txt and CMakeFiles/*.
          You must delete them, or cmake will refuse to work.")
endif()

project(note_c_tests)

# Automatically ignore CMake build directory.
if(NOT EXISTS ${PROJECT_BINARY_DIR}/.gitignore)
    file(WRITE ${PROJECT_BINARY_DIR}/.gitignore "*")
endif()

# GoogleTest requires C++11.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(COVERAGE "Compile for coverage reporting." OFF)

find_package(GTest CONFIG REQUIRED)
enable_testing()
include(GoogleTest)

get_filename_component(PARENT_DIR ../ ABSOLUTE)
set(
    NOTE_C_SOURCES
    ${PARENT_DIR}/n_atof.c
    ${PARENT_DIR}/n_cjson.c
    ${PARENT_DIR}/n_const.c
    ${PARENT_DIR}/n_helpers.c
    ${PARENT_DIR}/n_i2c.c
    ${PARENT_DIR}/n_printf.c
    ${PARENT_DIR}/n_serial.c
    ${PARENT_DIR}/n_ua.c
    ${PARENT_DIR}/n_b64.c
    ${PARENT_DIR}/n_cjson_helpers.c
    ${PARENT_DIR}/n_ftoa.c
    ${PARENT_DIR}/n_hooks.c
    ${PARENT_DIR}/n_md5.c
    ${PARENT_DIR}/n_request.c
    ${PARENT_DIR}/n_str.c
)

function(add_test TEST_NAME MOCKED_FNS)
    set(OBJCOPY_WEAKEN "")
    foreach(MOCKED_FN ${MOCKED_FNS})
        string(APPEND OBJCOPY_WEAKEN "-W ${MOCKED_FN} ")
    endforeach()
    separate_arguments(OBJCOPY_WEAKEN_LIST NATIVE_COMMAND "${OBJCOPY_WEAKEN}")

    set(NOTE_C_LIB ${TEST_NAME}_note_c)
    add_library(${NOTE_C_LIB} STATIC ${NOTE_C_SOURCES})
    # If we don't weaken the functions we're mocking in the test, the linker
    # will complain about multiple function definitions: the mocked one and the
    # "real" one from note-c. Weakening the real function will allow the mocked
    # one to take precedence in the resulting test executable.
    add_custom_command(TARGET ${NOTE_C_LIB} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_WEAKEN_LIST}
        $<TARGET_FILE:${NOTE_C_LIB}>
        COMMENT "Weakening mocked functions."
    )

    add_executable(
        ${TEST_NAME}
        ${TEST_NAME}.cpp
    )
    target_include_directories(
        ${TEST_NAME} PRIVATE
        ${PARENT_DIR}
    )
    target_link_libraries(
        ${TEST_NAME}
        ${NOTE_C_LIB}
        GTest::gtest
    )

    if(COVERAGE)
        target_compile_options(
            ${NOTE_C_LIB} PRIVATE
            -fprofile-arcs -ftest-coverage -g -O0
        )
        target_compile_options(
            ${TEST_NAME} PRIVATE
            -fprofile-arcs -ftest-coverage -g -O0
        )
        target_link_libraries(
            ${TEST_NAME}
            gcov
        )
    endif()

    gtest_discover_tests(${TEST_NAME})
endfunction()

add_test(NoteTransaction_test "NoteReset;NoteJSONTransaction")
add_test(NoteRequest_test "NoteTransaction")
