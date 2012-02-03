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

/* define %struct.sexpr* @car(%struct.sexpr*) */
void def_car(module_ref &m)
{
    Type *sexpr = defstruct::get("struct.sexpr")->getPointerTo(),
         *cell = defstruct::get("struct.sexpr.as.cell")->getPointerTo();

    type_list args;
    args.push_back(sexpr->getPointerTo());
    Function *fn = add_external_function("car", get_ftype(sexpr, args), m);
    BasicBlock *bb = add_block("entry", fn);

    LLVMContext &ctx = m->getContext();
    IRBuilder<> irb(bb);

    Value *array = fn->arg_begin();
    Value *arg = irb.CreateLoad(irb.CreateGEP(array, cint(ctx, 0)));

    Value *idx[] = { cint(ctx, 0), cint(ctx, 1), cint(ctx, 0) };
    Value *v = irb.CreateInBoundsGEP(arg, idx);
    v = irb.CreateBitCast(v, cell);
    v = irb.CreateStructGEP(v, 0);
    v = irb.CreateLoad(v);
    irb.CreateRet(v);

    r->add("car", fn);
}

/* define %struct.sexpr* @cdr(%struct.sexpr*) */
void def_cdr(module_ref &m)
{
    Type *sexpr = defstruct::get("struct.sexpr")->getPointerTo(),
         *cell = defstruct::get("struct.sexpr.as.cell")->getPointerTo();

    type_list args;
    args.push_back(sexpr->getPointerTo());
    Function *fn = add_external_function("cdr", get_ftype(sexpr, args), m);
    BasicBlock *bb = add_block("entry", fn);

    LLVMContext &ctx = m->getContext();
    IRBuilder<> irb(bb);

    Value *array = fn->arg_begin();
    Value *arg = irb.CreateLoad(irb.CreateGEP(array, cint(ctx, 0)));

    Value *idx[] = { cint(ctx, 0), cint(ctx, 1), cint(ctx, 0) };
    Value *v = irb.CreateInBoundsGEP(arg, idx);
    v = irb.CreateBitCast(v, cell);
    v = irb.CreateStructGEP(v, 1);
    v = irb.CreateLoad(v);
    irb.CreateRet(v);

    r->add("cdr", fn);
}

/* define %struct.sexpr* @set-car!(%struct.sexpr**) */
void def_setcar(module_ref &m)
{
    Type *sexpr = defstruct::get("struct.sexpr")->getPointerTo(),
         *cell = defstruct::get("struct.sexpr.as.cell")->getPointerTo();

    type_list args;
    args.push_back(sexpr->getPointerTo());
    Function *fn = add_external_function("set-car!", get_ftype(sexpr, args), m);
    BasicBlock *bb = add_block("entry", fn);

    LLVMContext &ctx = m->getContext();
    IRBuilder<> irb(bb);

    Value *cells = fn->arg_begin();
    Value *lhs = irb.CreateLoad(irb.CreateGEP(cells, cint(ctx, 0))),
          *rhs = irb.CreateLoad(irb.CreateGEP(cells, cint(ctx, 1)));
    Value *idx[] = { cint(ctx, 0), cint(ctx, 1), cint(ctx, 0) };
    Value *v = irb.CreateInBoundsGEP(lhs, idx);
    v = irb.CreateBitCast(v, cell);
    v = irb.CreateStructGEP(v, 0);
    irb.CreateStore(rhs, v);
    irb.CreateRet(nil().codegen(bb));

    r->add("set-car!", fn);
}

/* define %struct.sexpr* @set-cdr!(%struct.sexpr**) */
void def_setcdr(module_ref &m)
{
    Type *sexpr = defstruct::get("struct.sexpr")->getPointerTo(),
         *cell = defstruct::get("struct.sexpr.as.cell")->getPointerTo();

    type_list args;
    args.push_back(sexpr->getPointerTo());
    Function *fn = add_external_function("set-cdr!", get_ftype(sexpr, args), m);
    BasicBlock *bb = add_block("entry", fn);

    LLVMContext &ctx = m->getContext();
    IRBuilder<> irb(bb);

    Value *cells = fn->arg_begin();
    Value *lhs = irb.CreateLoad(irb.CreateGEP(cells, cint(ctx, 0))),
          *rhs = irb.CreateLoad(irb.CreateGEP(cells, cint(ctx, 1)));
    Value *idx[] = { cint(ctx, 0), cint(ctx, 1), cint(ctx, 0) };
    Value *v = irb.CreateInBoundsGEP(lhs, idx);
    v = irb.CreateBitCast(v, cell);
    v = irb.CreateStructGEP(v, 1);
    irb.CreateStore(rhs, v);
    irb.CreateRet(nil().codegen(bb));

    r->add("set-cdr!", fn);
}

/* define %struct.sexpr* @cons(%struct.sexpr**) */
void def_cons(module_ref &m)
{
    Type *sexpr = defstruct::get("struct.sexpr")->getPointerTo();

    type_list args;
    args.push_back(sexpr->getPointerTo());
    Function *fn = add_external_function("cons", get_ftype(sexpr, args), m);

    LLVMContext &ctx = m->getContext();
    BasicBlock *bb = add_block("entry", fn);
    IRBuilder<> irb(bb);

    Value *arg = fn->arg_begin();
    Value *lhs = irb.CreateLoad(irb.CreateGEP(arg, cint(ctx, 0))),
          *rhs = irb.CreateLoad(irb.CreateGEP(arg, cint(ctx, 1)));

    args.clear();
    Function *allocfn =
        add_external_function("allocate", get_ftype(sexpr, args), m);

    Value *cell = irb.CreateCall(allocfn, "cell");
    Value *arr = irb.CreateAlloca(sexpr, cint(ctx, 2));
    Value *lp = irb.CreateGEP(arr, cint(ctx, 0));
    Value *rp = irb.CreateGEP(arr, cint(ctx, 1));
    irb.CreateStore(cell, lp);
    irb.CreateStore(lhs, rp);
    irb.CreateCall(env::lookup("set-car!"), arr);
    irb.CreateStore(rhs, rp);
    irb.CreateCall(env::lookup("set-cdr!"), arr);

    Value *t = irb.CreateStructGEP(cell, 0);
    t = irb.CreateGEP(t, cint(ctx, 0));
    t = irb.CreateBitCast(t, i32_ptr_type());
    irb.CreateStore(cint(ctx, cell_t), t);

    irb.CreateRet(cell);

    r->add("cons", fn);
}

/* define %struct.sexpr* @list(%struct.sexpr**) */
void def_list(module_ref &m)
{
    Type *sexpr = defstruct::get("struct.sexpr")->getPointerTo();

    type_list args;
    args.push_back(sexpr->getPointerTo());
    Function *fn = add_external_function("list", get_ftype(sexpr, args), m),
             *cons = dyn_cast<Function>(env::lookup("cons"));

    BasicBlock *entry = add_block("entry", fn),
               *cont  = add_block("contenue", fn),
               *retn  = add_block("return-null", fn),
               *loop  = add_block("loop", fn),
               *body  = add_block("loop-body", fn),
               *exit  = add_block("exit", fn);

    LLVMContext &ctx = m->getContext();
    IRBuilder<> irb(entry);
    Value *in = fn->arg_begin();
    Value *cell = irb.CreateAlloca(sexpr, 0);
    Value *counter = irb.CreateAlloca(i32_type(), 0);
    irb.CreateStore(cint(ctx, 0), counter);
    Value *arr = irb.CreateAlloca(sexpr, cint(ctx, 2));
    Value *lhs = irb.CreateGEP(arr, cint(ctx, 0));
    Value *rhs = irb.CreateGEP(arr, cint(ctx, 1));

    Value *idx = irb.CreateLoad(counter);
    Value *v = irb.CreateLoad(irb.CreateGEP(in, idx));
    idx = irb.CreateAdd(idx, cint(ctx, 1));
    irb.CreateStore(idx, counter);
    Value *cond = irb.CreateIsNotNull(v);
    BranchInst::Create(cont, retn, cond, entry);

    irb.SetInsertPoint(cont);
    irb.CreateStore(v, lhs);
    irb.CreateStore(nil().codegen(cont), rhs);
    Value *root = irb.CreateCall(cons, arr);
    irb.CreateStore(root, cell);
    irb.CreateBr(loop);

    irb.SetInsertPoint(retn);
    irb.CreateRet(nil().codegen(retn));

    irb.SetInsertPoint(loop);
    idx = irb.CreateLoad(counter);
    v = irb.CreateLoad(irb.CreateGEP(in, idx));
    idx = irb.CreateAdd(idx, cint(ctx, 1));
    irb.CreateStore(idx, counter);
    cond = irb.CreateIsNotNull(v);
    BranchInst::Create(body, exit, cond, loop);

    irb.SetInsertPoint(body);
    irb.CreateStore(v, lhs);
    irb.CreateStore(nil().codegen(body), rhs);
    v = irb.CreateCall(cons, arr);
    Value *par = irb.CreateLoad(cell);
    irb.CreateStore(par, lhs);
    irb.CreateStore(v, rhs);
    irb.CreateCall(env::lookup("set-cdr!"), arr);
    irb.CreateStore(v, cell);
    irb.CreateBr(loop);

    irb.SetInsertPoint(exit);
    irb.CreateRet(root);

    r->add("list", fn);
}
}

namespace isana
{
namespace codegen
{
namespace builtin
{

void init_pairs_and_lists(module_ref &m)
{
    def_car(m);
    def_cdr(m);
    def_setcar(m);
    def_setcdr(m);
    def_cons(m);
    def_list(m);
}

} /* end of builtin */
} /* end of codegen */
} /* end of isana */
