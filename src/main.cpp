#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include "../include/interpreter.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

void print_banner() {
    std::cout << R"(
   _       _     
  | |     | |    
  | | ___ | | __ 
  | |/ _ \| |/ / 
  | | (_) |   <  
  |_|\___/|_|\_\ 
                 
  L Lang v0.1
  A evolução do Lua
)" << std::endl;
}

void run_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro: Não foi possível abrir o arquivo: " << filename << std::endl;
        return;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    
    try {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        Parser parser(tokens);
        auto ast = parser.parse();
        
        Interpreter interpreter;
        interpreter.interpret(ast.get());
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    }
}

void run_repl() {
    print_banner();
    std::cout << "Digite 'exit' para sair" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    std::string line;
    while (true) {
        std::cout << "l> ";
        std::getline(std::cin, line);
        
        if (line == "exit" || line == "quit") break;
        if (line.empty()) continue;
        
        try {
            Lexer lexer(line);
            auto tokens = lexer.tokenize();
            
            Parser parser(tokens);
            auto ast = parser.parse();
            
            Interpreter interpreter;
            interpreter.interpret(ast.get());
        } catch (const std::exception& e) {
            std::cerr << "Erro: " << e.what() << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        run_file(argv[1]);
    } else {
        run_repl();
    }
    return 0;
}