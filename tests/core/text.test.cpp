/**
 * @file text.test.cpp
 */

#include <cstddef>  // for std::size_t
#include <string>   // for std::string
#include <utility>  // for std::pair

#include <snitch/snitch.hpp>

#include "core/text.hpp"

TEST_CASE("remove_unwanted_characters replaces Unicode with ASCII", "[src][core][text.hpp]")
{
    static const std::pair<std::string, std::string> test_cases[] = {
        {"In 1969—long before ‘likes’, ‘tweets’, or ‘hashtags’—ARPANET connected four machines; today, billions exchange data across continents… “History repeats itself, only faster.”",
         "In 1969-long before 'likes', 'tweets', or 'hashtags'-ARPANET connected four machines; today, billions exchange data across continents... \"History repeats itself, only faster.\""},
        {"Email​adresse -⁠Protokoll•", "Emailadresse -Protokoll*"},  // Zero-width space, non-breaking space, and word joiner
        {"“Zażółć”—powiedział “gęślą”", "\"Zażółć\"-powiedział \"gęślą\""},
        {"“Grüße”—und “IA”… alles gut?", "\"Grüße\"-und \"IA\"... alles gut?"},
        {"﻿Agenda⁠plan", "Agendaplan"},  // Begins with a byte order mark and a word joiner
        {"Grüße aus Berlin und Hamburg", "Grüße aus Berlin und Hamburg"},
        {"Plan​⁠‍A", "PlanA"},
        {"Aufgaben: • Erste ‣ Zweite · Dritte ⋅ Vierte ▪ Fünfte", "Aufgaben: * Erste * Zweite * Dritte * Vierte * Fünfte"},
        {"Softer­hyphen bleibt weg", "Softerhyphen bleibt weg"},
        {"Minus − gleich – Strich — Ende", "Minus - gleich - Strich - Ende"},
    };

    for (const auto &[input_text, expected_text] : test_cases) {
        // `remove_unwanted_characters` modifies a string in place so we need a mutable copy
        std::string modified_text = input_text;
        core::text::remove_unwanted_characters(modified_text);
        CHECK(modified_text == expected_text);
    }
}

TEST_CASE("count_words returns correct word count", "[src][core][text.hpp]")
{
    static const std::pair<std::string, std::size_t> test_cases[] = {
        {"", 0},
        {"hello", 1},
        {"hello world", 2},
        {"  hello   world  ", 2},
        {"hello\nworld\ttest", 3},
        {"multiple\r\nlines with\vvaried\fforms", 5},
        {"Grüße aus Berlin", 3},
        {"Zażółć gęślą jaźń", 3},
        {"Quoted \"words\" stay one piece", 5},
        {"punctuation,does-not break again", 3},
        {" \n\t\r", 0},
        {"Zażółć   gęślą  \n jaźń", 3},
        {"Grüße,\tBerlin!\nFreunde", 3},
        {"Deutsch-polnische Zusammenarbeit", 2},
        {"Emailadresse -Protokoll erneut", 3},
        {"„Zażółć” gęślą jaźń.\n\n„Grüße“ überall!", 5},
    };

    for (const auto &[input_text, expected_count] : test_cases) {
        CHECK(core::text::count_words(input_text) == expected_count);
    }
}

TEST_CASE("count_characters returns correct character count", "[src][core][text.hpp]")
{
    static const std::pair<std::string, std::size_t> test_cases[] = {
        {"", 0},
        {"hello", 5},
        {"hello world", 11},
        {"café", 4},
        {"Grüße", 5},
        {"Zażółć", 6},
        {"emoji 😀", 7},
        {"​zero width mark", 16},  // Leading zero-width space should not count as a visible character
        {"Zażółć gęślą jaźń", 17},
        {"line\nbreak", 10},
        {"👍🏻", 2},
        {"👩‍💻", 3},
        {"Ȧ", 2},
        {"🇵🇱", 2},
    };

    for (const auto &[input_text, expected_count] : test_cases) {
        CHECK(core::text::count_characters(input_text) == expected_count);
    }
}
