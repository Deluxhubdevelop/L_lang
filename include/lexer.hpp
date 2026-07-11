#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "token.hpp"

class Lexer {
private:
    std::string source;
    int position;
    int line;
    int column;
    std::unordered_map<std::string, TokenType> keywords;
    
    char current_char() const;
    char peek_char() const;
    void advance();
    void skip_whitespace();
    void skip_comment();
    
    Token identifier_or_keyword();
    Token number();
    Token string_literal();
    Token handle_print_arrow();
    
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
};