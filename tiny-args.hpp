
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

#ifndef __TYNI_ARGS__TINY_ARGS_HPP_INCLUDED
#define __TYNI_ARGS__TINY_ARGS_HPP_INCLUDED

#include <type_traits>
#include <tuple>

namespace tinyargs {
namespace details {

/*************************************************************************************************/

template<
     typename T
    ,typename A0
    ,typename A1 = void
    ,typename A2 = void
    ,typename A3 = void
    ,typename A4 = void
    ,typename A5 = void
    ,typename A6 = void
    ,typename A7 = void
    ,typename A8 = void
    ,typename A9 = void
>
struct contains: std::integral_constant<
    bool
    ,  std::is_same<T, A0>::value
    || std::is_same<T, A1>::value
    || std::is_same<T, A2>::value
    || std::is_same<T, A3>::value
    || std::is_same<T, A4>::value
    || std::is_same<T, A5>::value
    || std::is_same<T, A6>::value
    || std::is_same<T, A7>::value
    || std::is_same<T, A8>::value
    || std::is_same<T, A9>::value
>
{};

template<
     typename T
    ,typename A0
    ,typename A1 = void
    ,typename A2 = void
    ,typename A3 = void
    ,typename A4 = void
    ,typename A5 = void
    ,typename A6 = void
    ,typename A7 = void
    ,typename A8 = void
    ,typename A9 = void
>
struct position: std::integral_constant<
    std::size_t
    , (std::is_same<T, A0>::value ? 0 : 0)
    + (std::is_same<T, A1>::value ? 1 : 0)
    + (std::is_same<T, A2>::value ? 2 : 0)
    + (std::is_same<T, A3>::value ? 3 : 0)
    + (std::is_same<T, A4>::value ? 4 : 0)
    + (std::is_same<T, A5>::value ? 5 : 0)
    + (std::is_same<T, A6>::value ? 6 : 0)
    + (std::is_same<T, A7>::value ? 7 : 0)
    + (std::is_same<T, A8>::value ? 8 : 0)
    + (std::is_same<T, A9>::value ? 9 : 0)
>
{};

template<bool ok>
struct get_arg_impl;

template<>
struct get_arg_impl<true> {
    template<typename T, typename ...Args>
    static typename T::type get(std::tuple<Args...> &args) {
        constexpr auto idx = position<T, Args...>::value;
        return std::get<idx>(args).v;
    }

    template<typename T, typename ...Args>
    static typename T::type get(T &&/*v*/, std::tuple<Args...> &args) {
        constexpr auto idx = position<T, Args...>::value;
        return std::get<idx>(args).v;
    }
};

template<>
struct get_arg_impl<false> {
    template<typename T, typename ...Args>
    static typename T::type get(T &&v, std::tuple<Args...> &/*args*/) {
        return std::forward<T>(v).v;
    }
};

/*************************************************************************************************/

} // ns details

/*************************************************************************************************/

template<typename T, typename ...Args>
typename T::type get_arg(std::tuple<Args...> &args, const T &) {
    constexpr bool ok = details::contains<T, Args...>::value;
    static_assert(ok == true, "looks like that argument is required");
    return details::get_arg_impl<ok>::template get<T>(args);
}

template<typename T, typename ...Args>
typename T::type get_arg(std::tuple<Args...> &args, const T &, T &&def) {
    constexpr bool ok = details::contains<T, Args...>::value;
    return details::get_arg_impl<ok>::template get<T>(std::forward<T>(def), args);
}

/*************************************************************************************************/

#define TINYARGS_PARENTHESIS_MUST_BE_PLACED_AROUND_THE_RETURN_TYPE(...) __VA_ARGS__>::type
#define TINYARGS_FUNCTION_ENABLE(T) \
    typename std::enable_if< \
        ::tinyargs::details::contains<typename std::decay<T>::type, Args...>::value == true \
            ,TINYARGS_PARENTHESIS_MUST_BE_PLACED_AROUND_THE_RETURN_TYPE

#define TINYARGS_FUNCTION_DISABLE(T) \
    typename std::enable_if< \
::tinyargs::details::contains<typename std::decay<T>::type, Args...>::value == false \
            ,TINYARGS_PARENTHESIS_MUST_BE_PLACED_AROUND_THE_RETURN_TYPE

/*************************************************************************************************/

#define TINY_ARGS_ARGUMENT(name, type_) \
    struct name ## _t { \
        using type = type_; \
        type v; \
        template<typename T> \
        name ## _t operator= (T &&v) const \
        { return {std::forward<T>(v)}; }\
    } const name{};

/*************************************************************************************************/

} // ns tinyargs

#endif // __TYNI_ARGS__TINY_ARGS_HPP_INCLUDED
