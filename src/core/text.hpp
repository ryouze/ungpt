/**
 * @file text.hpp
 *
 * @brief Modify text.
 */

#pragma once

#include <cstddef>  // for std::size_t
#include <string>   // for std::string
namespace core::text {

void remove_unwanted_characters(std::string &text);

[[nodiscard]] std::size_t count_words(const std::string &text);

[[nodiscard]] std::size_t count_characters(const std::string &text);

}  // namespace core::text
