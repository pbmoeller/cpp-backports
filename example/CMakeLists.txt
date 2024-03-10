add_executable(example_app
    "example.cpp"
)

target_include_directories(example_app
    PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
)

