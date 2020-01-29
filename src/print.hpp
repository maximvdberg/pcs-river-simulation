/**
 * Here generic template toString and print functions are supplied, for easy
 * printing of any data type. This file was not orginally created for this
 * project, and is only used here for convenience.
 *
 * @file print.hpp
 * @author Jurriaan van den Berg
 * @copyright? Jurriaan van den Berg 2020 ?
 */

#pragma once

#include <sstream>
#include <iostream>
#include <typeinfo>
#include <iomanip>


template <typename...>
using void_t = void;

template <typename T, typename = void>
struct isIterable : std::false_type {};
template <typename T>
struct isIterable<T, void_t<decltype ((*(T*)(0)).begin()),
                      decltype ((*(T*)(0)).end())>> : std::true_type {};

template <typename T, typename = void>
struct isStringable : std::false_type {};
template <typename T>
struct isStringable<T, void_t<decltype(operator<<((*(std::ostream*)(0)),
                                                  (*(T*)(0))))>> : std::true_type {};

template <typename T>
constexpr bool printIterable = !isStringable<T>::value && isIterable<T>::value;

// Generic toString function.
template <typename T, typename std::enable_if_t<!printIterable<T>, int> = 0>
std::string toString( const T& value ) {
    std::stringstream ss;
    ss << std::boolalpha << value;
    if (typeid(T) == typeid(double) && ss.str().find('.') == std::string::npos) {
        return ss.str() + ".0";
    }
    return ss.str();
}

// Specialization of toString for std::pair.
template <typename T, typename U>
std::string toString( const std::pair<T, U>& pair ) {
    std::stringstream ss;
    ss << '[' << toString(pair.first) << ": "
       << toString(pair.second) << ']';
    return ss.str();
}

// Specialization of toString  for iterables.
template <typename T, typename std::enable_if_t<printIterable<T>, int> = 0>
std::string toString( const T& it ) {
    std::stringstream ss;
    ss << '{';
    int i = 0;
    for (const auto& v : it) {
        if (i++) ss << ", ";
        ss << toString(v);
    }
    ss << '}';
    return ss.str();
}

/** A macro to add to logging functions, so that useful info, like the file
 * and line number, will be remembered. */
#define INFO_ std::string(__func__) + "() at " + __FILE__ + ":" + toString(__LINE__) + ":"


/**
 * Print anything you want. Every argument, or 'word', will be converted to
 * a string (using `toString()`) and printed. Words
 * @tparam Args Wow an argument pack.
 * @see toString()
 */
template<typename... Args>
void print( const Args&... to_print ) {
    const std::string words[] = {toString(to_print)...};

    std::string sepr = " ", end = "\n";

    // Print the words.
    for (size_t i = 0; i < sizeof...(to_print); ++i) {
        if (i != 0 && !sepr.empty()) std::cout << sepr;
        std::cout << words[i];
    }

    // End the line.
    std::cout << end;
}

