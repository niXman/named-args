
// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2022-2023 niXman (github dot nixman at pm dot me)
// This file is part of NamedArgs(github.com/niXman/named-args) project.
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

#include <iostream>
#include <string>
#include <cassert>

const char *k_fname = "1.txt";
const int   k_fsize = 1234;
const char  k_fmode = 'r';
const char *k_ipaddr = "192.168.1.101";

/*************************************************************************************************/

struct noncopyable {
    noncopyable(const noncopyable &) = delete;
    noncopyable& operator= (const noncopyable &) = delete;
    noncopyable(noncopyable &&) = default;
    noncopyable& operator= (noncopyable &&) = default;

    using type = std::string;
    type v;

    template<typename U>
    noncopyable operator=(U &&u) const {
        return {std::forward<U>(u)};
    }
};

// declaration of args-group with it's members
struct {
    NAMEDARGS_ARG(fname, std::string);
    NAMEDARGS_ARG(fsize, int);
    NAMEDARGS_ARG(fmode, char);
    NAMEDARGS_ARG(ipaddr, noncopyable);
} const args;

/*************************************************************************************************/

// fname - required
// fsize - required
// fmode - optional

// variadic packed as tuple
template<typename ...Args>
int process_file_0(Args && ...a) {
    // pack variadic-list into a tuple.
    auto tuple = std::make_tuple(std::forward<Args>(a)...);

    // get as required.
    // if the 'fmode' was not passed to this function - it will leads to compile-time error!
    auto fname = namedargs::get(args.fname, tuple);
    auto fsize = namedargs::get(args.fsize, tuple);

    // get as optional.
    // it the option was not passed to the function, then 'r' will be used for 'fmode'.
    auto fmode = namedargs::get(args.fmode, args.fmode = 'w', tuple);

    assert(fname == k_fname);
    assert(fsize == k_fsize);
    if ( namedargs::details::position<decltype(args.fmode), Args...>::value != -1 ) {
        assert(fmode == k_fmode);
    } else {
        assert(fmode == 'w');
    }

    return fmode;
}

// fname - required
// fsize - required
// fmode - optional

// variadic NOT packed as tuple
template<typename ...Args>
int process_file_1(Args && ...a) {
    // get as required.
    // if the 'fmode' was not passed to this function - it will leads to compile-time error!
    auto fname = namedargs::get(args.fname, std::forward<Args>(a)...);
    auto fsize = namedargs::get(args.fsize, std::forward<Args>(a)...);

    // get as optional.
    // it the option was not passed to the function, then 'r' will be used for 'fmode'.
    auto fmode = namedargs::get(args.fmode, args.fmode = 'w', std::forward<Args>(a)...);

    assert(fname == k_fname);
    assert(fsize == k_fsize);
    if ( namedargs::details::position<decltype(args.fmode), Args...>::value != -1 ) {
        assert(fmode == k_fmode);
    } else {
        assert(fmode == 'w');
    }

    return fmode;
}

/*************************************************************************************************/
// non-copyable test

// fname  - required
// fmode  - optional
// ipaddr - optional

// variadic packed as tuple
template<typename ...Args>
noncopyable process_file_2(Args && ...a) {
    // pack variadic-list into a tuple.
    auto tuple = std::make_tuple(std::forward<Args>(a)...);

    // get as required.
    // if the 'fmode' was not passed to this function - it will leads to compile-time error!
    auto fname = namedargs::get(args.fname, tuple);

    // get as optional.
    // it the option was not passed to the function, then 'r' will be used for 'fmode'.
    auto fmode = namedargs::get(args.fmode, args.fmode = 'w',  tuple);

    // the same as for above, but for non-copyable-but-movable  type
    auto ipaddr = namedargs::get(args.ipaddr, args.ipaddr = "192.168.1.102", tuple);

    assert(fname == k_fname);
    if ( namedargs::details::position<decltype(args.fmode), Args...>::value != -1 ) {
        assert(fmode == k_fmode);
    } else {
        assert(fmode == 'w');
    }
    if ( namedargs::details::position<decltype(args.ipaddr), Args...>::value != -1 ) {
        assert(ipaddr.v == k_ipaddr);
    } else {
        assert(ipaddr.v == "192.168.1.102");
    }

    return ipaddr;
}

/*************************************************************************************************/
// overloading example

template<typename ...Args>
NAMEDARGS_FUNC_ENABLE(Args..., args.ipaddr)
(int) overloaded(Args && .../*args*/) {
    return 0;
}

template<typename ...Args>
NAMEDARGS_FUNC_DISABLE(Args..., args.ipaddr)
(int) overloaded(Args && .../*args*/) {
    return 1;
}

/*************************************************************************************************/

template<std::size_t I, typename T, typename ...Args>
constexpr bool tuple_elem_equal(Args && ...) {
    using elem = typename namedargs::details::tuple_element<I, Args...>::type;
    return std::is_same<T, elem>::value;
}

/*************************************************************************************************/

// usage
int main(int argc, char **) {
    static_assert(namedargs::details::position<int>::value == -1, "");
    static_assert(namedargs::details::position<int, char, long>::value == -1, "");
    static_assert(namedargs::details::position<int, char, int>::value == 1, "");
    static_assert(namedargs::details::position<int, int, char>::value == 0, "");

    using types = namedargs::details::types_list<int, char, float, long>;
    constexpr bool ok0 = namedargs::details::multi_contains<types, double, short, int>::value;
    static_assert(ok0, "");

    constexpr bool ok1 = namedargs::details::multi_contains<types, double, short>::value;
    static_assert(!ok1, "");

    using tuple = std::tuple<char, int &, const double, const long &>;
    using Elem0 = namedargs::details::tuple_element<0, tuple>::type;
    static_assert(std::is_same<char, Elem0>::value, "");
    using Elem1 = namedargs::details::tuple_element<1, tuple>::type;
    static_assert(std::is_same<int &, Elem1>::value, "");
    using Elem2 = namedargs::details::tuple_element<2, tuple>::type;
    static_assert(std::is_same<const double, Elem2>::value, "");
    using Elem3 = namedargs::details::tuple_element<3, tuple>::type;
    static_assert(std::is_same<const long &, Elem3>::value, "");

    static_assert(tuple_elem_equal<0, int>(3) == true, "");
    static_assert(tuple_elem_equal<0, int &>(argc) == true, "");
    static_assert(tuple_elem_equal<0, const int &>(static_cast<const int &>(argc)) == true, "");
    static_assert(tuple_elem_equal<0, int>(std::move(argc)) == true, "");
    static_assert(tuple_elem_equal<0, int>(std::forward<int>(argc)) == true, "");

// variadic packed as tuple
    // the order of the specified arguments doesn't matter!
    int r = process_file_0(
         args.fname = k_fname
        ,args.fsize = k_fsize
        ,args.fmode = k_fmode
    );
    assert(r == 'r');

    r = process_file_0(
         args.fname = k_fname
        ,args.fmode = k_fmode
        ,args.fsize = k_fsize
    );
    assert(r == 'r');

    r = process_file_0(
         args.fname = k_fname
        ,args.fsize = k_fsize
    );
    assert(r == 'w');

    r = overloaded(args.fname = "");
    assert(r == 1);
    r = overloaded(args.ipaddr = "");
    assert(r == 0);

// variadic NOT packed as tuple

    r = process_file_1(
         args.fname = k_fname
        ,args.fsize = k_fsize
        ,args.fmode = k_fmode
    );
    assert(r == 'r');

    r = process_file_1(
         args.fname = k_fname
        ,args.fmode = k_fmode
        ,args.fsize = k_fsize
    );
    assert(r == 'r');

    r = process_file_1(
         args.fname = k_fname
        ,args.fsize = k_fsize
    );
    assert(r == 'w');

// non-copyable test

    auto nc0 = process_file_2(
         args.fname = k_fname
        ,args.ipaddr= k_ipaddr
        ,args.fmode = k_fmode
    );
    assert(nc0.v == k_ipaddr);

    auto nc1 = process_file_2(
         args.fname = k_fname
        ,args.fmode = k_fmode
        ,args.ipaddr= k_ipaddr
    );
    assert(nc1.v == k_ipaddr);

    auto nc2 = process_file_2(
         args.fname = k_fname
        ,args.fmode = k_fmode
    );
    assert(nc2.v == "192.168.1.102");

    return r;
}

/*************************************************************************************************/
