cmake_minimum_required(VERSION 3.14)

project(TaggedJsonObject LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:preprocessor")
endif()


######################### Examples ############################
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(Example
  Examples/example.cpp
  inc/taggedjsonobject.h
  inc/taggedjsonarray.h
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
add_executable(testRunner
Tests/taggedjsonobject_test.cpp
Tests/test_main.cpp
Tests/taggedjsonarray_test.cpp
)
target_include_directories(testRunner PRIVATE inc)
target_link_libraries(testRunner
    Qt${QT_VERSION_MAJOR}::Core
    gtest_main
    gmock_main)


