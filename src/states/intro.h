/*
 * Intro
 * A.D.S. Games
 * 30/12/2016
 * A pretty nice splash screen
 */
#pragma once

#include "../constants/controls.h"
#include "../constants/globals.h"
#include "state.h"

class IntroScene : public asw::scene::Scene<Scenes> {
public:
    // Construct/deconstruct
    using asw::scene::Scene<Scenes>::Scene;

    // Override parent
    void init() override;
    void update(float deltaTime) override;
    void draw() override;

private:
    // Splash image
    asw::game::Sprite img_intro;

    float timer;
};
