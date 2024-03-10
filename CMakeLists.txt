cmake_minimum_required(VERSION 3.23)

project(CppBackports
    VERSION
        0.1.0
    LANGUAGES
        C CXX
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CPPBP_BUILD_SAMPLES)
    add_subdirectory("example")
endif()

if(CPPBP_BUILD_TESTS)
    add_subdirectory("test")
endif()
