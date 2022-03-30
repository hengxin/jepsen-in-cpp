#ifndef UTILS_BASE64_H
#define UTILS_BASE64_H

#include <cassert>
#include <cctype>
#include <limits>
#include <stdexcept>
#include <string>

// refer to https://stackoverflow.com/questions/5288076/base64-encoding-and-decoding-with-openssl
class Base64Coder {
public:
    static std::string base64_encode(const ::std::string& bindata);
    static std::string base64_decode(const ::std::string& ascdata);
};


#endif