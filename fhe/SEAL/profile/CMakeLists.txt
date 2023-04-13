cmake_minimum_required(VERSION 3.13)

project(SEALExamples VERSION 4.1.1 LANGUAGES CXX)


find_package(SEAL 4.1.1 EXACT REQUIRED)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXE_LINKER_FLAGS "-static")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -no-pie -fno-builtin")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg -no-pie -fno-builtin")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg -no-pie -fno-builtin")

file (GLOB EXAMPLES_SRC_FILES CONFIGURE_DEPENDS ${CMAKE_BINARY_DIR}/src/*.cpp)
foreach (app ${EXAMPLES_SRC_FILES})
  get_filename_component( exe ${app} NAME_WE)
  add_executable ( ${exe} ${app} )
  set_target_properties(${exe} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
 target_link_libraries( ${exe} PRIVATE SEAL::seal)
endforeach()
