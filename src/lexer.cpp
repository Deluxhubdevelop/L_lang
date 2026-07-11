#include "../include/lexer.hpp"
#include <cctype>
#include <iostream>

Lexer::Lexer(const std::string& source) 
    : source(source), position(0), line(1), column(1) {
    
    keywords = {
        {"if", TokenType::IF},
        {"not", TokenType::NOT},
        {"is", TokenType::IS},
        {"then", TokenType::THEN},
        {"end", TokenType::END},
        {"while", TokenType::WHILE},
        {"do", TokenType::DO},
        {"for", TokenType::FOR},
        {"function", TokenType::FUNCTION},
        {"import", TokenType::IMPORT},
        {"export", TokenType::EXPORT},
        {"return", TokenType::RETURN},
        {"break", TokenType::BREAK},
        {"print", TokenType::PRINT},
        {"input", TokenType::INPUT},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE},
        {"nil", TokenType::NIL},
        {"fake", TokenType::FAKE}
    };
}

char Lexer::current_char() const {
    if (position >= (int)source.length()) return '\0';
    return source[position];
}

char Lexer::peek_char() const {
    if (position + 1 >= (int)source.length()) return '\0';
    return source[position + 1];
}

void Lexer::advance() {
    if (current_char() == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    position++;
}

void Lexer::skip_whitespace() {
    while (std::isspace(current_char())) {
        advance();
    }
}

void Lexer::skip_comment() {
    if (current_char() == '/' && peek_char() == '/') {
        while (current_char() != '\n' && current_char() != '\0') {
            advance();
        }
    }
}

Token Lexer::identifier_or_keyword() {
    int start_pos = position;
    while (std::isalnum(current_char()) || current_char() == '_') {
        advance();
    }
    std::string lexeme = source.substr(start_pos, position - start_pos);
    
    auto it = keywords.find(lexeme);
    if (it != keywords.end()) {
        return Token(it->second, lexeme, line, column - (position - start_pos));
    }
    return Token(TokenType::IDENTIFIER, lexeme, line, column - (position - start_pos));
}

Token Lexer::number() {
    int start_pos = position;
    while (std::isdigit(current_char()) || current_char() == '.') {
        advance();
    }
    std::string lexeme = source.substr(start_pos, position - start_pos);
    return Token(TokenType::NUMBER, lexeme, line, column - (position - start_pos));
}

Token Lexer::string_literal() {
    advance(); // Pula a primeira aspas
    int start_pos = position;
    
    while (current_char() != '"' && current_char() != '\0') {
        if (current_char() == '\\') {
            advance(); // Pula a barra invertida
        }
        advance();
    }
    
    std::string lexeme = source.substr(start_pos, position - start_pos);
    if (current_char() == '"') {
        advance(); // Pula a aspas final
    }
    return Token(TokenType::STRING, lexeme, line, column - (position - start_pos) - 1);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (current_char() != '\0') {
        char c = current_char();
        
        if (std::isspace(c)) {
            skip_whitespace();
            continue;
        }
        
        if (c == '/' && peek_char() == '/') {
            skip_comment();
            continue;
        }
        
        if (std::isalpha(c) || c == '_') {
            Token token = identifier_or_keyword();
            
            // Se for "print" e tiver ">" depois
            if (token.type == TokenType::PRINT) {
                int saved_pos = position;
                int saved_line = line;
                int saved_col = column;
                
                skip_whitespace();
                if (current_char() == '>') {
                    advance();
                    Token arrow = Token(TokenType::PRINT_ARROW, ">", line, column - 1);
                    tokens.push_back(token);
                    tokens.push_back(arrow);
                    continue;
                }
                position = saved_pos;
                line = saved_line;
                column = saved_col;
            }
            
            // Se for "input" e tiver ">" depois
            if (token.type == TokenType::INPUT) {
                int saved_pos = position;
                int saved_line = line;
                int saved_col = column;
                
                skip_whitespace();
                if (current_char() == '>') {
                    advance();
                    Token arrow = Token(TokenType::PRINT_ARROW, ">", line, column - 1);
                    tokens.push_back(token);
                    tokens.push_back(arrow);
                    continue;
                }
                position = saved_pos;
                line = saved_line;
                column = saved_col;
            }
            
            tokens.push_back(token);
            continue;
        }
        
        if (std::isdigit(c)) {
            tokens.push_back(number());
            continue;
        }
        
        if (c == '"') {
            tokens.push_back(string_literal());
            continue;
        }
        
        // Operadores
        switch (c) {
            case '+': advance(); tokens.push_back(Token(TokenType::PLUS, "+", line, column-1)); break;
            case '-': advance(); tokens.push_back(Token(TokenType::MINUS, "-", line, column-1)); break;
            case '*': advance(); tokens.push_back(Token(TokenType::MULTIPLY, "*", line, column-1)); break;
            case '/': advance(); tokens.push_back(Token(TokenType::DIVIDE, "/", line, column-1)); break;
            case '%': advance(); tokens.push_back(Token(TokenType::MODULO, "%", line, column-1)); break;
            case '=':
                if (peek_char() == '=') {
                    advance(); advance();
                    tokens.push_back(Token(TokenType::EQUALS, "==", line, column-2));
                } else {
                    advance();
                    tokens.push_back(Token(TokenType::ASSIGN, "=", line, column-1));
                }
                break;
            case '!':
                if (peek_char() == '=') {
                    advance(); advance();
                    tokens.push_back(Token(TokenType::NOT_EQUALS, "!=", line, column-2));
                }
                break;
            case '>':
                if (peek_char() == '=') {
                    advance(); advance();
                    tokens.push_back(Token(TokenType::GREATER_EQUAL, ">=", line, column-2));
                } else {
                    advance();
                    tokens.push_back(Token(TokenType::GREATER, ">", line, column-1));
                }
                break;
            case '<':
                if (peek_char() == '=') {
                    advance(); advance();
                    tokens.push_back(Token(TokenType::LESS_EQUAL, "<=", line, column-2));
                } else {
                    advance();
                    tokens.push_back(Token(TokenType::LESS, "<", line, column-1));
                }
                break;
            case '.': 
                if (peek_char() == '.') {
                    advance(); advance();
                    tokens.push_back(Token(TokenType::CONCAT, "..", line, column-2));
                }
                break;
            case '(': advance(); tokens.push_back(Token(TokenType::LPAREN, "(", line, column-1)); break;
            case ')': advance(); tokens.push_back(Token(TokenType::RPAREN, ")", line, column-1)); break;
            case '{': advance(); tokens.push_back(Token(TokenType::LBRACE, "{", line, column-1)); break;
            case '}': advance(); tokens.push_back(Token(TokenType::RBRACE, "}", line, column-1)); break;           
            case '[': advance(); tokens.push_back(Token(TokenType::LBRACKET, "[", line, column-1)); break;
            case ']': advance(); tokens.push_back(Token(TokenType::RBRACKET, "]", line, column-1)); break;
            case ',': advance(); tokens.push_back(Token(TokenType::COMMA, ",", line, column-1)); break;
            case ';': advance(); tokens.push_back(Token(TokenType::SEMICOLON, ";", line, column-1)); break;
            default:
                advance();
                tokens.push_back(Token(TokenType::UNKNOWN, std::string(1, c), line, column-1));
                break;
        }
    }
    
    tokens.push_back(Token(TokenType::END_OF_FILE, "EOF", line, column));
    return tokens;
}