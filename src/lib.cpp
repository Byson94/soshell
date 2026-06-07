#include "lib.hpp"
#include <dlfcn.h>
#include <iostream>
#include <vector>
#include <string>
#include <libelf.h>
#include <gelf.h>
#include <unistd.h> 
#include <fcntl.h>
#include <ffi.h>
#include <filesystem>

std::vector<std::string> Library::list_symbols() {
    elf_version(EV_CURRENT);

    int fd = open(lib_path, O_RDONLY);
    Elf* elf = elf_begin(fd, ELF_C_READ, nullptr);

    std::vector<std::string> syms;

    Elf_Scn* scn = nullptr;
    while ((scn = elf_nextscn(elf, scn)) != nullptr) {
        GElf_Shdr shdr;
        gelf_getshdr(scn, &shdr);
        
        if (shdr.sh_type == SHT_DYNSYM) {
            Elf_Data* data = elf_getdata(scn, nullptr);
            int count = shdr.sh_size / shdr.sh_entsize;

            for (int i = 0; i < count; i++) {
                GElf_Sym sym;
                gelf_getsym(data, i, &sym);
                
                if (GELF_ST_BIND(sym.st_info) == STB_GLOBAL && 
                    GELF_ST_TYPE(sym.st_info) == STT_FUNC) {
                    
                    const char* name = elf_strptr(elf, shdr.sh_link, sym.st_name);
                    syms.push_back(name);
                }
            }
        }
    }

    elf_end(elf);
    close(fd);

    return syms;
}

void Library::call_symbol(const std::string& symbol, const std::vector<std::string>& args) {
    void* sym = dlsym(handle, symbol.c_str());
    if (!sym) {
        std::cout << "Symbol not found: " << symbol << "\n";
        return;
    }

    std::vector<ffi_type*> arg_types;
    std::vector<void*> arg_values;
    std::vector<int> int_storage;
    std::vector<double> double_storage;
    std::vector<std::string> string_storage;

    int_storage.reserve(args.size());
    double_storage.reserve(args.size());
    string_storage.reserve(args.size());

    for (const auto& arg : args) {
        if (arg.front() == '"' && arg.back() == '"') {
            string_storage.push_back(arg.substr(1, arg.size() - 2));
            const char* cstr = string_storage.back().c_str();
            arg_types.push_back(&ffi_type_pointer);
            arg_values.push_back(&cstr);
        } else if (arg.find('.') != std::string::npos) {
            double_storage.push_back(std::stod(arg));
            arg_types.push_back(&ffi_type_double);
            arg_values.push_back(&double_storage.back());
        } else {
            int_storage.push_back(std::stoi(arg));
            arg_types.push_back(&ffi_type_sint32);
            arg_values.push_back(&int_storage.back());
        }
    }

    ffi_cif cif;
    if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, arg_types.size(),
                     &ffi_type_sint32, arg_types.data()) != FFI_OK) {
        std::cout << "Failed to prepare ffi call\n";
        return;
    }

    int result;
    ffi_call(&cif, FFI_FN(sym), &result, arg_values.data());
    std::cout << "Result: " << result << "\n";
}

Library::Library(const std::string& path) {
    std::cout << "Loading library: " << path.c_str() << "\n";
    handle = dlopen(std::filesystem::absolute(path).c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "[ERROR] dlopen failed: " << dlerror() << "\n";
    }
    lib_path = path.c_str();
}

Library::~Library() {
    if (handle) dlclose(handle);
}
