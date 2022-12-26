
// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2022-2023 niXman (github dot nixman at pm dot me)
// This file is part of TinyArgs(github.com/niXman/tiny-args) project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ----------------------------------------------------------------------------

#include "tiny-args.hpp"

#include <iostream>
#include <string>
#include <cassert>

const char *k_fname = "1.txt";
const int   k_fsize = 1234;
const char  k_fmode = 'r';

/*************************************************************************************************/

// declaration of args-group with it's members
struct {
    TINY_ARGS_ARGUMENT(fname, std::string);
    TINY_ARGS_ARGUMENT(fsize, int);
    TINY_ARGS_ARGUMENT(fmode, char);
    TINY_ARGS_ARGUMENT(ipaddr, std::string);
} static const args;

/*************************************************************************************************/

// fname - required
// fsize - required
// fmode - optional

// our function which we want to relax =)
template<typename ...Args>
int process_file(Args && ...a) {
    // pack variadic-list into a tuple.
    auto tuple = std::make_tuple(std::forward<Args>(a)...);

    // get as required.
    // if the 'fmode' was not passed to this function - it will leads to compile-time error!
    auto fname = tinyargs::get_arg(tuple, args.fname);
    auto fsize = tinyargs::get_arg(tuple, args.fsize);

    // get as optional.
    // it the option was not passed to the function, then 'r' will be used for 'fmode'.
    auto fmode = tinyargs::get_arg(tuple, args.fmode, args.fmode = 'r');

    assert(fmode == k_fmode);
    assert(fname == k_fname);
    assert(fsize == k_fsize);

    return fmode;
}

/*************************************************************************************************/
// overloading example

template<typename ...Args>
TINYARGS_FUNCTION_ENABLE(decltype(args.ipaddr))
(int) overloaded(Args && .../*args*/) {
    return 0;
}

template<typename ...Args>
TINYARGS_FUNCTION_DISABLE(decltype(args.ipaddr))
(int) overloaded(Args && .../*args*/) {
    return 1;
}

/*************************************************************************************************/

// usage
int main() {

    // the order of the specified arguments doesn't matter!
    int r = process_file(
         args.fname = k_fname
        ,args.fsize = k_fsize
        ,args.fmode = k_fmode
    );
    assert(r == 'r');

    r = process_file(
         args.fname = k_fname
        ,args.fmode = k_fmode
        ,args.fsize = k_fsize
    );
    assert(r == 'r');

    r = process_file(
         args.fname = k_fname
        ,args.fsize = k_fsize
    );
    assert(r == 'r');

    assert(overloaded(args.fname = "") == 1);
    assert(overloaded(args.ipaddr = "") == 0);

    return r;
}
/*************************************************************************************************/
