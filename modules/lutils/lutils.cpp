#include "../../include/module.hpp"
#include <cstdlib>
#include <ctime>
#include <unistd.h>

// Inicialização do módulo
extern "C" Module init_module() {
    Module module;
    module.name = "lutils";
    module.type = ModuleType::NATIVE;
    
    // Função random
    module.functions["random"] = [](const std::vector<std::any>& args) -> std::any {
        double min = 0, max = 1;
        if (args.size() > 0) min = std::any_cast<double>(args[0]);
        if (args.size() > 1) max = std::any_cast<double>(args[1]);
        return min + (rand() / (RAND_MAX / (max - min)));
    };
    
    // Função sleep
    module.functions["sleep"] = [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return 0.0;
        double ms = std::any_cast<double>(args[0]);
        usleep(ms * 1000);
        return 0.0;
    };
    
    // Função type (já existe no built-in, mas adicionamos no módulo)
    module.functions["type"] = [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return std::string("nil");
        const auto& arg = args[0];
        if (!arg.has_value()) return std::string("nil");
        if (arg.type() == typeid(double)) return std::string("number");
        if (arg.type() == typeid(std::string)) return std::string("string");
        if (arg.type() == typeid(bool)) return std::string("boolean");
        if (arg.type() == typeid(std::vector<std::any>)) return std::string("table");
        return std::string("unknown");
    };
    
    return module;
}