#ifndef BASICPLUSPLUS_INTERPRETER_HPP
#define BASICPLUSPLUS_INTERPRETER_HPP

#include "ExpressionsStatements.hpp"
#include "Tokenization.hpp"
#include "Parser.hpp"

namespace Interpreting {
    class InterpreterError : public std::exception {};
    class Break : public std::exception {};
    class Continue : public std::exception {};
    
    class Interpreter : public ExprStmt::AbstractExprVisitor, public ExprStmt::AbstractStmtVisitor {
    private:
        std::map<std::string, Tokenization::Literal> globalVariables;
        
//        std::unique_ptr<std::vector<ExprStmt::expr_ptr>> expressions;
        std::string errorMessage;
        uint32_t errorLine;

        void throwError(std::string message, ExprStmt::Expr &expr);
        
        void throwError(std::string message, ExprStmt::Stmt &stmt);
        
        std::string getLiteralTypeName(Tokenization::Literal &literal);
        
        std::string stringify(Tokenization::Literal &literal);
        
        Tokenization::Literal getVarValue(const std::string &varName, ExprStmt::Expr &expr);

        Tokenization::Literal getVarValue(const std::string &varName, ExprStmt::Stmt &stmt);

    public:
        Tokenization::Literal visit(ExprStmt::UnaryExpr &expr) override;

        Tokenization::Literal visit(ExprStmt::BinaryExpr &expr) override;

        Tokenization::Literal visit(ExprStmt::GroupingExpr &expr) override;

        Tokenization::Literal visit(ExprStmt::LiteralExpr &expr) override;
        
        Tokenization::Literal visit(ExprStmt::VarExpr &expr) override;
        
        void visit(ExprStmt::PrintStmt &stmt) override;
        
        void visit(ExprStmt::InputStmt &stmt) override;
        
        void visit(ExprStmt::LetStmt &stmt) override;
        
        void visit(ExprStmt::ToNumStmt &stmt) override;
        
        void visit(ExprStmt::ToStrStmt &stmt) override;
        
        void visit(ExprStmt::RndStmt &stmt) override;

        void visit(ExprStmt::BlockStmt &stmt) override;

        void visit(ExprStmt::IfStmt &stmt) override;

        void visit(ExprStmt::WhileStmt &stmt) override;

        void visit(ExprStmt::BreakStmt &stmt) override;

        void visit(ExprStmt::ContinueStmt &stmt) override;

        void interpret(ExprStmt::stmt_ptr &stmt);

        std::string &getErrorMessage();
        
        uint32_t getErrorLine();
    };
}

#endif //BASICPLUSPLUS_INTERPRETER_HPP
