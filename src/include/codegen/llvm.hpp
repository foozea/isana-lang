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

#ifndef ISANA_CODEGEN_LLVM_INCLUDED_
#define ISANA_CODEGEN_LLVM_INCLUDED_

#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <llvm/Constant.h>
#include <llvm/DerivedTypes.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Function.h>
#include <llvm/GlobalValue.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Type.h>
#include <llvm/Value.h>

namespace isana
{
namespace codegen
{

using namespace llvm;

typedef boost::shared_ptr<Module> module_ref;
typedef std::vector<Type*> type_list;
typedef std::vector<Value*> value_list;
typedef std::map<std::string, module_ref> module_map;
typedef std::map<std::string, StructType*> struct_map;

/* Initializes native target. */
inline void init_target(void) { InitializeNativeTarget(); };

/* Gets the global context. */
inline LLVMContext &context(void) { return getGlobalContext(); }

/* Get the llvm::Types */
inline Type *void_type(void) { return Type::getVoidTy(context()); }
inline Type *label_type(void) { return Type::getLabelTy(context()); }
inline Type *i1_type(void) { return Type::getInt1Ty(context()); }
inline Type *i8_type(void) { return Type::getInt8Ty(context()); }
inline Type *i16_type(void) { return Type::getInt16Ty(context()); }
inline Type *i32_type(void) { return Type::getInt32Ty(context()); }
inline Type *i64_type(void) { return Type::getInt64Ty(context()); }
inline Type *float_type(void) { return Type::getFloatTy(context()); }
inline Type *double_type(void) { return Type::getDoubleTy(context()); }
inline Type *i1_ptr_type(void) { return Type::getInt1PtrTy(context()); }
inline Type *i8_ptr_type(void) { return Type::getInt8PtrTy(context()); }
inline Type *i16_ptr_type(void) { return Type::getInt16PtrTy(context()); }
inline Type *i32_ptr_type(void) { return Type::getInt32PtrTy(context()); }
inline Type *i64_ptr_type(void) { return Type::getInt64PtrTy(context()); }
inline Type *float_ptr_type(void) { return Type::getFloatPtrTy(context()); }
inline Type *double_ptr_type(void) { return Type::getDoublePtrTy(context()); }

/* Manages the modules with map.
 * This class enables create/get a llvm::Module using key string. */
class defmodule
{
  public:
    /* Creates a new llvm::Module.
     * If already exists, returns form the map. */
    static module_ref &create(const std::string&);
    /* Gets the module from the map.
     * If not exists, runtime_error occurs. */
    static module_ref &get(const std::string&);
    /* Gets a llvm::ExecutionEngine of a module. */
    static ExecutionEngine *execute(const module_ref&);

  private:
    static module_map m_table_;
};

/* Manages structs with the map.
 * This class enables create/get a llvm::StructType using key string. */
class defstruct
{
  public:
    /* Creates a new llvm::StructType and set the lhs argument as the member.
     * If the rhs argument is empty, anonymous struct will be created.
     * If already exists, returns from the map. */
    static StructType *create(const type_list&, const std::string& = "");
    /* Gets a llvm::StructType from the map.
     * If not exists, runtime_error occurs. */
    static StructType *get(const std::string&);

  private:
    static struct_map s_table_;
};

/* Gets a function type. */
FunctionType *get_ftype(Type*, type_list&, bool = false);

/* Adds a new external function to the module. */
Function *add_external_function(
        const std::string&,
        FunctionType*,
        module_ref&);

/* Adds a new internal function to the module. */
Function *add_internal_function(
        const std::string&,
        FunctionType*,
        module_ref&);

/* Adds a BasicBlock to a function. */
BasicBlock *add_block(
        const std::string&,
        Function*);

/* Gets the function name. (format: #<module/name>) */
const std::string get_function_name(Function *f);

/* --- */

/* Gets a constant integer. */
template <int N, int R, class T>
inline Constant *cint(LLVMContext &c, T v)
{
    return ConstantInt::get(c, APInt(N, v, R));
}

template <class T>
inline Constant *cint(LLVMContext &c, T v)
{
    return cint<32, 10>(c, v);
}

/* Gets a constant floating point. */
template <class T>
inline Constant *cdouble(LLVMContext &c, T v)
{
    return ConstantFP::get(c, APFloat(APFloat::IEEEdouble, v));
}

/* Gets a constant string. */
inline Constant *cstr(LLVMContext &c,
                      const std::string &str)
{
    Constant *s = ConstantArray::get(c, str);
    GlobalVariable *g =
        new GlobalVariable(
            ArrayType::get(i8_type(), str.length()),
            true,
            GlobalValue::PrivateLinkage,
            s,
            ".lstr");
    return g;
}

} /* end of codegen */
} /* end of isana */
#endif
