/*
 * Copyright (c) 2012-, Tetsuo Fujii
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

#include <iostream>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#ifdef DEBUG
#include <llvm/Analysis/Verifier.h>
#endif

#include "codegen/llvm.hpp"
#include "codegen/builtin/utils.hpp"
#include "env/dict.hpp"
#include "env/resolver.hpp"
#include "reader/ast/atom.hpp"
#include "reader/ast/list.hpp"
#include "runtime/object.hpp"

namespace
{
using namespace llvm;
using namespace isana;
using namespace isana::codegen;
using namespace isana::reader;

env::resolver_ref r = env::resolver::get_instance();

Value *closure_codegen(module_ref &m,
                       BasicBlock *bb,
                       node_list args,
                       const std::string name = "")
{
    Type *sexpr = defstruct::get("struct.sexpr")->getPointerTo();
    node_list params = derive<list>(args[0])->get_nodes();
    const std::string fname = name.empty() ? "closure" : name;

    type_list tl;
    tl.push_back(sexpr->getPointerTo());
    Function *fn = add_external_function(fname, get_ftype(sexpr, tl), m);
    BasicBlock *entry = add_block("entry", fn);

    if (!name.empty()) {
        /* if not lambda. */
        r->add(fname, fn);
    }

    r->enter_scope();

    LLVMContext &ctx = m->getContext();
    IRBuilder<> irb(entry);
    Value *cell = fn->arg_begin();
    for (size_t i = 0; i < params.size(); ++i) {
        const std::string argn = params[i]->str();
        GlobalVariable *gv = builtin::internal_gvar(m, argn);
        Value *v = irb.CreateLoad(irb.CreateGEP(cell, cint(ctx, i)));
        irb.CreateStore(v, gv);
        r->add(argn, gv);
    }
    Value *ret;
    for (node_list::iterator it = ++args.begin();
            it != args.end(); ++it) {
        ret = (*it)->codegen(entry);
        if (!name.empty() && CallInst::classof(ret)) {
            const std::string callee = derive<list>(*it)->get_nodes()[0]->str();
            /* tail recursion optimization */
            if (!env::dict::get(callee) && env::lookup(callee) == fn)
                dyn_cast<CallInst>(ret)->setTailCall();
        }
    }
    irb.CreateRet(ret);

    if (name.empty()) {
        /* if lambda. this is just needed for gets a s-exp object. */
        r->add(fname, fn);
    }

    Value *fobj = symbol(fname).codegen(bb);

    r->exit_scope();

#ifdef DEBUG
    verifyFunction(*fn);
#endif

    return fobj;
}

Value *define_codegen(module_ref &m, BasicBlock *bb, node_list args)
{
    if (list *cast = derive<list>(args[0])) {
        const std::string fname = cast->first()->str();
        node_list passed_args;
        list *l = new list(); l->add_nodes(cast->rest());
        passed_args.push_back(node_ref(l));
        for (node_list::iterator it = ++args.begin();
                it != args.end(); ++it) {
            passed_args.push_back(*it);
        }
        return closure_codegen(m, bb, passed_args, fname);
    }

    std::string name = args[0]->str();
    env::scope::maybe mv = r->lookup(name);

    /* if one of the following conditions is matched, adds a new
     * global variable to current scope (global or local).
     *  1. Not defined anywhere
     *  2. Already defined but not in current local scope
     *  3. Already defined but is a function */
    if (!mv || !env::exists_in_current(name) || Function::classof(*mv)) {
        GlobalVariable *gv = builtin::external_gvar(m, name);
        r->add(name, gv);
    }
    IRBuilder<>(bb).CreateStore(args[1]->codegen(bb), env::lookup(name));
    return symbol(name).quoted_codegen(bb);
}

Value *set_codegen(module_ref &m, BasicBlock *bb, node_list args)
{
    std::string name = args[0]->str();
    Value *v = env::lookup(name);
    IRBuilder<>(bb).CreateStore(args[1]->codegen(bb), v);
    return symbol(name).codegen(bb);
}

Value *quote_codegen(module_ref &m, BasicBlock *bb, node_list args)
{
    node_ref arg = args[0];
    if (list *cast = derive<list>(arg)) {
        /* If the node is a list, apply `quote` recursively. */
        if (cast->get_nodes().empty())
            return cast->codegen(bb);
        list l;
        l.add_node(node_ref(new symbol("list")));
        BOOST_FOREACH(node_ref n, cast->get_nodes()) {
            list *quote = new list();
            quote->add_node(node_ref(new symbol("quote")));
            quote->add_node(n);
            l.add_node(node_ref(quote));
        }
        return l.codegen(bb);
    }

    /* If the node is an atom, gets s-exp object of quoted value. */
    if (symbol *cast = derive<symbol>(arg)) {
        return cast->quoted_codegen(bb);
    } else {
        return symbol(arg->str()).quoted_codegen(bb);
    }
}

Value *if_codegen(module_ref &m, BasicBlock *bb, node_list args)
{
    /* Defines an anonymous function for `if`. */
    Type *sexpr = defstruct::get("struct.sexpr")->getPointerTo();
    type_list tl; tl.push_back(sexpr);
    Function *fn = add_external_function("", get_ftype(sexpr, tl), m);
    BasicBlock *entry = add_block("entry", fn),
               *yes= add_block("yes", fn),
               *no = add_block("no", fn);
    IRBuilder<> irb(entry);

    Value *lhs = fn->arg_begin(), *rhs = boolean(true).codegen(entry);
    lhs = irb.CreatePtrToInt(lhs, i64_type()),
    rhs = irb.CreatePtrToInt(rhs, i64_type());
    Value *cond = irb.CreateICmpEQ(lhs , rhs);
    BranchInst::Create(yes, no, cond, entry);

    ReturnInst::Create(m->getContext(), args[1]->codegen(yes), yes);
    ReturnInst::Create(m->getContext(), args[2]->codegen(no), no);

#ifdef DEBUG
    verifyFunction(*fn);
#endif

    return CallInst::Create(fn, args[0]->codegen(bb), "if", bb);
}

Value *let_codegen(module_ref &m, BasicBlock *bb, node_list args)
{
    r->enter_scope();

    /* local variables */
    list *largs = derive<list>(args[0]);
    BOOST_FOREACH(node_ref n, largs->get_nodes()) {
        list l;
        l.add_node(node_ref(new symbol("define")));
        l.add_nodes(derive<list>(n)->get_nodes());
        l.codegen(bb);
    }

    Value *ret;
    for (node_list::const_iterator it = ++args.begin();
            it != args.end(); ++it) {
        ret = (*it)->codegen(bb);
    }

    r->exit_scope();

    return ret;
}
}

namespace isana
{
namespace codegen
{
namespace builtin
{

void init_syntaxes(module_ref &m)
{
    env::dict::put("define", boost::bind(&define_codegen, m, _1, _2));
    env::dict::put("set!", boost::bind(&set_codegen, m, _1, _2));
    env::dict::put("quote", boost::bind(&quote_codegen, m, _1, _2));
    env::dict::put("if", boost::bind(&if_codegen, m, _1, _2));
    env::dict::put("lambda", boost::bind(&closure_codegen, m, _1, _2, ""));
    env::dict::put("let", boost::bind(&let_codegen, m, _1, _2));
}

} /* end of builtin */
} /* end of codegen */
} /* end of isana */
