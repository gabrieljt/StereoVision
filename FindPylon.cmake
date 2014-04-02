set(PYLON_INCLUDE
    $ENV{PYLON_ROOT}/include/ 
    $ENV{GENICAM_ROOT_V2_3}/library/CPP/include/
)

set(PYLON_LIBS
    $ENV{PYLON_ROOT}/lib64/libpylonbase.so
    $ENV{PYLON_ROOT}/lib64/libpylonutility.so
    $ENV{PYLON_ROOT}/lib64/libXerces-C_gcc40_v2_7.so
    $ENV{PYLON_ROOT}/lib64/pylon/tl/pyloncamemu.so
    $ENV{GENICAM_ROOT_V2_3}/bin/Linux64_x64/libGCBase_gcc40_v2_3.so
    $ENV{GENICAM_ROOT_V2_3}/bin/Linux64_x64/libGenApi_gcc40_v2_3.so
    $ENV{GENICAM_ROOT_V2_3}/bin/Linux64_x64/libMathParser_gcc40_v2_3.so
    $ENV{GENICAM_ROOT_V2_3}/bin/Linux64_x64/libLog_gcc40_v2_3.so
    $ENV{GENICAM_ROOT_V2_3}/bin/Linux64_x64/liblog4cpp_gcc40_v2_3.so
)

# handle the QUIETLY and REQUIRED arguments and set PYLON_FOUND to TRUE if
# all listed variables are TRUE
include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PYLON REQUIRED_VARS PYLON_INCLUDE PYLON_LIBS)

if(PYLON_FOUND)
  set( PYLON_INCLUDE_DIRS ${PYLON_INCLUDE} )
  set( PYLON_LIBRARIES ${PYLON_LIBS} )
endif()

mark_as_advanced(PYLON_INCLUDE PYLON_LIBS)
