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

#include <stdexcept>

#include <boost/optional.hpp>
#include <boost/xpressive/xpressive.hpp>

#include "env/dict.hpp"
#include "reader/ast/atom.hpp"
#include "reader/ast/list.hpp"
#include "reader/parser.hpp"
#include "runtime/object.hpp"

namespace
{
using namespace isana;
using namespace isana::reader;

/* The types of nodes are typedef of token types.
 * reffer: runtime/object.hpp */
typedef token node_type;

typedef boost::optional<node_ref> maybe_node;

/* Identifies the node type of a string.
 * Regular expressions are used to the decision. */
node_type identify(const std::string &str)
{
    using namespace boost::xpressive;
    sregex r;
    std::string pattern;
    r = sregex::compile("[-+]?(?:(?:(?:[1-9]+[0-9]*)|0)(?:\\.0+)?)");
    if (regex_match(str, r)) return integer_t;
    r = sregex::compile("[-+]?(?:(?:(?:[1-9]+[0-9]*)|0)\\/(?:[1-9]+[0-9]*))");
    if (regex_match(str, r)) return rational_t;
    pattern = "[-+]?(?:(?:(?:[1-9]+[0-9]*)|0)(?:\\.[0-9]+)?)?[-+]?(?:(?:(?:[1-9]+[0-9]*)|0)(?:\\.[0-9]+)?)i";
    r = sregex::compile(pattern);
    if (regex_match(str, r)) return complex_t;
    r = sregex::compile("[-+]?(?:(?:(?:[1-9]+[0-9]*)|0)(?:\\.[0-9]+)?)(?:[eE][-+]?[0-9]+)?");
    if (regex_match(str, r)) return real_t;
    if (str == "#t" || str == "#f") return boolean_t;

    r = sregex::compile("^\"[^\"]*?\"$");
    if (regex_match(str, r)) return string_t;
    return symbol_t;
}

bool is_exact(const std::string &str)
{
    using namespace boost::xpressive;
    sregex r = sregex::compile("[-+]?(?:(?:[1-9]+[0-9]*)|0)");
    if (regex_match(str, r)) return true;
    return false;
}

/* Returns a node object from a string. */
node_ref get_atom(const std::string &str)
{
    switch(identify(str)) {
        case(integer_t):
            str.find_first_of(".");
            return node_ref(
                    new integer(
                        str.substr(0, str.find_first_of(".")),
                        is_exact(str)));
        case(real_t):
            return node_ref(new real(str));
        case(rational_t):
            return node_ref(new rational(str));
        case(complex_t):
            return node_ref(new complex(str));
        case(boolean_t):
            return node_ref(new boolean(str));
        case(string_t):
            return node_ref(new string(str));
        case(symbol_t):
            return node_ref(new symbol(str));
        default:
            std::runtime_error("Unidentified node.");
    }
    return node_ref();
}

/* Returns a node object from the iterators of tokens.
 * This funciton will be recursively processed. */
maybe_node eat_token(token_list::const_iterator &cur,
                     token_list::const_iterator &end)
{
    if (cur == end)  return boost::none;
    if (*cur == ")") return boost::none;
    if (*cur == "'") {
        reader::list *l = new reader::list();
        l->add_node(node_ref(new symbol("quote")));
        maybe_node next = eat_token(++cur, end);
        if (next) l->add_node(*next);
        return node_ref(l);
    }
    if (*cur == "(") {
        reader::list *l = new reader::list();
        for (;;) {
            maybe_node next = eat_token(++cur, end);
            if (!next) break;
            l->add_node(*next);
        }
        return node_ref(l);
    }
    return get_atom(*cur);
}
}

namespace isana
{
namespace reader
{

node_list parse(const token_list& ts)
{
    node_list nl;
    token_list::const_iterator cur = ts.begin(), end = ts.end();
    while (cur != end) {
        nl.push_back(*eat_token(cur, end));
        if (cur != end) ++cur;
    }
    return nl;
}

} /* end of reader */
} /* end of isana */
