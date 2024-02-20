#ifndef BASICPLUSPLUS_EXPRESSIONSSTATEMENTS_HPP
#define BASICPLUSPLUS_EXPRESSIONSSTATEMENTS_HPP

#include <vector>
#include <memory>
#include "Tokenization.hpp"

namespace ExprStmt {
    // Visitor for Expr
    class Expr;
    class UnaryExpr;
    class BinaryExpr;
    class GroupingExpr;
    class LiteralExpr;
    class VarExpr;

    class AbstractExprVisitor {
    public:
        virtual ~AbstractExprVisitor() noexcept = default;

        virtual Tokenization::Literal visit(UnaryExpr &expr) = 0;
        virtual Tokenization::Literal visit(BinaryExpr &expr) = 0;
        virtual Tokenization::Literal visit(GroupingExpr &expr) = 0;
        virtual Tokenization::Literal visit(LiteralExpr &expr) = 0;
        virtual Tokenization::Literal visit(VarExpr &expr) = 0;
    };

    // Visitor for Stmt
    class Stmt;
    class PrintStmt;
    class InputStmt;
    class LetStmt;
    class ToNumStmt;
    class ToStrStmt;

    class AbstractStmtVisitor {
    public:
        virtual ~AbstractStmtVisitor() noexcept = default;

        virtual void visit(PrintStmt &stmt) = 0;
        virtual void visit(InputStmt &stmt) = 0;
        virtual void visit(LetStmt &stmt) = 0;
        virtual void visit(ToNumStmt &stmt) = 0;
        virtual void visit(ToStrStmt &stmt) = 0;
    };

    using stmt_ptr = std::unique_ptr<Stmt>;

    // Implementations
    class Expr {
    public:
        virtual ~Expr() noexcept = default;

        virtual Tokenization::Literal accept(AbstractExprVisitor &) = 0;
    };

    using expr_ptr = std::unique_ptr<Expr>;


    class Stmt {
    public:
        virtual ~Stmt() noexcept = default;

        virtual void accept(AbstractStmtVisitor &) = 0;
    };

    using stmt_ptr = std::unique_ptr<Stmt>;

    // Definitions of all the different expression types
    class UnaryExpr : public Expr {
    public:
        const Tokenization::Token op;
        const expr_ptr right;

        UnaryExpr(Tokenization::Token &&op, expr_ptr &&right) : op(op), right(std::move(right)) {}

        virtual Tokenization::Literal accept(AbstractExprVisitor &v) override { return v.visit(*this); }
    };

    class BinaryExpr : public Expr {
    public:
        const expr_ptr left;
        const Tokenization::Token op;
        const expr_ptr right;

        BinaryExpr(expr_ptr &&left, Tokenization::Token &&op, expr_ptr &&right) : left(std::move(left)), op(op),
                                                                                  right(std::move(right)) {}

        virtual Tokenization::Literal accept(AbstractExprVisitor &v) override { return v.visit(*this); }
    };

    class GroupingExpr : public Expr {
    public:
        const expr_ptr expression;

        GroupingExpr(expr_ptr &&expression) : expression(std::move(expression)) {}

        virtual Tokenization::Literal accept(AbstractExprVisitor &v) override { return v.visit(*this); }
    };

    class LiteralExpr : public Expr {
    public:
        const Tokenization::Literal value;

        LiteralExpr(Tokenization::Literal &&value) : value(value) {}

        virtual Tokenization::Literal accept(AbstractExprVisitor &v) override { return v.visit(*this); }
    };

    class VarExpr : public Expr {
    public:
        const std::string varName;

        VarExpr(std::string &&varName) : varName(varName) {}

        virtual Tokenization::Literal accept(AbstractExprVisitor &v) override { return v.visit(*this); }
    };

    // Definitions of all the different statement types
    class PrintStmt : public Stmt {
    public:
        const expr_ptr expr;

        PrintStmt(expr_ptr &&expr) : expr(std::move(expr)) {}

        virtual void accept(AbstractStmtVisitor &v) override { return v.visit(*this); }
    };
    
    class InputStmt : public Stmt {
    public:
        const expr_ptr expr;
        const std::string targetVarName;

        InputStmt(expr_ptr &&expr, std::string &&targetVar) : expr(std::move(expr)),
                                                              targetVarName(std::move(targetVar)) {}

        virtual void accept(AbstractStmtVisitor &v) override { return v.visit(*this); }
    };

    class LetStmt : public Stmt {
    public:
        const expr_ptr expr;
        const std::string targetVarName;

        LetStmt(expr_ptr &&expr, std::string &&targetVar) : expr(std::move(expr)),
                                                            targetVarName(std::move(targetVar)) {}

        virtual void accept(AbstractStmtVisitor &v) override { return v.visit(*this); }
    };

    class ToNumStmt : public Stmt {
    public:
        const std::string srcVar;
        const std::optional<std::string> dstVar;

        ToNumStmt(std::string &&srcVar, std::optional<std::string> &&dstVar = std::nullopt) : srcVar(std::move(srcVar)),
                                                                                              dstVar(std::move(dstVar)) {}

        virtual void accept(AbstractStmtVisitor &v) override { return v.visit(*this); }
    };


    class ToStrStmt : public Stmt {
    public:
        const std::string srcVar;
        const std::optional<std::string> dstVar;

        ToStrStmt(std::string &&srcVar, std::optional<std::string> &&dstVar = std::nullopt) : srcVar(std::move(srcVar)),
                                                                                              dstVar(std::move(dstVar)) {}

        virtual void accept(AbstractStmtVisitor &v) override { return v.visit(*this); }
    };

}

#endif //BASICPLUSPLUS_EXPRESSIONSSTATEMENTS_HPP
