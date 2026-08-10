#include "utils.hpp"

const char* reset  = "\033[0m";
const char* bold   = "\033[1m";
const char* red    = "\033[31m";
const char* green  = "\033[32m";
const char* yellow = "\033[33m";
const char* blue   = "\033[34m";
const char* purple = "\033[35m";
const char* cyan   = "\033[36m";

void banner(const std::string& title,
            const char* color)
{
    const std::size_t padding = 4;
    const std::size_t width = title.size() + padding * 2;

    std::cout << '\n';

    std::cout << color << bold;
    std::cout << "╔";
    for (std::size_t i = 0; i < width; i++)
        std::cout << "═";
    std::cout << "╗\n";

    std::cout << "║";
    for (std::size_t i = 0; i < padding; i++)
        std::cout << ' ';

    std::cout << title;

    for (std::size_t i = 0; i < padding; i++)
        std::cout << ' ';
    std::cout << "║\n";

    std::cout << "╚";
    for (std::size_t i = 0; i < width; i++)
        std::cout << "═";
    std::cout << "╝";

    std::cout << reset << "\n\n";
}

