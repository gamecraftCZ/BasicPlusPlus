#include <iomanip>
#include <sstream>
#include <math.h>
#include <iostream>
#include "Interpreter.hpp"

namespace Interpreting {
    Tokenization::Literal Interpreter::visit(ExprStmt::LiteralExpr &expr) {
        return expr.value;
    }

    Tokenization::Literal Interpreter::visit(ExprStmt::GroupingExpr &expr) {
        return expr.expression->accept(*this);
    }

    Tokenization::Literal Interpreter::visit(ExprStmt::UnaryExpr &expr) {
        Tokenization::Literal right = expr.right->accept(*this);
        switch (expr.op.type) {
            case Tokenization::MINUS:
                if (std::holds_alternative<double>(right)) return right;
                throwError("Unary '-' is not allowed on '" + getLiteralTypeName(right) + "' type.", expr);
            case Tokenization::NOT:
                if (std::holds_alternative<bool>(right)) return right;
                throwError("Unary 'NOT' is not allowed on '" + getLiteralTypeName(right) + "' type.", expr);
        }
        
        // Unreachable
        throw std::runtime_error("UNREACHABLE!");
    }

    Tokenization::Literal Interpreter::visit(ExprStmt::BinaryExpr &expr) {
        Tokenization::Literal left = expr.left->accept(*this);
        Tokenization::Literal right = expr.right->accept(*this);

        switch (expr.op.type) {
            case Tokenization::PLUS:
                if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)){
                    return std::get<double>(left) + std::get<double>(right);
                }
                if (std::holds_alternative<std::string>(left)){
                    return std::get<std::string>(left) + stringify(right);
                }
                if (std::holds_alternative<std::string>(right)){
                    return stringify(right) + std::get<std::string>(right);
                }
                throwError("Binary '+' is not allowed on '" + getLiteralTypeName(left) + "' + '" + getLiteralTypeName(right) + "' types.", expr);
                
            case Tokenization::MINUS:
                if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)){
                    return std::get<double>(left) - std::get<double>(right);
                }
                throwError("Binary '-' is not allowed on '" + getLiteralTypeName(left) + "' - '" + getLiteralTypeName(right) + "' types.", expr);
                
            case Tokenization::STAR:
                if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)){
                    return std::get<double>(left) * std::get<double>(right);
                }
                throwError("Binary '*' is not allowed on '" + getLiteralTypeName(left) + "' * '" + getLiteralTypeName(right) + "' types.", expr);
                
            case Tokenization::SLASH:
                if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)){
                    if (std::get<double>(right) == 0) throwError("DivisionByZero", expr);
                    return std::get<double>(left) - std::get<double>(right);
                }
                throwError("Binary '/' is not allowed on '" + getLiteralTypeName(left) + "' / '" + getLiteralTypeName(right) + "' types.", expr);
                
            // TODO More Binary operations
        }
        
        
        // Unreachable
        throw std::runtime_error("UNREACHABLE!");
    }

    void Interpreter::throwError(std::string message, ExprStmt::Expr &expr) {
        errorMessage = std::move(message);
//        errorExpression = expr;  // TODO
        throw InterpreterError();
    }

    // helper type
    template<class... Ts>
    struct overloaded : Ts... { using Ts::operator()...; };
    
    std::string Interpreter::getLiteralTypeName(Tokenization::Literal &literal) {
        return std::visit(overloaded {
                [](std::string &arg) { return "string"; },
                [](double &arg) { return "number"; },
                [](bool &arg) { return "boolean"; }
        }, literal);
    }

    std::string Interpreter::stringify(Tokenization::Literal &literal) {
        return std::visit(overloaded {
                [](std::string &arg) { return arg; },
                [](double &arg) {
                    if (std::fmod(arg, 1) == 0) {
                        // Whole number is printed without decimal places
                        return std::format("{:.0f}", arg);
                    } else {
                        return std::format("{:.2f}", arg);
                    }
                },
                [](bool &arg) { return std::string(arg ? "TRUE" : "FALSE"); }
        }, literal);
    }

    void Interpreter::interpret(ExprStmt::stmt_ptr &stmt) {
        stmt->accept(*this);
    }

    void Interpreter::visit(ExprStmt::PrintStmt &stmt) {
        Tokenization::Literal value = stmt.expr->accept(*this);
        std::cout << stringify(value) << std::endl; 
    }

    std::string &Interpreter::getErrorMessage() {
        return errorMessage;
    }
}
