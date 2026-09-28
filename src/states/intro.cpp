#include "intro.h"

// Construct state
void IntroScene::init()
{
    // Load intro image
    img_intro.set_texture(asw::assets::load_texture("assets/images/backgrounds/intro.png"));
    timer = 0.0F;
}

// Update (goto menu!)
void IntroScene::update(float deltaTime)
{
    timer += deltaTime;

    // Go to menu
    if (timer >= 3.0F || asw::input::get_keyboard().any_pressed) {
        manager.set_next_scene(Scenes::Menu);
    }
}

void IntroScene::draw()
{
    img_intro.draw();
}
