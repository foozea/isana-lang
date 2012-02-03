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

#include "codegen/builtin/utils.hpp"

#include <iostream>

namespace isana
{
namespace codegen
{
namespace builtin
{

Constant *constant_meta(isana::token t, Constant *attr)
{
    LLVMContext &c = attr->getContext();
    StructType *meta_type = defstruct::get("struct.sexpr.meta");
    StructType *attr_type = defstruct::get("struct.sexpr.meta.attr");
    Constant *attr_val = ConstantStruct::get(attr_type, attr, NULL);
    return ConstantStruct::get(meta_type, cint(c, (int)t), attr_val, NULL);
}

Constant *constant_as(Constant *v)
{
    StructType *as_type = defstruct::get("struct.sexpr.as");
    return ConstantStruct::get(as_type, v, NULL);
}

Constant *constant_cell(Constant *car, Constant *cdr)
{
    StructType *cell_type = defstruct::get("struct.sexpr.as.cell");
    return ConstantStruct::get(cell_type, car, cdr, NULL);
}

Constant *constant_sexpr(isana::token t, Constant *attr, Constant *v)
{
    Constant *meta = constant_meta(t, attr), *as = constant_as(v);
    return ConstantStruct::get(defstruct::get("struct.sexpr"), meta, as, NULL);
}
} /* end of builtin */
} /* end of codegen */
} /* end of isana */
