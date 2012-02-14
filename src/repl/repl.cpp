/*
 * Copyright (c) 2011-, Tetsuo Fujii
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    - Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    - Neither the name of the author nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <deque>
#include <iostream>
#include <stdexcept>

#include <boost/foreach.hpp>

#ifdef DEBUG
#include <llvm/Analysis/Verifier.h>
#endif
#include <llvm/CodeGen/GCs.h>

#include "codegen/builtin.hpp"
#include "codegen/llvm.hpp"
#include "env/resolver.hpp"
#include "reader/ast/node.hpp"
#include "reader/parser.hpp"
#include "reader/lexer.hpp"
#include "runtime/intrinsics.hpp"

namespace
{
using namespace llvm;
using namespace isana;

bool is_closed(const std::string &code)
{
    std::deque<bool> paren_stack;
    for (std::string::const_iterator it = code.begin();
            it != code.end(); ++it) {
        if ((*it) == '(') {
            paren_stack.push_back(true);
            continue;
        } else if ((*it) == ')') {
            if (paren_stack.empty())
                throw std::runtime_error("Invalid statement");
            paren_stack.pop_back();
            continue;
        }
    }
    if (paren_stack.empty()) return true;
    return false;
}

Function *eval_func(reader::node_list &ns, codegen::module_ref &m)
{
    if (Function *exists = m->getFunction("repl"))
        exists->removeFromParent();

    codegen::type_list args;
    FunctionType *ft = codegen::get_ftype(codegen::void_type(), args);
    Function *fn = codegen::add_external_function("repl", ft, m),
             *unit = codegen::add_external_function("repl.unit", ft, m);

    BasicBlock *unit_entry = codegen::add_block("entry", unit);
    IRBuilder<> irb(unit_entry);
    BOOST_FOREACH(reader::node_ref n, ns) {
        irb.CreateCall(env::lookup("display"), n->codegen(unit_entry));
        irb.CreateCall(env::lookup("newline"));
    }
    irb.CreateRetVoid();

    BasicBlock *bb = codegen::add_block("entry", fn);
    irb.SetInsertPoint(bb);
    irb.CreateCall(unit);
    irb.CreateRetVoid();

#ifdef DEBUG
    verifyFunction(*unit);
    verifyFunction(*fn);
#endif

    return fn;
}
}

int main(void)
{
    using namespace llvm;
    using namespace isana;

    codegen::init_target();

    codegen::module_ref m = codegen::defmodule::create("isana.core");

    linkShadowStackGC();
    runtime::link_runtime();
    codegen::builtin::init(m);

#ifdef DEBUG
    verifyModule(*m.get());
#endif

    ExecutionEngine *ee = codegen::defmodule::execute(m);

    std::cout << "isana> ";
    std::string code;
    while (std::getline(std::cin, code)) {
        if (code.empty()) {
            std::cout << "isana> ";
            continue;
        }
        while (!is_closed(code)) {
            std::string next;
            std::getline(std::cin, next);
            code += " " + next;
        }
#ifdef DEBUG
        if (code == ":dump") {
            std::cout << std::endl << "--- dump ---" << std::endl;
            m->dump();
            std::cout << "---" << std::endl << std::endl;
            std::cout << "isana> ";
            continue;
        }
#endif
        try {
            reader::node_list ns = reader::parse(reader::tokenize(code));
            Function *eval = eval_func(ns, m);
            ee->recompileAndRelinkFunction(eval);
            std::vector<GenericValue> gvs;
            ee->runFunction(eval, gvs);
        } catch (const std::runtime_error &e) {
            std::cerr << e.what() << std::endl;
        }
        std::cout << "isana> ";
    }
}
