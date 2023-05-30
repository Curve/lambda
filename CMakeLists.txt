cmake_minimum_required(VERSION 3.16)
project(lambda_ptr LANGUAGES CXX VERSION 1.0)

# --------------------------------------------------------------------------------------------------------
# Library options
# --------------------------------------------------------------------------------------------------------

option(lambda_ptr_tests "Whether or not tests should be enabled" OFF)

# --------------------------------------------------------------------------------------------------------
# Create library
# --------------------------------------------------------------------------------------------------------

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 CXX_EXTENSIONS OFF CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------------------------------------------------------------
# Include directories
# --------------------------------------------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} INTERFACE "include")

# --------------------------------------------------------------------------------------------------------
# Link required libraries
# --------------------------------------------------------------------------------------------------------

include("cmake/cpm.cmake")

CPMAddPackage("gh:boostorg/callable_traits#2.3.3")
target_link_libraries(${PROJECT_NAME} INTERFACE boost_callable_traits)

# --------------------------------------------------------------------------------------------------------
# Tests
# --------------------------------------------------------------------------------------------------------

if (lambda_ptr_tests)
    CPMAddPackage("gh:catchorg/Catch2@3.3.2")

    file(GLOB src "tests/*.test.cpp")
    add_executable(${PROJECT_NAME}_tests ${src})

    target_compile_features(${PROJECT_NAME}_tests INTERFACE cxx_std_20)
    set_target_properties(${PROJECT_NAME}_tests PROPERTIES CXX_STANDARD 20 CXX_EXTENSIONS OFF CXX_STANDARD_REQUIRED ON)

    target_link_libraries(${PROJECT_NAME}_tests PUBLIC Catch2WithMain ${PROJECT_NAME})
endif()
