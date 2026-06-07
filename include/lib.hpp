#pragma once
#include <string>
#include <vector>
#include <ffi.h>

class Library {
    private:
        void* handle;
        const char* lib_path;
    public: 
        std::vector<std::string> list_symbols();
        void call_symbol(const std::string& symbol, const std::vector<std::string>& args);

        Library(const std::string& path);
        ~Library();
};
