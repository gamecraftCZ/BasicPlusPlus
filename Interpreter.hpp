#ifndef BASICPLUSPLUS_INTERPRETER_HPP
#define BASICPLUSPLUS_INTERPRETER_HPP

#include "ExpressionsStatements.hpp"
#include "Tokenization.hpp"
#include "Parser.hpp"

namespace Interpreting {
    class InterpreterError : public std::exception {};
    
    class Interpreter : public ExprStmt::AbstractExprVisitor, public ExprStmt::AbstractStmtVisitor {
    private:
//        std::unique_ptr<std::vector<ExprStmt::expr_ptr>> expressions;
        std::string errorMessage;
        ExprStmt::expr_ptr errorExpression;

        void throwError(std::string message, ExprStmt::Expr &expr);
        
        std::string getLiteralTypeName(Tokenization::Literal &literal);
        
        std::string stringify(Tokenization::Literal &literal);

    public:
        Tokenization::Literal visit(ExprStmt::UnaryExpr &expr) override;

        Tokenization::Literal visit(ExprStmt::BinaryExpr &expr) override;

        Tokenization::Literal visit(ExprStmt::GroupingExpr &expr) override;

        Tokenization::Literal visit(ExprStmt::LiteralExpr &expr) override;
        
        void visit(ExprStmt::PrintStmt &stmt) override;
        
        void interpret(ExprStmt::stmt_ptr &stmt);

        std::string &getErrorMessage();
    };
}

#endif //BASICPLUSPLUS_INTERPRETER_HPP
