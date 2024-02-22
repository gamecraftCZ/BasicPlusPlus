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
        return orWord();
    }

    expr_ptr Parser::orWord() {
        expr_ptr expr = andWord();

        while (match(OR)) {
            Token op = prev();
            expr_ptr right = andWord();
            expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right), prev().line);
        }

        return expr;
    }

    expr_ptr Parser::andWord() {
        expr_ptr expr = unaryNot();

        while (match(AND)) {
            Token op = prev();
            expr_ptr right = unaryNot();
            expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right), prev().line);
        }

        return expr;
    }
    
    expr_ptr Parser::unaryNot() {
        if (match(NOT)) {
            Token op = prev();
            expr_ptr right = unaryNot();
            return std::make_unique<UnaryExpr>(std::move(op), std::move(right), prev().line);
        }

        return comparison();
    }

    expr_ptr Parser::comparison() {
        expr_ptr expr = term();

        while (match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL, NOT_EQUAL, EQUAL_EQUAL)) {
            Token op = prev();
            expr_ptr right = term();
            expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right), prev().line);
        }

        return expr;
    }

    expr_ptr Parser::term() {
        expr_ptr expr = factor();

        while (match(MINUS, PLUS)) {
            Token op = prev();
            expr_ptr right = factor();
            expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right), prev().line);
        }

        return expr;
    }

    expr_ptr Parser::factor() {
        expr_ptr expr = unary();

        while (match(SLASH, STAR)) {
            Token op = prev();
            expr_ptr right = unary();
            expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right), prev().line);
        }

        return expr;
    }

    expr_ptr Parser::unary() {
        if (match(MINUS)) {
            Token op = prev();
            expr_ptr right = unary();
            return std::make_unique<UnaryExpr>(std::move(op), std::move(right), prev().line);
        }

        return primary();
    }

    expr_ptr Parser::primary() {
        if (match(NUMBER, STRING, BOOLEAN)) {
            Literal value = prev().literal.value();
            return std::make_unique<LiteralExpr>(std::move(value), prev().line);
        }

        if (match(LEFT_PAREN)) {
            expr_ptr expr = expression();
            consume(RIGHT_PAREN, "Expect ')' after expression.");
            return std::make_unique<GroupingExpr>(std::move(expr), prev().line);
        }

        if (match(IDENTIFIER)) {
            std::string varName = std::get<std::string>(prev().literal.value());
            return std::make_unique<VarExpr>(std::move(varName), prev().line);
        }

        throwErrorAtCurrentToken("Expression expected.");
        return nullptr;  // Unreachable
    }
    
    // Parsing statements
    stmt_ptr Parser::declaration() {
        if (match(LET)) return letDeclaration();

        return statement();
    }
    
    stmt_ptr Parser::statement() {
        if (match(IF)) return ifStmt();
        if (match(WHILE)) return whileStmt();
        
        if (match(PRINT)) return printStmt();
        if (match(INPUT)) return inputStmt();
        if (match(TONUM)) return toNumStmt();
        if (match(TOSTR)) return toStrStmt();
        if (match(RND)) return rndStmt();
        if (match(BREAK)) return std::make_unique<BreakStmt>(prev().line);
        if (match(CONTINUE)) return std::make_unique<ContinueStmt>(prev().line);
        
        throwErrorAtCurrentToken("Statement expected.");
        return nullptr;  // Unreachable
    }
    
    ExprStmt::stmt_ptr Parser::block() {
        std::vector<stmt_ptr> declars;
        
        while (!check(END) && !check(ELSE)) {
            declars.push_back(declaration());
        }
        
        return std::make_unique<BlockStmt>(std::move(declars), prev().line);
    }
    
    stmt_ptr Parser::printStmt() {
        expr_ptr value = expression();
        return std::make_unique<PrintStmt>(std::move(value), prev().line);
    }
    
    stmt_ptr Parser::inputStmt() {
        expr_ptr value = expression();
        consume(COMMA, "INPUT expects two parameters separated by comma.");
        Token targetVariableToken = consume(IDENTIFIER, "INPUT second parameter must be variable identifier.");
        std::string targetVariable = std::get<std::string>(targetVariableToken.literal.value());
        return std::make_unique<InputStmt>(std::move(value), std::move(targetVariable), prev().line);
    }
    
    stmt_ptr Parser::toNumStmt() {
        Token srcVarToken = consume(IDENTIFIER, "TONUM first parameter must be variable identifier.");
        std::string srcVarName = std::get<std::string>(srcVarToken.literal.value());
        
        std::optional<std::string> dstVarName = std::nullopt;
        
        if (check(COMMA)) {
            advance();

            Token dstVarToken = consume(IDENTIFIER, "TONUM second parameter must be variable identifier.");
            dstVarName = std::get<std::string>(dstVarToken.literal.value());
        }
        
        return std::make_unique<ToNumStmt>(std::move(srcVarName), std::move(dstVarName), prev().line);
    }
    
    stmt_ptr Parser::toStrStmt() {
        Token srcVarToken = consume(IDENTIFIER, "TOSTR first parameter must be variable identifier.");
        std::string srcVarName = std::get<std::string>(srcVarToken.literal.value());
        
        std::optional<std::string> dstVarName = std::nullopt;
        
        if (check(COMMA)) {
            advance();

            Token dstVarToken = consume(IDENTIFIER, "TOSTR second parameter must be variable identifier.");
            dstVarName = std::get<std::string>(dstVarToken.literal.value());
        }
        
        return std::make_unique<ToNumStmt>(std::move(srcVarName), std::move(dstVarName), prev().line);
    }
    
    stmt_ptr Parser::rndStmt() {
        Token dstVarToken = consume(IDENTIFIER, "RND first parameter must be variable identifier.");
        std::string dstVarName = std::get<std::string>(dstVarToken.literal.value());

        consume(COMMA, "RND expects three parameters separated by comma.");
        expr_ptr lowerBound = expression();
        
        consume(COMMA, "RND expects three parameters separated by comma.");
        expr_ptr upperBound = expression();
        
        return std::make_unique<RndStmt>(std::move(dstVarName), std::move(lowerBound), std::move(upperBound), prev().line);
    }
    
    stmt_ptr Parser::ifStmt() {
        expr_ptr condition = expression();
        consume(THEN, "THEN keyword expected after IF condition.");
        
        stmt_ptr thenBranch = block();
        std::optional<stmt_ptr> elseBranch = std::nullopt;
        
        if (match(ELSE)) {
            elseBranch = block();
        }

        consume(END, "END keyword expected at the end of IF condition block.");
        return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch), prev().line);
    }
    
    stmt_ptr Parser::whileStmt() {
        expr_ptr condition = expression();
        consume(DO, "DO keyword expected after WHILE condition.");
        
        stmt_ptr thenBranch = block();
        
        consume(END, "END keyword expected at the end of IF condition block.");
        return std::make_unique<WhileStmt>(std::move(condition), std::move(thenBranch), prev().line);
    }
    
    stmt_ptr Parser::letDeclaration() {
        Token variableToken = consume(IDENTIFIER, "Variable name expected after LET.");
        std::string variableName = std::get<std::string>(variableToken.literal.value());
        consume(EQUAL, "Equal sign expected after variable identifier.");
        expr_ptr value = expression();
        return std::make_unique<LetStmt>(std::move(value), std::move(variableName), prev().line);
    }
    
    // Parse all the statements
    std::unique_ptr<std::vector<stmt_ptr>> Parser::parse() {
        std::unique_ptr<std::vector<stmt_ptr>> statements = std::make_unique<std::vector<stmt_ptr>>();

        while (!isAtEnd()) {
            stmt_ptr stmt = declaration();
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
