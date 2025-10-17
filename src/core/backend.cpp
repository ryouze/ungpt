/**
 * @file backend.cpp
 */

#include <algorithm>  // for std::min
#include <format>     // for std::format
#include <string>     // for std::string

#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/ContextSettings.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <spdlog/spdlog.h>

#include "core/backend.hpp"
#include "generated.hpp"

namespace core::backend {

Window::Window()
{
    // Create context settings with the current anti-aliasing level
    const sf::ContextSettings settings{.antiAliasingLevel = 8};
    SPDLOG_DEBUG("Created context settings with '{}' anti-aliasing level", settings.antiAliasingLevel);

    // Create the window title based on the project name and version
    const std::string window_title = std::format("{} ({})", generated::PROJECT_NAME, generated::PROJECT_VERSION);
    SPDLOG_DEBUG("Created '{}' window title", window_title);

    // Get the video mode (resolution) based on current settings
    sf::VideoMode mode;

    // Set the default windowed resolution
    mode = sf::VideoMode{sf::Vector2u{800, 600}};
    SPDLOG_DEBUG("Current mode is windowed, using default resolution '{}x{}'", mode.size.x, mode.size.y);

    // Create the window with the determined video mode, title, state, and context settings
    this->window_.create(mode, window_title, sf::State::Windowed, settings);

    // Set minimum size (only relevant for windowed mode)
    this->window_.setMinimumSize(sf::Vector2u{800, 600});

    // Enable vsync (remember: you cannot use both FPS limit and vsync at the same time)
    this->window_.setVerticalSyncEnabled(true);

    // Log the successful creation of the window
    SPDLOG_DEBUG("Window created successfully with mode '{}x{}', title '{}', vsync, and context settings (anti-aliasing level: {})", mode.size.x, mode.size.y, window_title, settings.antiAliasingLevel);
}

void Window::run(const event_callback_type &on_event,
                 const update_callback_type &on_update,
                 const render_callback_type &on_render)
{
    SPDLOG_INFO("Starting main window loop!");
    sf::Clock clock;
    while (this->window_.isOpen()) {
        // Allow user of this call to explicitly handle events themselves
        this->window_.handleEvents(on_event);
        // Prevent extreme dt by clamping to 0.1 seconds
        constexpr float dt_max = 0.1f;
        const float dt = std::min(clock.restart().asSeconds(), dt_max);
        on_update(dt);
        on_render(this->window_);
    }
    SPDLOG_INFO("Main window loop ended!");
}

}  // namespace core::backend
