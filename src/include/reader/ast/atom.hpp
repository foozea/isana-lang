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

#ifndef ISANA_READER_AST_ATOM_INCLUDED_
#define ISANA_READER_AST_ATOM_INCLUDED_

#include <string>

#include "reader/ast/node.hpp"

namespace isana
{
namespace reader
{

/* atom nodes.
 * These classes has a std::string as the value.
 * The values will be converted to appropriate data types when generate
 * IR code. */

/* integer node */
class integer : public node
{
  public:
    integer(const std::string&);
    integer(const std::string&, const bool);
    ~integer(void);

    const std::string str(void) const;
    llvm::Value *codegen(llvm::BasicBlock*) const;

  private:
    const std::string value_;
    const bool exact_;
};

/* rational number node */
class rational : public node
{
  public:
    rational(const std::string&);
    ~rational(void);

    const std::string str(void) const;
    llvm::Value *codegen(llvm::BasicBlock*) const;

  private:
    const integer numer_, denom_;
};

/* real number node */
class real : public node
{
  public:
    real(const std::string&);
    ~real(void);

    const std::string str(void) const;
    llvm::Value *codegen(llvm::BasicBlock*) const;

  private:
    std::string value_;
};

/* complex number node */
class complex : public node
{
  public:
    complex(const std::string&);
    ~complex(void);

    const std::string str(void) const;
    llvm::Value *codegen(llvm::BasicBlock*) const;

  public:
    real real_, image_;
};

/* boolean node */
class boolean : public node
{
  public:
    boolean(const std::string&);
    boolean(const bool);
    ~boolean(void);

    const std::string str(void) const;
    llvm::Value *codegen(llvm::BasicBlock*) const;

  private:
    const bool value_;
};

/* string node */
class string : public node
{
  public:
    string(const std::string&);
    ~string(void);

    const std::string str(void) const;
    llvm::Value *codegen(llvm::BasicBlock*) const;

  private:
    const std::string value_;
};

class function;

/* symbol node */
class symbol : public node
{
  public:
    symbol(const std::string&);
    ~symbol(void);

    const std::string str(void) const;
    llvm::Value *codegen(llvm::BasicBlock*) const;
    llvm::Value *quoted_codegen(llvm::BasicBlock*) const;

  private:
    const std::string value_;
    friend class function;
};

/* nil node */
class nil : public node
{
  public:
    nil(void);
    ~nil(void);

    const std::string str(void) const;
    llvm::Value *codegen(llvm::BasicBlock*) const;
};
} /* end of reader */
} /* end of isana */
#endif
