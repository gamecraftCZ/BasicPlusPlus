#include "Parser.hpp"
#include "Tokenization.hpp"

using namespace Tokenization;
using namespace ExprStmt;

namespace Parsing {
    // Helper functions
    template<typename... Args>
    bool Parser::match(Args... types) {
        static_assert((std::is_same_v<Args, Tokenization::TokenType> && ...),
                      "All arguments must be of type TokenType");
        for (Tokenization::TokenType type: {types...}) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }

    bool Parser::check(Tokenization::TokenType type) {
        return cur().type == type;
    }

    Token &Parser::advance() {
        if (currentTokenIndex <= tokens->size()) currentTokenIndex++;
        return prev();
    }

    Tokenization::Token &Parser::consume(Tokenization::TokenType type, std::string &&message) {
        if (check(type)) return advance();

        throwErrorAtCurrentToken(std::move(message));
        // Unreachable
    }

    bool Parser::isAtEnd() {
        return cur().type == EOF_TOKEN || peek().type == EOF_TOKEN;
    }

    Token &Parser::peek() {
        return tokens->at(currentTokenIndex + 1);
    }

    Token &Parser::cur() {
        return tokens->at(currentTokenIndex);
    }

    Token &Parser::prev() {
        return tokens->at(currentTokenIndex - 1);
    }

    void Parser::throwErrorAtCurrentToken(std::string &&message) {
        errorTokenIndex = currentTokenIndex;
        errorMessage = std::move(message);
        throw ParsingError();
    }

    // Top down expression parsing
    expr_ptr Parser::expression() {
        return comparison();
    }

    expr_ptr Parser::comparison() {
        expr_ptr expr = term();

        while (match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL, NOT_EQUAL, EQUAL_EQUAL)) {
            Token op = prev();
            expr_ptr right = term();
            expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right));
        }

        return expr;
    }

    expr_ptr Parser::term() {
        expr_ptr expr = factor();

        while (match(MINUS, PLUS)) {
            Token op = prev();
            expr_ptr right = factor();
            expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right));
        }

        return expr;
    }

    expr_ptr Parser::factor() {
        expr_ptr expr = unary();

        while (match(SLASH, STAR)) {
            Token op = prev();
            expr_ptr right = unary();
            expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right));
        }

        return expr;
    }

    expr_ptr Parser::unary() {
        if (match(NOT, MINUS)) {
            Token op = prev();
            expr_ptr right = unary();
            return std::make_unique<UnaryExpr>(std::move(op), std::move(right));
        }

        return primary();
    }

    expr_ptr Parser::primary() {
        if (match(NUMBER, STRING, BOOLEAN)) {
            Literal value = prev().literal.value();
            return std::make_unique<LiteralExpr>(std::move(value));
        }

        if (match(LEFT_PAREN)) {
            expr_ptr expr = expression();
            consume(RIGHT_PAREN, "Expect ')' after expression.");
            return std::make_unique<GroupingExpr>(std::move(expr));
        }

        throwErrorAtCurrentToken("Expression expected.");
        return nullptr;  // Unreachable
    }
    
    // Parsing statements
    stmt_ptr Parser::statement() {
        while (match(REM)) {}  // Skip comments
        if (match(PRINT)) return printStatement();
        // TODO

        throwErrorAtCurrentToken("Statement expected.");
        return nullptr;  // Unreachable
    }
    
    stmt_ptr Parser::printStatement() {
        expr_ptr value = expression();
        return std::make_unique<PrintStmt>(std::move(value));
    }
    
    // Parse all the statements
    std::unique_ptr<std::vector<stmt_ptr>> Parser::parse() {
        std::unique_ptr<std::vector<stmt_ptr>> statements = std::make_unique<std::vector<stmt_ptr>>();

        while (!isAtEnd()) {
            stmt_ptr stmt = statement();
            statements->push_back(std::move(stmt));
        }

        return statements;
    }

    std::string &Parser::getErrorMessage() {
        return errorMessage;
    }

    Tokenization::Token &Parser::getErrorToken() {
        return tokens->at(errorTokenIndex);
    }

    void Parser::synchronize() {
        advance();

        while (!isAtEnd()) {
            switch (peek().type) {
                case Tokenization::REM:
                case Tokenization::LET:
                case Tokenization::INPUT:
                case Tokenization::PRINT:
                case Tokenization::TONUM:
                case Tokenization::TOSTR:
                case Tokenization::IF:
                case Tokenization::WHILE:
                    return;
            }

            advance();
        }
    }
}
