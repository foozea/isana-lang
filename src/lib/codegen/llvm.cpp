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
#include <stdexcept>

#include "codegen/llvm.hpp"

namespace
{
using namespace llvm;
using isana::codegen::module_ref;

Function *add_function(
        const std::string &name,
        FunctionType *type,
        module_ref &m,
        GlobalValue::LinkageTypes link)
{
    return Function::Create(type, link, name.c_str(), m.get());
}
}

namespace isana
{
namespace codegen
{

module_map defmodule::m_table_ = module_map();

module_ref &defmodule::create(const std::string &str)
{
    module_map::iterator it = m_table_.find(str);
    if (it != m_table_.end()) return it->second;
    m_table_.insert(std::make_pair(str, module_ref(new Module(str, context()))));
    module_map::iterator m = m_table_.find(str);
    return m->second;
}

module_ref &defmodule::get(const std::string &str)
{
    module_map::iterator it = m_table_.find(str);
    if (it == m_table_.end())
        std::runtime_error("Undefined module called: " + str);
    return it->second;
}

ExecutionEngine *defmodule::execute(const module_ref &m)
{
    std::string errstr;
    ExecutionEngine* ee = EngineBuilder(m.get()).setErrorStr(&errstr).create();
    if (!ee) {
        std::cerr << "Could not execute: " << errstr.c_str() << std::endl;
        exit(1);
    }
    return ee;
}

/* --- */

struct_map defstruct::s_table_ = struct_map();

StructType *defstruct::create(const type_list &mems,
                              const std::string &str)
{
    StructType *s;
    if (str.empty()) s = StructType::create(mems);
    else s = StructType::create(mems, str);
    if (!s) std::cout << "null!" << std::endl;
    s->setBody(mems);
    s_table_.insert(std::make_pair(str, s));
    return s;
}

StructType *defstruct::get(const std::string &str)
{
    struct_map::iterator it = s_table_.find(str);
    if (it == s_table_.end())
        std::runtime_error("Undefined data type called: " + str);
    return it->second;
}

/* --- */

FunctionType *get_ftype(Type *ret, type_list &args, bool varg)
{
    return FunctionType::get(ret, args, varg);
}

Function *add_external_function(
        const std::string &name,
        FunctionType *type,
        module_ref &m)
{
    return add_function(name, type, m, Function::ExternalLinkage);
}

Function *add_internal_function(
        const std::string &name,
        FunctionType *type,
        module_ref &m)
{
    return add_function(name, type,  m, Function::InternalLinkage);
}

BasicBlock *add_block(
        const std::string &name,
        Function *f)
{
    BasicBlock *bb = BasicBlock::Create(f->getContext(), name, f);
    f->setGC("shadow-stack");
    return bb;
}

const std::string get_function_name(Function *f)
{
    const std::string mname = f->getParent()->getModuleIdentifier(),
                      fname = f->getName();
    std::ostringstream oss;
    oss << "#<" << mname << "/" << fname << ">";
    return oss.str();
}

} /* end of codegen */
} /* end of isana */
