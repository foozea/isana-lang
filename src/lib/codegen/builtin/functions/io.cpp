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

/* declare void @display(%struct.sexpr*)
 * declare void @newline() */
void def_print(module_ref &m)
{
    Type *sexpr = defstruct::get("struct.sexpr")->getPointerTo();
    type_list args;
    args.push_back(sexpr);
    FunctionType *ftype = get_ftype(void_type(), args);
    Function *fn = add_external_function("display", ftype, m);

    args.clear();
    ftype = get_ftype(void_type(), args);
    Function *fnln = add_external_function("newline", ftype, m);

    r->add("display", fn);
    r->add("newline", fnln);
}
}

namespace isana
{
namespace codegen
{
namespace builtin
{

void init_io(module_ref &m)
{
    def_print(m);
}

} /* end of builtin */
} /* end of codegen */
} /* end of isana */
