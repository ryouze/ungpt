/**
 * @file app.cpp
 */

#include <SFML/Window/Event.hpp>

#include "core/backend.hpp"
#include "core/imgui_sfml_ctx.hpp"
#include "ui/editor.hpp"

namespace app {

void run()
{
    // Create SFML window with sane defaults
    core::backend::Window window;

    // Create RAII context with theme and no INI file
    core::imgui_sfml_ctx::ImGuiContext imgui_context{window.raw()};

    // Create the text editor interface (i.e., this actual app, everything so far was boilerplate)
    ui::editor::Editor text_editor;

    const auto on_event = [&](const sf::Event &event) {
        // Let ImGui handle the event
        imgui_context.process_event(event);

        // Window was closed
        if (event.is<sf::Event::Closed>()) [[unlikely]] {
            window.raw().close();
        }

        // Process keyboard shortcuts
        text_editor.on_event(event);
    };

    const auto on_update = [&](const float dt) {
        imgui_context.update(dt);
    };

    const auto on_render = [&](sf::RenderWindow &rt) {
        rt.clear();
        text_editor.draw();
        imgui_context.render();
        rt.display();
    };

    // Ask OS to switch to this window and start the main loop
    window.raw().requestFocus();
    window.run(on_event, on_update, on_render);
}

}  // namespace app
