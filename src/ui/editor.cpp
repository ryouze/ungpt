/**
 * @file editor.cpp
 */

#include <algorithm>  // for std::max
#include <array>      // for std::array
#include <cstddef>    // for std::size_t
#include <format>     // for std::format
#include <span>       // for std::span
#include <string>     // for std::string

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>  // for std::string with InputText
#include <spdlog/spdlog.h>

#include "core/clipboard.hpp"
#include "core/text.hpp"
#include "ui/editor.hpp"

namespace ui::editor {

void Editor::update_and_draw()
{
    // Fetch the global ImGui IO state for display size queries
    const ImGuiIO &io = ImGui::GetIO();

    // Fetch the style description for padding and spacing metrics
    const ImGuiStyle &style = ImGui::GetStyle();

    // Pin the next window position to the top left corner of the render area
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);

    // Force the next window size to match the current viewport
    ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

    // Combine window flags to hide decoration, disable moving, and ignore mouse-wheel scrolling
    constexpr ImGuiWindowFlags root_flags = ImGuiWindowFlags_NoDecoration |
                                            ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoScrollWithMouse;

    // Push consistent padding derived from the configured style
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, style.WindowPadding);

    // Remove rounded corners so the window fills the viewport edge to edge
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    // Remove window borders for a flat canvas look
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    // Begin the root window that contains the entire interface
    if (ImGui::Begin("##root", nullptr, root_flags)) {

        // Child windows should not expose scrollbars
        constexpr ImGuiWindowFlags child_flags = ImGuiWindowFlags_NoScrollbar;

        // Begin a child window that holds the toolbar widgets
        if (ImGui::BeginChild("##topbar", ImVec2(0.0f, 0.0f), ImGuiChildFlags_AutoResizeY, child_flags)) {
            // Draw the toolbar contents into the child window
            this->update_and_draw_top_bar();
        }

        // Close the toolbar child window
        ImGui::EndChild();

        // Push padding override for the main editor region
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, style.WindowPadding);

        // Compute the status bar height using the current font metrics
        const float bottom_row_h = ImGui::GetTextLineHeightWithSpacing();

        // Determine the main area height by subtracting the reserved status bar space
        const float main_height = std::max(0.0f, ImGui::GetContentRegionAvail().y - bottom_row_h);

        // Begin the child window that hosts the text editor
        if (ImGui::BeginChild("##main", ImVec2(0.0f, main_height), ImGuiChildFlags_None, child_flags)) {
            // Draw the multiline editor widget
            this->update_and_draw_editor();
        }

        // Close the main editor child window
        ImGui::EndChild();

        // Pop the padding override now that the main region is done
        ImGui::PopStyleVar();

        // Begin the child window that hosts the bottom status bar
        if (ImGui::BeginChild("##bottom", ImVec2(0.0f, 0.0f), ImGuiChildFlags_AutoResizeY, child_flags)) {
            // Draw the live metrics into the status bar
            this->update_and_draw_bottom_status();
        }

        // Close the status bar child window
        ImGui::EndChild();

        // Draw the modal even if not visible so state stays in sync
        this->update_and_draw_usage_modal();

        // Finish populating the root window contents
    }

    // End the root window definition
    ImGui::End();

    // Pop every style override pushed before creating the root window
    ImGui::PopStyleVar(3);
}

float Editor::calculate_center_offset_for_labels(std::span<const std::string> labels) const
{
    // Access the active style for padding and spacing metrics
    const ImGuiStyle &style = ImGui::GetStyle();

    // Extract the horizontal padding applied inside buttons
    const float frame_padding_x = style.FramePadding.x;

    // Extract the spacing between widgets measured along X
    const float spacing_x = style.ItemSpacing.x;

    // Initialize the accumulated width so the first addition cancels the spacing term
    float total_width = -spacing_x;

    // Iterate over each label to compute the final row width
    for (const std::string &label : labels) {
        // Calculate the width of a button by adding label width and padding
        const float button_width = ImGui::CalcTextSize(label.c_str()).x + frame_padding_x * 2.0f;

        // Accumulate the button width and the spacing that follows it
        total_width += button_width + spacing_x;
    }

    // Query the current region width available for the toolbar
    const float available_width = ImGui::GetContentRegionAvail().x;

    // Compute the horizontal offset that centers the toolbar row
    const float offset_x = (available_width > total_width) ? (available_width - total_width) * 0.5f : 0.0f;

    // Return the computed offset so callers can align the cursor
    return offset_x;
}

void Editor::update_and_draw_top_bar()
{
    // Prepare a fixed list of button labels for toolbar actions
    constexpr std::array<std::string, 5> labels = {"Paste", "Normalize", "Copy", "Clear", "?"};

    // Compute a horizontal offset that centers the toolbar buttons
    const float offset_x = this->calculate_center_offset_for_labels(std::span<const std::string>(labels));

    // Move the cursor so the buttons start centered within the region
    ImGui::SetCursorPosX(offset_x);

    // Render the paste button that pulls text from the clipboard helper
    if (ImGui::Button(labels[0].c_str())) {
        SPDLOG_DEBUG("Paste button was pressed");
        this->text_ = core::clipboard::read_from_clipboard();
    }

    // Keep subsequent buttons on the same row
    ImGui::SameLine();

    // Render the normalize button that cleans up smart punctuation via core::text
    if (ImGui::Button(labels[1].c_str())) {
        SPDLOG_DEBUG("Normalize button was pressed");
        core::text::remove_unwanted_characters(this->text_);
    }

    // Keep the next button on the same row
    ImGui::SameLine();

    // Render the copy button that pushes text to the clipboard helper
    if (ImGui::Button(labels[2].c_str())) {
        SPDLOG_DEBUG("Copy button was pressed");
        core::clipboard::write_to_clipboard(this->text_);
    }

    // Keep the next button on the same row
    ImGui::SameLine();

    // Render the clear button that empties the editor text
    if (ImGui::Button(labels[3].c_str())) {
        SPDLOG_DEBUG("Clear button was pressed");
        this->text_.clear();
    }

    // Keep the help button on the same row
    ImGui::SameLine();

    // Render the help button that opens the usage modal
    if (ImGui::Button(labels[4].c_str())) {
        SPDLOG_DEBUG("Help button was pressed");
        this->is_help_modal_open_ = true;
    }
}

void Editor::update_and_draw_editor()
{
    // Query the available size to grow the editor with the window
    const ImVec2 size = ImGui::GetContentRegionAvail();

    // Submit the multiline text widget that edits the internal text
    ImGui::InputTextMultiline("##text", &this->text_, size, ImGuiInputTextFlags_AllowTabInput);
}

void Editor::update_and_draw_bottom_status() const
{
    // Calculate the metrics and format them into a status string
    const std::string status = std::format("Words: {}  Characters: {}",
                                           core::text::count_words(this->text_),
                                           core::text::count_characters(this->text_));

    // Determine the available width within the status bar
    const float available_width = ImGui::GetContentRegionAvail().x;

    // Measure the pixel width of the status string with the active font
    const float text_width = ImGui::CalcTextSize(status.c_str()).x;

    // Compute a centered X offset when there is enough room
    const float x = (available_width > text_width) ? (available_width - text_width) * 0.5f : 0.0f;

    // Position the cursor to center the status text
    ImGui::SetCursorPosX(x);

    // Render the status line using the formatted string
    ImGui::TextUnformatted(status.c_str());
}

void Editor::update_and_draw_usage_modal()
{
    // Lock the modal size to its content and prevent manual repositioning
    constexpr ImGuiWindowFlags modal_flags = ImGuiWindowFlags_AlwaysAutoResize |
                                             ImGuiWindowFlags_NoMove;

    // Query whether the popup is already open to avoid redundant open calls
    const bool popup_visible = ImGui::IsPopupOpen("Usage", ImGuiPopupFlags_AnyPopupId);

    // Open the popup only when the UI requested it and it is currently closed
    if (this->is_help_modal_open_ && !popup_visible) {
        ImGui::OpenPopup("Usage");
    }

    // Fetch the global ImGui IO state for display size queries
    const ImGuiIO &io = ImGui::GetIO();

    // Compute the target center position based on the display size
    const ImVec2 display_center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);

    // Force the modal to stay centered relative to the display
    ImGui::SetNextWindowPos(display_center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    // Begin the modal popup and populate it when the window becomes visible
    if (ImGui::BeginPopupModal("Usage", nullptr, modal_flags)) {
        // Detect whether the user clicked outside the popup to dismiss it
        const bool clicked_outside = ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
                                     !ImGui::IsWindowHovered();

        // Close the popup when the toggle was cleared or an outside click occurred
        if (!this->is_help_modal_open_ || clicked_outside) {
            this->is_help_modal_open_ = false;
            ImGui::CloseCurrentPopup();
        }
        else {
            ImGui::TextUnformatted("1. Click Paste to load text from the clipboard.");
            ImGui::TextUnformatted("2. Click Normalize to modify the text in place.");
            ImGui::TextUnformatted("3. Click Copy to write the text to the clipboard.");
        }

        // End the popup modal after populating all widgets
        ImGui::EndPopup();
    }

    // Clear the open flag when the popup is no longer present
    if (!ImGui::IsPopupOpen("Usage", ImGuiPopupFlags_AnyPopupId)) {
        this->is_help_modal_open_ = false;
    }
}

}  // namespace ui::editor
