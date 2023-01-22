
// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2022-2023 niXman (github dot nixman at pm dot me)
// This file is part of TinyArgs(github.com/niXman/named-args) project.
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

#include <named-args/named-args.hpp>

#include <string>
#include <cassert>
#include <cstring>

/*************************************************************************************************/

// ungrouped holders declaration
NAMEDARGS_ARG(file_name, std::string);
NAMEDARGS_ARG(file_mode, int);

// grouped holders declaration
struct {
    NAMEDARGS_ARG(file_name, std::string);
    NAMEDARGS_ARG(file_mode, int);
} const holders;

/*************************************************************************************************/

template<typename ...Args>
bool ungrouped_process_file(Args &&...a) {
    auto args = std::make_tuple(std::forward<Args>(a)...);
    auto name = namedargs::get(file_name, args);
    // the default value will be used if `file_mode` was not specified
    auto mode = namedargs::get(file_mode, file_mode = 'e', args);

    return !name.empty() && mode == 'e';
}

template<typename ...Args>
bool grouped_process_file(Args &&...a) {
    auto args = std::make_tuple(std::forward<Args>(a)...);
    auto name = namedargs::get(holders.file_name, args);
    // the default value will be used if `file_mode` was not specified
    auto mode = namedargs::get(holders.file_mode, holders.file_mode = 'e', args);

    return !name.empty() && mode == 'e';
}

/*************************************************************************************************/

int main(int /*argc*/, char **argv) {
    const char *exename = std::strrchr(argv[0], '/');
    exename = exename ? exename+1 : "ex-common";

    // ungrouped holders example
    bool ok0 = ungrouped_process_file(
         file_name = argv[0]
        ,file_mode = *exename
    );
    assert(ok0);

    // grouped holders example
    bool ok1 = grouped_process_file(
         holders.file_name = argv[0]
        ,holders.file_mode = *exename
    );
    assert(ok1);

    return ok0 && ok1;
}

/*************************************************************************************************/
