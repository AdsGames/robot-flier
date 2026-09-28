/*
 * Menu
 * A.D.S. Games
 * 03/01/2016
 * The menu of Robot Flier
 */
#pragma once

#include <array>
#include <asw/asw.h>
#include <time.h>
#include <vector>

#include "../constants/controls.h"
#include "../constants/globals.h"
#include "score_table.h"
#include "state.h"

constexpr int MINISTATE_MENU = 0;
constexpr int MINISTATE_TUTORIAL = 1;
constexpr int MINISTATE_CREDITS = 2;
constexpr int MINISTATE_OPTIONS = 3;
constexpr int MINISTATE_CONTROLS = 4;
constexpr int MINISTATE_SCORES = 5;

class MenuScene : public asw::scene::Scene<Scenes> {
public:
    static constexpr float ANIMATION_DURATION = 1.3F;

    using asw::scene::Scene<Scenes>::Scene;

    // Override parent
    void init() override;
    void update(float deltaTime) override;
    void draw() override;

private:
    // Build the menu and options ui, once per scene
    void build_ui();

    // Match the options widgets to the settings
    void sync_options();

    // Change mini screen, main menu gets focus back on start
    void open_screen(int screen);

    // Start the closing animation, then the game
    void start_game();

    // Score table
    ScoreTable highscores;

    // Particle emitter
    asw::ParticleEmitter emitter;

    // Vars
    float animation_ticker;

    int mini_screen;
    bool startClicked;

    // Screens
    asw::game::Sprite img_menu;
    asw::game::Sprite options;
    asw::game::Sprite helpScreen;
    asw::game::Sprite controls;
    asw::game::Sprite credits;
    asw::game::Sprite highscores_table;

    // Title
    asw::game::Sprite title;

    // Start button for xbox control
    asw::game::Sprite xbox_start;

    // Main menu buttons
    asw::ui::Root menu_ui;
    asw::ui::Button* start { nullptr };
    asw::ui::Button* highscores_button { nullptr };
    asw::ui::Button* ui_credits { nullptr };
    asw::ui::Button* ui_controls { nullptr };
    asw::ui::Button* ui_help { nullptr };
    asw::ui::Button* ui_options { nullptr };

    // Options menu
    asw::ui::Root options_ui;
    asw::ui::Checkbox* ui_sound { nullptr };
    asw::ui::Checkbox* ui_music { nullptr };
    asw::ui::Checkbox* ui_window { nullptr };
    asw::ui::Button* ui_particle { nullptr };
    asw::ui::Button* ui_screenshake { nullptr };
    asw::ui::Button* ui_control { nullptr };
    asw::ui::Button* ui_exit { nullptr };
    asw::ui::Button* ui_back { nullptr };

    // Images for the options that cycle through more than two values
    std::array<asw::Texture, 4> tex_particle;
    std::array<asw::Texture, 4> tex_screenshake;
    std::array<asw::Texture, 3> tex_control;

    // Music
    asw::Music music_mainmenu;
};
