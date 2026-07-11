#include "../include/environment.hpp"
#include <stdexcept>

void Environment::define(const std::string& name, const std::any& value) {
    variables[name] = value;
}

std::any Environment::get(const std::string& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    throw std::runtime_error("Variável não definida: " + name);
}

void Environment::set(const std::string& name, const std::any& value) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        it->second = value;
        return;
    }
    throw std::runtime_error("Variável não definida: " + name);
}

bool Environment::exists(const std::string& name) const {
    return variables.find(name) != variables.end();
}