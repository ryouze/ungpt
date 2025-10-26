/**
 * @file editor.hpp
 *
 * @brief Actual ImGui-based GUI interface.
 */

#pragma once

#include <cstddef>  // for std::size_t
#include <span>     // for std::span
#include <string>   // for std::string

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

namespace ui::editor {

/**
 * @brief Owns widget submissions and hotkeys for the ungpt editor.
 *
 * The editor receives SFML events, maintains the text, and issues all ImGui draw calls per frame.
 */
class Editor {
  public:
    /**
     * @brief Submit all ImGui widgets for the current frame.
     *
     * The method restores layout hints, draws the toolbar and editor, updates the status bar, and opens the usage modal when required.
     */
    void update_and_draw();

  private:
    /**
     * @brief Calculate horizontal offset that centers the toolbar buttons.
     *
     * @param labels Captions that will be rendered on the toolbar buttons.
     *
     * @return Offset in pixels relative to the current window origin.
     */
    [[nodiscard]] float calculate_center_offset_for_labels(std::span<const std::string> labels) const;

    /**
     * @brief Draw the toolbar that provides clipboard and normalization actions.
     */
    void update_and_draw_top_bar();

    /**
     * @brief Render the multiline editor and handle caret focus.
     */
    void update_and_draw_editor();

    /**
     * @brief Update and render the bottom status line.
     */
    void update_and_draw_bottom_status();

    /**
     * @brief Render the usage modal and close it on toggle or outside click.
     */
    void update_and_draw_usage_modal();

    /**
     * @brief Text displayed inside the editor widget.
     */
    std::string text_;

    /**
     * @brief Track whether the usage modal should be visible.
     */
    bool is_help_modal_open_ = false;

    /**
     * @brief Cached metrics stale flag used by the status bar.
     *
     * If true, the `update_and_draw_bottom_status()` call will recalculate `word_count_` and `character_count_` before it renders the status bar.
     */
    bool text_metrics_need_update_ = true;

    /**
     * @brief Cached word total shown in the status bar.
     */
    std::size_t word_count_ = 0;

    /**
     * @brief Cached character total shown in the status bar.
     */
    std::size_t character_count_ = 0;
};

}  // namespace ui::editor
