#pragma once
#include <string>
#include <vector>
#include <memory>
#include <variant>

struct ASTNode {
    virtual ~ASTNode() = default;
};

struct ProgramNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
};

struct NumberLiteralNode : ASTNode {
    double value;
    NumberLiteralNode(double value) : value(value) {}
};

struct StringLiteralNode : ASTNode {
    std::string value;
    StringLiteralNode(const std::string& value) : value(value) {}
};

struct VariableNode : ASTNode {
    std::string name;
    VariableNode(const std::string& name) : name(name) {}
};

struct BinaryOpNode : ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    
    BinaryOpNode(const std::string& op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
};

struct AssignmentNode : ASTNode {
    std::string variable;
    std::unique_ptr<ASTNode> expression;
    
    AssignmentNode(const std::string& variable, std::unique_ptr<ASTNode> expression)
        : variable(variable), expression(std::move(expression)) {}
};

struct PrintNode : ASTNode {
    std::unique_ptr<ASTNode> expression;
    bool is_fake;
    std::string fake_source;
    
    PrintNode(std::unique_ptr<ASTNode> expression, bool fake = false, const std::string& source = "")
        : expression(std::move(expression)), is_fake(fake), fake_source(source) {}
};

struct InputNode : ASTNode {
    std::string variable;
    InputNode(const std::string& var) : variable(var) {}
};

struct ImportNode : ASTNode {
    std::string module_name;
    std::string alias;
    
    ImportNode(const std::string& name, const std::string& alias = "")
        : module_name(name), alias(alias) {}
};

struct ExportNode : ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> value;
    
    ExportNode(const std::string& name, std::unique_ptr<ASTNode> value)
        : name(name), value(std::move(value)) {}
};

struct IfNode : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> then_branch;
    std::unique_ptr<ASTNode> else_branch;
    
    IfNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> then_b, std::unique_ptr<ASTNode> else_b = nullptr)
        : condition(std::move(cond)), then_branch(std::move(then_b)), else_branch(std::move(else_b)) {}
};

struct WhileNode : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;
    
    WhileNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> body)
        : condition(std::move(cond)), body(std::move(body)) {}
};

struct ForNode : ASTNode {
    std::string variable;
    std::unique_ptr<ASTNode> start;
    std::unique_ptr<ASTNode> end;
    std::unique_ptr<ASTNode> step;
    std::unique_ptr<ASTNode> body;
    
    ForNode(const std::string& var, std::unique_ptr<ASTNode> start, std::unique_ptr<ASTNode> end,
            std::unique_ptr<ASTNode> step, std::unique_ptr<ASTNode> body)
        : variable(var), start(std::move(start)), end(std::move(end)), 
          step(std::move(step)), body(std::move(body)) {}
};

struct FunctionDefNode : ASTNode {
    std::string name;
    std::vector<std::string> parameters;
    std::unique_ptr<ASTNode> body;
    
    FunctionDefNode(const std::string& name, const std::vector<std::string>& params, std::unique_ptr<ASTNode> body)
        : name(name), parameters(params), body(std::move(body)) {}
};

struct FunctionCallNode : ASTNode {
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    
    FunctionCallNode(const std::string& name, std::vector<std::unique_ptr<ASTNode>> args)
        : name(name), arguments(std::move(args)) {}
};

struct ReturnNode : ASTNode {
    std::unique_ptr<ASTNode> value;
    ReturnNode(std::unique_ptr<ASTNode> value) : value(std::move(value)) {}
};

struct BreakNode : ASTNode {};
struct NilNode : ASTNode {};
struct BooleanNode : ASTNode {
    bool value;
    BooleanNode(bool value) : value(value) {}
};

// ========== NOVOS NÓS PARA ARRAYS ==========

struct ArrayNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> elements;
};

struct ArrayAccessNode : ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> index;
    
    ArrayAccessNode(const std::string& name, std::unique_ptr<ASTNode> idx)
        : name(name), index(std::move(idx)) {}
};

struct ArrayAssignmentNode : ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> index;
    std::unique_ptr<ASTNode> value;
    
    ArrayAssignmentNode(const std::string& name, std::unique_ptr<ASTNode> idx, std::unique_ptr<ASTNode> val)
        : name(name), index(std::move(idx)), value(std::move(val)) {}
};