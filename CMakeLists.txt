cmake_minimum_required(VERSION 3.14)

project(TaggedJsonObject LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


######################### Examples ############################
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(Example
  Examples/example.cpp
  inc/taggedjsonobject.h
  Examples/example.json
  inc/taggedjsonobjectmacros.h
  inc/map.h
)
target_include_directories(Example PRIVATE inc)
target_link_libraries(Example Qt${QT_VERSION_MAJOR}::Core)

######################## Tests ###############################

# GTest package directives
include(FetchContent)
FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG release-1.11.0)
FetchContent_GetProperties(googletest)

if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BUILD_DIR})
endif()

# Test files
add_executable(testRunner Tests/taggedjsonobject_test.cpp
)
target_include_directories(testRunner PRIVATE inc)
target_link_libraries(testRunner
    Qt${QT_VERSION_MAJOR}::Core
    gtest_main
    gmock_main)


