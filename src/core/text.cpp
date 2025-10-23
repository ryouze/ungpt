/**
 * @file text.cpp
 */

#include <cstddef>  // for std::size_t
#include <string>   // for std::string
#include <utility>  // for std::pair

#include <SFML/System/Utf.hpp>
#include <spdlog/spdlog.h>

#include "core/text.hpp"

namespace core::text {

void remove_unwanted_characters(std::string &text)
{
    // NOTE: This array cannot be `constexpr` because `std::string` is not a literal type in C++20 so GCC will reject it
    static const std::pair<std::string, std::string> replacements[] = {
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
        {"\u00AD", ""},  // U+00AD soft hyphen (shy)
        {"\ufeff", ""},  // U+FEFF zero-width no-break (BOM)

        // Replace miscellaneous symbols with ASCII equivalents
        {"·", "*"},  // U+00B7 middle dot
        {"•", "*"},  // U+2022 bullet
        {"‣", "*"},  // U+2023 triangular bullet
        {"▪", "*"},  // U+25AA small square bullet
        {"⋅", "*"},  // U+22C5 dot operator
    };

    for (const auto &[from, to] : replacements) {

        // Used for keeping track of where we are in the string as we perform replacements
        std::size_t pos = 0;

        // Continuously search the text for the next occurrence of the substring 'from', starting from the current position 'pos'
        // Stop when no more matches are found
        while ((pos = text.find(from, pos)) != std::string::npos) {

            // Replace the found substring starting at 'pos' with the replacement string 'to'
            // The number of characters replaced is equal to the size of the substring 'from'
            text.replace(pos, from.size(), to);

            // Move the search index forward by the length of the replacement text
            // This ensures that the next call to `find()` starts after the part of the string we just modified
            pos += to.size();

            SPDLOG_INFO("Replaced '{}' with '{}' at position {}", from, to, pos);
        }
    }

    SPDLOG_DEBUG("Normalized text, resulting length: {}", text.size());
}

std::size_t count_words(const std::string &text)
{
    std::size_t word_count = 0;

    // Used for keeping track of whether we are currently inside a word
    bool inside_word = false;

    for (const char character : text) {

        // Determine if the current character is a whitespace character
        const bool is_whitespace = (character == ' ' ||
                                    character == '\n' ||
                                    character == '\t' ||
                                    character == '\r' ||
                                    character == '\v' ||
                                    character == '\f');

        // If the current character is not whitespace and we were previously outside a word, then we just found the first character of a new word
        // We increment the word counter and mark that we are now inside a word
        if (!is_whitespace && !inside_word) {
            ++word_count;
            inside_word = true;
        }

        // Otherwise, if the current character is whitespace, we mark that we are no longer inside a word
        else if (is_whitespace) {
            inside_word = false;
        }
    }

    return word_count;
}

std::size_t count_characters(const std::string &text)
{
    return sf::Utf8::count(text.cbegin(), text.cend());
}

}  // namespace core::text
