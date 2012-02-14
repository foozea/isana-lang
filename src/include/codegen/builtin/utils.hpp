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

#ifndef ISANA_CODEGEN_BUILTIN_UTILS_INCLUDED_
#define ISANA_CODEGEN_BUILTIN_UTILS_INCLUDED_

#include "codegen/llvm.hpp"
#include "runtime/object.hpp"

namespace isana
{
namespace codegen
{
namespace builtin
{

template <GlobalVariable::LinkageTypes L>
GlobalVariable *gvar(module_ref &m,
                     const std::string &name,
                     Type *type,
                     Constant *init = NULL)
{
    if (!init) init = cint(m->getContext(), 0);
    return new GlobalVariable(*m.get(), type, false, L, init, name);
}

inline GlobalVariable *internal_gvar(module_ref &m,
                                     const std::string &name,
                                     Type *type,
                                     Constant *init = NULL)
{
    return gvar<GlobalVariable::InternalLinkage>(m, name, type, init);
}

inline GlobalVariable *external_gvar(module_ref &m,
                                     const std::string &name,
                                     Type *type,
                                     Constant *init = NULL)
{
    return gvar<GlobalVariable::ExternalLinkage>(m, name, type, init);
}

inline GlobalVariable *internal_gvar(module_ref &m,
                                     const std::string &name,
                                     Constant *init = NULL)
{
    Type *t = defstruct::get("struct.sexpr")->getPointerTo();
    return gvar<GlobalVariable::InternalLinkage>(m, name, t, init);
}

inline GlobalVariable *external_gvar(module_ref &m,
                                     const std::string &name,
                                     Constant *init = NULL)
{
    Type *t = defstruct::get("struct.sexpr")->getPointerTo();
    return gvar<GlobalVariable::ExternalLinkage>(m, name, t, init);
}

Constant *constant_meta(isana::token, Constant*);
Constant *constant_as(Constant*);
Constant *constant_cell(Constant*, Constant*);
Constant *constant_sexpr(isana::token, Constant*, Constant*);

} /* end of builtin */
} /* end of codegen */
} /* end of isana */
#endif
