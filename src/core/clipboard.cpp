/**
 * @file clipboard.cpp
 */

#include <string>  // for std::string

#include <SFML/System/String.hpp>
#include <SFML/Window/Clipboard.hpp>

#include "core/clipboard.hpp"

namespace core::clipboard {

std::string read_from_clipboard()
{
    // sf::Clipboard.getString() returns a sf::String (UTF-32)
    // We convert it to UTF-8 and then to raw std::string
    const sf::U8String utf8 = sf::Clipboard::getString().toUtf8();
    return {utf8.cbegin(), utf8.cend()};
}

void write_to_clipboard(const std::string &utf8)
{
    // sf::String handles conversion from UTF-8 to UTF-32
    const sf::String text = sf::String::fromUtf8(utf8.cbegin(), utf8.cend());
    sf::Clipboard::setString(text);
}

}  // namespace core::clipboard
