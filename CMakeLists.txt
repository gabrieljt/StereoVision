#Change this if you need to target a specific CMake version
cmake_minimum_required(VERSION 2.8)
project(StereoVision)

#C++11
list(APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -Wall -g")

# Enable debug symbols by default
if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Include header files in Compiler's path
include_directories("${PROJECT_SOURCE_DIR}/Include/")

# Define Executable Source and Headers
set(SOURCE
        ${PROJECT_SOURCE_DIR}/Source/Application.cpp
        ${PROJECT_SOURCE_DIR}/Source/Main.cpp
)

set(HEADERS
        ${PROJECT_SOURCE_DIR}/Include/SV/Application.hpp
)

# Define Executable
set(EXECUTABLE_NAME StereoVision)
add_executable(${EXECUTABLE_NAME} ${SOURCE} ${HEADERS})

# Install Target
install(TARGETS ${EXECUTABLE_NAME} DESTINATION .)

# CPack packaging
include(InstallRequiredSystemLibraries)
include(CPack)

