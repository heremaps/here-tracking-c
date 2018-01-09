
cmake_minimum_required(VERSION 3.1)

project(here-tracking C)

option(BuildTests "Build Tests" OFF)

option(BuildSampleApp "Build sample application" OFF)

option(CodeCoverage "Build for code coverage" OFF)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(BUILD_SHARED_LIBS OFF)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g -std=c99")

set(LOG_LEVEL "NONE" CACHE STRING "Log level")

if(NOT LOG_LEVEL MATCHES "^NONE$|^FATAL$|^ERROR$|^WARNING$|^INFO$")
  message(WARNING
          "Log level must be set to NONE, FATAL, ERROR, WARNING or INFO. Default to NONE.")
  set(LOG_LEVEL "NONE")
endif()

add_definitions(-DHERE_TRACKING_LOG_LEVEL=HERE_TRACKING_LOG_LEVEL_${LOG_LEVEL})

if(CodeCoverage)
  string(APPEND
         CMAKE_C_FLAGS
         " -O0 -fprofile-arcs -ftest-coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs")
  find_program(GCOVR_BIN gcovr)
  if(NOT GCOVR_BIN)
    message(FATAL_ERROR "gcovr not found")
  else()
    set(COVERAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}/coverage")
    string(CONCAT
           GCOVR_OPTIONS
           "-v -x --exclude='.*test_*' --exclude='.*mock_*' -o ${COVERAGE_DIR}/coverage.xml"
           " -r ${CMAKE_CURRENT_SOURCE_DIR}")
    separate_arguments(GCOVR_ARGS UNIX_COMMAND ${GCOVR_OPTIONS})
    add_custom_target(coverage
                      COMMAND ${CMAKE_COMMAND} -E make_directory ${COVERAGE_DIR}
                      COMMAND ${GCOVR_BIN} ${GCOVR_ARGS}
                      VERBATIM)
  endif()
endif()

include_directories(include include/internal)

add_subdirectory(src)

if(BuildTests)
  find_package(Check REQUIRED)
  include_directories(test/include ${CHECK_INCLUDE_DIRS})
  enable_testing()
  add_subdirectory(test)
endif()

if(BuildSampleApp)
  add_subdirectory(app)
endif()

find_package(Doxygen)

if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
                 ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                 @ONLY)
  add_custom_target(doxygen
                    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                    VERBATIM)
endif()
