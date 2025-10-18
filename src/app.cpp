/**
 * @file app.cpp
 */

#include <algorithm>  // for std::max
#include <cstddef>    // for std::size_t
#include <string>     // for std::string

#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>  // ImGui std::string helpers

#include "core/backend.hpp"
#include "core/clipboard.hpp"
#include "core/imgui_sfml_ctx.hpp"
#include "core/text.hpp"

namespace app {

[[nodiscard]] float compute_controls_total_width()
{
    const ImGuiStyle &style = ImGui::GetStyle();
    const float frame_padding_x = style.FramePadding.x;
    const float spacing_x = style.ItemSpacing.x;
    const float w_paste = ImGui::CalcTextSize("Paste").x + frame_padding_x * 2.0f;
    const float w_norm = ImGui::CalcTextSize("Normalize").x + frame_padding_x * 2.0f;
    const float w_copy = ImGui::CalcTextSize("Copy").x + frame_padding_x * 2.0f;
    const float w_clear = ImGui::CalcTextSize("Clear").x + frame_padding_x * 2.0f;
    const float w_help = ImGui::CalcTextSize("?").x + frame_padding_x * 2.0f;
    return w_paste + w_norm + w_copy + w_clear + w_help + spacing_x * 4.0f;
}

void show_shortcuts_help_modal(bool *open)
{
    if (*open)
        ImGui::OpenPopup("Shortcuts");
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 12.0f));
    if (ImGui::BeginPopupModal("Shortcuts", open, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize)) {
#if defined(__APPLE__)
        const char *mod = "Cmd";
#else
        const char *mod = "Ctrl";
#endif
        ImGui::Text("%s+V : Paste", mod);
        ImGui::Text("%s+N : Normalize", mod);
        ImGui::Text("%s+C : Copy normalized", mod);
        ImGui::Text("%s+L : Clear both", mod);
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

void run()
{
    // Create SFML window with sane defaults
    core::backend::Window window;

    // Create RAII context with theme and no INI file
    core::imgui_sfml_ctx::ImGuiContext imgui_context{window.raw()};

    std::string input_text;
    std::string output_text;
    bool open_help_modal = false;

    const auto on_event = [&](const sf::Event &event) {
        // Let ImGui handle the event
        imgui_context.process_event(event);

        // Window was closed
        if (event.is<sf::Event::Closed>()) [[unlikely]] {
            window.raw().close();
        }

        if (const auto *key = event.getIf<sf::Event::KeyPressed>()) {
#if defined(__APPLE__)
            const bool modifier_down = key->system;
#else
            const bool modifier_down = key->control;
#endif
            if (modifier_down) {
                if (key->code == sf::Keyboard::Key::V) {
                    input_text = core::clipboard::read_from_clipboard();
                }
                else if (key->code == sf::Keyboard::Key::N) {
                    output_text = input_text;
                    core::text::normalize_text_in_place(output_text);
                }
                else if (key->code == sf::Keyboard::Key::C) {
                    core::clipboard::write_to_clipboard(output_text);
                }
                else if (key->code == sf::Keyboard::Key::L) {
                    input_text.clear();
                    output_text.clear();
                }
            }
        }
    };

    const auto on_update = [&](const float dt) {
        (void)dt;
        imgui_context.update(dt);

        const ImGuiIO &io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

        const ImGuiWindowFlags root_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        if (ImGui::Begin("##root", nullptr, root_flags)) {
            // Top controls bar
            if (ImGui::BeginChild("##topbar", ImVec2(0, 0), ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
                const float available_width = ImGui::GetContentRegionAvail().x;
                const float total_width = compute_controls_total_width();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + std::max(0.0f, (available_width - total_width) * 0.5f));

                bool do_paste = false, do_norm = false, do_copy = false, do_clear = false;
                if (ImGui::Button("Paste"))
                    do_paste = true;
                ImGui::SameLine();
                if (ImGui::Button("Normalize"))
                    do_norm = true;
                ImGui::SameLine();
                if (ImGui::Button("Copy"))
                    do_copy = true;
                ImGui::SameLine();
                if (ImGui::Button("Clear"))
                    do_clear = true;
                ImGui::SameLine();
                if (ImGui::Button("?"))
                    open_help_modal = true;

                if (do_paste) {
                    input_text = core::clipboard::read_from_clipboard();
                }
                if (do_norm) {
                    output_text = input_text;
                    core::text::normalize_text_in_place(output_text);
                }
                if (do_copy) {
                    core::clipboard::write_to_clipboard(output_text);
                }
                if (do_clear) {
                    input_text.clear();
                    output_text.clear();
                }
            }
            ImGui::EndChild();

            // Main split area, fills remaining height minus bottom status
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
            const float bottom_row_h = ImGui::GetTextLineHeightWithSpacing();
            const float main_height = std::max(100.0f, ImGui::GetContentRegionAvail().y - bottom_row_h);
            if (ImGui::BeginChild("##main", ImVec2(0, main_height), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
                const ImGuiTableFlags table_flags = ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerV;
                if (ImGui::BeginTable("split", 2, table_flags, ImVec2(-1.0f, -1.0f))) {
                    ImGui::TableSetupColumn("Original", ImGuiTableColumnFlags_WidthStretch, 1.0f);
                    ImGui::TableSetupColumn("Normalized", ImGuiTableColumnFlags_WidthStretch, 1.0f);
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted("Original");
                    {
                        const ImVec2 sz = ImGui::GetContentRegionAvail();
                        ImGui::InputTextMultiline("##input", &input_text, sz, ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_NoHorizontalScroll);
                    }

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted("Normalized");
                    {
                        const ImVec2 sz = ImGui::GetContentRegionAvail();
                        ImGui::InputTextMultiline("##output", &output_text, sz, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_NoHorizontalScroll);
                    }

                    ImGui::EndTable();
                }
            }
            ImGui::EndChild();
            ImGui::PopStyleVar();  // main padding

            // bottom status
            if (ImGui::BeginChild("##bottom", ImVec2(0, 0), ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
                const std::size_t in_words = core::text::count_words_ascii_whitespace(input_text);
                const std::size_t out_words = core::text::count_words_ascii_whitespace(output_text);
                const std::size_t in_chars = core::text::count_codepoints_utf8(input_text);
                const std::size_t out_chars = core::text::count_codepoints_utf8(output_text);

                const float half = ImGui::GetContentRegionAvail().x * 0.5f;
                ImGui::Text("Words: %zu  Chars: %zu", in_words, in_chars);
                ImGui::SameLine(half);
                ImGui::Text("Words: %zu  Chars: %zu", out_words, out_chars);
            }
            ImGui::EndChild();

            show_shortcuts_help_modal(&open_help_modal);
        }
        ImGui::End();
        ImGui::PopStyleVar(3);  // Rounding, border, root padding
    };

    const auto on_render = [&](sf::RenderWindow &rt) {
        rt.clear(sf::Color{255, 255, 255});
        imgui_context.render();
        rt.display();
    };

    // Ask OS to switch to this window and start the main loop
    window.raw().requestFocus();
    window.run(on_event, on_update, on_render);
}

}  // namespace app
