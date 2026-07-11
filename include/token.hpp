#pragma once
#include <string>
#include <unordered_map>

enum class TokenType {
    // Palavras-chave
    IF, NOT, THEN, END,
    WHILE, DO, FOR, FUNCTION, RETURN, BREAK,
    PRINT, INPUT, TRUE, FALSE, NIL, IS, IMPORT, EXPORT,
    
    // Operadores
    PLUS, MINUS, MULTIPLY, DIVIDE, MODULO,
    ASSIGN, EQUALS, NOT_EQUALS, GREATER, LESS, GREATER_EQUAL, LESS_EQUAL,
    AND, OR,
    
    // Literais
    IDENTIFIER, NUMBER, STRING,
    
    // Delimitadores
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET, COMMA, SEMICOLON,
    CONCAT, // ".."
    
    // Especiais
    PRINT_ARROW, // ">"
    FAKE,        // "fake"
    
    COMMENT,
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    
    Token() : type(TokenType::UNKNOWN), line(0), column(0) {}
    Token(TokenType type, const std::string& lexeme, int line, int column)
        : type(type), lexeme(lexeme), line(line), column(column) {}
};