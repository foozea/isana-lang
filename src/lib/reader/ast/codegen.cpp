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
#include <sstream>

#include <boost/foreach.hpp>

#include "codegen/llvm.hpp"
#include "codegen/builtin/utils.hpp"
#include "env/dict.hpp"
#include "env/resolver.hpp"
#include "reader/ast/atom.hpp"
#include "reader/ast/function.hpp"
#include "reader/ast/syntax.hpp"
#include "reader/ast/list.hpp"

namespace
{

using namespace llvm;
using namespace isana::codegen;

template <isana::token T>
Value *new_constant(BasicBlock *bb,
                    const std::string &name,
                    Constant *attr,
                    Constant *v)
{
    Module *m = bb->getParent()->getParent();
    if (GlobalVariable *exists = m->getGlobalVariable(name, true)) {
        if (exists->getInitializer() == v) return exists;
        if (T == isana::string_t) return exists;
        IRBuilder<> irb(bb);
        LLVMContext &ctx = bb->getContext();
        Value *idx[] = { cint(ctx, 0), cint(ctx, 1), cint(ctx, 0) };
        Value *ptr = irb.CreateInBoundsGEP(exists, idx, "gep");
        ptr = irb.CreateBitCast(ptr, v->getType()->getPointerTo(), "ptr");
        irb.CreateStore(v, ptr);
        return exists;
    }
    Constant *cs = builtin::constant_sexpr(T, attr, v);
    GlobalVariable *gv =
        new GlobalVariable(
            *m,
            defstruct::get("struct.sexpr"),
            false,
            GlobalVariable::InternalLinkage,
            cs,
            name);
    return gv;
}
}

namespace isana
{
namespace reader
{

using namespace llvm;
using namespace isana::codegen;

/* All nodes generate a constant struct of "struct.sexpr".
 * The type and the value depend on the each node. */

Value *integer::codegen(BasicBlock *bb) const
{
    return new_constant<integer_t>(
            bb,
            ".numeric." + value_ + (exact_ ? "e" : "i"),
            ConstantInt::get(i1_type(), exact_),
            cint(bb->getContext(), value_));
}

Value *rational::codegen(BasicBlock *bb) const
{
    Constant *cell = builtin::constant_cell(
            dyn_cast<Constant>(numer_.codegen(bb)),
            dyn_cast<Constant>(denom_.codegen(bb)));
    return new_constant<rational_t>(
            bb,
            ".numeric." + str(),
            ConstantInt::get(i1_type(), 1),
            cell);
}

Value *real::codegen(BasicBlock *bb) const
{
    return new_constant<real_t>(
            bb,
            ".numeric." + value_,
            ConstantInt::get(i1_type(), 0),
            cdouble(bb->getContext(), value_));
}

Value *complex::codegen(BasicBlock *bb) const
{
    Constant *cell = builtin::constant_cell(
            dyn_cast<Constant>(real_.codegen(bb)),
            dyn_cast<Constant>(image_.codegen(bb)));
    return new_constant<complex_t>(
            bb,
            ".numeric." + str(),
            ConstantInt::get(i1_type(), 1),
            cell);
}

Value *boolean::codegen(BasicBlock *bb) const
{
    const std::string str = value_ ? "#t" : "#f";
    return new LoadInst(env::lookup(str), str, bb);;
}

Value *string::codegen(BasicBlock *bb) const
{
    Constant *str = cstr(bb->getContext(), value_);
    return new_constant<string_t>(bb, ".str." + value_, str, str);
}

Value *symbol::codegen(BasicBlock *bb) const
{
    Value *v = env::lookup(value_);
    if (GlobalVariable::classof(v)) {
        Value *load = IRBuilder<>(bb).CreateLoad(v);
        return load->getType()->isPointerTy() ? load : v;
    }
    if (Function::classof(v)) {
        Function *f = dyn_cast<Function>(v);
        Constant *fname = cstr(bb->getContext(), get_function_name(f));
        return new_constant<function_t>(bb, ".proc." + value_, fname, f);
    }
    return v;
}

Value *symbol::quoted_codegen(BasicBlock *bb) const
{
    PointerType *sexpr = defstruct::get("struct.sexpr")->getPointerTo();
    env::resolver_ref r = env::resolver::get_instance();
    env::scope::maybe mb = r->lookup(value_);
    Value *v = ConstantPointerNull::get(sexpr);
    if (mb) {
        if (GlobalVariable::classof(*mb)) {
            Value *load = new LoadInst(*mb, "value", bb);
            v = load->getType()->isPointerTy() ? load : *mb;
        } else {
            v = *mb;
        }
    }

    Module *m = bb->getParent()->getParent();
    Value *gv;
    if (GlobalVariable *exists =
            m->getGlobalVariable(".quoted." + value_, true)) {
        gv = exists;
    } else {
        Constant *nullp = ConstantPointerNull::get(sexpr);
        Constant *str = cstr(bb->getContext(), value_);
        gv = new_constant<symbol_t>(bb, ".quoted." + value_, str, nullp);
    }

    if (!mb) return gv;

    IRBuilder<> irb(bb);
    LLVMContext &ctx = bb->getContext();
    Value *idx[] = { cint(ctx, 0), cint(ctx, 1), cint(ctx, 0) };
    Value *ptr = irb.CreateInBoundsGEP(gv, idx);
    ptr = irb.CreateBitCast(ptr, sexpr->getPointerTo());
    irb.CreateStore(v, ptr);
    return gv;
}

Value *nil::codegen(BasicBlock *bb) const
{
    const std::string str = "nil";
    return new LoadInst(env::lookup(str), str, bb);
}

Value *function::codegen(BasicBlock *bb) const
{
    Type *sexpr = defstruct::get("struct.sexpr")->getPointerTo();

    LLVMContext &ctx = bb->getContext();
    IRBuilder<> irb(bb);
    Value *argv = irb.CreateAlloca(sexpr, cint(ctx, args_.size() + 1));
    for (size_t i = 0; i < args_.size(); ++i) {
        Value *p = irb.CreateGEP(argv, cint(ctx, i));
        Value *arg = args_[i]->codegen(bb);
        irb.CreateStore(arg, p);
    }
    Value *p = irb.CreateGEP(argv, cint(ctx, args_.size()));
    irb.CreateStore(ConstantPointerNull::get(dyn_cast<PointerType>(sexpr)), p);

    type_list tl;
    Type *ftype = get_ftype(sexpr, tl, true)->getPointerTo();

    Value *f = func_->codegen(bb);
    Value *idx[] = { cint(ctx, 0), cint(ctx, 1), cint(ctx, 0) };
    Value *v = irb.CreateInBoundsGEP(f, idx);
    v = irb.CreateLoad(v);
    v = irb.CreateBitCast(v, ftype);

    return CallInst::Create(v, argv, "call", bb);
}

Value *syntax::codegen(BasicBlock *bb) const
{
    return (*env::dict::get(name_))(bb, args_);
}

Value *list::codegen(BasicBlock *bb) const
{
    if (nodes_.empty()) return nil().codegen(bb);
    const std::string name = nodes_[0]->str();
    if (env::dict::get(name)) {
        syntax s;
        s.set_name(name);
        for (node_list::const_iterator it = ++(nodes_.begin());
                it != nodes_.end(); ++it) {
            s.add_arg(*it);
        }
        return s.codegen(bb);
    }
    function f;
    f.set_callee(nodes_[0]);
    for (node_list::const_iterator it = ++(nodes_.begin());
            it != nodes_.end(); ++it) {
        f.add_arg(*it);
    }
    return f.codegen(bb);
}

} /* end of reader */
} /* end of isana */
