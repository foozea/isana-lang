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
#include "env/resolver.hpp"

namespace
{

isana::env::resolver_ref r = isana::env::resolver::get_instance();

void def_sexpr(void)
{
    using namespace llvm;
    using namespace isana::codegen;

    type_list ms;
    ms.push_back(i8_ptr_type());
    StructType *attr = defstruct::create(ms, "struct.sexpr.meta.attr");

    ms.clear();
    ms.push_back(i32_type());
    ms.push_back(attr);
    StructType *meta = defstruct::create(ms, "struct.sexpr.meta");

    ms.clear();
    ms.push_back(i8_ptr_type());
    StructType *as = defstruct::create(ms, "struct.sexpr.as");

    ms.clear();
    ms.push_back(meta); ms.push_back(as);
    StructType *sexpr = defstruct::create(ms, "struct.sexpr");

    ms.clear();
    ms.push_back(sexpr->getPointerTo());
    ms.push_back(sexpr->getPointerTo());
    defstruct::create(ms, "struct.sexpr.as.cell");
}
}

namespace isana
{
namespace codegen
{
namespace builtin
{

void init_datatypes(module_ref &m)
{
    def_sexpr();
}

} /* end of builtin */
} /* end of codegen */
} /* end of isana */
