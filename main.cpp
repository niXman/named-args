
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

#include <string>

/*************************************************************************************************/

struct {
    TINY_ARGS_ARGUMENT(fname, std::string);
    TINY_ARGS_ARGUMENT(fsize, int);
    TINY_ARGS_ARGUMENT(fmode, char);
} static const args;

/*************************************************************************************************/

// fname - required
// fsize - required
// fmode - optional

template<typename ...Args>
int process_file(Args && ...a) {
    auto tuple = std::make_tuple(std::forward<Args>(a)...);
    auto fname = tinyargs::get_arg(tuple, args.fname);
    auto fsize = tinyargs::get_arg(tuple, args.fsize);
    auto fmode = tinyargs::get_arg(tuple, args.fmode, args.fmode = 'r');

    return 0;
}

/*************************************************************************************************/

int main(int argc, char **argv) {
    int r = process_file(
         args.fname = argv[1]
        ,args.fsize = atoi(argv[2])
        ,args.fmode = argv[3][0]
    );

    return r;
}

/*************************************************************************************************/
