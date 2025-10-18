/**
 * @file text.cpp
 */

#include <cstddef>  // for std::size_t
#include <string>   // for std::string

#include "SFML/System/Utf.hpp"

#include "core/text.hpp"

namespace core::text {

namespace {

void replace_all_occurrences(std::string &str,
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

void remove_unwanted_characters(std::string &text)
{
    using Pair = std::pair<std::string, std::string>;

    constexpr std::array<Pair, 6> replacements = {{
        // Single quotes
        {"‘", "'"},
        {"’", "'"},
        // Double quotes
        {"“", "\""},
        {"”", "\""},
        // Dashes
        {"—", "-"},
        // Dots
        {"…", "..."},
    }};

    for (const auto &pair : replacements) {
        replace_all_occurrences(text, pair.first, pair.second);
    }
}

std::size_t count_words(const std::string &text)
{
    std::size_t words = 0;
    bool in_word = false;
    for (const char c : text) {
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

std::size_t count_characters(const std::string &text)
{
    return sf::Utf8::count(text.cbegin(), text.cend());
}

}  // namespace core::text
