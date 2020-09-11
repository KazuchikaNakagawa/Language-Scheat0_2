//
//  ScheatNodes.h
//  ProjectScheat
//
//  Created by かずちか on 2020/08/26.
//

#ifndef ScheatNodes_h
#define ScheatNodes_h


#define unique(id) std::unique_ptr<id>
#include <string>
#include "ScheatContext.h"
#include "ScheatObjects.h"
namespace scheat {
namespace node{

// NodeData
// have a parsed data.
// basically, value is register
class NodeData {
    
public:
    std::string value;
    TypeData size;
    NodeData(std::string a, std::string b) : value(a), size(b) {};
    NodeData(std::string a, TypeData t) : value(a), size(t) {};
};

// base of node.
// location may be undefined by accident
class Node {
    
public:
    SourceLocation location;
    
    TypeData node_size;
    
    virtual node::NodeData* codegen(IRStream &) { return nullptr; };
    
    virtual ~Node() {};
};

// base of term
class TermNode : public Node {
    
    
public:
    node::NodeData * codegen(IRStream &) override{ return nullptr; };
    virtual ~TermNode(){};
};

class PrimaryExprNode : public Node {
    
public:
    node::NodeData * codegen(IRStream &) override { return nullptr; };
    virtual ~PrimaryExprNode(){};
    PrimaryExprNode() {};
};

class ExprNode : public Node {
    
public:
    node::NodeData * codegen(IRStream &) override { return nullptr; };
    virtual ~ExprNode(){};
    ExprNode() {};
};

class IdentifierTerm : public TermNode {
    std::string value;
    
public:
    __deprecated_msg("this class is only available for unique_ptr")
    IdentifierTerm(std::string v) : value(v) {};
    static unique(IdentifierTerm) create(std::string, bool);
    node::NodeData * codegen(IRStream &) override;
};

class IdentifierExpr : public ExprNode {
    unique(IdentifierExpr) expr;
    // operator or . token
    Token * opTok;
    unique(IdentifierTerm) term;
public:
    node::NodeData * codegen(IRStream &) override;
};

class IntTerm : public TermNode {
    Token *valToken;
    unique(IdentifierExpr) ident;
public:
    IntTerm() : TermNode() {};
    static unique(IntTerm) make(Token *);
    static unique(IntTerm) make(unique(IdentifierExpr));
    node::NodeData * codegen(IRStream &) override;
};

class Term : public Node {
    unique(Term) terms;
    Token *opTok;
    unique(TermNode) node;
public:
    node::NodeData * codegen(IRStream &) override;
    static unique(Term) create(unique(TermNode));
    static unique(Term) create(unique(Term), Token *, unique(TermNode));
};

class PrimaryExpr : public ExprNode {
    unique(PrimaryExpr) exprs;
    Token *opTok;
    unique(Term) term;
public:
    __deprecated PrimaryExpr() {};
    
    node::NodeData * codegen(IRStream &) override;
    static unique(PrimaryExpr) make(unique(Term));
    static unique(PrimaryExpr) make(unique(PrimaryExpr), Token *, unique(Term));
};

class Expr : public Node {
    unique(PrimaryExpr) body;
    unique(Expr) exprs;
    Token *op;
public:
    node::NodeData * codegen(IRStream &) override{ return nullptr; };
    static unique(Expr) make(unique(PrimaryExpr) lhs, Token *opTok = nullptr,
                             unique(Expr) rhs = nullptr);
};


class PrototypeExpr : public ExprNode {
    Token *identifier;
    TypeData *type;
public:
    __deprecated PrototypeExpr(Token *t, TypeData *ty) : identifier(t), type(ty), ExprNode() {};
    
};

class StatementNode : public Node {
    
public:
    virtual void dump(IRStream &) {};
    node::NodeData * codegen(IRStream &) override{ return nullptr; };
    virtual ~StatementNode() {};
    StatementNode() {};
};

class Statements : public Node {
public:
    unique(Statements) stmts;
    unique(StatementNode) stmt;
    node::NodeData * codegen(IRStream &) override;
    static unique(Statements) make(unique(StatementNode), unique(Statements));
};



class PrintStatement : public StatementNode {
    unique(Expr) ex;
public:
    ~PrintStatement(){};
    __deprecated PrintStatement() {};
    node::NodeData * codegen(IRStream &) override;
    void dump(IRStream &) override;
    static unique(PrintStatement) make(unique(Expr));
};


} // end of node namespace

}
#undef unique
#endif /* ScheatNodes_h */
