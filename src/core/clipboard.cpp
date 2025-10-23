/**
 * @file clipboard.cpp
 */

#include <string>  // for std::string

#include <SFML/System/String.hpp>
#include <SFML/Window/Clipboard.hpp>
#include <spdlog/spdlog.h>

#include "core/clipboard.hpp"

namespace core::clipboard {

std::string read_from_clipboard()
{
    // sf::Clipboard.getString() returns a sf::String (UTF-32)
    // We convert it to UTF-8 and then to raw std::string
    const sf::U8String utf8 = sf::Clipboard::getString().toUtf8();
    const std::string result = {utf8.cbegin(), utf8.cend()};

    SPDLOG_DEBUG("Read '{}' characters from clipboard", result.size());

    return result;
}

void write_to_clipboard(const std::string &text)
{
    // sf::String handles conversion from UTF-8 to UTF-32
    const sf::String utf32 = sf::String::fromUtf8(text.cbegin(), text.cend());
    sf::Clipboard::setString(utf32);

    SPDLOG_DEBUG("Wrote '{}' characters to clipboard", text.size());
}

}  // namespace core::clipboard
