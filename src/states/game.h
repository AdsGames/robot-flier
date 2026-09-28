/*
 * Game state
 * A.D.S. Games
 * 03/01/2016
 * Main game state
 */
#pragma once

#include <asw/asw.h>

#include "../constants/controls.h"
#include "../constants/globals.h"
#include "../entities/debris.h"
#include "../entities/energy.h"
#include "../entities/powerup.h"
#include "../entities/robot.h"
#include "score_table.h"
#include "state.h"

// Game class
class GameScene : public asw::scene::Scene<Scenes> {
public:
    using asw::scene::Scene<Scenes>::Scene;

    // Override parent
    void init() override;
    void update(float deltaTime) override;
    void draw() override;

private:
    // Score table
    ScoreTable highscores;

    // Change theme
    void changeTheme(int NewThemeNumber);

    // Ticker
    void gameTick();

    // Declare bitmaps
    asw::Texture screenshot;

    // Game images
    asw::Texture space;
    asw::Texture parallaxBack;
    asw::Texture groundOverlay;
    asw::Texture groundUnderlay;

    // GUI Images
    asw::Texture debug;
    asw::Texture pauseMenu;
    asw::Texture ui_game_end;
    asw::Texture ui_a;
    asw::Texture ui_b;
    asw::Texture ui_up;

    // Danger images
    asw::Texture energyImage;
    asw::Texture asteroidImage;
    asw::Texture bombImage;
    asw::Texture cometImage;

    // Powerup Images
    asw::Texture powerStar;
    asw::Texture powerMagnet[4];

    // Declare sounds
    asw::Sample sound_orb;
    asw::Sample sound_bomb;
    asw::Sample sound_asteroid;
    asw::Sample sound_magnet;
    asw::Sample sound_star;
    asw::Sample sound_snap;

    // Music
    asw::Music music_ingame;
    asw::Music music_death;

    // Our robot
    Robot hectar;

    // Shakes the world, the HUD stays still
    asw::Camera camera;

    // Declare integers
    float scroll;
    int themeNumber;
    float arrow_animation;
    float motion;
    float ticker;

    // Declare booleans
    bool paused;
    bool take_screenshot { false };

    // Name entry for a new highscore
    asw::ui::Root ui;
    asw::ui::InputBox* name_input { nullptr };

    // Containers of objects
    std::vector<Energy> energys;
    std::vector<Debris> debries;
    std::vector<Powerup> powerups;
};
