#include "../include/module.hpp"
#include "../include/interpreter.hpp"
#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>  // ← ADICIONA ESTA LINHA
#include <thread>
#include <chrono>

ModuleManager::ModuleManager() {
    module_paths.push_back("./modules/");
    module_paths.push_back("./lib/");
    srand(time(nullptr));
}

void ModuleManager::add_module_path(const std::string& path) {
    module_paths.push_back(path);
}

Module* ModuleManager::import(const std::string& name) {
    // Verifica se já está carregado
    auto it = loaded_modules.find(name);
    if (it != loaded_modules.end()) {
        return &it->second;
    }
    
    // Tenta carregar como módulo nativo (C++)
    try {
        Module module = load_native_module(name);
        loaded_modules[name] = module;
        return &loaded_modules[name];
    } catch (const std::exception& e) {
        // Tenta carregar como módulo L
        try {
            Module module = load_lang_module(name);
            loaded_modules[name] = module;
            return &loaded_modules[name];
        } catch (const std::exception& e2) {
            // Tenta carregar como módulo binário (.so)
            try {
                Module module = load_binary_module(name);
                loaded_modules[name] = module;
                return &loaded_modules[name];
            } catch (const std::exception& e3) {
                throw std::runtime_error("Módulo não encontrado: " + name);
            }
        }
    }
}

bool ModuleManager::is_loaded(const std::string& name) {
    return loaded_modules.find(name) != loaded_modules.end();
}

std::string ModuleManager::to_string(const std::any& value) {
    if (!value.has_value()) return "nil";
    if (value.type() == typeid(std::string)) return std::any_cast<std::string>(value);
    if (value.type() == typeid(double)) {
        double d = std::any_cast<double>(value);
        if (d == (int)d) return std::to_string((int)d);
        return std::to_string(d);
    }
    if (value.type() == typeid(bool)) return std::any_cast<bool>(value) ? "true" : "false";
    if (value.type() == typeid(std::vector<std::any>)) {
        try {
            const auto& vec = std::any_cast<const std::vector<std::any>&>(value);
            std::string result = "{";
            for (size_t i = 0; i < vec.size(); i++) {
                if (i > 0) result += ", ";
                result += to_string(vec[i]);
            }
            result += "}";
            return result;
        } catch (const std::bad_any_cast&) {
            return "{...}";
        }
    }
    return "unknown";
}

Module ModuleManager::load_lang_module(const std::string& name) {
    Module module;
    module.name = name;
    module.type = ModuleType::LANG;
    
    // Procura o arquivo init.l no caminho do módulo
    for (const auto& path : module_paths) {
        std::string filename = path + name + "/init.l";
        std::ifstream file(filename);
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string source = buffer.str();
            
            // Executa o código L para carregar o módulo
            Lexer lexer(source);
            auto tokens = lexer.tokenize();
            Parser parser(tokens);
            auto ast = parser.parse();
            
            Interpreter interpreter;
            interpreter.interpret(ast.get());
            
            return module;
        }
    }
    
    throw std::runtime_error("Módulo L não encontrado: " + name);
}

Module ModuleManager::load_native_module(const std::string& name) {
    Module module;
    module.name = name;
    module.type = ModuleType::NATIVE;
    
    if (name == "lutils") {
        // random(min, max)
        module.functions["random"] = [](const std::vector<std::any>& args) -> std::any {
            double min = 0, max = 1;
            if (args.size() > 0) {
                try {
                    min = std::any_cast<double>(args[0]);
                } catch (...) {
                    min = 0;
                }
            }
            if (args.size() > 1) {
                try {
                    max = std::any_cast<double>(args[1]);
                } catch (...) {
                    max = 1;
                }
            }
            return min + (rand() / (RAND_MAX / (max - min)));
        };
        
        // sleep(ms) - usando std::this_thread
        module.functions["sleep"] = [](const std::vector<std::any>& args) -> std::any {
            if (args.empty()) return 0.0;
            try {
                double ms = std::any_cast<double>(args[0]);
                std::this_thread::sleep_for(std::chrono::milliseconds((int)ms));
            } catch (...) {
                return 0.0;
            }
            return 0.0;
        };
        
        // table_join(tabela, separador)
        module.functions["table_join"] = [this](const std::vector<std::any>& args) -> std::any {
            if (args.empty()) return std::string("");
            
            std::string separator = ", ";
            if (args.size() > 1 && args[1].has_value()) {
                try {
                    separator = std::any_cast<std::string>(args[1]);
                } catch (...) {
                    separator = ", ";
                }
            }
            
            const auto& arg = args[0];
            if (arg.type() != typeid(std::vector<std::any>)) {
                return std::string("");
            }
            
            try {
                const auto& vec = std::any_cast<const std::vector<std::any>&>(arg);
                std::string result;
                for (size_t i = 0; i < vec.size(); i++) {
                    if (i > 0) result += separator;
                    result += to_string(vec[i]);
                }
                return result;
            } catch (const std::bad_any_cast&) {
                return std::string("");
            }
        };
        
        // type(valor)
        module.functions["type"] = [this](const std::vector<std::any>& args) -> std::any {
            if (args.empty()) return std::string("nil");
            const auto& arg = args[0];
            if (!arg.has_value()) return std::string("nil");
            if (arg.type() == typeid(double)) return std::string("number");
            if (arg.type() == typeid(std::string)) return std::string("string");
            if (arg.type() == typeid(bool)) return std::string("boolean");
            if (arg.type() == typeid(std::vector<std::any>)) return std::string("table");
            return std::string("unknown");
        };
    }
    
    if (name == "lgrafics") {
        // Placeholder para funções gráficas
        module.functions["create_window"] = [](const std::vector<std::any>& args) -> std::any {
            return 0.0;
        };
        
        module.functions["draw_rect"] = [](const std::vector<std::any>& args) -> std::any {
            return 0.0;
        };
        
        module.functions["clear"] = [](const std::vector<std::any>& args) -> std::any {
            return 0.0;
        };
        
        module.functions["render"] = [](const std::vector<std::any>& args) -> std::any {
            return 0.0;
        };
    }
    
    return module;
}

Module ModuleManager::load_binary_module(const std::string& name) {
    // TODO: Carregar .so com dlopen()
    throw std::runtime_error("Módulo binário não suportado ainda");
}

void ModuleManager::register_native_module(const std::string& name, Module module) {
    loaded_modules[name] = module;
}