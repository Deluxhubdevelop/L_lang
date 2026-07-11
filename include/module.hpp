#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <any>
#include <vector>

enum class ModuleType {
    LANG,
    NATIVE,
    BINARY
};

struct Module {
    std::string name;
    ModuleType type;
    std::unordered_map<std::string, std::any> exports;
    std::unordered_map<std::string, std::function<std::any(const std::vector<std::any>&)>> functions;
};

class ModuleManager {
private:
    std::unordered_map<std::string, Module> loaded_modules;
    std::vector<std::string> module_paths;
    
    Module load_lang_module(const std::string& name);
    Module load_native_module(const std::string& name);
    Module load_binary_module(const std::string& name);
    std::string to_string(const std::any& value);
    
public:
    ModuleManager();
    
    void add_module_path(const std::string& path);
    Module* import(const std::string& name);
    bool is_loaded(const std::string& name);
    void register_native_module(const std::string& name, Module module);
};