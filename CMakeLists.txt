cmake_minimum_required(VERSION 3.16)
project(MyCppTools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#find headers
include_directories(${CMAKE_SOURCE_DIR}/Headers)

# find programs
file(GLOB PROGRAM_SOURCES "Programs/*.cc")

file(GLOB COMMON_SOURCES "stb_image/stb_image_impl.cc")

foreach(SOURCE_FILE ${PROGRAM_SOURCES})
    get_filename_component(EXEC_NAME ${SOURCE_FILE} NAME_WE)

    add_executable(${EXEC_NAME}
        ${SOURCE_FILE}
        ${COMMON_SOURCES}
    )

    # compiler flags
    target_compile_options(${EXEC_NAME}
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -g
    )

    # removes these warnings
    add_compile_options(
        -Wno-sign-compare # warnings about comparing ints and size_t
        #-Wno-unused-parameter
        -Wno-reorder # warnings about ctor MIL order
    )

    set_target_properties(${EXEC_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    # ignore warnings for these specific files
    set_source_files_properties(
        ${COMMON_SOURCES}
        ${CMAKE_SOURCE_DIR}/Headers/stb_image.h
        ${CMAKE_SOURCE_DIR}/Headers/stb_image_write.h
        PROPERTIES
            COMPILE_FLAGS "-w"
    )
endforeach()