/**
 * @file text.cpp
 */

#include <cstddef>  // for std::size_t
#include <string>   // for std::string

#include "SFML/System/Utf.hpp"

#include "core/text.hpp"

namespace core::text {

namespace {

void replace_all_in_place(std::string &str,
                          const std::string &from,
                          const std::string &to)
{
    if (from.empty()) {
        return;
    }

    std::size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos) {
        str.replace(pos, from.size(), to);
        pos += to.size();
    }
}

}  // namespace

void normalize_text_in_place(std::string &utf8)
{
    replace_all_in_place(utf8, std::string("\xE2\x80\x9C"), "\"");
    replace_all_in_place(utf8, std::string("\xE2\x80\x9D"), "\"");
    replace_all_in_place(utf8, std::string("\xE2\x80\x98"), "'");
    replace_all_in_place(utf8, std::string("\xE2\x80\x99"), "'");
    replace_all_in_place(utf8, std::string("\xE2\x80\x93"), "-");
    replace_all_in_place(utf8, std::string("\xE2\x80\x94"), "-");
    replace_all_in_place(utf8, std::string("\xE2\x80\xA6"), "...");
    replace_all_in_place(utf8, std::string("\xC2\xA0"), " ");
    replace_all_in_place(utf8, std::string("\xE2\x80\x8B"), "");
}

std::size_t count_words_ascii_whitespace(const std::string &str)
{
    std::size_t words = 0;
    bool in_word = false;
    for (const char c : str) {
        const bool ws = (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\v' || c == '\f');
        if (!ws && !in_word) {
            in_word = true;
            ++words;
        }
        else if (ws) {
            in_word = false;
        }
    }
    return words;
}

std::size_t count_codepoints_utf8(const std::string &str)
{
    return sf::Utf8::count(str.cbegin(), str.cend());
}

}  // namespace core::text
