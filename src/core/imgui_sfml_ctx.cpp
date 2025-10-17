/**
 * @file imgui_sfml_ctx.cpp
 */

#include <stdexcept>  // for std::runtime_error

#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/System/Time.hpp>
#include <spdlog/spdlog.h>

#include "core/imgui_sfml_ctx.hpp"

namespace core::imgui_sfml_ctx {

ImGuiContext::ImGuiContext(sf::RenderWindow &window)
    : window_(window)
{
    SPDLOG_DEBUG("Creating ImGui context...");
    if (!ImGui::SFML::Init(window)) [[unlikely]] {
        throw std::runtime_error("Failed to initialize ImGui-SFML");
    }
    SPDLOG_DEBUG("ImGui context created, applying settings...");

    this->disable_ini_saving();
    SPDLOG_DEBUG("Disabled INI file saving!");

    this->apply_theme();
    SPDLOG_DEBUG("Applied ImGui theme!");

    SPDLOG_DEBUG("ImGui context created successfully, exiting constructor!");
}

ImGuiContext::~ImGuiContext()
{
    ImGui::SFML::Shutdown();
}

void ImGuiContext::disable_ini_saving() const
{
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;
}

void ImGuiContext::apply_theme() const
{
    // Get the current style
    ImGuiStyle &style = ImGui::GetStyle();

    // Define base constants for consistency
    constexpr float rounding = 6.0f;
    constexpr float padding = 8.0f;
    constexpr float spacing = 10.0f;

    /* Global */
    style.DisabledAlpha = 0.5f;  // Additional alpha multiplier applied by BeginDisabled()

    /* Rounding, paddings, borders */
    style.WindowPadding = {padding, padding};
    style.WindowRounding = rounding;
    style.WindowBorderSize = 1.0f;
    style.WindowMinSize = {20.0f, 20.0f};
    style.WindowTitleAlign = {0.0f, 0.5f};
    style.WindowMenuButtonPosition = ImGuiDir_Right;

    style.ChildRounding = rounding;
    style.PopupRounding = rounding;

    style.FramePadding = {padding, padding * 0.75f};  // 8,6
    style.FrameRounding = 4.0f;
    style.FrameBorderSize = 1.0f;

    style.ItemSpacing = {spacing, spacing * 0.8f};  // 10.8
    style.ItemInnerSpacing = {padding, padding * 0.75f};
    style.CellPadding = {padding, padding * 0.75f};

    style.IndentSpacing = padding;
    style.ColumnsMinSpacing = spacing;

    style.ScrollbarRounding = rounding;

    style.GrabMinSize = 5.0f;
    style.GrabRounding = 4.0f;

    style.TabRounding = 4.0f;
    style.TabBorderSize = 1.0f;

    /* Palette */
    const ImVec4 bg0 = ImVec4(0.12f, 0.12f, 0.13f, 1.0f);
    const ImVec4 bg1 = ImVec4(0.16f, 0.16f, 0.18f, 1.0f);
    const ImVec4 bg2 = ImVec4(0.20f, 0.20f, 0.22f, 1.0f);
    const ImVec4 hi1 = ImVec4(0.28f, 0.44f, 0.80f, 1.0f);
    const ImVec4 hi2 = ImVec4(0.24f, 0.38f, 0.70f, 1.0f);
    const ImVec4 text = ImVec4(0.86f, 0.86f, 0.88f, 1.0f);
    const ImVec4 text_dim = ImVec4(0.70f, 0.70f, 0.72f, 1.0f);

    /* Text */
    style.Colors[ImGuiCol_Text] = text;
    style.Colors[ImGuiCol_TextDisabled] = text_dim;

    /* Window */
    style.Colors[ImGuiCol_WindowBg] = bg0;
    style.Colors[ImGuiCol_ChildBg] = bg0;
    style.Colors[ImGuiCol_PopupBg] = bg0;

    /* Borders and title */
    style.Colors[ImGuiCol_Border] = bg2;
    style.Colors[ImGuiCol_BorderShadow] = bg0;
    style.Colors[ImGuiCol_TitleBg] = bg0;
    style.Colors[ImGuiCol_TitleBgActive] = bg0;
    style.Colors[ImGuiCol_TitleBgCollapsed] = bg0;

    /* Menu bar and scrollbars */
    style.Colors[ImGuiCol_MenuBarBg] = bg1;
    style.Colors[ImGuiCol_ScrollbarBg] = bg0;
    style.Colors[ImGuiCol_ScrollbarGrab] = bg1;
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = bg2;
    style.Colors[ImGuiCol_ScrollbarGrabActive] = bg2;

    /* Frames */
    style.Colors[ImGuiCol_FrameBg] = bg1;
    style.Colors[ImGuiCol_FrameBgHovered] = bg2;
    style.Colors[ImGuiCol_FrameBgActive] = bg2;

    /* Buttons */
    style.Colors[ImGuiCol_Button] = bg1;
    style.Colors[ImGuiCol_ButtonHovered] = bg2;
    style.Colors[ImGuiCol_ButtonActive] = bg2;

    /* Headers */
    style.Colors[ImGuiCol_Header] = bg1;
    style.Colors[ImGuiCol_HeaderHovered] = bg2;
    style.Colors[ImGuiCol_HeaderActive] = bg2;

    /* Separators and resizers */
    style.Colors[ImGuiCol_Separator] = bg2;
    style.Colors[ImGuiCol_SeparatorHovered] = bg2;
    style.Colors[ImGuiCol_SeparatorActive] = bg2;
    style.Colors[ImGuiCol_ResizeGrip] = bg1;
    style.Colors[ImGuiCol_ResizeGripHovered] = bg2;
    style.Colors[ImGuiCol_ResizeGripActive] = bg2;

    /* Sliders and checks */
    style.Colors[ImGuiCol_CheckMark] = hi1;
    style.Colors[ImGuiCol_SliderGrab] = hi1;
    style.Colors[ImGuiCol_SliderGrabActive] = hi2;

    /* Tabs */
    style.Colors[ImGuiCol_Tab] = bg1;
    style.Colors[ImGuiCol_TabHovered] = bg2;
    style.Colors[ImGuiCol_TabActive] = bg2;
    style.Colors[ImGuiCol_TabUnfocused] = bg1;
    style.Colors[ImGuiCol_TabUnfocusedActive] = bg2;

    /* Tables */
    style.Colors[ImGuiCol_TableHeaderBg] = bg0;
    style.Colors[ImGuiCol_TableBorderStrong] = bg0;
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0, 0, 0, 1);
    style.Colors[ImGuiCol_TableRowBg] = bg1;
    style.Colors[ImGuiCol_TableRowBgAlt] = bg1;

    /* Navigation and overlays */
    style.Colors[ImGuiCol_DragDropTarget] = hi1;
    style.Colors[ImGuiCol_NavHighlight] = hi1;
    style.Colors[ImGuiCol_NavWindowingHighlight] = hi1;
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0, 0, 0, 0.5f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0, 0, 0, 0.5f);
}

void ImGuiContext::process_event(const sf::Event &event) const
{
    ImGui::SFML::ProcessEvent(this->window_, event);
}

void ImGuiContext::update(const float dt) const
{
    ImGui::SFML::Update(this->window_, sf::seconds(dt));
}

void ImGuiContext::render() const
{
    ImGui::SFML::Render(this->window_);
}

}  // namespace core::imgui_sfml_ctx
