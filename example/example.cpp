#include <cppbp/string_view.hpp>

#include <iostream>

using namespace  cppbp::literals;

int main(int argc, char **argv)
{
    std::cout << "Hello world" << std::endl;

    cppbp::string_view s("Test");

    cppbp::string_view s = "1312332"sv;

    char arr[32]{0};

    s.copy(arr, 4);


    return 0;
}