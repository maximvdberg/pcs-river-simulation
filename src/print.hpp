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

/** Special character to identify a print argument must not be printed. */
constexpr char log_action_char = '\v';

/**
 * Print anything you want. Every argument, or 'word', will be converted to
 * a string (using `toString()`) and printed. Words
 * @tparam Args Wow an argument pack.
 * @see lsepr()
 * @see lend()
 * @see lflush()
 * @see toString()
 */
template<typename... Args>
void print( const Args&... to_print ) {
    const std::string args[] = {toString(to_print)...};
    std::string words[sizeof... (Args)];

    std::string sepr = " ", end = "\n";
    bool flush = false;

    // Read the provided arguments.
    size_t i = 0;
    for (const std::string& word : args) {
        if (word[0] == log_action_char) {
            switch(word[1]) {
            case 's': sepr = word.substr(2); break;
            case 'e': end = word.substr(2); break;
            case 'f': flush = true; break;
            case 'w': std::cout << std::setw(std::stoi(word.substr(2))); break;
            case 'l': std::cout << std::left; break;
            case 'r': std::cout << std::right; break;
            default: break;
            }
        }
        else words[i++] = word;
    }

    // Print the words.
    for (size_t j = 0; j < i; ++j) {
        if (j != 0 && !sepr.empty()) std::cout << sepr;
        std::cout << words[j];
    }

    // End, and flush if requested.
    std::cout << end;
    if (flush) std::cout << std::flush;
}


/**
 * Set the printing word seperator. The provided string will be placed
 * inbetween words when printed using an overture logging/print function.
 * If not specified spaces will be used.
 *
 * Usage: `print_func("print", "me", lsepr("-"))` prints "print-me".
 *
 * @param seperator The word seperator.
 * @see print()
 * @return An action string to be passed to a printing function.
 */
inline std::string lsepr( std::string seperator ) {
    return std::string({log_action_char, 's'}) + seperator;
}

/**
 * Set the printing line ender. This is the string that will be printed at
 * the end of print/logging function call. If not specified a newline (\\n)
 * will be used.
 * Usage: `print_func("print", "me", lend("~"))` --> "print me~"
 *
 * @param end The print ending string.
 * @see print()
 * @return An action string to be passed to a printing function.
 */
inline std::string lend( std::string end ) {
    return std::string({log_action_char, 'e'}) + end;
}

/**
 * Enable flushing after printing. Make sure that the buffer gets
 * flushed. By default the buffer does not get flushed.
 * Usage: `print_func("cool", lflush())`
 *
 * @see print()
 * @return An action string to be passed to a printing function.
 */
inline std::string lflush() {
    return std::string({log_action_char, 'f'});
}


/**
 * TODO
 * @see print()
 * @return An action string to be passed to a printing function.
 */
inline std::string lwidth( int width ) {
    return std::string({log_action_char, 'w'}) + toString(width);
}


/**
 * TODO
 * @see print()
 * @return An action string to be passed to a printing function.
 */
inline std::string lalignl() {
    return std::string({log_action_char, 'l'});
}

/**
 * TODO
 * @see print()
 * @return An action string to be passed to a printing function.
 */
inline std::string lalignr() {
    return std::string({log_action_char, 'r'});
}
