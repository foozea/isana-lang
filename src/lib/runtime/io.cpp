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

#include <cstdlib>
#include <iostream>
#include <iomanip>

#include "runtime/object.hpp"

#ifdef __cplusplus
namespace isana
{
#endif

void link_io(void) {}

#ifdef __cplusplus
extern "C"
{
#endif

void display(sexpr *exp)
{
    if (exp->meta.type == cell_t) {
        sexpr *cur = exp;
        std::cout << "(";
        display(cur->as.cell.head);
        while (cur->as.cell.tail->meta.type == cell_t) {
            cur = cur->as.cell.tail;
            std::cout << " ";
            display(cur->as.cell.head);
        }
        if (cur->as.cell.tail->meta.type != nil_t) {
            std::cout << " ";
            display(cur->as.cell.tail);
        }
        std::cout << ")";
        return;
    }
    switch (exp->meta.type) {
        case (integer_t):
            std::cout << exp->as.integer;
            if (!exp->meta.attr.exact) std::cout << ".0";
            break;
        case (real_t):
            std::cout << std::setprecision(10) << exp->as.real;
            break;
        case (rational_t):
            display(exp->as.cell.head);
            std::cout << "/";
            display(exp->as.cell.tail);
            break;
        case (complex_t):
            if (exp->as.cell.head->as.real > 0) {
                display(exp->as.cell.head);
                if (exp->as.cell.tail->as.real >= 0)
                    std::cout << "+";
            }
            display(exp->as.cell.tail);
            std::cout << "i";
            break;
        case (string_t):
            std::cout << exp->as.str;
            break;
        case (boolean_t):
            std::cout << (exp->as.integer == 1 ? "#t" : "#f");
            break;
        case (symbol_t):
            std::cout << exp->meta.attr.name;
            break;
        case (function_t):
            std::cout << exp->meta.attr.name;
            break;
        case (nil_t):
            std::cout << "nil";
            break;
        default:
            std::cout << "nil";
            break;
    }
}

void newline(void)
{
    std::cout << std::endl;
}
#ifdef __cplusplus
}
} /* end of isana */
#endif
