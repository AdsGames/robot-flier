/*
 * Init state
 * Allan Legemaate
 * 30/12/2016
 * Program inits here
 */
#pragma once

#include <asw/asw.h>

#include "../constants/globals.h"
#include "state.h"

class InitScene : public asw::scene::Scene<Scenes> {
public:
    using asw::scene::Scene<Scenes>::Scene;

    // Override parent
    void init() override;

    void update(float /*deltaTime*/) override
    {
        manager.set_next_scene(Scenes::Intro);
    }
};
