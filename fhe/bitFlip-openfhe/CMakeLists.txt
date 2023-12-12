cmake_minimum_required (VERSION 3.5.1)

### To use gcc/g++ on a Macintosh, you must set the Compilers
### here, not inside the project
##if(APPLE)
##       set(CMAKE_C_COMPILER "/usr/local/bin/gcc-7")
##       set(CMAKE_CXX_COMPILER "/usr/local/bin/g++-7")
##endif()
### TODO: for now, we use CLang for Mac
###
### In order to create OpenFHE's static libraries you should enable
### the BUILD_STATIC option. For that, you run "cmake .. -DBUILD_STATIC=ON".
### After having your link completed you will find static libs
### with the suffix "_static" in ./build/libs/.
### Examples: OPENFHEpke_static.a, OPENFHEcore_static.a, etc.
### After you run "make install" in your build directory, you can build your custom application.
### If you need your application to be linked statically, then run "cmake .. -DBUILD_STATIC=ON"

project(demo CXX)
set(CMAKE_CXX_STANDARD 17)
option( BUILD_STATIC "Set to ON to include static versions of the library" OFF)

find_package(OpenFHE CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

if (OpenFHE_FOUND)
    message(STATUS "FOUND PACKAGE OpenFHE")
    message(STATUS "OpenFHE Version: ${BASE_OPENFHE_VERSION}")
    message(STATUS "OpenFHE installed as shared libraries: ${OpenFHE_SHARED}")
    message(STATUS "OpenFHE include files location: ${OpenFHE_INCLUDE}")
    message(STATUS "OpenFHE lib files location: ${OpenFHE_LIBDIR}")
    message(STATUS "OpenFHE Native Backend size: ${OpenFHE_NATIVE_SIZE}")
else()
    message(FATAL_ERROR "PACKAGE OpenFHE NOT FOUND")
endif ()

set( CMAKE_CXX_FLAGS ${OpenFHE_CXX_FLAGS} )

include_directories( ${OPENMP_INCLUDES} )
include_directories( ${OpenFHE_INCLUDE} )
include_directories( ${OpenFHE_INCLUDE}/third-party/include )
include_directories( ${OpenFHE_INCLUDE}/core )
include_directories( ${OpenFHE_INCLUDE}/pke )
include_directories( ${OpenFHE_INCLUDE}/binfhe )
### add directories for other OpenFHE modules as needed for your project

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -pg -no-pie -fno-builtin -pthread")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -std=c++2a -static -pg -no-pie -fno-builtin ")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -std=c++2a -pg -no-pie -fno-builtin ")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_BUILD_TYPE Debug)
link_directories( ${OpenFHE_LIBDIR} )
link_directories( ${OPENMP_LIBRARIES} )
if(BUILD_STATIC)
    set( CMAKE_EXE_LINKER_FLAGS "${OpenFHE_EXE_LINKER_FLAGS} -static")
    link_libraries( ${OpenFHE_STATIC_LIBRARIES} )
else()
    set( CMAKE_EXE_LINKER_FLAGS ${OpenFHE_EXE_LINKER_FLAGS} )
    link_libraries( ${OpenFHE_SHARED_LIBRARIES} )
endif()




set(GTEST_LIBRARIES gtest gmock gtest_main gmock_main pthread)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/logs)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/logs/log_encrypt)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/logs/log_encode)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/logs/log_input)

file (GLOB EXAMPLES_SRC_FILES CONFIGURE_DEPENDS ${CMAKE_BINARY_DIR}/src/*.cpp)
foreach (app ${EXAMPLES_SRC_FILES})
  get_filename_component( exe ${app} NAME_WE)
  add_executable ( ${exe} ${app} )
  set_target_properties(${exe} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
  #target_link_libraries( ${exe} PRIVATE OpenFHE::openfhe)
endforeach()
#
#file (GLOB TEST_SRC_FILES CONFIGURE_DEPENDS ${CMAKE_BINARY_DIR}/test/*.cpp)
#foreach (app ${TEST_SRC_FILES})
#  get_filename_component( exe ${app} NAME_WE)
#  add_executable ( ${exe} ${app} )
#  set_target_properties(${exe} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
#  target_link_libraries( ${exe} PRIVATE ${GTEST_LIBRARIES})
#  target_link_libraries( ${exe} PRIVATE SEAL::seal)
#endforeach()
