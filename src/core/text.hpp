/**
 * @file text.hpp
 *
 * @brief Modify text.
 */

#pragma once

#include <cstddef>  // for std::size_t
#include <string>   // for std::string
namespace core::text {

void normalize_text_in_place(std::string &utf8);

[[nodiscard]] std::size_t count_words_ascii_whitespace(const std::string &str);

[[nodiscard]] std::size_t count_codepoints_utf8(const std::string &str);

}  // namespace core::text
