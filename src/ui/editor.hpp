/**
 * @file editor.hpp
 *
 * @brief Declares the ungpt ImGui editor module.
 */

#pragma once

#include <span>    // for std::span
#include <string>  // for std::string

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
     * @brief Handle one SFML event emitted by the main loop.
     *
     * The method checks platform modifier state and dispatches registered shortcuts that affect the editor text or the help modal.
     *
     * @param event SFML event forwarded by the application.
     */
    void on_event(const sf::Event &event);

    /**
     * @brief Submit all ImGui widgets for the current frame.
     *
     * The method restores layout hints, draws the toolbar and editor, updates the status bar, and opens the shortcuts modal when required.
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
    float calculate_center_offset_for_labels(std::span<const std::string> labels) const;

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
    void update_and_draw_bottom_status() const;

    /**
     * @brief Render the shortcuts modal and close it on toggle or outside click.
     */
    void update_and_draw_shortcuts_modal();

    /**
     * @brief Text displayed inside the editor widget.
     */
    std::string text_;

    /**
     * @brief Track whether the shortcuts modal should be visible.
     */
    bool is_help_modal_open_ = false;
};

}  // namespace ui::editor
