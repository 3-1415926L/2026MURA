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

    set_target_properties(${EXEC_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endforeach()