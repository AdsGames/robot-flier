#include "menu.h"

#include <algorithm>
#include <format>

// Construct state
void MenuScene::init()
{
    using namespace asw::assets;
    using namespace asw::random;

    // Init vars
    startClicked = false;

    // Screen on
    mini_screen = MINISTATE_MENU;

    // Load intro image
    // Random menu
    const auto background
        = std::format("assets/images/backgrounds/background_{}.png", between(0, 3));
    img_menu.set_texture(load_texture(background));

    start.set_texture(load_texture("assets/images/gui/start.png"));
    start.transform.position.y = 400;

    highscores_button.set_texture(load_texture("assets/images/gui/highscores.png"));
    highscores_button.transform.position.y = 30;

    title.set_texture(load_texture("assets/images/gui/title.png"));
    title.transform.position.x = 20;

    options.set_texture(load_texture("assets/images/gui/options.png"));

    ui_sound[1].set_texture(load_texture("assets/images/gui/ui_sound_on.png"));
    ui_sound[0].set_texture(load_texture("assets/images/gui/ui_sound_off.png"));

    ui_sound[0].transform.position = asw::Vec2<float>(120, 180);
    ui_sound[1].transform.position = asw::Vec2<float>(120, 180);

    ui_music[1].set_texture(load_texture("assets/images/gui/ui_music_on.png"));
    ui_music[0].set_texture(load_texture("assets/images/gui/ui_music_off.png"));

    ui_music[0].transform.position = asw::Vec2<float>(280, 180);
    ui_music[1].transform.position = asw::Vec2<float>(280, 180);

    ui_window[1].set_texture(load_texture("assets/images/gui/ui_window_windowed.png"));
    ui_window[0].set_texture(load_texture("assets/images/gui/ui_window_fullscreen.png"));

    ui_window[0].transform.position = asw::Vec2<float>(120, 407);
    ui_window[1].transform.position = asw::Vec2<float>(120, 407);

    ui_particle[0].set_texture(load_texture("assets/images/gui/ui_particle_circle.png"));
    ui_particle[1].set_texture(load_texture("assets/images/gui/ui_particle_square.png"));
    ui_particle[2].set_texture(load_texture("assets/images/gui/ui_particle_pixel.png"));
    ui_particle[3].set_texture(load_texture("assets/images/gui/ui_particle_off.png"));

    ui_particle[0].transform.position = asw::Vec2<float>(280, 407);
    ui_particle[1].transform.position = asw::Vec2<float>(280, 407);
    ui_particle[2].transform.position = asw::Vec2<float>(280, 407);
    ui_particle[3].transform.position = asw::Vec2<float>(280, 407);

    ui_control[0].set_texture(load_texture("assets/images/gui/ui_control_xbox.png"));
    ui_control[1].set_texture(load_texture("assets/images/gui/ui_control_keyboard.png"));
    ui_control[2].set_texture(load_texture("assets/images/gui/ui_control_auto.png"));

    ui_control[0].transform.position = asw::Vec2<float>(120, 295);
    ui_control[1].transform.position = asw::Vec2<float>(120, 295);
    ui_control[2].transform.position = asw::Vec2<float>(120, 295);

    ui_screenshake[0].set_texture(load_texture("assets/images/gui/ui_screenshake_none.png"));
    ui_screenshake[1].set_texture(load_texture("assets/images/gui/ui_screenshake_low.png"));
    ui_screenshake[2].set_texture(load_texture("assets/images/gui/ui_screenshake_medium.png"));
    ui_screenshake[3].set_texture(load_texture("assets/images/gui/ui_screenshake_high.png"));

    ui_screenshake[0].transform.position = asw::Vec2<float>(280, 295);
    ui_screenshake[1].transform.position = asw::Vec2<float>(280, 295);
    ui_screenshake[2].transform.position = asw::Vec2<float>(280, 295);
    ui_screenshake[3].transform.position = asw::Vec2<float>(280, 295);

    ui_back.set_texture(load_texture("assets/images/gui/ui_back.png"));
    ui_back.transform.position = asw::Vec2<float>(540, 407);

    credits.set_texture(load_texture("assets/images/gui/credits.png"));

    highscores_table.set_texture(load_texture("assets/images/gui/highscores_table.png"));
    highscores_table.transform.position = asw::Vec2<float>(200, 50);

    ui_help.set_texture(load_texture("assets/images/gui/ui_help.png"));
    ui_help.transform.position.x = 697;

    ui_controls.set_texture(load_texture("assets/images/gui/ui_controls.png"));
    ui_controls.transform.position.x = 645;

    ui_credits.set_texture(load_texture("assets/images/gui/ui_credits.png"));
    ui_credits.transform.position.x = 541;

    ui_options.set_texture(load_texture("assets/images/gui/ui_options.png"));
    ui_options.transform.position.x = 749;

    helpScreen.set_texture(load_texture("assets/images/gui/helpScreen.png"));

    ui_exit.set_texture(load_texture("assets/images/gui/ui_exit.png"));
    ui_exit.transform.position = asw::Vec2<float>(540, 180);

    xbox_start.set_texture(load_texture("assets/images/gui/xbox_start.png"));
    xbox_start.transform.position.y = 430;

    controls.set_texture(load_texture("assets/images/gui/controls.png"));

    // Load that menu music
    music_mainmenu = load_music("assets/audio/music_mainmenu.ogg");

    // Read settings from file
    settings.load();
    settings.apply();

    // Load scores
    highscores = ScoreTable("scores.dat");

    // Play music
    asw::sound::play_music(music_mainmenu);

    // Setup particle emitter
    asw::ParticleConfig config;
    config.lifetime_min = 0.5F;
    config.lifetime_max = 2.0F;
    config.speed_min = 5.0F;
    config.speed_max = 50.0F;
    config.color_start = { 255, 200, 50, 255 };
    config.color_end = { 255, 50, 0, 0 };
    config.size_start = 6.0F;
    config.size_end = 1.0F;
    config.gravity = { 0.0F, 0.1F };

    // Create emitter in a scene
    emitter = asw::ParticleEmitter(config, 512);
    emitter.set_emission_rate(0.0F);
    emitter.start();
}

// Update loop
void MenuScene::update(float deltaTime)
{
    using namespace asw::easing;

    animation_ticker += startClicked ? -deltaTime : deltaTime;
    auto t = std::clamp(animation_ticker / ANIMATION_DURATION, 0.0F, 1.0F);
    auto ease_func = startClicked ? ease_in_expo : ease_out_elastic;

    // Animation
    title.transform.position.y = ease(-100.0F, 20.0F, t, ease_func);
    ui_credits.transform.position.y = ease(S_H_F, S_H_F - 52.0F, t, ease_func);
    ui_controls.transform.position.y = ease(S_H_F, S_H_F - 52.0F, t, ease_func);
    ui_help.transform.position.y = ease(S_H_F, S_H_F - 52.0F, t, ease_func);
    ui_options.transform.position.y = ease(S_H_F, S_H_F - 52.0F, t, ease_func);
    start.transform.position.x = ease(-400.0F, 40.0F, t, ease_func);
    xbox_start.transform.position.x = ease(-400.0F, 40.0F, t, ease_func);
    highscores_button.transform.position.x = ease(S_W_F, S_W_F - 138.0F, t, ease_func);

    // Start the game
    if (startClicked && t <= 0.01F) {
        manager.set_next_scene(Scenes::Game);
    }

    // Exit menus
    if (mini_screen == MINISTATE_TUTORIAL || mini_screen == MINISTATE_CREDITS
        || mini_screen == MINISTATE_CONTROLS || mini_screen == MINISTATE_SCORES) {
        if (asw::input::get_keyboard().any_pressed
            || asw::input::get_mouse_button_down(asw::input::MouseButton::Left)
            || asw::input::get_action_down(controls::CONFIRM)) {
            mini_screen = MINISTATE_MENU;
        }
    }

    // Open submenu or start game
    else if (mini_screen == MINISTATE_MENU) {
        // Start game with keyboard or controller
        if (asw::input::get_action_down(controls::CONFIRM)) {
            startClicked = true;
            animation_ticker = ANIMATION_DURATION;
        }

        // Buttons
        if (asw::input::get_mouse_button_down(asw::input::MouseButton::Left)) {
            // Start game
            if (start.transform.contains(asw::input::get_mouse().position)) {
                startClicked = true;
                animation_ticker = ANIMATION_DURATION;
            }
            // Scores
            else if (highscores_button.transform.contains(asw::input::get_mouse().position)) {
                mini_screen = MINISTATE_SCORES;
            }
            // Credits menu
            else if (ui_credits.transform.contains(asw::input::get_mouse().position)) {
                mini_screen = MINISTATE_CREDITS;
            }
            // Controls menu
            else if (ui_controls.transform.contains(asw::input::get_mouse().position)) {
                mini_screen = MINISTATE_CONTROLS;
            }
            // Help screen
            else if (ui_help.transform.contains(asw::input::get_mouse().position)) {
                mini_screen = MINISTATE_TUTORIAL;
            }
            // Options menu
            else if (ui_options.transform.contains(asw::input::get_mouse().position)) {
                mini_screen = MINISTATE_OPTIONS;
            }
        }
    }

    // Options
    if (mini_screen == MINISTATE_OPTIONS
        && asw::input::get_mouse_button_down(asw::input::MouseButton::Left)) {
        // Particles toggle
        if (ui_particle[0].transform.contains(asw::input::get_mouse().position)) {
            settings.cycleParticleType();
            settings.save();
        }
        // Sound button toggle
        else if (ui_sound[0].transform.contains(asw::input::get_mouse().position)) {
            settings.cycleSound();
            settings.save();
            settings.applyAudio();
        }
        // Music button toggle
        else if (ui_music[0].transform.contains(asw::input::get_mouse().position)) {
            settings.cycleMusic();
            settings.save();
            settings.applyAudio();
            if (settings.music && !asw::sound::is_music_playing()) {
                asw::sound::play_music(music_mainmenu);
            }
        }
        // Fullscreen toggle
        else if (ui_window[0].transform.contains(asw::input::get_mouse().position)) {
            settings.cycleFullscreen();
            settings.save();
            settings.applyFullscreen();
        }
        // Screen shake
        else if (ui_screenshake[0].transform.contains(asw::input::get_mouse().position)) {
            settings.cycleScreenShake();
            settings.save();
        }
        // Control Toggle
        else if (ui_control[0].transform.contains(asw::input::get_mouse().position)) {
            settings.cycleControlMode();
            settings.save();
        }
        // Power off
        else if (ui_exit.transform.contains(asw::input::get_mouse().position)) {
            asw::core::exit();
        }
        // Exit menu
        else if (ui_back.transform.contains(asw::input::get_mouse().position)) {
            mini_screen = MINISTATE_MENU;
        }
    }

    // Update mouse particles
    if (settings.particlesEnabled() && asw::input::get_mouse().change.y < 0) {
        emitter.set_emission_rate(200.0F);
    } else {
        emitter.set_emission_rate(0.0F);
    }

    // Update emitter
    emitter.transform.position = asw::input::get_mouse().position;
    emitter.update(deltaTime);
}

// Draw to screen
void MenuScene::draw()
{
    // Menu Background
    img_menu.draw();

    // Start button
    start.draw();

    // Highscores button
    highscores_button.draw();

    // Joystick Mode
    if (settings.controlMode != ControlMode::Keyboard && asw::input::get_controller_count() > 0) {
        xbox_start.draw();
    }

    // Nice title image
    title.draw();

    // Bottom Right Buttons
    ui_credits.draw();
    ui_controls.draw();
    ui_help.draw();
    ui_options.draw();

    // Draw scores
    if (mini_screen == MINISTATE_SCORES) {
        // Highscore background
        highscores_table.draw();

        // Title
        asw::draw::text(orbitron_36, "Highscores", asw::Vec2<float>(400, 75), asw::Color(0, 0, 0),
            asw::TextJustify::Center);

        // Read the top 10 scores
        for (int i = 0; i < 10; i++) {
            asw::draw::text(orbitron_24, std::to_string(highscores.getScore(i)),
                asw::Vec2<float>(225, (i * 40) + 130), asw::Color(0, 0, 0));
            asw::draw::text(orbitron_18, highscores.getName(i),
                asw::Vec2<float>(575, (i * 40) + 132), asw::Color(0, 0, 0),
                asw::TextJustify::Right);
        }
    }
    // Tutorial screen
    else if (mini_screen == MINISTATE_TUTORIAL) {
        helpScreen.draw();
    }
    // Credits screen
    else if (mini_screen == MINISTATE_CREDITS) {
        credits.draw();
    }
    // Credits screen
    else if (mini_screen == MINISTATE_CONTROLS) {
        controls.draw();
    }
    // Option Menu drawing(page and ingame)
    else if (mini_screen == MINISTATE_OPTIONS) {
        // Background
        options.draw();

        // Buttons
        ui_particle[static_cast<int>(settings.particleType)].draw();
        ui_sound[settings.sound ? 1 : 0].draw();
        ui_music[settings.music ? 1 : 0].draw();
        ui_window[settings.fullscreen ? 1 : 0].draw();
        ui_screenshake[static_cast<int>(settings.screenshake)].draw();
        ui_control[static_cast<int>(settings.controlMode)].draw();

        // Button Text
        asw::draw::text(orbitron_24, "Sounds         Music                            Exit",
            asw::Vec2<float>(110, 154), asw::Color(255, 250, 250));
        asw::draw::text(orbitron_24, "Input      Screen Shake", asw::Vec2<float>(126, 268),
            asw::Color(255, 250, 250));
        asw::draw::text(orbitron_24, "Window       Particles                        Back",
            asw::Vec2<float>(108, 382), asw::Color(255, 250, 250));

        // Exit and back
        ui_exit.draw();
        ui_back.draw();
    }

    // Debug
    if (settings.debug) {
        // Joystick testing
        if (asw::input::get_controller_count() > 0) {
            for (auto i = 0; i < asw::input::NUM_CONTROLLER_BUTTONS; i++) {
                const auto button = static_cast<asw::input::ControllerButton>(i);
                asw::draw::text(orbitron_12,
                    std::format("Joystick {}: {}", i, asw::input::get_controller_button(0, button)),
                    asw::Vec2<float>(120, 25 + (20 * i)), asw::Color(255, 255, 255));
            }
        }

        // FPS
        asw::draw::text(orbitron_12, std::format("FPS:{}", fps),
            asw::Vec2<float>(SCREEN_W - 100, 20), asw::Color(255, 255, 255));
    }

    // Draw particle emitter
    emitter.draw();
}
