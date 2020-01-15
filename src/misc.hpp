/**
 * Miscellaneous header only functions.
 *
 * @file misc.hpp
 * @author Jurriaan van den Berg
 * @author Maxim van den Berg
 * @author Melvin Seitner
 * @date 11-01-2020
 */

#pragma once

#include <string>
#include <fstream>

#include "print.hpp"


static inline std::string loadFile( const std::string& path ) {
    std::ifstream file(path);
    std::stringstream buffer;
    if (file) {
        buffer << file.rdbuf();
        return buffer.str();
    }
    else {
        print(INFO_, "file", path, "does not exists!");
        return "";
    }
}
