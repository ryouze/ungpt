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

void run()
{
    // Create SFML window with sane defaults
    core::backend::Window window;

    // Create RAII context with theme and no INI file
    core::imgui_sfml_ctx::ImGuiContext imgui_context{window.raw()};

    std::string text;
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
                    text = core::clipboard::read_from_clipboard();
                }
                else if (key->code == sf::Keyboard::Key::N) {
                    core::text::remove_unwanted_characters(text);
                }
                else if (key->code == sf::Keyboard::Key::C) {
                    core::clipboard::write_to_clipboard(text);
                }
                else if (key->code == sf::Keyboard::Key::L) {
                    text.clear();
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
            if (ImGui::BeginChild("##topbar", ImVec2(0, 0), ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
                const ImGuiStyle &style = ImGui::GetStyle();
                const float frame_padding_x = style.FramePadding.x;
                const float spacing_x = style.ItemSpacing.x;
                const float total_width = (ImGui::CalcTextSize("Paste").x + frame_padding_x * 2.0f) + (ImGui::CalcTextSize("Normalize").x + frame_padding_x * 2.0f) + (ImGui::CalcTextSize("Copy").x + frame_padding_x * 2.0f) + (ImGui::CalcTextSize("Clear").x + frame_padding_x * 2.0f) + (ImGui::CalcTextSize("?").x + frame_padding_x * 2.0f) + spacing_x * 4.0f;
                const float available_width = ImGui::GetContentRegionAvail().x;
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

                if (do_paste)
                    text = core::clipboard::read_from_clipboard();
                if (do_norm)
                    core::text::remove_unwanted_characters(text);
                if (do_copy)
                    core::clipboard::write_to_clipboard(text);
                if (do_clear)
                    text.clear();
            }
            ImGui::EndChild();

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
            const float bottom_row_h = ImGui::GetTextLineHeightWithSpacing();
            const float main_height = std::max(100.0f, ImGui::GetContentRegionAvail().y - bottom_row_h);
            if (ImGui::BeginChild("##main", ImVec2(0, main_height), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
                const ImVec2 sz = ImGui::GetContentRegionAvail();
                ImGui::InputTextMultiline("##text", &text, sz, ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_NoHorizontalScroll);
            }
            ImGui::EndChild();
            ImGui::PopStyleVar();

            if (ImGui::BeginChild("##bottom", ImVec2(0, 0), ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
                const std::size_t words = core::text::count_words(text);
                const std::size_t chars = core::text::count_characters(text);
                ImGui::Text("Words: %zu  Chars: %zu", words, chars);
            }
            ImGui::EndChild();

            if (open_help_modal)
                ImGui::OpenPopup("Shortcuts");
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 12.0f));
            if (ImGui::BeginPopupModal("Shortcuts", &open_help_modal, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize)) {
#if defined(__APPLE__)
                const char *mod = "Cmd";
#else
                const char *mod = "Ctrl";
#endif
                ImGui::Text("%s+V : Paste", mod);
                ImGui::Text("%s+N : Normalize", mod);
                ImGui::Text("%s+C : Copy", mod);
                ImGui::Text("%s+L : Clear", mod);
                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();
        }
        ImGui::End();
        ImGui::PopStyleVar(3);
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
