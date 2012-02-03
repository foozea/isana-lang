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
#include <sstream>

#include "reader/ast/atom.hpp"

namespace isana
{
namespace reader
{
/* integer node */
integer::integer(const std::string &val) : value_(val), exact_(true) {}
integer::integer(const std::string &val, const bool exa)
    : value_(val), exact_(exa) {}
integer::~integer(void) {}

const std::string integer::str(void) const
{
    return value_;
}
/* */

/* rational node */
rational::rational(const std::string &val)
    : numer_(val.substr(0, val.find_first_of("/"))),
      denom_(val.substr(val.find_first_of("/") + 1)) {}
rational::~rational(void) {}

const std::string rational::str(void) const
{
    return numer_.str() + "/" + denom_.str();
}
/* */

/* real node */
real::real(const std::string &val) : value_(val) {}
real::~real(void) {}

const std::string real::str(void) const
{
    return value_;
}
/* */

/* complex node */
complex::complex(const std::string &val)
    : real_("0"), image_("0")
{
    if (!val.empty()) {
        size_t pos = val.find_last_of("+");
        if (pos == std::string::npos) pos = val.find_last_of("-");

        std::string re = "", im = "";
        if (pos != std::string::npos) {
            re = val.substr(0, pos);
            im = val.substr(pos);
        } else {
            im = val;
        }
        im.erase(--im.end());
        real_ = real(re);
        image_ = real(im);
    }
}
complex::~complex(void) {}

const std::string complex::str(void) const
{
    const std::string re = real_.str(), im = image_.str() + "i";
    return re + ((!im.empty() && im.at(0)) != '-' ? "+" : "") + im;
}
/* */

/* boolean node */
boolean::boolean(const std::string &val) : value_(val == "#t" ? true : false) {}
boolean::boolean(const bool val) : value_(val) {}
boolean::~boolean(void) {}

const std::string boolean::str(void) const
{
    return value_ ? "#t" : "#f";
}
/* */

/* string */
string::string(const std::string &val) : value_(val) {}
string::~string(void) {}

const std::string string::str(void) const
{
    return value_;
}
/* */

/* symbol node */
symbol::symbol(const std::string &val) : value_(val) {}
symbol::~symbol(void) {}

const std::string symbol::str(void) const
{
    return value_;
}
/* */

/* nil node */
nil::nil(void) {}
nil::~nil(void) {}

const std::string nil::str(void) const
{
    return "nil";
}
/* */
} /* end of reader */
} /* end of isana */
