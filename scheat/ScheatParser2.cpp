//
//  YAScheatParser.cpp
//  scheat
//
//  Created by かずちか on 2020/09/26.
//

#include "ScheatParser2.h"

/*
 Parsing
 
 Statement
    1. check the keyword
    this -> parseDeclareVarStatement
    print -> parsePrintStatement
    to -> parseDeclareFuncStatement
    if -> parseIfStatement
    unless ->
    for -> parseForStatement
    while -> parseWhileStatement
    
    id_expr -> parseIdentifierExpressionStatement
 */

using namespace std;
using namespace scheat;
using namespace basics;
using namespace nodes2;
using namespace parser2;
using scheat::statics::contextCenter;
using scheat::statics::global_context;
using scheat::statics::main_Context;
using scheat::statics::local_context;
using scheat::statics::objects;
using scheat::statics::fname;
using scheat::statics::mTokens;
using scheat::statics::scheato;

unique_ptr<StatementNode> parseStatement_single(Token *&);

static bool isValue(Token *tok){
    switch (tok->kind) {
        case scheat::TokenKind::val_num:
            return true;
            break;
        case scheat::TokenKind::val_operator:
            return false;
        case scheat::TokenKind::val_str:
            return true;
        case scheat::TokenKind::val_bool:
            return true;
        case scheat::TokenKind::val_double:
            return true;
        case scheat::TokenKind::val_identifier:
            return true;
        default:
            break;
    }
    return false;
}

static Operator *findOperator(Token *tok, TypeData type, OperatorPosition position, OperatorPresidence priority){
    auto cla = global_context->findClass(type.name);
    if (!cla) {
        return nullptr;
    }
    auto iter = cla->operators.find(tok->value.strValue);
    if (iter == cla->operators.end()) {
        return nullptr;
    }
    
    
    if (iter->second->precidence != priority) {
        return nullptr;
    }
    
    
    if (iter->second->position != position) {
        return nullptr;
    }
    
    
    return iter->second;
}

extern unique_ptr<IdentifierExprTemplate> parseIdentifierExpr(Token *&);

static unique_ptr<IdentifierTermTemplate> parseIdentifierTerm(Token *&tok){
    if (tok->kind == scheat::TokenKind::tok_this) {
        auto idexpr = parseIdentifierExpr(tok);
        if (!idexpr) {
            return nullptr;
        }
        auto ptr = TheIdentifierTerm::init(move(idexpr));
        return ptr;
    }
    return nullptr;
}

int hasProperty(TypeData type, string k){
    Class *cl = global_context->findClass(type.name);
    if (!cl) {
        scheato->FatalError(SourceLocation(), __FILE_NAME__, __LINE__, "-? type %s does not exist...", type.name.c_str());
        return 0;
    }
    auto itervar = cl->properties.find(k);
    if (itervar == cl->properties.end()) {
        scheato->FatalError(SourceLocation(), __FILE_NAME__, __LINE__, "-? property %s does not exist...", k.c_str());
        return 0;
    }else{
        return itervar->second;
    }
    return 0;
}

unique_ptr<IdentifierExprTemplate> parseIdentifierExpr(Token *&tok){
    
    // identifierExpr : the ID
    //                | this ID
    //                | ID . ID
    if (tok->kind == scheat::TokenKind::tok_the) {
        eatThis(tok);
        auto ptr = parseIdentifierExpr(tok);
        auto ret = TheIdentifierTerm::init(move(ptr));
        return ret;
    }
    
    if (tok->kind == scheat::TokenKind::tok_this) {
        // auto ptr = parseNewIdentifierExpr(tok);
        // if (!ptr) {
        //     return nullptr;
        // }
        // return NewIdentifierExpr::init(move(ptr), ptr->type);
        
    }
    
    if (tok->kind == scheat::TokenKind::val_identifier) {
        auto ptr = parseIdentifierTerm(tok);
        if (!ptr) {
            return nullptr;
        }
        if (tok->kind == scheat::TokenKind::tok_access) {
            unique_ptr<IdentifierExprTemplate> expr = nullptr;
            eatThis(tok);
            auto child = parseIdentifierTerm(tok);
            
            
            
            if (!child) {
                return nullptr;
            }
            expr = AccessIdentifierTerm::init(move(ptr), move(child), 0);
            while (true) {
                if (true) {
                    
                }
            }
            
        }else{
            return ptr;
        }
    }
    
    
    
    return nullptr;
}

static unique_ptr<Term> parseTermNodes(Token*& tok){
    if (tok->kind == scheat::TokenKind::val_num) {
        auto ptr = make_unique<IntTerm>(tok);
        return ptr;
    }
    if (tok->kind == scheat::TokenKind::val_str) {
        auto ptr = make_unique<StringTerm>(tok);
        return ptr;
    }
    if (tok->kind == scheat::TokenKind::val_bool) {
        auto ptr = make_unique<BoolTerm>(tok);
        return ptr;
    }
    if (tok->kind == scheat::TokenKind::val_double) {
        auto ptr = make_unique<FloatTerm>(tok);
        return ptr;
    }
    if (tok->kind == scheat::TokenKind::val_identifier) {
        auto ptr = parseIdentifierExpr(tok);
    }
    return nullptr;
}

extern unique_ptr<Term> scheat::parser2::parseTerm(Token *&tok){
    // term : identifierExpr
    //      | int
    //      | string
    //      | bool
    //      | float
    //      | PrifixOperatedTerm
    //      | ...
    
    // prefix
    //-------------------------------------------------------------------------
    if (tok->kind == TokenKind::val_operator) {
        auto saved = tok;
        eatThis(tok);
        auto term = parseTerm(tok);
        if (!term) {
            return nullptr;
        }
        
        auto op = findOperator(saved, term->type, prefix, termly);
        if (!op) {
            scheato->FatalError(saved->location, __FILE_NAME__, __LINE__,
                                "%s was not found in class %s",
                                saved->value.strValue.c_str(),
                                term->type.name.c_str());
            return nullptr;
        }
        
        auto operTerm = PrefixOperatorTerm::init(op, move(term));
        return operTerm;
    }
    
    unique_ptr<Term> ptr = nullptr;
    
    // immediates
    if (tok->kind == TokenKind::val_num) {
        ptr = make_unique<IntTerm>(tok);
        eatThis(tok);
    }
    
    else if (tok->kind == TokenKind::val_str) {
        ptr = make_unique<StringTerm>(tok);
        eatThis(tok);
    }
    
    else if (tok->kind == TokenKind::val_bool) {
        ptr = make_unique<BoolTerm>(tok);
        eatThis(tok);
    }
    
    else if (tok->kind == TokenKind::val_double) {
        ptr = make_unique<FloatTerm>(tok);
        eatThis(tok);
    }
    
    else if (tok->kind == TokenKind::val_identifier || tok->kind == TokenKind::tok_the) {
        ptr = parseIdentifierExpr(tok);
        eatThis(tok);
    }
    
    else{
        scheato->FatalError(tok->location, __FILE_NAME__, __LINE__, "Regular value was not found.");
        return nullptr;
    }
    
    // -------------------------------------------------------------------------
    
    // infix/postfix
    
    if (tok == nullptr) {
        return ptr;
    }
    
    if (tok->kind == TokenKind::val_operator) {
        
        if (isValue(tok->next)) {
            goto ParseInfix;
        }
        
        auto op = findOperator(tok, ptr->type, postfix, termly);
        if (!op) {
//            scheato->FatalError(tok->location, __FILE_NAME__, __LINE__,
//                                "%s was not found in class %s",
//                                tok->value.strValue.c_str(),
//                                ptr->type.name.c_str());
            return ptr;
        }
        eatThis(tok);
        auto operatedTerm = PostfixOperatorTerm::init(move(ptr), op);
        return operatedTerm;
    }
    
    ParseInfix:
    auto op = findOperator(tok, ptr->type, infix, termly);
    if (!op) {
        return ptr;
    }
    eatThis(tok);
    auto rhs = parsePrimary(tok);
    if (!rhs) {
        return nullptr;
    }
    return nullptr;
}

extern unique_ptr<PrimaryExpr> scheat::parser2::parsePrimary(Token *&tok){
    // primary : term
    //         | term op primary
    //         | op primary
    unique_ptr<Term> ptr = nullptr;
    if (tok->kind == TokenKind::val_operator) {
        auto saved = tok;
        eatThis(tok);
        auto term = parsePrimary(tok);
        auto op = findOperator(tok, term->type, prefix, primary);
        if (!op) {
            tok = saved;
            goto infix_postfix;
        }
        auto ptr = PrefixOperatorPrimaryExpr::init(op, move(term));
    }
    
    infix_postfix:
    ptr = parseTerm(tok);
    
    if (!ptr) {
        return nullptr;
    }
    
    if (!tok) {
        return ptr;
    }
    
    if (tok->kind == TokenKind::val_operator) {
        if (isValue(tok->next)) {
            // infix
            auto saved = tok;
            eatThis(tok);
            auto primaryptr = parsePrimary(tok);
            if (!primaryptr) {
                return nullptr;
            }
            auto op = findOperator(saved, ptr->type, infix, primary);
            if (!op) {
                tok = saved;
                return ptr;
            }
            
            auto ret = InfixOperatorPrimaryExpr::init(move(ptr), op, move(primaryptr));
            return ret;
            
        }else{
            // postfix
            auto op = findOperator(tok, ptr->type, postfix, primary);
            if (!op) {
                return ptr;
            }
            eatThis(tok);
            auto ret = PostfixOperatorPrimaryExpr::init(move(ptr), op);
            return ret;
        }
        
    }
    
    
    return ptr;
}

static unique_ptr<Expr> parseOperatedExpr(Token *&tok){
    // 1. op expr
    unique_ptr<PrimaryExpr> ptr = nullptr;
    
    if (tok->kind == TokenKind::val_operator) {
        auto saved = tok;
        eatThis(tok);
        auto rhs = parseExpr(tok);
        if (!rhs) {
            return nullptr;
        }
        auto op = findOperator(saved, rhs->type, prefix, secondary);
        if (!op) {
            // (!primary)
            tok = saved;
            goto infix_postfix;
        }
        
        auto ret = PrefixOperatorExpr::init(op, move(rhs));
        return ret;
    }
    
infix_postfix:
    auto prim = parsePrimary(tok);
    
    if (!prim) {
        return nullptr;
    }
    
    if (!tok) {
        return prim;
    }
    
    if (tok->kind == TokenKind::val_operator) {
        
        if (isValue(tok->next)) {
            auto saved = tok;
            eatThis(tok);
            auto expr = parseExpr(tok);
            if (!expr) {
                return nullptr;
            }
            auto op = findOperator(saved, expr->type, infix, secondary);
            if (!op) {
                scheato->FatalError(saved->location, __FILE_NAME__, __LINE__,
                                    "%s was not found in class %s",
                                    saved->value.strValue.c_str(),
                                    expr->type.name.c_str());
                return nullptr;
            }
            auto ret = InfixOperatorExpr::init(move(prim), op, move(expr));
            return ret;
        }else{
            auto saved = tok;
            auto op = findOperator(saved, prim->type, postfix, secondary);
            if (!op) {
                scheato->FatalError(saved->location, __FILE_NAME__, __LINE__,
                                    "%s was not found in class %s",
                                    saved->value.strValue.c_str(),
                                    prim->type.name.c_str());
                return nullptr;
            }
            auto ret = PostfixOperatorExpr::init(move(prim), op);
        }
        
    }else{
        return prim;
    }
    
    return nullptr;
}

extern unique_ptr<Expr> scheat::parser2::parseExpr(Token* &tok) {
    // expr : operatedExpr t_of id
    auto ptr = parseOperatedExpr(tok);
    return nullptr;
}

extern void parser2::parse(Scheat *sch,Token *tokens){
    scheato = sch;
    sch->statements = new DataHolder();
    auto stmts = make_unique<Statements>();
    stmts->statements = nullptr;
    stmts->statement = parseStatement(tokens);
    if (scheato->hasProbrem()) {
        return;
    }
    if (!stmts->statement) {
        return;
    }
    
    
    while (tokens == nullptr){
    
        auto s = parseStatement(tokens);
        if (!s && !scheato->hasProbrem()) {
            return;
        }
        
        if (scheato->hasProbrem()) {
            return;
        }
        
        stmts = make_unique<Statements>(move(stmts), move(s));
        
    }
    
    scheato->statements->statements = move(stmts);
    return;
};

extern unique_ptr<Statement> parser2::parseStatement(Token *&tokens){
    auto sts = make_unique<Statement>();
    sts -> statement = parseStatement_single(tokens);
    if (!sts->statement) {
        return nullptr;
    }
    unique_ptr<StatementNode> s = nullptr;
    while (s = parseStatement_single(tokens) , s != nullptr) {
        sts->statement = move(s);
        if (tokens->kind == TokenKind::tok_period) {
            sts->perTok = tokens;
            eatThis(tokens);
        }else if (tokens->kind == TokenKind::tok_comma){
            sts->perTok = tokens;
            eatThis(tokens);
        }else{
            return nullptr;
        }
        sts = make_unique<Statement>(move(sts));
    }
    return nullptr;
}

extern unique_ptr<StatementNode> parseStatement_single(Token *&tokens){
    // statement : this id is expr.
    if (tokens->kind == TokenKind::tok_this) {
        // return parseDeclareVariableStatement(tokens);
    }
    
    return nullptr;
}
