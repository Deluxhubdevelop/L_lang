#pragma once
#include <unordered_map>
#include <any>
#include <string>

class Environment {
private:
    std::unordered_map<std::string, std::any> variables;
    
public:
    Environment() = default;
    
    void define(const std::string& name, const std::any& value);
    std::any get(const std::string& name) const;
    void set(const std::string& name, const std::any& value);
    bool exists(const std::string& name) const;
};