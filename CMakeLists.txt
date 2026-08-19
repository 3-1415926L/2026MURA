cmake_minimum_required(VERSION 3.16)
project(MyCppTools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build the Qt GUI?
option(BUILD_GUI "Build the Qt GUI" OFF)

# Find headers
include_directories(${CMAKE_SOURCE_DIR}/src/Headers)

# Find terminal programs
file(GLOB PROGRAM_SOURCES "src/Programs/*.cc")

file(GLOB COMMON_SOURCES "src/stb_image_impl.cc")

foreach(SOURCE_FILE ${PROGRAM_SOURCES})
    get_filename_component(EXEC_NAME ${SOURCE_FILE} NAME_WE)

    add_executable(${EXEC_NAME}
        ${SOURCE_FILE}
        ${COMMON_SOURCES}
    )

    # Compiler flags
    target_compile_options(${EXEC_NAME}
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -g
    )

    # Removes these warnings
    add_compile_options(
        -Wno-sign-compare # warnings about comparing ints and size_t
        #-Wno-unused-parameter
        -Wno-reorder # warnings about ctor MIL order
    )

    set_target_properties(${EXEC_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    # Ignore warnings for these specific files
    set_source_files_properties(
        ${COMMON_SOURCES}
        ${CMAKE_SOURCE_DIR}/src/Headers/stb_image.h
        ${CMAKE_SOURCE_DIR}/src/Headers/stb_image_write.h
        PROPERTIES
            COMPILE_FLAGS "-w"
    )
endforeach()


# ---------------------------------------------------------
# Qt GUI
# ---------------------------------------------------------

if(BUILD_GUI)

    find_package(Qt6 REQUIRED COMPONENTS Widgets)

    qt_add_executable(main_Qt
        src/main_Qt.cc
        ${COMMON_SOURCES}
    )

    target_link_libraries(main_Qt
        PRIVATE
            Qt6::Widgets
    )

    target_compile_options(main_Qt
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -g
    )

    set_target_properties(main_Qt PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    # Ignore warnings for stb_image
    set_source_files_properties(
        ${COMMON_SOURCES}
        ${CMAKE_SOURCE_DIR}/src/Headers/stb_image.h
        ${CMAKE_SOURCE_DIR}/src/Headers/stb_image_write.h
        PROPERTIES
            COMPILE_FLAGS "-w"
    )

endif()