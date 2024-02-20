#include <iostream>
#include "Tokenization.hpp"
#include "Parser.hpp"
#include "Interpreter.hpp"

int main() {
    try {
        // Tokenization
        std::unique_ptr<std::vector<Tokenization::Token>> tokens;
        Tokenization::Tokenizer tokenizer(std::cin);
        try {
            tokenizer.scanTokens();
            tokens = tokenizer.getTokens();
        } catch (const Tokenization::TokenizationError &) {
            std::cout << "[line " << tokenizer.getErrorLine() << "]"
                      << " Tokenization error: " << tokenizer.getErrorMessage() << std::endl;
            return 11;
        }
        
        // Parsing
        Parsing::Parser parser(std::move(tokens));
        std::unique_ptr<std::vector<ExprStmt::stmt_ptr>> statements;
        try {
            statements = parser.parse();
        } catch (const Parsing::ParsingError &) {
            Tokenization::Token &errorToken = parser.getErrorToken();
            if (errorToken.type == Tokenization::EOF_TOKEN) {
                std::cout << "[line " << errorToken.line << " (at end of file)]"
                          << " Parsing error: " << parser.getErrorMessage() << std::endl;
            } else {
                std::cout << "[line " << errorToken.line << "] (at '" << errorToken.lexeme << "')"
                          << " Parsing error: " << parser.getErrorMessage() << std::endl;
            }
            return 12;
        }
        
        // Interpreting
        Interpreting::Interpreter interpreter;

        try {
            for (auto &statement : *statements) {
                interpreter.interpret(statement);
            }
        } catch (const Interpreting::InterpreterError &) {
            // TODO
            std::cout //<< "[line " << errorToken.line << "] (at '" << errorToken.lexeme << "')"
                      << " Interpreter error: " << interpreter.getErrorMessage() << std::endl;
//            Tokenization::Token &errorToken = parser.getErrorToken();
//            if (errorToken.type == Tokenization::EOF_TOKEN) {
//                std::cout << "[line " << errorToken.line << " (at end of file)]"
//                          << " Parsing error: " << parser.getErrorMessage() << std::endl;
//            } else {
//                std::cout << "[line " << errorToken.line << "] (at '" << errorToken.lexeme << "')"
//                          << " Parsing error: " << parser.getErrorMessage() << std::endl;
//            }
            return 13;
        }

        return 0;

    } catch (const std::exception &e) {
        std::cerr << "Unexpected exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
        return 2;
    }
}
