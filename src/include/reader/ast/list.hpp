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

#ifndef ISANA_READER_AST_LIST_INCLUDED_
#define ISANA_READER_AST_LIST_INCLUDED_

#include <string>

#include "reader/ast/node.hpp"

namespace isana
{
namespace reader
{

/* unquoted list node.
 * This class has a list of nodes. */
class list : public node
{
  public:
    ~list(void);

    /* Adds a node to the list. */
    void add_node(const node_ref&);
    /* Adds nodes to the list. */
    void add_nodes(const node_list&);
    /* Gets the first node of the list. */
    const node_ref &first(void) const;
    /* Gets the node list that excludes the first node. */
    const node_list rest(void) const;
    /* Gets the node list. */
    const node_list &get_nodes(void) const;

    const std::string str(void) const;
    llvm::Value *codegen(llvm::BasicBlock*) const;

  private:
    node_list nodes_;
};
} /* end of reader */
} /* end of isana */
#endif
