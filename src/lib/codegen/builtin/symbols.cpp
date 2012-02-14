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

#include "codegen/llvm.hpp"
#include "codegen/builtin/utils.hpp"
#include "env/resolver.hpp"
#include "runtime/object.hpp"

namespace
{
using namespace llvm;
using namespace isana;
using namespace isana::codegen;

env::resolver_ref r = env::resolver::get_instance();

void def_symbols(module_ref &m,
                 token t,
                 const std::string &name,
                 Constant *c,
                 const std::string prefix = "")
{
    Constant *str = cstr(m->getContext(), name);
    Constant *cs = builtin::constant_sexpr(t, str, c);
    GlobalVariable *gv =
        new GlobalVariable(
            *m.get(),
            defstruct::get("struct.sexpr"),
            false,
            GlobalVariable::ExternalLinkage,
            cs,
            prefix + name);
    GlobalVariable *store = builtin::external_gvar(m, name, gv);
    r->add(name, store);
}

}

namespace isana
{
namespace codegen
{
namespace builtin
{

void init_symbols(module_ref &m)
{
    def_symbols(m, nil_t, "nil", cint(m->getContext(), 0), ".symbol.");
    def_symbols(m, boolean_t, "#t", cint(m->getContext(), 1), ".bool.");
    def_symbols(m, boolean_t, "#f", cint(m->getContext(), 0), ".bool.");
}

} /* end of builtin */
} /* end of codegen */
} /* end of isana */
