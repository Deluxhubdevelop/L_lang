#pragma once
#include <vector>
#include <memory>
#include "token.hpp"
#include "ast.hpp"

class Parser {
private:
    std::vector<Token> tokens;
    int position;
    
    Token current_token() const;
    Token peek_token() const;
    void advance();
    bool match(TokenType type);
    bool expect(TokenType type, const std::string& error_message);
    
    std::unique_ptr<ASTNode> parse_program();
    std::unique_ptr<ASTNode> parse_statement();
    std::unique_ptr<ASTNode> parse_expression();
    std::unique_ptr<ASTNode> parse_primary();
    std::unique_ptr<ASTNode> parse_if();
    std::unique_ptr<ASTNode> parse_while();
    std::unique_ptr<ASTNode> parse_for();
    std::unique_ptr<ASTNode> parse_import();
    std::unique_ptr<ASTNode> parse_export();
    std::unique_ptr<ASTNode> parse_function_def();
    std::unique_ptr<ASTNode> parse_block();
    std::unique_ptr<ASTNode> parse_print();
    std::unique_ptr<ASTNode> parse_input();  // ← ADICIONA ESTA LINHA
    
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<ASTNode> parse();
};