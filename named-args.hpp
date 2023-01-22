
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

#ifndef __NAMED_ARGS__NAMED_ARGS_HPP_INCLUDED
#define __NAMED_ARGS__NAMED_ARGS_HPP_INCLUDED

#include <type_traits>
#include <tuple>

namespace namedargs {
namespace details {

/*************************************************************************************************/
// tools

template<typename T, T v>
struct int_const {
    static constexpr T value = v;
};

template<typename ...Types>
struct types_list;

template<typename T>
struct identity_type {
    using type = T;
};

template<typename ...Args>
struct is_tuple
    :std::false_type
{};

template<typename... Args>
struct is_tuple<std::tuple<Args...>>
    :std::true_type
{};

template<typename Arg0, typename... Args>
struct is_tuple<Arg0, Args...>
    :std::false_type
{};

template<typename Ret, typename ...T>
struct enable_if_tuple
    :std::enable_if<is_tuple<T...>::value == true, Ret>
{};

template<typename Ret, typename ...T>
struct disable_if_tuple
    :std::enable_if<is_tuple<T...>::value == false, Ret>
{};

/*************************************************************************************************/
// position

template<typename T, int I, typename ...Args>
struct position_impl;

template<typename T, int I>
struct position_impl<T, I>
    :std::integral_constant<int, -1>
{};

template<typename T, int I, typename Arg0, typename ...Args>
struct position_impl<T, I, Arg0, Args...>
    :std::integral_constant<
         int
        ,std::is_same<T, Arg0>::value
            ? I
            : position_impl<T, I+1, Args...>::value
    >
{};

template<typename T, typename ...Args>
struct position;

template<typename T, typename ...Args>
struct position
    :position_impl<typename std::decay<T>::type, 0, Args...>
{};

template<typename T, typename ...Args>
struct position<T, std::tuple<Args...>>
    :position_impl<typename std::decay<T>::type, 0, Args...>
{};

/*************************************************************************************************/
// tuple element

template<std::size_t I, typename ...Args>
struct tuple_element;

template<std::size_t I, typename T0, typename ...Args>
struct tuple_element<I, T0, Args...> {
    using type = typename std::conditional<
         I == 0
        ,identity_type<T0>
        ,tuple_element<I-1, Args...>
    >::type::type;
};

template<std::size_t I, typename ...Args>
struct tuple_element<I, std::tuple<Args...>>
    :tuple_element<I, Args...>
{};

/*************************************************************************************************/
// multi-contains

template<bool OK, typename T0, typename ...Args>
struct multi_contains_impl;

template<typename T0, typename ...Types, typename ...Args>
struct multi_contains_impl<false, types_list<T0, Types...>, Args...>
    : multi_contains_impl<
         position<T0, Args...>::value != -1
        ,types_list<Types...>
        ,Args...
    >::type
{};

template<typename ...Args>
struct multi_contains_impl<false, types_list<>, Args...>
    : std::false_type
{};

template<typename ...Types, typename ...Args>
struct multi_contains_impl<true, types_list<Types...>, Args...>
    : std::true_type
{};

template<typename T, typename ...Args>
struct multi_contains;

template<typename T0, typename ...Types, typename ...Args>
struct multi_contains<types_list<T0, Types...>, Args...>
    : multi_contains_impl<
          position<T0, Args...>::value != -1
        ,types_list<T0, Types...>
        ,Args...
    >::type
{};

/*************************************************************************************************/
// get() for tuple

template<bool ok>
struct get_arg_impl_tuple;

template<>
struct get_arg_impl_tuple<true> {
    template<
         typename K
        ,typename Tuple
        ,std::size_t I
    >
    static typename K::type get(const K &, int_const<std::size_t, I>, Tuple &&tuple) {
        return std::get<I>(std::forward<Tuple>(tuple)).v;
    }
    template<
         typename K
        ,typename Def
        ,typename Tuple
        ,std::size_t I
    >
    static typename K::type get(const K &, Def &&, int_const<std::size_t, I>, Tuple &&tuple) {
        using Elem = typename tuple_element<I, typename std::decay<Tuple>::type>::type;
        using T = typename Elem::type;
        using cast_to = typename std::conditional<
             std::is_copy_assignable<T>::value || std::is_copy_assignable<T>::value
            ,const T &
            ,T &&
        >::type;
        return static_cast<cast_to>(std::get<I>(std::forward<Tuple>(tuple)).v);
    }
};

template<>
struct get_arg_impl_tuple<false> {
    template<
         typename K
        ,typename Def
        ,typename Tuple
        ,std::size_t I
    >
    static typename K::type get(const K &, Def &&def, int_const<std::size_t, I>, Tuple &&/*tuple*/) {
        return std::forward<typename Def::type>(std::forward<Def>(def).v);
    }
};

/*************************************************************************************************/
// get() for variadic

template<bool ok>
struct get_arg_impl_variadic;

template<>
struct get_arg_impl_variadic<true> {
    template<
         typename K
        ,typename Def
        ,typename Arg0
        ,typename ...Args
    >
    static typename std::enable_if<!std::is_same<K, Arg0>::value, typename K::type>::type
    get(const K &unused0, Def &&unused1, Arg0 &&/*arg0*/, Args && ...args) {
        return get(unused0, std::forward<Def>(unused1), std::forward<Args>(args)...);
    }
    template<
         typename K
        ,typename Def
        ,typename Arg0
        ,typename ...Args
    >
    static typename std::enable_if<std::is_same<K, Arg0>::value, typename K::type>::type
    get(const K &/*unused0*/, Def &&/*unused1*/, Arg0 &&arg0, Args && .../*args*/) {
        return std::forward<Arg0>(arg0).v;
    }
};

template<>
struct get_arg_impl_variadic<false> {
    template<
         typename K
        ,typename Def
        ,typename ...Args
    >
    static typename Def::type get(const K &/*k*/, Def &&def, Args &&.../*args*/) {
        return std::forward<Def>(def).v;
    }
};

/*************************************************************************************************/

} // ns details

/*************************************************************************************************/
// for tuple

template<
     typename K
    ,typename Tuple
    ,typename UCT = typename std::decay<Tuple>::type
>
typename details::enable_if_tuple<typename K::type, UCT>::type
get(const K &k, Tuple &&tuple) {
    constexpr int idx = details::position<K, UCT>::value;
    static_assert(idx != -1, "looks like that argument is required");
    return details::get_arg_impl_tuple<idx != -1>::template get(
        k, details::int_const<std::size_t, idx>{}, std::forward<Tuple>(tuple));
}

template<
     typename K
    ,typename Def
    ,typename Tuple
    ,typename UCT = typename std::decay<Tuple>::type
>
typename details::enable_if_tuple<typename K::type, UCT>::type
get(const K &k, Def &&def, Tuple &&tuple) {
    constexpr int idx = details::position<K, UCT>::value;
    return details::get_arg_impl_tuple<idx != -1>::template get(
        k, std::forward<Def>(def), details::int_const<std::size_t, idx>{}
            ,std::forward<Tuple>(tuple));
}

/*************************************************************************************************/
// for variadic

template<typename K, typename ...Args>
typename details::disable_if_tuple<typename K::type, Args...>::type
get(const K &k, Args && ...args) {
    constexpr int idx = details::position<K, Args...>::value;
    static_assert(idx != -1, "looks like that argument is required");
    return details::get_arg_impl_variadic<idx != -1>::template get(
        k, K{}, std::forward<Args>(args)...);
}

template<typename K, typename Def, typename ...Args>
typename details::disable_if_tuple<typename K::type, Args...>::type
get(const K &k, Def &&def, Args && ...args) {
    constexpr int idx = details::position<K, Args...>::value;
    return details::get_arg_impl_variadic<idx != -1>::template get(
        k, std::forward<Def>(def), std::forward<Args>(args)...);
}

/*************************************************************************************************/

#define NAMEDARGS_PARENTHESIS_MUST_BE_PLACED_AROUND_THE_RETURN_TYPE(...) __VA_ARGS__>::type

#define NAMEDARGS_FUNC_ENABLE(VARIADIC, ...) \
    typename std::enable_if< \
        ::namedargs::details::multi_contains<::namedargs::details::types_list<__VA_ARGS__> \
            ,VARIADIC>::value == true \
                ,NAMEDARGS_PARENTHESIS_MUST_BE_PLACED_AROUND_THE_RETURN_TYPE

#define NAMEDARGS_FUNC_DISABLE(VARIADIC, ...) \
    typename std::enable_if< \
        ::namedargs::details::multi_contains<::namedargs::details::types_list<__VA_ARGS__> \
            ,VARIADIC>::value == false \
                ,NAMEDARGS_PARENTHESIS_MUST_BE_PLACED_AROUND_THE_RETURN_TYPE

/*************************************************************************************************/

#define NAMEDARGS_ARG(name, type_) \
    struct name ## _t { \
        using type = type_; \
        type v; \
        template<typename U> \
        name ## _t operator= (U &&u) const \
        { return {std::forward<U>(u)}; }\
    } const name{};

/*************************************************************************************************/

} // ns namedargs

#endif // __NAMED_ARGS__NAMED_ARGS_HPP_INCLUDED
