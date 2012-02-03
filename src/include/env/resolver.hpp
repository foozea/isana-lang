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

#ifndef ISANA_ENV_RESOLVER_INCLUDED_
#define ISANA_ENV_RESOLVER_INCLUDED_

#include <deque>
#include <map>
#include <string>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <llvm/Value.h>

namespace isana
{
namespace env
{

class scope;
typedef boost::shared_ptr<scope> scope_ref;

/* Scope class has a map of symbols and functions.
 * The interfaces are prepared for put/get a value using a key string. */
class scope
{
  public:
    typedef std::map<const std::string, llvm::Value*> value_type;
    typedef std::deque<scope_ref> deque;
    typedef boost::optional<llvm::Value*> maybe;

    /* Puts a value with key string to the map */
    void put(const std::string&, llvm::Value*);
    /* Gets a value from the map. If not exists, returns boost::none. */
    maybe get(const std::string&) const;

  private:
    value_type value_;
};

class resolver;
typedef boost::shared_ptr<resolver> resolver_ref;

/* Resolver is the manager of the scopes, global scope and local scopes.
 * This class enables add values to each scopes, and enables lookup values
 * already added (the lookup order is 1st: local, 2nd: global). */
class resolver
{
  public:
    /* Adds a new local scope to the deque. */
    void enter_scope(void);
    /* Removes the current local scope from the deque. */
    void exit_scope(void);
    /* Adds a value to the scope. */
    void add(const std::string&, llvm::Value*);
    /* Gets a value from the scopes. If not exists, returns boost::none. */ 
    scope::maybe lookup(const std::string&) const;
    /* Determines if a value is exist in current scope. */
    bool exists_in_current(const std::string&) const;

    static const resolver_ref &get_instance(void);

  private:
    resolver(void);
    resolver(const resolver&) {};

    /* Adds a value to the global scope. */
    void add_global(const std::string&, llvm::Value*);
    /* Adds a value to the current local scope. */
    void add_local(const std::string&, llvm::Value*);

    scope_ref global_;
    scope::deque local_;
    static resolver_ref instance_;
};

/* Gets a value from the scopes. If not exists, runtime_error occurs. */
llvm::Value *lookup(const std::string&);

/* Determines if a value is exist in current scope. */
bool exists_in_current(const std::string&);

} /* end of env */
} /* end of isana */
#endif
