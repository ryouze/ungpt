/**
 * @file clipboard.hpp
 *
 * @brief Read and write UTF-8 text from the system clipboard.
 */

#pragma once

#include <string>  // for std::string

namespace core::clipboard {

/**
 * @brief Return a string read from the system clipboard.
 *
 * @return UTF-8 string from the clipboard (e.g., "hello world").
 */
[[nodiscard]] std::string read_from_clipboard();

/**
 * @brief Write a string to the system clipboard.
 *
 * @param text UTF-8 string to write to the clipboard (e.g., "hello world").
 */
void write_to_clipboard(const std::string &text);

}  // namespace core::clipboard
