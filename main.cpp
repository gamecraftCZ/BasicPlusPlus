#include <iostream>
#include <memory>
#include <fstream>
#include "Tokenization.hpp"
#include "Parser.hpp"
#include "Interpreter.hpp"

int main(int argc, char** argv) {
    try {
        // Open file with input
        std::vector<std::string> args(argv, argv + argc);

        // TODO Input params handling
//        if (argc < 3) {
//            std::cout << "Usage: " << args[0] << " [conditions] <input_file> <output_file>" << std::endl;
//            throw std::runtime_error("No input and output file specified.");
//        }


        std::string inputFilename = args[1];
        std::ifstream inStream(inputFilename);
        if (inStream.fail()) throw std::runtime_error("Failed to open input file");
        
//        Tokenization::Tokenizer tokenizer(std::cin);
        Tokenization::Tokenizer tokenizer(inStream);
        
        // Tokenization
        std::unique_ptr<std::vector<Tokenization::Token>> tokens;
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
        // Set seed for rnd generator
        std::srand(std::time(0));

        try {
            for (auto &statement : *statements) {
                interpreter.interpret(statement);
            }
        } catch (const Interpreting::InterpreterError &) {
            std::cout << "[line " << interpreter.getErrorLine() << "]"
                      << " Interpreter error: " << interpreter.getErrorMessage() << std::endl;
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
