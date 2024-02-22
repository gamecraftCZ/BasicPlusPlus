#ifndef BASICPLUSPLUS_PARSER_HPP
#define BASICPLUSPLUS_PARSER_HPP

#include <vector>
#include <memory>
#include "Tokenization.hpp"
#include "ExpressionsStatements.hpp"

namespace Parsing {
    class ParsingError : public std::exception {};

    class Parser {
    private:
        std::unique_ptr<std::vector<Tokenization::Token>> tokens;
        uint32_t currentTokenIndex = 0;

        uint32_t errorTokenIndex;
        std::string errorMessage;

        // Top down expression parsing
        ExprStmt::expr_ptr expression();
        
        ExprStmt::expr_ptr orWord();
        
        ExprStmt::expr_ptr andWord();
        
        ExprStmt::expr_ptr unaryNot();

        ExprStmt::expr_ptr comparison();

        ExprStmt::expr_ptr term();

        ExprStmt::expr_ptr factor();

        ExprStmt::expr_ptr unary();

        ExprStmt::expr_ptr primary();
        
        // Statement parsing
        ExprStmt::stmt_ptr declaration();
        
        ExprStmt::stmt_ptr statement();
        
        ExprStmt::stmt_ptr block();
        
        ExprStmt::stmt_ptr printStmt();
        
        ExprStmt::stmt_ptr inputStmt();
        
        ExprStmt::stmt_ptr letDeclaration();
        
        ExprStmt::stmt_ptr toNumStmt();
        
        ExprStmt::stmt_ptr rndStmt();
        
        ExprStmt::stmt_ptr toStrStmt();
        
        ExprStmt::stmt_ptr ifStmt();
        
        ExprStmt::stmt_ptr whileStmt();
        
        // Helper functions
        template<typename... Args>
        bool match(Args... types);  // Check if current token type is any of types
        
        bool check(Tokenization::TokenType);  // Check if current token is of type
        
        Tokenization::Token &advance();  // Return cur token and advance
        
        Tokenization::Token &consume(Tokenization::TokenType type, std::string &&message);  // advance(), but throws if next token is not of type 
        
        bool isAtEnd();
        
        Tokenization::Token &peek();
        
        Tokenization::Token &cur();
        
        Tokenization::Token &prev();

        // Error handling
        void throwErrorAtCurrentToken(std::string &&message);
        
        void synchronize();

    public:
        explicit Parser(std::unique_ptr<std::vector<Tokenization::Token>> &&tokens) : tokens(std::move(tokens)) {}

        std::unique_ptr<std::vector<ExprStmt::stmt_ptr>> parse();
        
        std::string &getErrorMessage();

        Tokenization::Token & getErrorToken();
    };
}

#endif //BASICPLUSPLUS_PARSER_HPP
