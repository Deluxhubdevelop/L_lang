#include "../include/interpreter.hpp"
#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <memory>

Interpreter::Interpreter() {
    register_builtin_functions();
}

Interpreter::~Interpreter() {}

void Interpreter::register_builtin_functions() {
    functions["print"] = [this](const std::vector<std::any>& args) -> std::any {
        for (const auto& arg : args) {
            std::cout << to_string(arg) << " ";
        }
        std::cout << std::endl;
        return std::any();
    };
    
    functions["len"] = [this](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return 0.0;
        const auto& arg = args[0];
        if (arg.type() == typeid(std::string)) {
            return static_cast<double>(std::any_cast<std::string>(arg).length());
        }
        return 0.0;
    };
    
    functions["tonumber"] = [this](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return 0.0;
        return to_number(args[0]);
    };
    
    functions["tostring"] = [this](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return std::string("");
        return to_string(args[0]);
    };
    
    functions["type"] = [this](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return std::string("nil");
        const auto& arg = args[0];
        if (!arg.has_value()) return std::string("nil");
        if (arg.type() == typeid(double)) return std::string("number");
        if (arg.type() == typeid(std::string)) return std::string("string");
        if (arg.type() == typeid(bool)) return std::string("boolean");
        return std::string("unknown");
    };
}

void Interpreter::execute_print(const std::any& value, bool is_fake, const std::string& source) {
    if (!is_fake) {
        std::cout << to_string(value) << std::endl;
        return;
    }
    
    std::string content = to_string(value);
    std::string source_path = source.empty() ? content : source;
    
    std::ifstream file(source_path);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << std::endl;
        }
        file.close();
        return;
    }
    
    std::string cmd = source_path + " 2>&1";
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (pipe) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
            std::cout << buffer;
        }
        return;
    }
    
    std::cout << "[FAKE] " << source_path << std::endl;
}

std::any Interpreter::evaluate(ASTNode* node) {
    if (auto* num = dynamic_cast<NumberLiteralNode*>(node)) {
        return num->value;
    }
    
    if (auto* str = dynamic_cast<StringLiteralNode*>(node)) {
        return str->value;
    }
    
    if (auto* var = dynamic_cast<VariableNode*>(node)) {
        if (variables.find(var->name) != variables.end()) {
            return variables[var->name];
        }
        throw std::runtime_error("Variável não definida: " + var->name);
    }
    
    if (auto* bin = dynamic_cast<BinaryOpNode*>(node)) {
        auto left = evaluate(bin->left.get());
        auto right = evaluate(bin->right.get());
        return evaluate_binary_op(bin->op, left, right);
    }
    
    if (auto* assign = dynamic_cast<AssignmentNode*>(node)) {
        auto value = evaluate(assign->expression.get());
        variables[assign->variable] = value;
        return value;
    }
    
    if (auto* print = dynamic_cast<PrintNode*>(node)) {
        auto value = evaluate(print->expression.get());
        execute_print(value, print->is_fake, print->fake_source);
        return value;
    }
    
    if (auto* input = dynamic_cast<InputNode*>(node)) {
        std::string line;
        std::getline(std::cin, line);
        variables[input->variable] = line;
        return line;
    }

    if (auto* if_node = dynamic_cast<IfNode*>(node)) {
        auto condition = evaluate(if_node->condition.get());
        if (is_truthy(condition)) {
            return evaluate(if_node->then_branch.get());
        } else if (if_node->else_branch) {
            return evaluate(if_node->else_branch.get());
        }
        return std::any();
    }
    
    if (auto* while_node = dynamic_cast<WhileNode*>(node)) {
        while (is_truthy(evaluate(while_node->condition.get()))) {
            evaluate(while_node->body.get());
            if (break_flag) {
                break_flag = false;
                break;
            }
        }
        return std::any();
    }
    
    if (auto* for_node = dynamic_cast<ForNode*>(node)) {
        double start = std::any_cast<double>(evaluate(for_node->start.get()));
        double end = std::any_cast<double>(evaluate(for_node->end.get()));
        double step = for_node->step ? std::any_cast<double>(evaluate(for_node->step.get())) : 1.0;
        
        if (step == 0) step = 1.0;
        
        for (double i = start; (step > 0 ? i <= end : i >= end); i += step) {
            variables[for_node->variable] = i;
            evaluate(for_node->body.get());
            if (break_flag) {
                break_flag = false;
                break;
            }
        }
        return std::any();
    }
    
    if (auto* import_node = dynamic_cast<ImportNode*>(node)) {
    Module* module = module_manager.import(import_node->module_name);
    if (module) {
        // Registra as funções do módulo no interpretador
        for (auto& [name, func] : module->functions) {
            functions[name] = func;
        }
        // Registra as variáveis exportadas
        for (auto& [name, value] : module->exports) {
            variables[name] = value;
        }
        
        // Se tem alias, cria uma variável com o alias
        if (!import_node->alias.empty()) {
            // Cria um wrapper com todas as funções do módulo
            std::unordered_map<std::string, std::any> module_wrapper;
            for (auto& [name, func] : module->functions) {
                module_wrapper[name] = func;
            }
            for (auto& [name, value] : module->exports) {
                module_wrapper[name] = value;
            }
            variables[import_node->alias] = module_wrapper;
        }
    }
    return std::any();
    }

    if (auto* array = dynamic_cast<ArrayNode*>(node)) {
    std::vector<std::any> elements;
    for (auto& elem : array->elements) {
        elements.push_back(evaluate(elem.get()));
    }
    return elements;
    }

    if (auto* access = dynamic_cast<ArrayAccessNode*>(node)) {
    auto idx_val = evaluate(access->index.get());
    double idx = to_number(idx_val);
    
    auto it = variables.find(access->name);
    if (it == variables.end()) {
        throw std::runtime_error("Variável não definida: " + access->name);
    }
    
    if (it->second.type() != typeid(std::vector<std::any>)) {
        throw std::runtime_error(access->name + " não é um array");
    }
    
    auto& vec = std::any_cast<std::vector<std::any>&>(it->second);
    int i = (int)idx;
    if (i < 0 || i >= (int)vec.size()) {
        throw std::runtime_error("Índice fora do array: " + std::to_string(i));
    }
    return vec[i];
    }

    if (auto* arr_assign = dynamic_cast<ArrayAssignmentNode*>(node)) {
    auto idx_val = evaluate(arr_assign->index.get());
    double idx = to_number(idx_val);
    auto value = evaluate(arr_assign->value.get());
    
    auto it = variables.find(arr_assign->name);
    if (it == variables.end()) {
        throw std::runtime_error("Variável não definida: " + arr_assign->name);
    }
    
    if (it->second.type() != typeid(std::vector<std::any>)) {
        throw std::runtime_error(arr_assign->name + " não é um array");
    }
    
    auto& vec = std::any_cast<std::vector<std::any>&>(it->second);
    int i = (int)idx;
    if (i < 0 || i >= (int)vec.size()) {
        throw std::runtime_error("Índice fora do array: " + std::to_string(i));
    }
    vec[i] = value;
    return value;
    }

    if (auto* func = dynamic_cast<FunctionDefNode*>(node)) {
        functions[func->name] = [this, func](const std::vector<std::any>& args) -> std::any {
            auto saved_variables = variables;
            
            for (size_t i = 0; i < func->parameters.size() && i < args.size(); i++) {
                variables[func->parameters[i]] = args[i];
            }
            
            std::any result;
            bool has_return = false;
            
            // Percorre as statements do corpo
            auto* body = dynamic_cast<ProgramNode*>(func->body.get());
            if (body) {
                for (auto& stmt : body->statements) {
                    // Se for um ReturnNode, captura o valor
                    if (auto* return_stmt = dynamic_cast<ReturnNode*>(stmt.get())) {
                        result = evaluate(return_stmt->value.get());
                        has_return = true;
                        break;
                    } else {
                        evaluate(stmt.get());
                    }
                }
            } else {
                // Se o corpo não for um ProgramNode, avalia diretamente
                result = evaluate(func->body.get());
                has_return = true;
            }
            
            variables = saved_variables;
            
            if (!has_return || !result.has_value()) {
                return 0.0;
            }
            return result;
        };
        return std::any();
    }
    
    if (auto* call = dynamic_cast<FunctionCallNode*>(node)) {
        std::vector<std::any> args;
        for (auto& arg : call->arguments) {
            args.push_back(evaluate(arg.get()));
        }
        
        auto it = functions.find(call->name);
        if (it != functions.end()) {
            return it->second(args);
        }
        throw std::runtime_error("Função não definida: " + call->name);
    }
    
    if (auto* return_node = dynamic_cast<ReturnNode*>(node)) {
        return evaluate(return_node->value.get());
    }
    
    if (dynamic_cast<BreakNode*>(node)) {
        break_flag = true;
        return std::any();
    }
    
    if (dynamic_cast<NilNode*>(node)) {
        return std::any();
    }
    
    if (auto* bool_node = dynamic_cast<BooleanNode*>(node)) {
        return bool_node->value;
    }
    
    if (dynamic_cast<ProgramNode*>(node)) {
        auto* program = static_cast<ProgramNode*>(node);
        for (auto& stmt : program->statements) {
            evaluate(stmt.get());
        }
        return std::any();
    }
    
    throw std::runtime_error("Nó AST desconhecido");
}

std::any Interpreter::evaluate_binary_op(const std::string& op, const std::any& left, const std::any& right) {
    if (op == "..") {
        return to_string(left) + to_string(right);
    }
    
    if (op == "is") {
        if (left.type() == right.type()) {
            if (left.type() == typeid(double)) {
                return std::any_cast<double>(left) == std::any_cast<double>(right);
            }
            if (left.type() == typeid(std::string)) {
                return std::any_cast<std::string>(left) == std::any_cast<std::string>(right);
            }
            if (left.type() == typeid(bool)) {
                return std::any_cast<bool>(left) == std::any_cast<bool>(right);
            }
        }
        return false;
    }
    
    if (op == "is not") {
        if (left.type() == right.type()) {
            if (left.type() == typeid(double)) {
                return std::any_cast<double>(left) != std::any_cast<double>(right);
            }
            if (left.type() == typeid(std::string)) {
                return std::any_cast<std::string>(left) != std::any_cast<std::string>(right);
            }
            if (left.type() == typeid(bool)) {
                return std::any_cast<bool>(left) != std::any_cast<bool>(right);
            }
        }
        return true;
    }
    
    double l = to_number(left);
    double r = to_number(right);
    
    if (op == "+") return l + r;
    if (op == "-") return l - r;
    if (op == "*") return l * r;
    if (op == "/") {
        if (r == 0) throw std::runtime_error("Divisão por zero");
        return l / r;
    }
    if (op == "%") return std::fmod(l, r);
    if (op == "==") return l == r;
    if (op == "!=") return l != r;
    if (op == ">") return l > r;
    if (op == "<") return l < r;
    if (op == ">=") return l >= r;
    if (op == "<=") return l <= r;
    
    throw std::runtime_error("Operador desconhecido: " + op);
}

bool Interpreter::is_truthy(const std::any& value) {
    if (!value.has_value()) return false;
    if (value.type() == typeid(bool)) return std::any_cast<bool>(value);
    if (value.type() == typeid(double)) return std::any_cast<double>(value) != 0;
    if (value.type() == typeid(std::string)) return !std::any_cast<std::string>(value).empty();
    return true;
}

std::string Interpreter::to_string(const std::any& value) {
    if (!value.has_value()) return "nil";
    if (value.type() == typeid(std::string)) return std::any_cast<std::string>(value);
    if (value.type() == typeid(double)) {
        double d = std::any_cast<double>(value);
        if (d == (int)d) return std::to_string((int)d);
        return std::to_string(d);
    }
    if (value.type() == typeid(bool)) return std::any_cast<bool>(value) ? "true" : "false";
    if (value.type() == typeid(char)) return std::string(1, std::any_cast<char>(value));
    return "unknown";
}

double Interpreter::to_number(const std::any& value) {
    if (value.type() == typeid(double)) return std::any_cast<double>(value);
    if (value.type() == typeid(int)) return static_cast<double>(std::any_cast<int>(value));
    if (value.type() == typeid(std::string)) {
        try {
            return std::stod(std::any_cast<std::string>(value));
        } catch (...) {
            return 0.0;
        }
    }
    if (value.type() == typeid(bool)) return std::any_cast<bool>(value) ? 1.0 : 0.0;
    return 0.0;
}

void Interpreter::interpret(ASTNode* program) {
    evaluate(program);
}