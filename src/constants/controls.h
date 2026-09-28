/*
 * Controls
 * Input actions shared by keyboard, mouse and controllers
 * A.D.S. Games
 */
#pragma once

namespace controls {

// Fly the robot up
inline constexpr const char* FLY = "fly";

// Pause and resume the game
inline constexpr const char* PAUSE = "pause";

// Start the game, skip screens and submit a score
inline constexpr const char* CONFIRM = "confirm";

// Save a screenshot
inline constexpr const char* SCREENSHOT = "screenshot";

// Bind every action, call once after asw::core::init
void bind();

} // namespace controls
