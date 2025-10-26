/**
 * @file backend.hpp
 *
 * @brief SFML window abstraction.
 */

#pragma once

#include <functional>  // for std::function

#include <SFML/Graphics.hpp>

namespace core::backend {

/**
 * @brief SFML window abstraction class.
 *
 * On construction, the window is created with sane defaults. Use `run()` to start the main loop.
 */
class Window {
  public:
    using event_callback_t = std::function<void(const sf::Event &)>;
    using update_callback_t = std::function<void(const float)>;
    using render_callback_t = std::function<void(sf::RenderWindow &)>;

    /**
     * @brief Construct a new SFML window.
     *
     * This creates the window with sane defaults for resolution, anti-aliasing, and frame rate.
     *
     * @note The window is immediately ready for use via the "run()" method or direct access through the "raw()" method.
     */
    explicit Window();

    /**
     * @brief Default destructor.
     */
    ~Window() = default;

    // Disable copy semantics - Window manages unique SFML resources
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    // Allow move construction but disable move assignment for safety
    Window(Window &&) = default;
    Window &operator=(Window &&) = delete;

    /**
     * @brief Get direct access to the underlying SFML RenderWindow.
     *
     * @return Mutable reference to the SFML RenderWindow instance.
     */
    [[nodiscard]] sf::RenderWindow &raw()
    {
        return this->window_;
    }

    /**
     * @brief Get read-only access to the underlying SFML RenderWindow.
     *
     * @return Read-only reference to the SFML RenderWindow instance.
     */
    [[nodiscard]] const sf::RenderWindow &raw() const
    {
        return this->window_;
    }

    /**
     * @brief Run the main application loop with provided callbacks.
     *
     * @param on_event Callback function for handling SFML events.
     * @param on_update Callback function for updating game state (receives delta time).
     * @param on_render Callback function for rendering (receives render window reference).
     *
     * @note The loop continues until the window is closed. Delta time is clamped to prevent extreme values.
     */
    void run(const event_callback_t &on_event,
             const update_callback_t &on_update,
             const render_callback_t &on_render);

  private:
    /**
     * @brief Underlying SFML RenderWindow instance.
     *
     * @note This can be accessed using the "raw()" method.
     */
    sf::RenderWindow window_;
};

}  // namespace core::backend
