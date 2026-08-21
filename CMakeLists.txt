cmake_minimum_required(VERSION 3.19)
project(MyCppTools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt GUI version?
option(BUILD_GUI "Build the Qt GUI" ON) # change to OFF later

file(GLOB HEADER_FILES
    "${CMAKE_SOURCE_DIR}/src/Headers/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/Headers/*.h"
)

include_directories(${CMAKE_SOURCE_DIR}/src/Headers)

file(GLOB PROGRAM_SOURCES "src/TerminalPrograms/*.cc")

file(GLOB COMMON_SOURCES "src/stb_image_impl.cc")

foreach(SOURCE_FILE ${PROGRAM_SOURCES})
    get_filename_component(EXEC_NAME ${SOURCE_FILE} NAME_WE)

    add_executable(${EXEC_NAME}
        ${SOURCE_FILE}
        ${COMMON_SOURCES}
        ${HEADER_FILES}
    )

    # Compiler flags
    target_compile_options(${EXEC_NAME}
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -g              # change to -O3 later
    )

    # Ignore these warnings
    add_compile_options(
        -Wno-sign-compare # warnings about comparing ints and size_t
        -Wno-reorder # warnings about ctor MIL order
        #-Wno-unused-parameter # warnings about unused variables
    )

    set_target_properties(${EXEC_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    # Ignore all warnings for stb_image
    set_source_files_properties(
        ${COMMON_SOURCES}
        ${CMAKE_SOURCE_DIR}/src/Headers/stb_image.h
        ${CMAKE_SOURCE_DIR}/src/Headers/stb_image_write.h
        PROPERTIES
            COMPILE_FLAGS "-w"
    )
endforeach()


# Qt GUI version

if(BUILD_GUI)

    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

    qt_standard_project_setup()

    qt_add_executable(NumberWallExplorer
        ${CMAKE_SOURCE_DIR}/src/NumberWallExplorer.cc
        ${CMAKE_SOURCE_DIR}/src/mainwindow.cc
        ${CMAKE_SOURCE_DIR}/src/mainwindow.h
        ${CMAKE_SOURCE_DIR}/src/mainwindow.ui
        ${COMMON_SOURCES}
        ${HEADER_FILES}
    )

    target_link_libraries(NumberWallExplorer
        PRIVATE
            Qt6::Core
            Qt6::Widgets
    )

    target_compile_options(NumberWallExplorer
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -g              # change to -O3 later
    )

    # Ignore these warnings
    add_compile_options(
        -Wno-sign-compare # warnings about comparing ints and size_t
        -Wno-reorder # warnings about ctor MIL order
        #-Wno-unused-parameter # warnings about unused variables
    )

    set_target_properties(NumberWallExplorer PROPERTIES
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