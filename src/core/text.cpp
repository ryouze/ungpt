/**
 * @file text.cpp
 */

#include <cstddef>  // for std::size_t
#include <string>   // for std::string
#include <utility>  // for std::pair

#include "SFML/System/Utf.hpp"

#include "core/text.hpp"

namespace core::text {

void remove_unwanted_characters(std::string &text)
{
    constexpr std::pair<std::string, std::string> replacements[] = {
        // Replace curly quotation marks with straight quotes
        {"\xE2\x80\x9C", "\""},  // U+201C left double quote
        {"\xE2\x80\x9D", "\""},  // U+201D right double quote
        {"\xE2\x80\x98", "'"},   // U+2018 left single quote
        {"\xE2\x80\x99", "'"},   // U+2019 right single quote (apostrophe)

        // Replace dashes and hyphens with a standard ASCII hyphen-minus
        {"\xE2\x80\x93", "-"},    // U+2013 en dash
        {"\xE2\x80\x94", "-"},    // U+2014 em dash
        {"\xE2\x80\x95", "-"},    // U+2015 horizontal bar
        {"\xE2\x80\x90", "-"},    // U+2010 hard hyphen
        {"\xE2\x80\x91", "-"},    // U+2011 non-breaking hyphen
        {"\xE2\x80\x92", "-"},    // U+2012 figure dash
        {"\xE2\x88\x92", "-"},    // U+2212 minus sign (math)
        {"\xE2\x80\xA6", "..."},  // U+2026 ellipsis

        // Replace non-standard spaces with a normal ASCII space
        {"\xC2\xA0", " "},      // U+00A0 non-breaking space
        {"\xE2\x80\xAF", " "},  // U+202F narrow no-break space
        {"\xE1\x9A\x80", " "},  // U+1680 Ogham space mark
        {"\xE2\x80\x80", " "},  // U+2000 en quad
        {"\xE2\x80\x81", " "},  // U+2001 em quad
        {"\xE2\x80\x82", " "},  // U+2002 en space
        {"\xE2\x80\x83", " "},  // U+2003 em space
        {"\xE2\x80\x84", " "},  // U+2004 three-per-em space
        {"\xE2\x80\x85", " "},  // U+2005 four-per-em space
        {"\xE2\x80\x86", " "},  // U+2006 six-per-em space
        {"\xE2\x80\x87", " "},  // U+2007 figure space
        {"\xE2\x80\x88", " "},  // U+2008 punctuation space
        {"\xE2\x80\x89", " "},  // U+2009 thin space
        {"\xE2\x80\x8A", " "},  // U+200A hair space
        {"\xE2\x81\x9F", " "},  // U+205F mathematical space
        {"\xE3\x80\x80", " "},  // U+3000 ideographic space

        // Remove zero-width and other invisible characters entirely
        {"\xE2\x80\x8B", ""},  // U+200B zero-width space
        {"\xE2\x80\x8C", ""},  // U+200C zero-width non-joiner
        {"\xE2\x80\x8D", ""},  // U+200D zero-width joiner
        {"\xE2\x81\xA0", ""},  // U+2060 word joiner (zero-width no-break)
        {"\xE2\x80\x8E", ""},  // U+200E left-to-right mark
        {"\xE2\x80\x8F", ""},  // U+200F right-to-left mark
        {"\xC2\xAD", ""},      // U+00AD soft hyphen (shy)
        {"\xEF\xBB\xBF", ""},  // U+FEFF zero-width no-break (BOM)

        // Replace miscellaneous symbols with ASCII equivalents
        {"\xC2\xB7", "*"},      // U+00B7 middle dot
        {"\xE2\x80\xA2", "*"},  // U+2022 bullet
        {"\xE2\x80\xA3", "*"},  // U+2023 triangular bullet
        {"\xE2\x96\xAA", "*"},  // U+25AA small square bullet
        {"\xE2\x8B\x85", "*"},  // U+22C5 dot operator
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
        }
    }
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
