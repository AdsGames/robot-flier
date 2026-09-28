/*
 * Robot Flier
 * A.D.S. Games
 * 03/01/2016
 * Robots in space!
 */
#include <asw/asw.h>

#include "./constants/controls.h"
#include "./constants/globals.h"
#include "./constants/settings.h"
#include "./states/game.h"
#include "./states/init.h"
#include "./states/intro.h"
#include "./states/menu.h"

// main function of program
int main(int argc, char* argv[])
{
    // Copy over the command line args
    settings.parseArgs(argc, argv);

    asw::core::init(SCREEN_W, SCREEN_H, 1);
    asw::core::print_info();
    controls::bind();

    // Starts Game
    auto app = asw::scene::SceneManager<Scenes>();
    app.register_scene<InitScene>(Scenes::Init, app);
    app.register_scene<IntroScene>(Scenes::Intro, app);
    app.register_scene<MenuScene>(Scenes::Menu, app);
    app.register_scene<GameScene>(Scenes::Game, app);
    app.set_next_scene(Scenes::Init);
    app.start();

    return 0;
}
