//
//  ScheatContext.cpp
//  scheat
//
//  Created by かずちか on 2020/09/13.
//

#include <stdio.h>
#include "ScheatContext.h"
#include "ScheatStatics.h"
//#include "ScheatNodes.h"
#include <string>
#include <iostream>
#include <fstream>
#include <map>

using namespace scheat;
//using namespace scheat::node;

Class *Context::findClass(std::string key){
    return classes[key];
}

string Context::getRegister(){
    std::string v = "%r" + std::to_string(rnum);
    rnum++;
    return v;
}

Context *Context::create(std::string name, Context *parents){
    Context *cnt = new Context(name, parents);
    if (parents == nullptr) {
        ScheatContext::contextCenter.push_back(cnt);
    }
    return cnt;
}

Variable *Context::findVariable(std::string key){
    Variable *v = variables[key];
    if (v != nullptr) {
        return v;
    }else{
        if (base == nullptr) {
            return nullptr;
        }
        v = base->findVariable(key);
        return v;
    }
}

void ScheatContext::exportTo(ofstream &f){
    for (auto con : contextCenter) {
        if (con->name != "global") f << "; " << con->name << endl;
        con->stream_entry.exportTo(f);
        con->stream_body.exportTo(f);
        con->stream_tail.exportTo(f);
        f << endl;
    }
}

Function::Function(std::string type, std::string nm, bool demangle) : return_type(type){
    mangledName = ScheatContext::local()->name + "_" + nm;
    if (!demangle) {
        mangledName = nm;
    }
    name = nm;
    argTypes = {};
    context = Context::create(nm, ScheatContext::local());
}

void Context::addFunction(std::string key, Function *value){
    funcs[key] = value;
}

Function *Context::findFunc(std::string key){
    return funcs[key];
}

void Context::addClass(std::string key, Class *value){
    classes[key] = value;
}


void Context::addVariable(std::string key, Variable *value){
    variables[key] = value;
}

bool Context::isExists(std::string key){
    if (variables[key] != nullptr) {
        return true;
    }
    if (funcs[key] != nullptr) {
        return true;
    }
    return false;
}

void Context::dump(std::ofstream &f){
    
    f << "; " << name << "\n";
    stream_entry.exportTo(f);
    stream_body.exportTo(f);
    stream_tail.exportTo(f);
    
    auto iter_f = begin(funcs);
    while (iter_f != funcs.end()) {
        auto pair = *iter_f;
        pair.second->context->dump(f);
    }
    
    for (auto p = funcs.begin(); p != funcs.end(); p = std::next(p)) {
        (*p).second->context->dump(f);
    }
    
}
