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

#include "reader/lexer.hpp"

namespace
{
/* Determins if a character is '(' or ')'. */
bool isparen(const char c) { return (c == '(' || c == ')'); }
/* Determins if a character is '"'. */
bool isdquote(const char c) { return c == '"'; }
/* Determins if a character is '''. */
bool issquote(const char c) { return c == '\''; }
}

namespace isana
{
namespace reader
{

token_list tokenize(const std::string& str) {
    token_list tlist;
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (issquote(*it)) {
            std::string s; s.push_back(*it);
            tlist.push_back(s);
            continue;
        }
        if (isdquote(*it)) {
            std::string s; s.push_back(*it++);
            while (it != str.end() && !isdquote(*it)) s.push_back(*it++);
            if (isdquote(*it)) s.push_back(*it);
            tlist.push_back(s);
            continue;
        }
        if (isspace(*it)) continue;
        if (isparen(*it)) {
            std::string s; s.push_back(*it);
            tlist.push_back(s);
        } else {
            std::string s;
            while (it != str.end() && !isspace(*it) && !isparen(*it))
                s.push_back(*it++);
            tlist.push_back(s);
            if (isparen(*it)) {
                std::string s; s.push_back(*it);
                tlist.push_back(s);
            }
        }
        if (it == str.end()) break;
    }
    return tlist;
}

} /* end of reader */
} /* end of isana */
