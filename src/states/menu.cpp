#include "menu.h"

#include <algorithm>
#include <format>
#include <string>
#include <vector>

// Construct state
void MenuScene::init()
{
    using namespace asw::assets;
    using namespace asw::random;

    // Init vars
    startClicked = false;

    // Load intro image
    // Random menu
    const auto background
        = std::format("assets/images/backgrounds/background_{}.png", between(0, 3));
    img_menu.set_texture(load_texture(background));

    title.set_texture(load_texture("assets/images/gui/title.png"));
    title.transform.position.x = 20;

    options.set_texture(load_texture("assets/images/gui/options.png"));

    credits.set_texture(load_texture("assets/images/gui/credits.png"));

    highscores_table.set_texture(load_texture("assets/images/gui/highscores_table.png"));
    highscores_table.transform.position = asw::Vec2<float>(200, 50);

    helpScreen.set_texture(load_texture("assets/images/gui/helpScreen.png"));

    xbox_start.set_texture(load_texture("assets/images/gui/xbox_start.png"));
    xbox_start.transform.position.y = 430;

    controls.set_texture(load_texture("assets/images/gui/controls.png"));

    // Load that menu music
    music_mainmenu = load_music("assets/audio/music_mainmenu.ogg");

    // Read settings from file
    settings.load();
    settings.apply();

    // Buttons
    build_ui();
    sync_options();
    open_screen(MINISTATE_MENU);

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

void MenuScene::build_ui()
{
    using asw::assets::load_texture;

    if (start != nullptr) {
        return;
    }

    // Image only buttons, fire on release
    auto add_button = [](asw::ui::Root& ui, const std::string& path, asw::Vec2<float> position) {
        auto& button = ui.root.add_child<asw::ui::Button>();
        button.set_images(load_texture(path));
        button.transform.position = position;
        return &button;
    };

    // Image on / off toggles, checked is the on image
    auto add_toggle = [](asw::ui::Root& ui, const std::string& off, const std::string& on,
                          asw::Vec2<float> position) {
        auto& toggle = ui.root.add_child<asw::ui::Checkbox>();
        toggle.set_images(load_texture(off));
        toggle.texture_checked = load_texture(on);
        toggle.transform.position = position;
        return &toggle;
    };

    // Image options that move to the next value on click
    auto add_choice = [](asw::ui::Root& ui, const std::vector<std::string>& paths,
                          asw::Vec2<float> position) {
        auto& choice = ui.root.add_child<asw::ui::Choice>();
        for (const auto& path : paths) {
            choice.images.push_back(load_texture(path));
        }
        choice.set_images(choice.images.front());
        choice.transform.position = position;

        // Options sit in a grid, so left and right move focus
        choice.adjust_on_left_right = false;
        return &choice;
    };

    menu_ui.ctx.navigation = controls::ui_navigation();
    options_ui.ctx.navigation = controls::ui_navigation();

    // Main menu, positions animate in update
    start = add_button(menu_ui, "assets/images/gui/start.png", { 0, 400 });
    start->on_click = [this]() { start_game(); };
    menu_ui.ctx.focus.default_focus = start;

    highscores_button = add_button(menu_ui, "assets/images/gui/highscores.png", { 0, 30 });
    highscores_button->on_click = [this]() { open_screen(MINISTATE_SCORES); };

    ui_credits = add_button(menu_ui, "assets/images/gui/ui_credits.png", { 541, 0 });
    ui_credits->on_click = [this]() { open_screen(MINISTATE_CREDITS); };

    ui_controls = add_button(menu_ui, "assets/images/gui/ui_controls.png", { 645, 0 });
    ui_controls->on_click = [this]() { open_screen(MINISTATE_CONTROLS); };

    ui_help = add_button(menu_ui, "assets/images/gui/ui_help.png", { 697, 0 });
    ui_help->on_click = [this]() { open_screen(MINISTATE_TUTORIAL); };

    ui_options = add_button(menu_ui, "assets/images/gui/ui_options.png", { 749, 0 });
    ui_options->on_click = [this]() { open_screen(MINISTATE_OPTIONS); };

    // Options menu, back leaves it
    options_ui.on_back = [this]() { open_screen(MINISTATE_MENU); };

    ui_sound = add_toggle(options_ui, "assets/images/gui/ui_sound_off.png",
        "assets/images/gui/ui_sound_on.png", { 120, 180 });
    ui_sound->on_change = [](bool checked) {
        settings.sound = checked;
        settings.save();
        settings.applyAudio();
    };

    ui_music = add_toggle(options_ui, "assets/images/gui/ui_music_off.png",
        "assets/images/gui/ui_music_on.png", { 280, 180 });
    ui_music->on_change = [this](bool checked) {
        settings.music = checked;
        settings.save();
        settings.applyAudio();
        if (settings.music && !asw::sound::is_music_playing()) {
            asw::sound::play_music(music_mainmenu);
        }
    };

    ui_exit = add_button(options_ui, "assets/images/gui/ui_exit.png", { 540, 180 });
    ui_exit->on_click = []() { asw::core::exit(); };

    // Order matches ControlMode
    ui_control = add_choice(options_ui,
        { "assets/images/gui/ui_control_xbox.png", "assets/images/gui/ui_control_keyboard.png",
            "assets/images/gui/ui_control_auto.png" },
        { 120, 295 });
    ui_control->on_change = [](std::size_t index) {
        settings.controlMode = static_cast<ControlMode>(index);
        settings.save();
    };

    // Order matches ScreenShake
    ui_screenshake = add_choice(options_ui,
        { "assets/images/gui/ui_screenshake_none.png", "assets/images/gui/ui_screenshake_low.png",
            "assets/images/gui/ui_screenshake_medium.png",
            "assets/images/gui/ui_screenshake_high.png" },
        { 280, 295 });
    ui_screenshake->on_change = [](std::size_t index) {
        settings.screenshake = static_cast<ScreenShake>(index);
        settings.save();
    };

    // The image shows the mode the button switches to
    ui_window = add_toggle(options_ui, "assets/images/gui/ui_window_fullscreen.png",
        "assets/images/gui/ui_window_windowed.png", { 120, 407 });
    ui_window->on_change = [](bool checked) {
        settings.fullscreen = checked;
        settings.save();
        settings.applyFullscreen();
    };

    // Order matches ParticleType
    ui_particle = add_choice(options_ui,
        { "assets/images/gui/ui_particle_circle.png", "assets/images/gui/ui_particle_square.png",
            "assets/images/gui/ui_particle_pixel.png", "assets/images/gui/ui_particle_off.png" },
        { 280, 407 });
    ui_particle->on_change = [](std::size_t index) {
        settings.particleType = static_cast<ParticleType>(index);
        settings.save();
    };

    ui_back = add_button(options_ui, "assets/images/gui/ui_back.png", { 540, 407 });
    ui_back->on_click = [this]() { open_screen(MINISTATE_MENU); };
}

void MenuScene::sync_options()
{
    ui_sound->checked = settings.sound;
    ui_music->checked = settings.music;
    ui_window->checked = settings.fullscreen;
    ui_particle->select(static_cast<std::size_t>(settings.particleType));
    ui_screenshake->select(static_cast<std::size_t>(settings.screenshake));
    ui_control->select(static_cast<std::size_t>(settings.controlMode));
}

void MenuScene::open_screen(int screen)
{
    mini_screen = screen;

    // So Return and A start the game again
    if (screen == MINISTATE_MENU) {
        menu_ui.focus(*start);
    }
}

void MenuScene::start_game()
{
    startClicked = true;
    animation_ticker = ANIMATION_DURATION;
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
    ui_credits->transform.position.y = ease(S_H_F, S_H_F - 52.0F, t, ease_func);
    ui_controls->transform.position.y = ease(S_H_F, S_H_F - 52.0F, t, ease_func);
    ui_help->transform.position.y = ease(S_H_F, S_H_F - 52.0F, t, ease_func);
    ui_options->transform.position.y = ease(S_H_F, S_H_F - 52.0F, t, ease_func);
    start->transform.position.x = ease(-400.0F, 40.0F, t, ease_func);
    xbox_start.transform.position.x = ease(-400.0F, 40.0F, t, ease_func);
    highscores_button->transform.position.x = ease(S_W_F, S_W_F - 138.0F, t, ease_func);

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
            open_screen(MINISTATE_MENU);
        }
    }
    // Main menu, Return and controller A activate the focused button
    else if (mini_screen == MINISTATE_MENU) {
        menu_ui.update();
    }
    // Options
    else if (mini_screen == MINISTATE_OPTIONS) {
        options_ui.update();
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

    // Start, highscores and bottom right buttons
    menu_ui.draw();

    // Joystick Mode
    if (settings.controlMode != ControlMode::Keyboard && asw::input::get_controller_count() > 0) {
        xbox_start.draw();
    }

    // Nice title image
    title.draw();

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

        // Buttons, exit and back
        options_ui.draw();

        // Button Text
        asw::draw::text(orbitron_24, "Sounds         Music                            Exit",
            asw::Vec2<float>(110, 154), asw::Color(255, 250, 250));
        asw::draw::text(orbitron_24, "Input      Screen Shake", asw::Vec2<float>(126, 268),
            asw::Color(255, 250, 250));
        asw::draw::text(orbitron_24, "Window       Particles                        Back",
            asw::Vec2<float>(108, 382), asw::Color(255, 250, 250));
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
