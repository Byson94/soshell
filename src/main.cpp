#include "lib.hpp"
#include "shell.hpp"
#include <iostream>
#include <filesystem>

void print_help() {
    std::cout << "soshell - shell for your shared objects!\n\n";
    std::cout << "Usage: soshell [OPTIONS] <FILE>\n";

    std::cout << "\n";

    std::cout << "Arguments:\n";
    std::cout << "  <FILE>      The so file to load\n";

    std::cout << "\n";

    std::cout << "Options:\n";
    std::cout << "  -h, --help  Print help\n";
}

int main(int argc, char** argv) {
    if (argc <= 1) {
        print_help();
        return 0;
    }

    std::string cmd = argv[1];

    // check if it is an option
    if (cmd == "--help" || cmd == "-h") {
        print_help();
        return 0;
    }

    // handle shared object
    std::filesystem::path path = cmd;

    if (!std::filesystem::exists(path)) {
        std::cerr << "[ERROR] Provided path does not exist.\n";
    }

    Library lib(path);
    Shell new_shell(lib);

    new_shell.start_loop();

    return 0;
}
