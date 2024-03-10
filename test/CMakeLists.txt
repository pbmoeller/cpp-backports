enable_testing()

# Fetch the sources of GoogleTest
# Limit the Branch to 1.12.1, because it is the last Branch that supports C++11
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.12.1
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Test application
add_executable(cppbp_test
    "tests.cpp"
    "string_view_test.cpp"
)

target_link_libraries(cppbp_test
    PRIVATE
        gtest
        gtest_main
)