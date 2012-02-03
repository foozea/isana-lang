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

#include <llvm/Intrinsics.h>

#include "codegen/llvm.hpp"
#include "env/resolver.hpp"
#include "reader/ast/atom.hpp"
#include "runtime/object.hpp"

namespace
{
using namespace llvm;
using namespace isana;
using namespace isana::codegen;
using namespace isana::reader;

env::resolver_ref r = env::resolver::get_instance();

/* define %struct.sexpr* @"eq?"(%struct.sexpr**) */
void def_eq(module_ref &m)
{
    Type *sexpr = defstruct::get("struct.sexpr")->getPointerTo();
    type_list args;
    args.push_back(sexpr->getPointerTo());
    Function *fn = add_external_function("eq?", get_ftype(sexpr, args), m);

    BasicBlock *entry = add_block("entry", fn),
               *yes = add_block("yes", fn),
               *no = add_block("no", fn);

    LLVMContext &ctx = m->getContext();
    IRBuilder<> irb(entry);

    Value *array = fn->arg_begin();
    Value *lhs = irb.CreateLoad(irb.CreateGEP(array, cint(ctx, 0))),
          *rhs = irb.CreateLoad(irb.CreateGEP(array, cint(ctx, 1)));

    Value *cond = irb.CreatePtrDiff(lhs , rhs);
    cond = irb.CreateICmpEQ(cond , cint<64, 10>(ctx, 0));

    BranchInst::Create(yes, no, cond, entry);
    ReturnInst::Create(m->getContext(), boolean(true).codegen(yes), yes);
    ReturnInst::Create(m->getContext(), boolean(false).codegen(no), no);

    r->add("eq?", fn);
}

/* define %struct.sexpr* @"eq?"(%struct.sexpr**) */
void def_eqv(module_ref &m)
{
    Type *sexpr = defstruct::get("struct.sexpr")->getPointerTo();
    type_list args;
    args.push_back(sexpr->getPointerTo());
    Function *eqvfn = add_external_function("eqv", get_ftype(i1_type(), args), m),
             *fn = add_external_function("eqv?", get_ftype(sexpr, args), m);

    BasicBlock *entry = add_block("entry", fn),
               *yes = add_block("yes", fn),
               *no = add_block("no", fn);

    IRBuilder<> irb(entry);

    Value *array = fn->arg_begin();
    Value *cond = irb.CreateCall(eqvfn, array);

    BranchInst::Create(yes, no, cond, entry);
    ReturnInst::Create(m->getContext(), boolean(true).codegen(yes), yes);
    ReturnInst::Create(m->getContext(), boolean(false).codegen(no), no);

    r->add("eqv?", fn);
}

void def_exact(module_ref &m)
{
    Type *sexpr = defstruct::get("struct.sexpr")->getPointerTo();
    type_list args; args.push_back(sexpr->getPointerTo());
    Function *fn = add_external_function("exact?", get_ftype(sexpr, args), m);

    BasicBlock *entry = add_block("entry", fn),
               *yes = add_block("yes", fn),
               *no = add_block("no", fn);

    LLVMContext &ctx = m->getContext();
    IRBuilder<> irb(entry);

    Value *array = fn->arg_begin();
    Value *arg = irb.CreateLoad(irb.CreateGEP(array, cint(ctx, 0)));

    Value *idx[] = { cint(ctx, 0), cint(ctx, 0), cint(ctx, 1), cint(ctx, 0) };
    Value *e = irb.CreateInBoundsGEP(arg, idx);
    e = irb.CreateBitCast(e, i1_ptr_type());
    e = irb.CreateLoad(e);

    BranchInst::Create(yes, no, e, entry);
    ReturnInst::Create(m->getContext(), boolean(true).codegen(yes), yes);
    ReturnInst::Create(m->getContext(), boolean(false).codegen(no), no);

    r->add("exact?", fn);
}

void def_inexact(module_ref &m)
{
    Type *sexpr = defstruct::get("struct.sexpr")->getPointerTo();
    type_list args; args.push_back(sexpr->getPointerTo());
    Function *fn = add_external_function("inexact?", get_ftype(sexpr, args), m);

    BasicBlock *entry = add_block("entry", fn),
               *yes = add_block("yes", fn),
               *no = add_block("no", fn);

    LLVMContext &ctx = m->getContext();
    IRBuilder<> irb(entry);

    Value *array = fn->arg_begin();
    Value *arg = irb.CreateLoad(irb.CreateGEP(array, cint(ctx, 0)));

    Value *idx[] = { cint(ctx, 0), cint(ctx, 0), cint(ctx, 1), cint(ctx, 0) };
    Value *e = irb.CreateInBoundsGEP(arg, idx);
    e = irb.CreateBitCast(e, i1_ptr_type());
    e = irb.CreateLoad(e);

    BranchInst::Create(yes, no, irb.CreateNot(e), entry);
    ReturnInst::Create(m->getContext(), boolean(true).codegen(yes), yes);
    ReturnInst::Create(m->getContext(), boolean(false).codegen(no), no);

    r->add("inexact?", fn);
}

/* define %struct.sexpr* @"(type name)?"(%struct.sexpr*) */
void def_typecheck(module_ref &m, const std::string &name, uint64_t mask)
{
    Type *sexpr = defstruct::get("struct.sexpr")->getPointerTo();
    type_list args;
    args.push_back(sexpr->getPointerTo());
    Function *fn = add_external_function(name, get_ftype(sexpr, args), m);

    BasicBlock *entry = add_block("entry", fn),
               *yes = add_block("yes", fn),
               *no = add_block("no", fn);

    LLVMContext &ctx = m->getContext();
    IRBuilder<> irb(entry);

    Value *array = fn->arg_begin();
    Value *arg = irb.CreateLoad(irb.CreateGEP(array, cint(ctx, 0)));

    Value *idx[] = { cint(ctx, 0), cint(ctx, 0), cint(ctx, 0) };
    Value *t = irb.CreateInBoundsGEP(arg, idx, "gep");
    t = irb.CreateLoad(t, "typeval");
    Value *cond = irb.CreateAnd(t, mask);
    cond = irb.CreateICmpEQ(cond, cint(m->getContext(), mask));

    BranchInst::Create(yes, no, cond, entry);
    ReturnInst::Create(m->getContext(), boolean(true).codegen(yes), yes);
    ReturnInst::Create(m->getContext(), boolean(false).codegen(no), no);

    r->add(name, fn);
}

/* define %struct.sexpr* @"(type name)?"(%struct.sexpr*) */
void def_typecheck(module_ref &m, const std::string &name, Constant *c)
{
    Type *sexpr = defstruct::get("struct.sexpr")->getPointerTo();
    type_list args;
    args.push_back(sexpr->getPointerTo());
    Function *fn = add_external_function(name, get_ftype(sexpr, args), m);

    BasicBlock *entry = add_block("entry", fn),
               *yes = add_block("yes", fn),
               *no = add_block("no", fn);

    LLVMContext &ctx = m->getContext();
    IRBuilder<> irb(entry);

    Value *array = fn->arg_begin();
    Value *arg = irb.CreateLoad(irb.CreateGEP(array, cint(ctx, 0)));

    Value *idx[] = { cint(ctx, 0), cint(ctx, 0), cint(ctx, 0) };
    Value *t = irb.CreateInBoundsGEP(arg, idx);
    t = irb.CreateLoad(t);
    Value *cond = irb.CreateICmpEQ(t, c);

    BranchInst::Create(yes, no, cond, entry);
    ReturnInst::Create(m->getContext(), boolean(true).codegen(yes), yes);
    ReturnInst::Create(m->getContext(), boolean(false).codegen(no), no);

    r->add(name, fn);
}
}

namespace isana
{
namespace codegen
{
namespace builtin
{

void init_predicates(module_ref &m)
{
    def_eq(m);
    def_eqv(m);
    def_exact(m);
    def_inexact(m);
    def_typecheck(m, "atom?",      1);
    def_typecheck(m, "number?",    2);
    def_typecheck(m, "null?",      cint(m->getContext(), nil_t));
    def_typecheck(m, "integer?",   cint(m->getContext(), integer_t));
    def_typecheck(m, "rational?",  cint(m->getContext(), rational_t));
    def_typecheck(m, "real?",      cint(m->getContext(), real_t));
    def_typecheck(m, "complex?",   cint(m->getContext(), complex_t));
    def_typecheck(m, "boolean?",   cint(m->getContext(), boolean_t));
    def_typecheck(m, "string?",    cint(m->getContext(), string_t));
    def_typecheck(m, "symbol?",    cint(m->getContext(), symbol_t));
    def_typecheck(m, "procedure?", cint(m->getContext(), function_t));
    def_typecheck(m, "pair?",      cint(m->getContext(), cell_t));
}

} /* end of builtin */
} /* end of codegen */
} /* end of isana */
