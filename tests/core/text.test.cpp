/**
 * @file text.test.cpp
 */

#include <snitch/snitch.hpp>

#include "core/text.hpp"

constexpr std::string_view sample_text = R"(In 1969—long before ‘likes’, ‘tweets’, or ‘hashtags’—ARPANET connected four machines; today, billions exchange data across continents… “History repeats itself, only faster.”)";

TEST_CASE("remove_unwanted_characters replaces Unicode with ASCII", "[src][core][text.hpp]")
{
    // `remove_unwanted_characters` modifies a string in place so we need a mutable copy
    std::string text = std::string{sample_text};
    core::text::remove_unwanted_characters(text);
    CHECK(text == R"(In 1969-long before 'likes', 'tweets', or 'hashtags'-ARPANET connected four machines; today, billions exchange data across continents... "History repeats itself, only faster.")");
}

TEST_CASE("count_words returns correct word count", "[src][core][text.hpp]")
{
    CHECK(core::text::count_words("") == 0);
    CHECK(core::text::count_words("hello") == 1);
    CHECK(core::text::count_words("hello world") == 2);
    CHECK(core::text::count_words("  hello   world  ") == 2);
    CHECK(core::text::count_words("hello\nworld\ttest") == 3);
}

TEST_CASE("count_characters returns correct character count", "[src][core][text.hpp]")
{
    CHECK(core::text::count_characters("") == 0);
    CHECK(core::text::count_characters("hello") == 5);
    CHECK(core::text::count_characters("hello world") == 11);
    CHECK(core::text::count_characters("café") == 4);
}
