cmake_minimum_required(VERSION 3.23)

# Set the project name to your project name, my project isn't very descriptive
project(videoprcess CXX)

include(cmake/Common.cmake)
include(cmake/BuildLocation.cmake)
include(cmake/StandardProjectSettings.cmake)
# enable cache system
include(cmake/Cache.cmake)
# Add linker configuration
include(cmake/Linker.cmake)
# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
# sanitizer options if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(${PROJECT_NAME})
# enable doxygen
include(cmake/Doxygen.cmake)
# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

option(BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF)
option(ENABLE_TESTING "Enable Test Builds" OFF)

# Very basic PCH example

add_subdirectory(3rdparty)
if(ENABLE_TESTING)
  enable_testing()
  message("Building Tests.")
  add_subdirectory(test)
endif()

find_package(OpenCV REQUIRED)


if(${OpenCV_FOUND})
  message(STATUS "OpenCV_VERSION: ${OpenCV_VERSION}")
endif()


add_subdirectory(src)

option(ENABLE_UNITY "Enable Unity builds of projects" OFF)
if(ENABLE_UNITY)
  # Add for any project you want to apply unity builds for
  set_target_properties(intro PROPERTIES UNITY_BUILD ON)
endif()
