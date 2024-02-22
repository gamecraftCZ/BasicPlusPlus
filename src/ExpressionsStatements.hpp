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
    class RndStmt;
    class BlockStmt;
    class IfStmt;
    class WhileStmt;
    class ContinueStmt;
    class BreakStmt;

    class AbstractStmtVisitor {
    public:
        virtual ~AbstractStmtVisitor() noexcept = default;

        virtual void visit(PrintStmt &stmt) = 0;
        virtual void visit(InputStmt &stmt) = 0;
        virtual void visit(LetStmt &stmt) = 0;
        virtual void visit(ToNumStmt &stmt) = 0;
        virtual void visit(ToStrStmt &stmt) = 0;
        virtual void visit(RndStmt &stmt) = 0;
        virtual void visit(BlockStmt &stmt) = 0;
        virtual void visit(IfStmt &stmt) = 0;
        virtual void visit(WhileStmt &stmt) = 0;
        virtual void visit(ContinueStmt &stmt) = 0;
        virtual void visit(BreakStmt &stmt) = 0;
    };

    using stmt_ptr = std::unique_ptr<Stmt>;

    // Implementations
    class Expr {
    protected:
        explicit Expr(uint32_t line) : line(line) {}
    public:
        uint32_t line;
        virtual ~Expr() noexcept = default;

        virtual Tokenization::Literal accept(AbstractExprVisitor &) = 0;
    };

    using expr_ptr = std::unique_ptr<Expr>;


    class Stmt {
    protected:
        explicit Stmt(uint32_t line) : line(line) {}
    public:
        uint32_t line;
        virtual ~Stmt() noexcept = default;

        virtual void accept(AbstractStmtVisitor &) = 0;
    };

    using stmt_ptr = std::unique_ptr<Stmt>;

    // Definitions of all the different expression types
    class UnaryExpr : public Expr {
    public:
        const Tokenization::Token op;
        const expr_ptr right;

        UnaryExpr(Tokenization::Token &&op, expr_ptr &&right, uint32_t line) : op(op), right(std::move(right)), Expr(line) {}

        virtual Tokenization::Literal accept(AbstractExprVisitor &v) override { return v.visit(*this); }
    };

    class BinaryExpr : public Expr {
    public:
        const expr_ptr left;
        const Tokenization::Token op;
        const expr_ptr right;

        BinaryExpr(expr_ptr &&left, Tokenization::Token &&op, expr_ptr &&right, uint32_t line) : left(std::move(left)), op(op),
                                                                                  right(std::move(right)), Expr(line) {}

        virtual Tokenization::Literal accept(AbstractExprVisitor &v) override { return v.visit(*this); }
    };

    class GroupingExpr : public Expr {
    public:
        const expr_ptr expression;

        GroupingExpr(expr_ptr &&expression, uint32_t line) : expression(std::move(expression)), Expr(line) {}

        virtual Tokenization::Literal accept(AbstractExprVisitor &v) override { return v.visit(*this); }
    };

    class LiteralExpr : public Expr {
    public:
        const Tokenization::Literal value;

        LiteralExpr(Tokenization::Literal &&value, uint32_t line) : value(value), Expr(line) {}

        virtual Tokenization::Literal accept(AbstractExprVisitor &v) override { return v.visit(*this); }
    };

    class VarExpr : public Expr {
    public:
        const std::string varName;

        VarExpr(std::string &&varName, uint32_t line) : varName(varName), Expr(line) {}

        virtual Tokenization::Literal accept(AbstractExprVisitor &v) override { return v.visit(*this); }
    };

    // Definitions of all the different statement types
    class PrintStmt : public Stmt {
    public:
        const expr_ptr expr;

        PrintStmt(expr_ptr &&expr, uint32_t line) : expr(std::move(expr)), Stmt(line) {}

        virtual void accept(AbstractStmtVisitor &v) override { return v.visit(*this); }
    };
    
    class InputStmt : public Stmt {
    public:
        const expr_ptr expr;
        const std::string targetVarName;

        InputStmt(expr_ptr &&expr, std::string &&targetVar, uint32_t line) : expr(std::move(expr)),
                                                              targetVarName(std::move(targetVar)), Stmt(line) {}

        virtual void accept(AbstractStmtVisitor &v) override { return v.visit(*this); }
    };

    class LetStmt : public Stmt {
    public:
        const expr_ptr expr;
        const std::string targetVarName;

        LetStmt(expr_ptr &&expr, std::string &&targetVar, uint32_t line) : expr(std::move(expr)),
                                                            targetVarName(std::move(targetVar)), Stmt(line) {}

        virtual void accept(AbstractStmtVisitor &v) override { return v.visit(*this); }
    };

    class ToNumStmt : public Stmt {
    public:
        const std::string srcVar;
        const std::optional<std::string> dstVar;

        ToNumStmt(std::string &&srcVar, std::optional<std::string> &&dstVar, uint32_t line) : srcVar(std::move(srcVar)),
                                                                                              dstVar(std::move(dstVar)), Stmt(line) {}

        virtual void accept(AbstractStmtVisitor &v) override { return v.visit(*this); }
    };
    
    class ToStrStmt : public Stmt {
    public:
        const std::string srcVar;
        const std::optional<std::string> dstVar;

        ToStrStmt(std::string &&srcVar, std::optional<std::string> &&dstVar, uint32_t line) : srcVar(std::move(srcVar)),
                                                                                              dstVar(std::move(dstVar)), Stmt(line) {}

        virtual void accept(AbstractStmtVisitor &v) override { return v.visit(*this); }
    };
    
    class RndStmt : public Stmt {
    public:
        const std::string dstVar;
        const expr_ptr lowerBound;
        const expr_ptr upperBound;

        RndStmt(std::string &&dstVar, expr_ptr &&lowerBound, expr_ptr &&upperBound, uint32_t line) : dstVar(std::move(dstVar)),
                                                                                      lowerBound(std::move(lowerBound)),
                                                                                      upperBound(std::move(upperBound)), Stmt(line) {}

        virtual void accept(AbstractStmtVisitor &v) override { return v.visit(*this); }
    };
    
    class BlockStmt : public Stmt {
    public:
        const std::vector<stmt_ptr> statementsList;

        BlockStmt(std::vector<stmt_ptr> &&statementsList, uint32_t line): statementsList(std::move(statementsList)), Stmt(line) {}

        virtual void accept(AbstractStmtVisitor &v) override { return v.visit(*this); }
    };

    class IfStmt : public Stmt {
    public:
        const expr_ptr conditionExpr;
        const stmt_ptr thenBranch;
        const std::optional<stmt_ptr> elseBranch;

        IfStmt(expr_ptr &&conditionExpr, stmt_ptr &&thenBranch, std::optional<stmt_ptr> &&elseBranch, uint32_t line) : 
            conditionExpr(std::move(conditionExpr)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)), Stmt(line) {}

        virtual void accept(AbstractStmtVisitor &v) override { return v.visit(*this); }
    };

    class WhileStmt : public Stmt {
    public:
        const expr_ptr conditionExpr;
        const stmt_ptr thenBranch;

        WhileStmt(expr_ptr &&conditionExpr, stmt_ptr &&thenBranch, uint32_t line) : 
            conditionExpr(std::move(conditionExpr)), thenBranch(std::move(thenBranch)), Stmt(line) {}

        virtual void accept(AbstractStmtVisitor &v) override { return v.visit(*this); }
    };

    class BreakStmt : public Stmt {
    public:
        BreakStmt(uint32_t line) : Stmt(line) {}

        virtual void accept(AbstractStmtVisitor &v) override { return v.visit(*this); }
    };
    
    class ContinueStmt : public Stmt {
    public:
        ContinueStmt(uint32_t line) : Stmt(line) {}

        virtual void accept(AbstractStmtVisitor &v) override { return v.visit(*this); }
    };

}

#endif //BASICPLUSPLUS_EXPRESSIONSSTATEMENTS_HPP
