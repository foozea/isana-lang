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

#ifndef ISANA_ENV_DICT_INCLUDED_
#define ISANA_ENV_DICT_INCLUDED_

#include <map>
#include <string>

#include <boost/function.hpp>
#include <boost/optional.hpp>

#include <llvm/Function.h>
#include <llvm/Value.h>

#include "reader/ast/node.hpp"

namespace isana
{
namespace env
{

using namespace llvm;

/* dict class has a map of dict-strings and functions that generates
 * LLVM-IR code. */
class dict
{
  public:
    typedef boost::function<Value*(BasicBlock*, const reader::node_list&)> handler;
    typedef std::map<const std::string, const handler> value_type;
    typedef boost::optional<handler> maybe;

    /* Puts a value with key string to the map */
    static void put(const std::string&, const handler&);
    /* Gets a value from the map. If not exists, returns boost::none. */
    static maybe get(const std::string&);

  private:
    static value_type value_;
};

} /* end of env */
} /* end of isana */
#endif
