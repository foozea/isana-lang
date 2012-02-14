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

#ifndef ISANA_RUNTIME_OBJECT_INCLUDED_
#define ISANA_RUNTIME_OBJECT_INCLUDED_

#include <cstddef>
#include <stdint.h>
#include <string>

#ifdef __cplusplus
namespace isana
{
extern "C"
{
#endif

/* The types of s-expression. */
enum token
{
    /*
     * b 0 0 0 0 1
     *   | | | | |___ atom or not
     *   | | | |_____ number or not
     *   | | |_____
     *   | |_______|_ sequence no
     *   |_________|
     */
    opaque_t   = 0,  /* b00000 */
    integer_t  = 7,  /* b00111 */
    real_t     = 11, /* b01011 */
    rational_t = 15, /* b01111 */
    complex_t  = 19, /* b10011 */
    boolean_t  = 5,  /* b00101 */
    string_t   = 9,  /* b01001 */
    symbol_t   = 13, /* b01101 */
    nil_t      = 17, /* b10001 */
    cell_t     = 4,  /* b00100 */
    function_t = 8,  /* b01000 */
    lambda_t   = 12  /* b01100 */
};

/* s-expression */
struct sexpr
{
    struct {
        int32_t    type;    /* object type */
        union {
            void   *ptr;    /* generic pointer */
            const char *name; /* symbol or function name */
            bool   exact;   /* exact or inexact */
        } attr;
    } meta;
    union {
        void       *ptr;    /* generic pointer */
        int64_t    integer; /* as integer */
        double     real;    /* as real number */
        const char *str;    /* as string */
        sexpr      *quoted; /* as quoted value */
        struct { sexpr *head, *tail; } cell; /* as cons cell */
    } as;
};

#ifdef __cplusplus
}
} /* end of isana */
#endif
#endif
