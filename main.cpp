#include <iostream>
#include "Tokenization.hpp"

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

        return 0;

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
        return 2;
    }
}
