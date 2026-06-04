cmake_minimum_required(VERSION 3.16)
project(MyCppTools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Headers folder for #include
include_directories(${CMAKE_SOURCE_DIR}/Headers)

# Find all programs
file(GLOB PROGRAM_SOURCES "Programs/*.cc")

foreach(SOURCE_FILE ${PROGRAM_SOURCES})
    get_filename_component(EXEC_NAME ${SOURCE_FILE} NAME_WE)
    
    add_executable(${EXEC_NAME} ${SOURCE_FILE})
    
    # === This puts .exe files directly in the project root ===
    set_target_properties(${EXEC_NAME} PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endforeach()