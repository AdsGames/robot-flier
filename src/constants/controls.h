/*
 * Controls
 * Input actions shared by keyboard, mouse and controllers
 * A.D.S. Games
 */
#pragma once

namespace asw::ui {
class Root;
} // namespace asw::ui

namespace controls {

// Fly the robot up
inline constexpr const char* FLY = "fly";

// Pause and resume the game
inline constexpr const char* PAUSE = "pause";

// Start the game, skip screens and submit a score
inline constexpr const char* CONFIRM = "confirm";

// Save a screenshot
inline constexpr const char* SCREENSHOT = "screenshot";

// Menu navigation, controller only, the asw ui root handles the keyboard
inline constexpr const char* UI_UP = "ui_up";
inline constexpr const char* UI_DOWN = "ui_down";
inline constexpr const char* UI_LEFT = "ui_left";
inline constexpr const char* UI_RIGHT = "ui_right";
inline constexpr const char* UI_CONFIRM = "ui_confirm";

// Bind every action, call once after asw::core::init
void bind();

// Update a ui root and give controllers focus navigation on it
void update_ui(asw::ui::Root& ui);

} // namespace controls
