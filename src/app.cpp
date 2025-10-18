/**
 * @file app.cpp
 */

#include <algorithm>    // for std::max
#include <array>        // for std::array
#include <cstddef>      // for std::size_t
#include <format>       // for std::format
#include <string_view>  // for std::string_view

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
    bool is_help_modal_open = false;

    const auto on_event = [&](const sf::Event &event) {
        // Let ImGui handle the event
        imgui_context.process_event(event);

        // Window was closed
        if (event.is<sf::Event::Closed>()) [[unlikely]] {
            window.raw().close();
        }

        // Handle key presses for shortcuts
        if (const auto *key = event.getIf<sf::Event::KeyPressed>()) {
#if defined(__APPLE__)
            // On macOS the Command key is reported as system
            const bool modifier_down = key->system;
#else
            // On other platforms we fall back to CTRL
            const bool modifier_down = key->control;
#endif
            if (modifier_down) {
                // Cmd/CTRL + V pastes clipboard text into the editor
                if (key->code == sf::Keyboard::Key::V) {
                    text = core::clipboard::read_from_clipboard();
                }
                // Cmd/CTRL + N normalizes the text in place
                else if (key->code == sf::Keyboard::Key::N) {
                    core::text::remove_unwanted_characters(text);
                }
                // Cmd/CTRL + C copies the editor contents to the clipboard
                else if (key->code == sf::Keyboard::Key::C) {
                    core::clipboard::write_to_clipboard(text);
                }
                // Cmd/CTRL + L clears the editor
                else if (key->code == sf::Keyboard::Key::L) {
                    text.clear();
                }
            }
        }
    };

    const auto on_update = [&](const float dt) {
        // Pass elapsed time to ImGui
        imgui_context.update(dt);

        // Access ImGui IO and style for theme-aware layout
        const ImGuiIO &io = ImGui::GetIO();
        const ImGuiStyle &style = ImGui::GetStyle();
        // Lock the root window to the top-left corner every frame
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        // Make the root window fill the entire display area
        ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

        // Disable all window decorations and interactions for the root window
        constexpr ImGuiWindowFlags root_flags = ImGuiWindowFlags_NoTitleBar |
                                                ImGuiWindowFlags_NoResize |
                                                ImGuiWindowFlags_NoMove |
                                                ImGuiWindowFlags_NoCollapse |
                                                ImGuiWindowFlags_NoScrollbar |
                                                ImGuiWindowFlags_NoScrollWithMouse;

        // Reduce padding and rounding for the root window
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, style.WindowPadding);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        if (ImGui::Begin("##root", nullptr, root_flags)) {
            // The root window holds every other panel for the application
            // Render toolbar with action buttons centered along the top
            if (ImGui::BeginChild("##topbar",
                                  ImVec2(0, 0),
                                  ImGuiChildFlags_AutoResizeY,
                                  ImGuiWindowFlags_NoScrollbar |
                                      ImGuiWindowFlags_NoScrollWithMouse)) {

                // Use style metrics for consistent sizing
                // Labels used by the toolbar buttons
                constexpr std::array<const char *, 5> button_labels = {
                    "Paste",
                    "Normalize",
                    "Copy",
                    "Clear",
                    "?"};
                // Horizontal padding applied to each side of a button label
                const float frame_padding_x = style.FramePadding.x;
                // Horizontal spacing between consecutive widgets
                const float spacing_x = style.ItemSpacing.x;
                // Measure button widths so we can center the row regardless of font
                float total_width = -spacing_x;
                for (const char *label : button_labels) {
                    const float button_width = ImGui::CalcTextSize(label).x +
                                               frame_padding_x * 2.0f;
                    total_width += button_width + spacing_x;
                }

                // Center the button row by shifting the cursor within the child
                const float available_width = ImGui::GetContentRegionAvail().x;
                const float offset_x = std::max(0.0f, (available_width - total_width) * 0.5f);
                ImGui::SetCursorPosX(offset_x);

                // Paste button reads clipboard content into the editor
                if (ImGui::Button("Paste")) {
                    text = core::clipboard::read_from_clipboard();
                }

                // Keep the next widget on the same horizontal row
                ImGui::SameLine();

                // Normalize button cleans the text with the project formatter
                if (ImGui::Button("Normalize")) {
                    core::text::remove_unwanted_characters(text);
                }

                // Keep the next widget on the same horizontal row
                ImGui::SameLine();

                // Copy button writes the current text to the clipboard
                if (ImGui::Button("Copy")) {
                    core::clipboard::write_to_clipboard(text);
                }

                // Keep the next widget on the same horizontal row
                ImGui::SameLine();

                // Clear button empties the editor immediately
                if (ImGui::Button("Clear")) {
                    text.clear();
                }

                // Keep the next widget on the same horizontal row
                ImGui::SameLine();

                // Question mark button toggles the shortcut modal
                if (ImGui::Button("?"))
                    is_help_modal_open = true;
            }

            // Finish the toolbar child window and return to the root window
            ImGui::EndChild();

            // Restore default padding for the main editor area
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, style.WindowPadding);

            // Reserve space for the statistics row at the bottom
            const float bottom_row_h = ImGui::GetTextLineHeightWithSpacing();
            const float main_height = std::max(100.0f, ImGui::GetContentRegionAvail().y - bottom_row_h);

            // Present the multiline text editor occupying most of the window
            if (ImGui::BeginChild("##main",
                                  ImVec2(0, main_height),
                                  ImGuiChildFlags_None,
                                  ImGuiWindowFlags_NoScrollbar |
                                      ImGuiWindowFlags_NoScrollWithMouse)) {

                // Use all remaining content space for the multiline widget
                const ImVec2 sz = ImGui::GetContentRegionAvail();
                ImGui::InputTextMultiline("##text",
                                          &text,
                                          sz,
                                          ImGuiInputTextFlags_AllowTabInput);
            }

            // Finish the text editor child window and restore to the root level
            ImGui::EndChild();

            // Restore the earlier window padding so later panels use defaults
            ImGui::PopStyleVar();

            // Show statistics row at the bottom to report words and characters
            if (ImGui::BeginChild("##bottom",
                                  ImVec2(0, 0),
                                  ImGuiChildFlags_AutoResizeY,
                                  ImGuiWindowFlags_NoScrollbar |
                                      ImGuiWindowFlags_NoScrollWithMouse)) {

                // Calculate human friendly metrics every frame
                const std::size_t words = core::text::count_words(text);
                const std::size_t chars = core::text::count_characters(text);
                const std::string status = std::format("Words: {}  Characters: {}", words, chars);

                // Center the metrics by offsetting the cursor before drawing text
                const float available_width = ImGui::GetContentRegionAvail().x;
                const float text_width = ImGui::CalcTextSize(status.c_str()).x;
                const float x = std::max(0.0f,
                                         (available_width - text_width) * 0.5f);
                ImGui::SetCursorPosX(x);
                ImGui::TextUnformatted(status.c_str());
            }
            ImGui::EndChild();

            // Open the help popup if a toolbar or shortcut action requested it
            if (is_help_modal_open) {
                ImGui::OpenPopup("Shortcuts");
            }

            // Display a quick reference modal for keyboard shortcuts
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                                style.WindowPadding);
            if (ImGui::BeginPopupModal("Shortcuts",
                                       &is_help_modal_open,
                                       ImGuiWindowFlags_AlwaysAutoResize)) {
#if defined(__APPLE__)
                // Label the modifier key appropriately for macOS users
                const std::string_view mod = "Cmd";
#else
                // Label the modifier key appropriately for other platforms
                const std::string_view mod = "Ctrl";
#endif
                // Build human readable shortcut labels to display in order
                const std::string l1 = std::format("{}+V : Paste", mod);
                const std::string l2 = std::format("{}+N : Normalize", mod);
                const std::string l3 = std::format("{}+C : Copy", mod);
                const std::string l4 = std::format("{}+L : Clear", mod);
                ImGui::TextUnformatted(l1.c_str());
                ImGui::TextUnformatted(l2.c_str());
                ImGui::TextUnformatted(l3.c_str());
                ImGui::TextUnformatted(l4.c_str());
                ImGui::EndPopup();
            }
            // Restore the modal window padding override
            ImGui::PopStyleVar();
        }
        // Close the root window so ImGui knows the layout is complete
        ImGui::End();
        // Pop the three style overrides pushed before the root window
        ImGui::PopStyleVar(3);
    };

    // Render callback clears the frame, draws ImGui, and presents the buffer
    const auto on_render = [&](sf::RenderWindow &rt) {
        // Clear removes artifacts from the previous frame
        rt.clear();
        // Render draws the ImGui command list into the render target
        imgui_context.render();
        // Display swaps the back buffer onto the screen
        rt.display();
    };

    // Bring the application window to the foreground for easier testing
    // Ask OS to switch to this window and start the main loop
    window.raw().requestFocus();
    // Start the backend event loop with the configured callbacks
    window.run(on_event, on_update, on_render);
}

}  // namespace app
