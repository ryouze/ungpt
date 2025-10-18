/**
 * @file text.hpp
 *
 * @brief Process and analyze text.
 */

#pragma once

#include <cstddef>  // for std::size_t
#include <string>   // for std::string

namespace core::text {

/**
 * @brief Remove unwanted characters from the provided text in place.
 *
 * @param text String to modify in place (e.g., "hello world").
 */
void remove_unwanted_characters(std::string &text);

/**
 * @brief Count the number of words in the provided text.
 *
 * @param text String to analyze (e.g., "hello world")
 *
 * @return Number of words in the text (e.g., "2").
 */
[[nodiscard]] std::size_t count_words(const std::string &text);

/**
 * @brief Count the number of characters in the provided text.
 *
 * @param text String to analyze (e.g., "hello world").
 *
 * @return Number of characters in the text (e.g., "11").
 */
[[nodiscard]] std::size_t count_characters(const std::string &text);

}  // namespace core::text
