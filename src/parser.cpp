#include "../include/parser.hpp"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), position(0) {}

Token Parser::current_token() const {
    if (position >= (int)tokens.size()) {
        return Token(TokenType::END_OF_FILE, "EOF", 0, 0);
    }
    return tokens[position];
}

Token Parser::peek_token() const {
    if (position + 1 >= (int)tokens.size()) {
        return Token(TokenType::END_OF_FILE, "EOF", 0, 0);
    }
    return tokens[position + 1];
}

void Parser::advance() {
    if (position < (int)tokens.size()) {
        position++;
    }
}

bool Parser::match(TokenType type) {
    if (current_token().type == type) {
        advance();
        return true;
    }
    return false;
}

bool Parser::expect(TokenType type, const std::string& error_message) {
    if (current_token().type == type) {
        advance();
        return true;
    }
    throw std::runtime_error(error_message + " na linha " + std::to_string(current_token().line));
}

std::unique_ptr<ASTNode> Parser::parse_program() {
    auto program = std::make_unique<ProgramNode>();
    
    while (current_token().type != TokenType::END_OF_FILE) {
        program->statements.push_back(parse_statement());
    }
    
    return program;
}

std::unique_ptr<ASTNode> Parser::parse_statement() {
    Token token = current_token();
    
    switch (token.type) {
        case TokenType::IF:
            return parse_if();
        case TokenType::WHILE:
            return parse_while();
        case TokenType::FOR:
            return parse_for();
        case TokenType::FUNCTION:
            return parse_function_def();
        case TokenType::IMPORT:
            return parse_import();
        case TokenType::EXPORT:
            return parse_export();
        case TokenType::RETURN: {
            advance();
            auto value = parse_expression();
            return std::make_unique<ReturnNode>(std::move(value));
        }
        case TokenType::BREAK:
            advance();
            return std::make_unique<BreakNode>();
        case TokenType::PRINT:
            return parse_print();
        case TokenType::INPUT:
            return parse_input();
        case TokenType::IDENTIFIER: {
            std::string name = token.lexeme;
            advance();
            
            // Verifica se é chamada de função
            if (current_token().type == TokenType::LPAREN) {
                advance();
                std::vector<std::unique_ptr<ASTNode>> args;
                
                if (current_token().type != TokenType::RPAREN) {
                    args.push_back(parse_expression());
                    while (current_token().type == TokenType::COMMA) {
                        advance();
                        args.push_back(parse_expression());
                    }
                }
                expect(TokenType::RPAREN, "Esperado ')'");
                return std::make_unique<FunctionCallNode>(name, std::move(args));
            }
            // Verifica se é atribuição
            else if (current_token().type == TokenType::ASSIGN) {
                advance();
                auto expr = parse_expression();
                return std::make_unique<AssignmentNode>(name, std::move(expr));
            }
            // É uma expressão simples
            else {
                position--;
                return parse_expression();
            }
        }
        default:
            return parse_expression();
    }
}

std::unique_ptr<ASTNode> Parser::parse_if() {
    expect(TokenType::IF, "Esperado 'if'");
    
    auto condition = parse_expression();
    expect(TokenType::THEN, "Esperado 'then'");
    
    auto then_branch = parse_block();
    
    std::unique_ptr<ASTNode> else_branch = nullptr;
    if (current_token().type == TokenType::IF && peek_token().type == TokenType::NOT) {
        advance();
        advance();
        else_branch = parse_block();
    }
    
    expect(TokenType::END, "Esperado 'end'");
    
    return std::make_unique<IfNode>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

std::unique_ptr<ASTNode> Parser::parse_while() {
    expect(TokenType::WHILE, "Esperado 'while'");
    auto condition = parse_expression();
    auto body = parse_block();
    return std::make_unique<WhileNode>(std::move(condition), std::move(body));
}

std::unique_ptr<ASTNode> Parser::parse_for() {
    expect(TokenType::FOR, "Esperado 'for'");
    
    std::string var = current_token().lexeme;
    expect(TokenType::IDENTIFIER, "Esperado nome de variável");
    expect(TokenType::ASSIGN, "Esperado '='");
    
    auto start = parse_expression();
    expect(TokenType::COMMA, "Esperado ','");
    auto end = parse_expression();
    
    std::unique_ptr<ASTNode> step = nullptr;
    if (current_token().type == TokenType::COMMA) {
        advance();
        step = parse_expression();
    }
    
    expect(TokenType::THEN, "Esperado 'then'");
    auto body = parse_block();
    expect(TokenType::END, "Esperado 'end'");
    
    return std::make_unique<ForNode>(var, std::move(start), std::move(end), std::move(step), std::move(body));
}

std::unique_ptr<ASTNode> Parser::parse_import() {
    expect(TokenType::IMPORT, "Esperado 'import'");
    
    std::string module_name = current_token().lexeme;
    expect(TokenType::STRING, "Esperado nome do módulo");
    
    std::string alias = module_name;
    
    // Verifica se tem "as" (opcional)
    if (current_token().type == TokenType::IDENTIFIER && current_token().lexeme == "as") {
        advance();
        alias = current_token().lexeme;
        expect(TokenType::IDENTIFIER, "Esperado nome do alias");
    }
    
    return std::make_unique<ImportNode>(module_name, alias);
}

std::unique_ptr<ASTNode> Parser::parse_export() {
    expect(TokenType::EXPORT, "Esperado 'export'");
    
    // Verifica se é função ou variável
    if (current_token().type == TokenType::FUNCTION) {
        auto func = parse_function_def();
        // Faz o cast para FunctionDefNode para pegar o nome
        auto* func_node = dynamic_cast<FunctionDefNode*>(func.get());
        if (func_node) {
            std::string name = func_node->name;
            return std::make_unique<ExportNode>(name, std::move(func));
        }
        throw std::runtime_error("Erro ao exportar função");
    }
    
    // É uma variável
    std::string name = current_token().lexeme;
    expect(TokenType::IDENTIFIER, "Esperado nome");
    expect(TokenType::ASSIGN, "Esperado '='");
    auto value = parse_expression();
    return std::make_unique<ExportNode>(name, std::move(value));
}

std::unique_ptr<ASTNode> Parser::parse_input() {
    expect(TokenType::INPUT, "Esperado 'input'");
    
    // Consome o ">" se existir (input>)
    if (current_token().type == TokenType::PRINT_ARROW) {
        advance();
    }
    
    // Pega o nome da variável
    std::string var_name = current_token().lexeme;
    expect(TokenType::IDENTIFIER, "Esperado nome da variável");
    
    return std::make_unique<InputNode>(var_name);
}

std::unique_ptr<ASTNode> Parser::parse_function_def() {
    expect(TokenType::FUNCTION, "Esperado 'function'");
    
    std::string name = current_token().lexeme;
    expect(TokenType::IDENTIFIER, "Esperado nome da função");
    expect(TokenType::LPAREN, "Esperado '('");
    
    std::vector<std::string> params;
    if (current_token().type != TokenType::RPAREN) {
        params.push_back(current_token().lexeme);
        expect(TokenType::IDENTIFIER, "Esperado nome do parâmetro");
        
        while (current_token().type == TokenType::COMMA) {
            advance();
            params.push_back(current_token().lexeme);
            expect(TokenType::IDENTIFIER, "Esperado nome do parâmetro");
        }
    }
    expect(TokenType::RPAREN, "Esperado ')'");
    
    auto body = parse_block();
    expect(TokenType::END, "Esperado 'end'");
    
    return std::make_unique<FunctionDefNode>(name, params, std::move(body));
}

std::unique_ptr<ASTNode> Parser::parse_block() {
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    while (current_token().type != TokenType::END && 
           current_token().type != TokenType::END_OF_FILE) {
        
        if (current_token().type == TokenType::IF && peek_token().type == TokenType::NOT) {
            break;
        }
        
        statements.push_back(parse_statement());
    }
    
    auto block = std::make_unique<ProgramNode>();
    block->statements = std::move(statements);
    return block;
}

std::unique_ptr<ASTNode> Parser::parse_print() {
    expect(TokenType::PRINT, "Esperado 'print'");
    
    bool is_fake = false;
    std::string fake_source;
    
    if (current_token().type == TokenType::PRINT_ARROW) {
        advance();
    }
    
    if (current_token().type == TokenType::FAKE) {
        advance();
        expect(TokenType::ASSIGN, "Esperado '='");
        
        std::string value = current_token().lexeme;
        if (value == "Y" || value == "y") {
            is_fake = true;
            advance();
            if (current_token().type == TokenType::STRING) {
                fake_source = current_token().lexeme;
                advance();
            } else if (current_token().type == TokenType::IDENTIFIER) {
                fake_source = current_token().lexeme;
                advance();
            }
        }
    }
    
    auto expr = parse_expression();
    return std::make_unique<PrintNode>(std::move(expr), is_fake, fake_source);
}

std::unique_ptr<ASTNode> Parser::parse_expression() {
    auto left = parse_primary();
    
    while (true) {
        Token token = current_token();
        std::string op;
        
        switch (token.type) {
            case TokenType::PLUS: op = "+"; break;
            case TokenType::MINUS: op = "-"; break;
            case TokenType::MULTIPLY: op = "*"; break;
            case TokenType::DIVIDE: op = "/"; break;
            case TokenType::MODULO: op = "%"; break;
            case TokenType::EQUALS: op = "=="; break;
            case TokenType::NOT_EQUALS: op = "!="; break;
            case TokenType::GREATER: op = ">"; break;
            case TokenType::LESS: op = "<"; break;
            case TokenType::GREATER_EQUAL: op = ">="; break;
            case TokenType::LESS_EQUAL: op = "<="; break;
            case TokenType::IS: {
                // Verifica se é "is not"
                if (peek_token().type == TokenType::NOT) {
                    advance(); // Consome IS
                    advance(); // Consome NOT
                    op = "is not";
                } else {
                    advance();
                    op = "is";
                }
                break;
            }
            case TokenType::CONCAT: op = ".."; break;
            default: return left;
        }
        
        if (op != "is" && op != "is not") {
            advance();
        }
        // Se for "is" ou "is not", já consumimos os tokens acima
        
        auto right = parse_primary();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
}

std::unique_ptr<ASTNode> Parser::parse_primary() {
    Token token = current_token();
    
    switch (token.type) {
        case TokenType::NUMBER:
            advance();
            return std::make_unique<NumberLiteralNode>(std::stod(token.lexeme));
            
        case TokenType::STRING:
            advance();
            return std::make_unique<StringLiteralNode>(token.lexeme);
            
        case TokenType::TRUE:
            advance();
            return std::make_unique<BooleanNode>(true);
            
        case TokenType::FALSE:
            advance();
            return std::make_unique<BooleanNode>(false);
            
        case TokenType::NIL:
            advance();
            return std::make_unique<NilNode>();
            
        case TokenType::LBRACE: {  // ARRAY: {1, 2, 3}
            advance();
            auto array = std::make_unique<ArrayNode>();
            
            if (current_token().type != TokenType::RBRACE) {
                array->elements.push_back(parse_expression());
                while (current_token().type == TokenType::COMMA) {
                    advance();
                    if (current_token().type == TokenType::RBRACE) break;
                    array->elements.push_back(parse_expression());
                }
            }
            expect(TokenType::RBRACE, "Esperado '}'");
            return array;
        }
            
        case TokenType::IDENTIFIER: {
            std::string name = token.lexeme;
            advance();
            
            // ACESSO A ARRAY: lista[0]
            if (current_token().type == TokenType::LBRACKET) {
                advance();
                auto index = parse_expression();
                expect(TokenType::RBRACKET, "Esperado ']'");
                
                // Verifica se é atribuição: lista[0] = 10
                if (current_token().type == TokenType::ASSIGN) {
                    advance();
                    auto value = parse_expression();
                    return std::make_unique<ArrayAssignmentNode>(name, std::move(index), std::move(value));
                }
                
                return std::make_unique<ArrayAccessNode>(name, std::move(index));
            }
            
            // Chamada de função
            if (current_token().type == TokenType::LPAREN) {
                advance();
                std::vector<std::unique_ptr<ASTNode>> args;
                
                if (current_token().type != TokenType::RPAREN) {
                    args.push_back(parse_expression());
                    while (current_token().type == TokenType::COMMA) {
                        advance();
                        args.push_back(parse_expression());
                    }
                }
                expect(TokenType::RPAREN, "Esperado ')'");
                return std::make_unique<FunctionCallNode>(name, std::move(args));
            }
            
            // Atribuição normal
            if (current_token().type == TokenType::ASSIGN) {
                advance();
                auto expr = parse_expression();
                return std::make_unique<AssignmentNode>(name, std::move(expr));
            }
            
            return std::make_unique<VariableNode>(name);
        }
            
        case TokenType::LPAREN: {
            advance();
            auto expr = parse_expression();
            expect(TokenType::RPAREN, "Esperado ')'");
            return expr;
        }
        
        default:
            throw std::runtime_error("Token inesperado: " + token.lexeme);
    }
}

std::unique_ptr<ASTNode> Parser::parse() {
    return parse_program();
}