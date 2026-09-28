#include "game.h"

#include <format>
#include <fstream>

// Constructor
void GameScene::init()
{
    using namespace asw::assets;

    // From globals
    score = 0;
    screenshake = 0;

    // Game related
    scroll = 0;
    motion = 5;
    themeNumber = 0;
    screenshake_x = 0;
    screenshake_y = 0;
    arrow_animation = 0.0F;
    paused = false;

    // End game menu
    edittext = "Player";
    iter = edittext.end();

    // Reset stats
    for (int i = 0; i < 4; i++) {
        stats[i] = 0;
    }

    // Sounds
    sound_bomb = load_sample("assets/audio/sound_bomb.wav");
    sound_orb = load_sample("assets/audio/sound_orb.wav");
    sound_asteroid = load_sample("assets/audio/sound_asteroid.wav");
    sound_magnet = load_sample("assets/audio/sound_magnet.wav");
    sound_star = load_sample("assets/audio/sound_star.wav");
    sound_snap = load_sample("assets/audio/sound_snap.wav");

    // Music
    music_death = load_music("assets/audio/music_death.ogg");
    music_ingame = load_music("assets/audio/music_ingame.ogg");

    // Images
    // Gui
    pauseMenu = load_texture("assets/images/gui/pauseMenu.png");
    ui_game_end = load_texture("assets/images/gui/ui_game_end.png");
    ui_a = load_texture("assets/images/gui/ui_a.png");
    ui_b = load_texture("assets/images/gui/ui_b.png");
    ui_up = load_texture("assets/images/gui/ui_up.png");
    debug = load_texture("assets/images/gui/debug.png");

    // Background
    space = load_texture("assets/images/backgrounds/space.png");

    // Nullfiy bitmaps not loaded yet
    screenshot = nullptr;
    asteroidImage = nullptr;
    parallaxBack = nullptr;
    groundOverlay = nullptr;
    groundUnderlay = nullptr;

    // Objects
    cometImage = load_texture("assets/images/objects/comet.png");
    powerStar = load_texture("assets/images/objects/powerStar.png");
    powerMagnet[0] = load_texture("assets/images/objects/powerMagnet.png");
    powerMagnet[1] = load_texture("assets/images/objects/powerMagnetTwo.png");
    powerMagnet[2] = load_texture("assets/images/objects/powerMagnetThree.png");
    powerMagnet[3] = load_texture("assets/images/objects/powerMagnetFour.png");

    if (settings.christmas) {
        energyImage = load_texture("assets/images/objects/energy_christmas.png");
        bombImage = load_texture("assets/images/objects/bomb_christmas.png");
    } else {
        energyImage = load_texture("assets/images/objects/energy.png");
        bombImage = load_texture("assets/images/objects/bomb.png");
    }

    // Sets the level to 1
    changeTheme(0);

    // Init hectar
    hectar = Robot({ 80, 300 });

    // Load scores
    highscores = ScoreTable("scores.dat");

    // Play music
    asw::sound::play_music(music_ingame);
}

// Themes
void GameScene::changeTheme(int theme)
{
    using namespace asw::assets;

    std::string themeName = "";

    if (theme == 0) {
        themeName = "moon";
    } else if (theme == 1) {
        themeName = "mars";
    } else if (theme == 2) {
        themeName = "sun";
    } else if (theme == 3) {
        themeName = "dark";
    }

    themeNumber = theme;

    // Replace textures
    groundOverlay = load_texture("assets/images/ground/groundOverlay_" + themeName + ".png");
    groundUnderlay = load_texture("assets/images/ground/groundUnderlay_" + themeName + ".png");
    parallaxBack = load_texture("assets/images/ground/paralax_" + themeName + ".png");

    if (settings.christmas) {
        asteroidImage = load_texture("assets/images/objects/asteroid_christmas.png");
    } else {
        asteroidImage = load_texture("assets/images/objects/asteroid_" + themeName + ".png");
    }
}

// Update logic of game
void GameScene::update(float deltaTime)
{
    using namespace asw::input;

    // Actual game stuff
    if (!paused) {
        // Add to ticker
        ticker += deltaTime;

        // Check if hectar has died between logic();
        const auto hectarAlive = hectar.isAlive();

        // Update robot
        hectar.logic(deltaTime);

        // If its different he died play music
        if (hectarAlive != hectar.isAlive()) {
            asw::sound::stop_music();
            asw::sound::play_music(music_death);
        }

        // Add to distance travelled
        stats[STAT_DISTANCE] += motion;

        // Changes speed
        if (hectar.isAlive() && hectar.hasBegun()) {
            motion = ((score / 36) + 6) * deltaTime * 62.5F;
        } else {
            motion *= 0.95F;
        }

        // Arrow animation
        arrow_animation += 0.15F * deltaTime * 62.5F;

        // No negative scores
        score = std::max(score, 0);

        // Scrolls background
        scroll -= motion;
        if (scroll / 6 + SCREEN_W <= 0) {
            scroll = 0;
        }

        // Change theme
        if (score > 199 && themeNumber == 0) {
            changeTheme(1);
        } else if (score > 399 && themeNumber == 1) {
            changeTheme(2);
        } else if (score > 600 && themeNumber == 2) {
            changeTheme(3);
        }

        // Energy
        for (auto& energy : energys) {
            energy.logic(motion, &hectar);

            // Magnet
            if (hectar.isMagnetic()) {
                const auto& target
                    = hectar.getTransform().position + hectar.getTransform().size / 2.0F;
                energy.moveTowards(target, (float)hectar.getMagneticTimer());
            }
        }

        std::erase_if(energys, [](const auto& energy) { return !energy.alive; });

        // Debries
        for (auto& debris : debries) {
            debris.logic(motion, &hectar, deltaTime);
        }

        std::erase_if(debries, [](const auto& debris) { return !debris.alive; });

        // Powerups
        for (auto& powerup : powerups) {
            powerup.logic(motion, &hectar);
        }

        std::erase_if(powerups, [](const auto& powerup) { return !powerup.alive; });

        // Spawning (every 0.1 seconds)
        if (hectar.isAlive() && hectar.hasBegun() && ticker > 0.1F) {
            // Subtract from ticker
            ticker -= 0.1F;

            gameTick();

            // Double fun
            if (settings.mega) {
                gameTick();
            }
        }

        // Lose scripts
        if (hectar.isOnGround()) {
            // Name input
            if (score > highscores.getScore(9) && asw::input::get_keyboard().any_pressed) {
                // Last key pressed
                int newkey = asw::input::get_keyboard().last_pressed;

                // Letters
                if (newkey >= SDL_SCANCODE_A && newkey <= SDL_SCANCODE_Z
                    && edittext.length() < 14) {
                    iter = edittext.insert(
                        iter, newkey + 96 - (asw::input::get_keyboard().down[SDL_SCANCODE_LSHIFT] * 32));
                    ++iter;
                }
                // Numbers
                else if (newkey >= SDL_SCANCODE_0 && newkey <= SDL_SCANCODE_9
                    && edittext.length() < 14) {
                    iter = edittext.insert(iter, newkey + 21);
                    ++iter;
                }
                // Some other, "special" key was pressed, handle it here
                else if (newkey == SDL_SCANCODE_BACKSPACE && iter != edittext.begin()) {
                    --iter;
                    iter = edittext.erase(iter);
                } else if (newkey == SDL_SCANCODE_RIGHT && iter != edittext.end()) {
                    ++iter;
                } else if (newkey == SDL_SCANCODE_LEFT && iter != edittext.begin()) {
                    --iter;
                }
            }

            if (get_key(Key::Return) || get_controller_button(0, ControllerButton::Start)
                || get_controller_button(0, ControllerButton::A)) {
                highscores.add(edittext, score);
                manager.set_next_scene(Scenes::Menu);
            }
        }
    }

    // Screenshot
    if (get_key_down(Key::F11) || get_controller_button_down(0, ControllerButton::Y)) {
        // Count screenshots
        int screenshotNumber;

        // Get current number
        std::ifstream read("screenshots/screenshot.dat");
        read >> screenshotNumber;
        read.close();

        // State new number
        std::ofstream write("screenshots/screenshot.dat");
        write << screenshotNumber + 1;
        write.close();

        // Save to file
        // TODO
        // al_save_bitmap((std::string("screenshots/screenshot_") +
        //                 std::to_string(screenshotNumber).c_str() + ".png")
        //                    .c_str(),
        //                al_get_backbuffer(display));

        // Snap sound
        asw::sound::play(sound_snap);
    }

    // Screen shake
    if (screenshake > 0 && settings.screenshakeMultiplier() != 0) {
        const auto shake_amount = screenshake * settings.screenshakeMultiplier()
            + 100 * static_cast<int>(settings.supershake);

        screenshake_x = screenshake_y = asw::random::between(-shake_amount, shake_amount);
        screenshake--;
    }

    if (screenshake <= 0 || !hectar.isAlive()) {
        screenshake_x = screenshake_y = 0;
    }

    // Random test stuff for devs
    if (settings.debug) {
        if (get_key(Key::R)) {
            score += 10;
        }

        if (get_key(Key::E) || get_controller_button(0, ControllerButton::B)) {
            hectar.addHealth(1);
        }

        if (get_key(Key::T)) {
            hectar.addHealth(-100);
        }
    }

    // Pause loop code
    if (get_key_down(Key::Escape) || get_mouse_button_down(MouseButton::Right)
        || get_key_down(Key::Space) || get_controller_button_down(0, ControllerButton::Start)) {
        if (paused) {
            paused = false;
        } else if (hectar.isAlive()) {
            paused = true;
        }
    }

    // Pause Menu Scripts
    if (paused) {
        const auto& quitQuad = asw::Quad<float>(220, 435, 60, 25);
        const auto& menuQuad = asw::Quad<float>(300, 435, 130, 25);
        const auto& resumeQuad = asw::Quad<float>(470, 435, 70, 25);

        if (get_mouse_button_down(MouseButton::Left)) {
            // Quit game
            if (quitQuad.contains(get_mouse().position)) {
                asw::core::exit();
            }

            // Menu
            if (menuQuad.contains(get_mouse().position)) {
                manager.set_next_scene(Scenes::Menu);
            }

            // Resume
            if (resumeQuad.contains(get_mouse().position)) {
                paused = false;
            }
        }
    }
}

// Spawning
void GameScene::gameTick()
{
    // Energy ball spawning
    if (asw::random::chance(0.08F)) {
        const auto position = asw::Vec2<float>(SCREEN_W, asw::random::between(30.0F, 550.0F));
        energys.emplace_back(energyImage, sound_orb, position);
    }

    // Asteroids spawning
    if (score >= 50 && asw::random::chance(0.04F)) {
        const auto position = asw::Vec2<float>(SCREEN_W, asw::random::between(30.0F, 400.0F));
        debries.emplace_back(
            asteroidImage, sound_asteroid, position, 5, 1.0F, 0.0F, asw::random::between(4, 20));
    }

    // Bomb spawning
    if (score >= 150 && asw::random::chance(0.1F)) {
        const auto position = asw::Vec2<float>(SCREEN_W, asw::random::between(30.0F, 550.0F));
        debries.emplace_back(bombImage, sound_bomb, position, 10, 1.0F, 0.01F);
    }

    // Comets spawning
    if (score >= 300 && asw::random::chance(0.05F)) {
        const auto position = asw::Vec2<float>(SCREEN_W, asw::random::between(30.0F, 550.0F));
        debries.emplace_back(cometImage, sound_asteroid, position, 5, 1.4F, 0.01F);
    }

    // Powerup spawning
    if (score >= 100 && asw::random::chance(0.01F)) {
        const auto position = asw::Vec2<float>(SCREEN_W, asw::random::between(30.0F, 600.0F));
        powerups.emplace_back(powerStar, sound_star, position, 5.0F, 1);
    }

    if (asw::random::chance(0.02F)) {
        const auto position = asw::Vec2<float>(SCREEN_W, asw::random::between(30.0F, 600.0F));
        if (score >= 500) {
            powerups.emplace_back(powerMagnet[3], sound_magnet, position, 15.0F, 13);
        } else if (score >= 300) {
            powerups.emplace_back(powerMagnet[2], sound_magnet, position, 10.0F, 12);
        } else if (score >= 200) {
            powerups.emplace_back(powerMagnet[1], sound_magnet, position, 7.5F, 11);
        } else if (score >= 100) {
            powerups.emplace_back(powerMagnet[0], sound_magnet, position, 5.0F, 10);
        }
    }
}

// Draw to screen
void GameScene::draw()
{
    using namespace asw::input;

    // Draw backgrounds and Ground Overlay
    asw::draw::sprite(space, asw::Vec2<float>(scroll / 6, 0));
    asw::draw::sprite(space, asw::Vec2<float>(scroll / 6 + SCREEN_W, 0));

    // Draw HUD
    // Info
    asw::draw::text(orbitron_30, std::format("Score:{}", score), asw::Vec2<float>(10, 10),
        asw::Color(255, 255, 255));

    asw::draw::text(orbitron_30, std::format("Health:{}", hectar.getHealth()),
        asw::Vec2<float>(10, 38), asw::Color(255, 255, 255));

    // Health bar
    const auto healthFloat = static_cast<float>(hectar.getHealth());
    auto healthColor = asw::Color(255, 0, 0);
    healthColor.r -= static_cast<int>(healthFloat * 2.5F);
    healthColor.g += static_cast<int>(healthFloat * 2.5F);
    asw::draw::rect_fill(asw::Quad<float>(10.0F, 68.0F, healthFloat * 1.7F, 10.0F), healthColor);

    // Power up timers
    if (hectar.isInvincible()) {
        asw::draw::circle_fill(asw::Vec2<float>(45, 105), 20, asw::Color(255, 255, 255));
        asw::draw::sprite(powerStar, asw::Vec2<float>(20, 80));
        asw::draw::text(orbitron_24, std::format("{:.0f}", hectar.getInvincibleTimer()),
            asw::Vec2<float>(44, 94), asw::Color(255, 255, 255), asw::TextJustify::Center);
        asw::draw::text(orbitron_24, std::format("{:.0f}", hectar.getInvincibleTimer()),
            asw::Vec2<float>(45, 96), asw::Color(255, 0, 0), asw::TextJustify::Center);
    }

    if (hectar.isMagnetic()) {
        asw::draw::circle_fill(asw::Vec2<float>(175, 105), 20, asw::Color(255, 255, 255));
        asw::draw::sprite(powerMagnet[0], asw::Vec2<float>(150, 80));
        asw::draw::text(orbitron_24, std::format("{:.0f}", hectar.getMagneticTimer()),
            asw::Vec2<float>(174, 94), asw::Color(255, 255, 255), asw::TextJustify::Center);
        asw::draw::text(orbitron_24, std::format("{:.0f}", hectar.getMagneticTimer()),
            asw::Vec2<float>(175, 96), asw::Color(255, 0, 0), asw::TextJustify::Center);
    }

    // Draw the debug window
    if (settings.debug) {
        asw::draw::sprite(debug, asw::Vec2<float>(0, 0));

        // Column 1
        asw::draw::text(orbitron_12, std::format("Motion:{:.2}", motion), asw::Vec2<float>(5, 25),
            asw::Color(255, 255, 255));
        asw::draw::text(orbitron_12, std::format("Robot X:{:.2}", hectar.getTransform().position.x),
            asw::Vec2<float>(5, 35), asw::Color(255, 255, 255));
        asw::draw::text(orbitron_12, std::format("Robot Y:{:.2}", hectar.getTransform().position.y),
            asw::Vec2<float>(5, 45), asw::Color(255, 255, 255));
        asw::draw::text(orbitron_12, std::format("Motion:{:.2}", motion), asw::Vec2<float>(5, 55),
            asw::Color(255, 255, 255));
        asw::draw::text(orbitron_12, std::format("Invincible:{}", hectar.getInvincibleTimer()),
            asw::Vec2<float>(5, 65), asw::Color(255, 255, 255));

        // Column 2
        asw::draw::text(orbitron_12, std::format("Score:{}", score), asw::Vec2<float>(120, 25),
            asw::Color(255, 255, 255));
        asw::draw::text(orbitron_12, std::format("Magnetic:{}", hectar.getMagneticTimer()),
            asw::Vec2<float>(120, 35), asw::Color(255, 255, 255));
        asw::draw::text(orbitron_12, std::format("Mouse X:{}", get_mouse().position.x),
            asw::Vec2<float>(120, 45), asw::Color(255, 255, 255));
        asw::draw::text(orbitron_12, std::format("Mouse Y:{}", get_mouse().position.y),
            asw::Vec2<float>(120, 55), asw::Color(255, 255, 255));
        asw::draw::text(orbitron_12,
            std::format("Particles On:{}", static_cast<int>(settings.particleType)),
            asw::Vec2<float>(120, 65), asw::Color(255, 255, 255));

        // Column 3
        asw::draw::text(orbitron_12, std::format("LowScore:{}", highscores.getScore(9)),
            asw::Vec2<float>(245, 25), asw::Color(255, 255, 255));
        asw::draw::text(orbitron_12, std::format("Theme:{}", themeNumber),
            asw::Vec2<float>(245, 35), asw::Color(255, 255, 255));
        asw::draw::text(orbitron_12, std::format("Energys:{}", energys.size()),
            asw::Vec2<float>(245, 45), asw::Color(255, 255, 255));
        asw::draw::text(orbitron_12, std::format("Debris:{}", debries.size()),
            asw::Vec2<float>(245, 55), asw::Color(255, 255, 255));
        asw::draw::text(orbitron_12, std::format("Powerups:{}", powerups.size()),
            asw::Vec2<float>(245, 65), asw::Color(255, 255, 255));

        // Column 4
        asw::draw::text(orbitron_12, std::format("Last key:{}", get_keyboard().last_pressed),
            asw::Vec2<float>(360, 25), asw::Color(255, 255, 255));
        asw::draw::text(orbitron_12,
            std::format("Has highscore:{}", score > highscores.getScore(9)),
            asw::Vec2<float>(360, 35), asw::Color(255, 255, 255));

        // FPS
        asw::draw::text(orbitron_18, std::format("FPS:%i", fps),
            asw::Vec2<float>(SCREEN_W - 100, 25), asw::Color(255, 255, 255));
    }

    // Scrolling
    const auto scroll_int = static_cast<int>(scroll);

    // Mountain Paralax
    asw::draw::sprite(parallaxBack, asw::Vec2<float>((scroll_int / 3) % SCREEN_W, 0));
    asw::draw::sprite(parallaxBack, asw::Vec2<float>((scroll_int / 3) % SCREEN_W + SCREEN_W, 0));

    // Ground
    asw::draw::sprite(groundUnderlay, asw::Vec2<float>(scroll_int % SCREEN_W, SCREEN_H - 40));
    asw::draw::sprite(
        groundUnderlay, asw::Vec2<float>(scroll_int % SCREEN_W + SCREEN_W, SCREEN_H - 40));

    // Energy
    for (auto& energy : energys) {
        energy.draw();
    }

    // Powerups
    for (auto& powerup : powerups) {
        powerup.draw();
    }

    // Draw robot
    hectar.draw();

    // Start arrow
    if (!hectar.hasBegun()) {
        if (asw::input::get_controller_count() > 0) {
            asw::draw::sprite(ui_a,
                hectar.getTransform().position
                    + asw::Vec2<float>(15, -60 - (sinf(arrow_animation) * 10)));
        } else {
            asw::draw::sprite(ui_up,
                hectar.getTransform().position
                    + asw::Vec2<float>(15, -70 - (sinf(arrow_animation) * 10)));
        }
    }

    // Debris
    for (auto& debris : debries) {
        debris.draw();
    }

    // Ground underlay
    asw::draw::sprite(groundOverlay, asw::Vec2<float>(scroll_int % SCREEN_W, SCREEN_H - 20));
    asw::draw::sprite(
        groundOverlay, asw::Vec2<float>(scroll_int % SCREEN_W + SCREEN_W, SCREEN_H - 20));

    // Robot above asteroids
    hectar.drawOverlay();

    // Lose scripts
    if (hectar.isOnGround()) {
        asw::draw::sprite(ui_game_end, asw::Vec2<float>(0, 0));

        asw::draw::text(orbitron_18, std::format("Final Score:{}", score),
            asw::Vec2<float>(130, 125), asw::Color(0, 0, 0));
        asw::draw::text(orbitron_18,
            std::format("Distance Flown: {} ft", stats[STAT_DISTANCE] / 10),
            asw::Vec2<float>(130, 165), asw::Color(0, 0, 0));
        asw::draw::text(orbitron_18, std::format("Energy Collected: {}", stats[STAT_ENERGY]),
            asw::Vec2<float>(130, 205), asw::Color(0, 0, 0));
        asw::draw::text(orbitron_18, std::format("Powerups Received: {}", stats[STAT_POWERUPS]),
            asw::Vec2<float>(130, 245), asw::Color(0, 0, 0));
        asw::draw::text(orbitron_18, std::format("Debris Collided: {}", stats[STAT_DEBRIS]),
            asw::Vec2<float>(130, 285), asw::Color(0, 0, 0));

        if (score > highscores.getScore(9)) {
            // Input rectangle
            asw::draw::rect_fill(
                asw::Quad<float>(
                    120, 388, asw::util::get_text_size(orbitron_24, edittext.c_str()).x + 18, 44),
                asw::Color(0, 0, 0));
            asw::draw::rect_fill(
                asw::Quad<float>(
                    122, 390, asw::util::get_text_size(orbitron_24, edittext.c_str()).x + 14, 40),
                asw::Color(255, 255, 255));

            // Textbox lable
            asw::draw::text(
                orbitron_18, "Enter your name:", asw::Vec2<float>(129, 370), asw::Color(0, 0, 0));

            // Output the string to the screen
            asw::draw::text(
                orbitron_24, edittext, asw::Vec2<float>(130, 390), asw::Color(255, 255, 255));

            // Draw the caret
            asw::draw::line(
                asw::Vec2<float>(
                    asw::util::get_text_size(orbitron_24,
                        edittext.substr(0, std::distance(edittext.begin(), iter)).c_str())
                            .x
                        + 130,
                    392),
                asw::Vec2<float>(
                    asw::util::get_text_size(orbitron_24,
                        edittext.substr(0, std::distance(edittext.begin(), iter)).c_str())
                            .x
                        + 130,
                    428),
                asw::Color(0, 0, 0));

            // Draw the congrats message
            asw::draw::text(
                orbitron_18, "New highscore!", asw::Vec2<float>(150, 330), asw::Color(0, 255, 0));
            asw::draw::text(orbitron_24, "Press Enter/   to continue", asw::Vec2<float>(150, 450),
                asw::Color(0, 0, 0));
            asw::draw::sprite(ui_b, asw::Vec2<float>(370, 450));
        } else {
            asw::draw::text(orbitron_24, "Press Enter/   to continue", asw::Vec2<float>(150, 395),
                asw::Color(0, 0, 0));
            asw::draw::sprite(ui_b, asw::Vec2<float>(370, 395));
        }
    }

    // Pause Menu Scripts
    if (paused) {
        // Menu
        asw::draw::sprite(pauseMenu, asw::Vec2<float>(130, 140));

        // Stats
        asw::draw::text(orbitron_18,
            std::format("Distance Flown: {} ft", stats[STAT_DISTANCE] / 10),
            asw::Vec2<float>(220, 250), asw::Color(255, 255, 255));
        asw::draw::text(orbitron_18, std::format("Energy Collected: {}", stats[STAT_ENERGY]),
            asw::Vec2<float>(220, 280), asw::Color(255, 255, 255));
        asw::draw::text(orbitron_18, std::format("Powerups Received: {}", stats[STAT_POWERUPS]),
            asw::Vec2<float>(220, 310), asw::Color(255, 255, 255));
        asw::draw::text(orbitron_18, std::format("Debris Collided: {}", stats[STAT_DEBRIS]),
            asw::Vec2<float>(220, 340), asw::Color(255, 255, 255));

        // Buttons
        asw::draw::text(orbitron_18, "Quit", asw::Vec2<float>(220, 445), asw::Color(0, 0, 0));
        asw::draw::text(orbitron_18, "Main Menu", asw::Vec2<float>(300, 445), asw::Color(0, 0, 0));
        asw::draw::text(orbitron_18, "Resume", asw::Vec2<float>(470, 445), asw::Color(0, 0, 0));
    }
}
