#Change this if you need to target a specific CMake version
cmake_minimum_required(VERSION 2.8)
project(StereoVision)

#Linux C++11
list(APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

# Enable debug symbols by default
if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Include header files in Compiler's path
include_directories("${PROJECT_SOURCE_DIR}/Include/")

# Define Executable Source and Headers
set(SOURCE
        ${PROJECT_SOURCE_DIR}/Source/Application.cpp
        ${PROJECT_SOURCE_DIR}/Source/CameraCapture.cpp
        ${PROJECT_SOURCE_DIR}/Source/CameraConfiguration.cpp
        ${PROJECT_SOURCE_DIR}/Source/Main.cpp
        ${PROJECT_SOURCE_DIR}/Source/Utility.cpp
)

set(HEADERS
        ${PROJECT_SOURCE_DIR}/Include/SV/Application.hpp
        ${PROJECT_SOURCE_DIR}/Include/SV/CameraCapture.hpp
        ${PROJECT_SOURCE_DIR}/Include/SV/CameraConfiguration.hpp
        ${PROJECT_SOURCE_DIR}/Include/SV/Utility.hpp
)

# Define Executable
set(EXECUTABLE_NAME StereoVision)
add_executable(${EXECUTABLE_NAME} ${SOURCE} ${HEADERS})

# Find OpenCV libraries
find_package(OpenCV REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} ${OpenCV_LIBS})

# Find Basler's Pylon SDK; TODO: improve FindPylon.cmake module for Cross-Compilation

# Linux Build
find_package(Pylon REQUIRED) 

# Windows Build
#set (PYLON_INCLUDE_DIRS $ENV{PYLON_ROOT}/include
#			$ENV{PYLON_GENICAM_ROOT}/library/CPP/include
#)

#set (PYLON_LIBRARIES 
#		$ENV{PYLON_ROOT}/lib/x64/PylonBase_MD_VC100.lib
#		$ENV{PYLON_ROOT}/lib/x64/PylonGUI_MD_VC100.lib
#		$ENV{PYLON_ROOT}/lib/x64/PylonUtility_MD_VC100.lib
#		$ENV{PYLON_ROOT}/lib/x64/PylonBootstrapper.lib
#		$ENV{PYLON_GENICAM_ROOT}/library/cpp/lib/win64_x64/GCBase_MD_VC100_$ENV{PYLON_GENICAM_VERSION}.lib
#		$ENV{PYLON_GENICAM_ROOT}/library/cpp/lib/win64_x64/GenApi_MD_VC100_$ENV{PYLON_GENICAM_VERSION}.lib
#		$ENV{PYLON_GENICAM_ROOT}/library/cpp/lib/win64_x64/MathParser_MD_VC100_$ENV{PYLON_GENICAM_VERSION}.lib
#		$ENV{PYLON_GENICAM_ROOT}/library/cpp/lib/win64_x64/Log_MD_VC100_$ENV{PYLON_GENICAM_VERSION}.lib
#		$ENV{PYLON_GENICAM_ROOT}/library/cpp/lib/win64_x64/Log4cpp_MD_VC100_$ENV{PYLON_GENICAM_VERSION}.lib
#)

include_directories(${PYLON_INCLUDE_DIRS})
target_link_libraries(${EXECUTABLE_NAME} ${PYLON_LIBRARIES})

# Install Target
install(TARGETS ${EXECUTABLE_NAME} DESTINATION .)
file(COPY Config DESTINATION .)

# CPack packaging
include(InstallRequiredSystemLibraries)
include(CPack)
