#pragma once
#include <unordered_map>
#include <vector>
#include <any>
#include <functional>
#include <string>
#include "ast.hpp"
#include "module.hpp"

class Interpreter {
private:
    std::unordered_map<std::string, std::any> variables;
    std::unordered_map<std::string, std::function<std::any(const std::vector<std::any>&)>> functions;
    bool break_flag = false;
    ModuleManager module_manager;  // ← ADICIONA
    
    std::any evaluate(ASTNode* node);
    std::any evaluate_binary_op(const std::string& op, const std::any& left, const std::any& right);
    bool is_truthy(const std::any& value);
    std::string to_string(const std::any& value);
    double to_number(const std::any& value);
    void execute_print(const std::any& value, bool is_fake, const std::string& source);
    
public:
    Interpreter();
    ~Interpreter();
    void interpret(ASTNode* program);
    void register_builtin_functions();
};