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
    constexpr std::pair<std::string, std::string> replacements[] = {
        // Replace curly quotation marks with straight quotes
        {"“", "\""},  // U+201C left double quote
        {"”", "\""},  // U+201D right double quote
        {"‘", "'"},   // U+2018 left single quote
        {"’", "'"},   // U+2019 right single quote (apostrophe)

        // Replace dashes and hyphens with a standard ASCII hyphen-minus
        {"–", "-"},    // U+2013 en dash
        {"—", "-"},    // U+2014 em dash
        {"―", "-"},    // U+2015 horizontal bar
        {"‐", "-"},    // U+2010 hard hyphen
        {"‑", "-"},    // U+2011 non-breaking hyphen
        {"‒", "-"},    // U+2012 figure dash
        {"−", "-"},    // U+2212 minus sign (math)
        {"…", "..."},  // U+2026 ellipsis

        // Replace non-standard spaces with a normal ASCII space
        {" ", " "},   // U+00A0 non-breaking space
        {" ", " "},   // U+202F narrow no-break space
        {" ", " "},   // U+1680 Ogham space mark
        {" ", " "},   // U+2000 en quad
        {" ", " "},   // U+2001 em quad
        {" ", " "},   // U+2002 en space
        {" ", " "},   // U+2003 em space
        {" ", " "},   // U+2004 three-per-em space
        {" ", " "},   // U+2005 four-per-em space
        {" ", " "},   // U+2006 six-per-em space
        {" ", " "},   // U+2007 figure space
        {" ", " "},   // U+2008 punctuation space
        {" ", " "},   // U+2009 thin space
        {" ", " "},   // U+200A hair space
        {" ", " "},   // U+205F mathematical space
        {"　", " "},  // U+3000 ideographic space

        // Remove zero-width and other invisible characters entirely
        {"​", ""},     // U+200B zero-width space
        {"‌", ""},     // U+200C zero-width non-joiner
        {"‍", ""},     // U+200D zero-width joiner
        {"⁠", ""},     // U+2060 word joiner (zero-width no-break)
        {"\u200E", ""},  // U+200E left-to-right mark
        {"\u200F", ""},  // U+200F right-to-left mark
        {"\xad", ""},    // U+00AD soft hyphen (shy)
        {"\ufeff", ""},  // U+FEFF zero-width no-break (BOM)

        // Replace miscellaneous symbols with ASCII equivalents
        {"·", "*"},  // U+00B7 middle dot
        {"•", "*"},  // U+2022 bullet
        {"‣", "*"},  // U+2023 triangular bullet
        {"▪", "*"},  // U+25AA small square bullet
        {"⋅", "*"},  // U+22C5 dot operator
    };

    for (const auto &[from, to] : replacements) {
        replace_all_occurrences(text, from, to);
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
