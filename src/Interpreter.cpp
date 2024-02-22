#include <sstream>
#include <cmath>
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
                if (std::holds_alternative<double>(right)) return -std::get<double>(right);
                throwError("Unary '-' is not allowed on '" + getLiteralTypeName(right) + "' type.", expr);
            case Tokenization::NOT:
                if (std::holds_alternative<bool>(right)) return !std::get<bool>(right);
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
                
            case Tokenization::LESS:
                if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)){
                    return std::get<double>(left) < std::get<double>(right);
                }
                throwError("Binary '<' is not allowed on '" + getLiteralTypeName(left) + "' < '" + getLiteralTypeName(right) + "' types.", expr);
                
            case Tokenization::GREATER:
                if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)){
                    return std::get<double>(left) > std::get<double>(right);
                }
                throwError("Binary '>' is not allowed on '" + getLiteralTypeName(left) + "' > '" + getLiteralTypeName(right) + "' types.", expr);
                
            case Tokenization::LESS_EQUAL:
                if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)){
                    return std::get<double>(left) <= std::get<double>(right);
                }
                throwError("Binary '<=' is not allowed on '" + getLiteralTypeName(left) + "' <= '" + getLiteralTypeName(right) + "' types.", expr);
                
            case Tokenization::GREATER_EQUAL:
                if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)){
                    return std::get<double>(left) >= std::get<double>(right);
                }
                throwError("Binary '>=' is not allowed on '" + getLiteralTypeName(left) + "' >= '" + getLiteralTypeName(right) + "' types.", expr);
                
            case Tokenization::EQUAL_EQUAL:
                if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)){
                    return std::get<double>(left) == std::get<double>(right);
                }
                if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)){
                    return std::get<std::string>(left) == std::get<std::string>(right);
                }
                throwError("Binary '==' is not allowed on '" + getLiteralTypeName(left) + "' == '" + getLiteralTypeName(right) + "' types.", expr);
                
            case Tokenization::NOT_EQUAL:
                if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)){
                    return std::get<double>(left) != std::get<double>(right);
                }
                if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)){
                    return std::get<std::string>(left) != std::get<std::string>(right);
                }
                throwError("Binary '<>' is not allowed on '" + getLiteralTypeName(left) + "' <> '" + getLiteralTypeName(right) + "' types.", expr);
                
            case Tokenization::AND:
                if (std::holds_alternative<bool>(left) && std::holds_alternative<bool>(right)){
                    return std::get<bool>(left) && std::get<bool>(right);
                }
                throwError("Binary 'AND' is not allowed on '" + getLiteralTypeName(left) + "' AND '" + getLiteralTypeName(right) + "' types.", expr);
                
            case Tokenization::OR:
                if (std::holds_alternative<bool>(left) && std::holds_alternative<bool>(right)){
                    return std::get<bool>(left) || std::get<bool>(right);
                }
                throwError("Binary 'OR' is not allowed on '" + getLiteralTypeName(left) + "' OR '" + getLiteralTypeName(right) + "' types.", expr);
        }
        
        // Unreachable
        throw std::runtime_error("UNREACHABLE!");
    }

    Tokenization::Literal Interpreter::visit(ExprStmt::VarExpr &expr) {
        return getVarValue(expr.varName, expr);
    }

    void Interpreter::throwError(std::string message, ExprStmt::Expr &expr) {
        errorMessage = std::move(message);
        errorLine = expr.line;
        throw InterpreterError();
    }

    void Interpreter::throwError(std::string message, ExprStmt::Stmt &stmt) {
        errorMessage = std::move(message);
        errorLine = stmt.line;
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

    Tokenization::Literal Interpreter::getVarValue(const std::string &varName, ExprStmt::Expr &expr) {
        auto value = globalVariables.find(varName);
        if (value == globalVariables.end()) throwError("VariableNotDeclared '" + varName + "'", expr);
        return value->second;
    }
    
    Tokenization::Literal Interpreter::getVarValue(const std::string &varName, ExprStmt::Stmt &stmt) {
        auto value = globalVariables.find(varName);
        if (value == globalVariables.end()) throwError("VariableNotDeclared '" + varName + "'", stmt);
        return value->second;
    }
    
    std::string &Interpreter::getErrorMessage() {
        return errorMessage;
    }

    uint32_t Interpreter::getErrorLine() {
        return errorLine;
    }
    
    void Interpreter::interpret(ExprStmt::stmt_ptr &stmt) {
        stmt->accept(*this);
    }

    void Interpreter::visit(ExprStmt::PrintStmt &stmt) {
        Tokenization::Literal value = stmt.expr->accept(*this);
        std::cout << stringify(value) << std::endl; 
    }
    
    void Interpreter::visit(ExprStmt::InputStmt &stmt) {
        Tokenization::Literal value = stmt.expr->accept(*this);
        std::cout << stringify(value);
        std::string outValue;
        std::getline(std::cin, outValue);
        globalVariables[stmt.targetVarName] = outValue;
    }
    
    void Interpreter::visit(ExprStmt::LetStmt &stmt) {
        Tokenization::Literal value = stmt.expr->accept(*this);
        globalVariables[stmt.targetVarName] = value;
    }
    
    void Interpreter::visit(ExprStmt::ToNumStmt &stmt) {
        Tokenization::Literal value = getVarValue(stmt.srcVar, stmt);
        Tokenization::Literal newValue;
        if (std::holds_alternative<double>(value)) newValue = value;
        if (std::holds_alternative<bool>(value)) newValue = std::get<bool>(value) ? 1. : 0.;
        if (std::holds_alternative<std::string>(value)) {
            // Parse string
            try {
                newValue = std::stod(std::get<std::string>(value));
            } catch (const std::invalid_argument&) {
                throwError("InvalidNumberFormat", stmt);
            } catch (const std::out_of_range&) {
                throwError("InvalidNumberFormat", stmt);
            }
        }
        globalVariables[stmt.dstVar.has_value() ? stmt.dstVar.value() : stmt.srcVar] = newValue;
    }
    
    void Interpreter::visit(ExprStmt::ToStrStmt &stmt) {
        Tokenization::Literal value = getVarValue(stmt.srcVar, stmt);
        Tokenization::Literal newValue = stringify(value);
        globalVariables[stmt.dstVar.has_value() ? stmt.dstVar.value() : stmt.srcVar] = newValue;
    }
    
    void Interpreter::visit(ExprStmt::RndStmt &stmt) {
        Tokenization::Literal lowerBound = stmt.lowerBound->accept(*this);
        Tokenization::Literal upperBound = stmt.upperBound->accept(*this);

        if (std::holds_alternative<double>(lowerBound) && std::holds_alternative<double>(upperBound)) {
            int lowerBoundInt = ceil(std::get<double>(lowerBound));
            int upperBoundInt = floor(std::get<double>(upperBound));
            int range = upperBoundInt - lowerBoundInt;
            double rndValue = (std::rand() % range) + lowerBoundInt;
            globalVariables[stmt.dstVar] = rndValue;
        } else {
            throwError("'RND' is not allowed on '" + getLiteralTypeName(lowerBound) + "', '" + getLiteralTypeName(lowerBound) + "' types.", stmt);
        }
    }

    void Interpreter::visit(ExprStmt::BlockStmt &stmt) {
        for (auto & statement : stmt.statementsList) {
            statement->accept(*this);
        }
    }

    void Interpreter::visit(ExprStmt::IfStmt &stmt) {
        Tokenization::Literal cond = stmt.conditionExpr->accept(*this);
        if (std::holds_alternative<bool>(cond)) {
            if (std::get<bool>(cond)) {
                stmt.thenBranch->accept(*this);
            } else {
                if (stmt.elseBranch.has_value()) {
                    stmt.elseBranch.value()->accept(*this);
                }
            }
        } else {
            throwError("ConditionNotBoolean", stmt);
        }
    }
    
    void Interpreter::visit(ExprStmt::WhileStmt &stmt) {
        Tokenization::Literal cond = stmt.conditionExpr->accept(*this);
        while (std::holds_alternative<bool>(cond) && std::get<bool>(cond)) {
            try {
                stmt.thenBranch->accept(*this);
            } catch (const Break&) {
                // TODO break
                break;
            } catch (const Continue&) {
                // continue
            }
            cond = stmt.conditionExpr->accept(*this);
        }
        if (!std::holds_alternative<bool>(cond)) {
            throwError("ConditionNotBoolean", stmt);
        }
    }

    void Interpreter::visit(ExprStmt::BreakStmt &stmt) {
        throw Break();
    }
    
    void Interpreter::visit(ExprStmt::ContinueStmt &stmt) {
        throw Continue();
    }

}
