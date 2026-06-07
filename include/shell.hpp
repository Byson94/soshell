#pragma once

#include "lib.hpp"

class Shell {
    private:
        bool should_exit = false;
        Library& library;
    public: 
        void start_loop();
        void process_command(std::string cmd);
        Shell(Library& lib);
};
