/*
 * Powerup
 * Various powerups do various things
 * A.D.S. Games
 */
#pragma once

#include <asw/asw.h>

#include "./robot.h"

class Powerup : public asw::game::GameObject {
public:
    // Constructor
    Powerup(asw::Texture sprite, asw::Sample sound, const asw::Vec2<float>& position,
        const float timerLength, const int type);

    // Logic override
    void logic(const float motion, Robot* robot);

    // Draw through the camera
    using asw::game::GameObject::draw;
    void draw(const asw::Camera& camera) const;

private:
    int type;
    float timerLength;
    asw::Sample sound;
    asw::Texture texture;
};
