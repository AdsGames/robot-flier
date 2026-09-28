/*
 * Energy
 * Gives the player health
 * A.D.S. Games
 */
#pragma once

#include <asw/asw.h>

#include "./robot.h"

class Energy : public asw::game::GameObject {
public:
    // Constructor
    Energy(asw::Texture sprite, asw::Sample sound, const asw::Vec2<float>& position);

    // Logic override
    void logic(const float motion, Robot* robot);

    // Draw through the camera
    using asw::game::GameObject::draw;
    void draw(const asw::Camera& camera) const;

    // Move towards robot
    void moveTowards(const asw::Vec2<float>& target, const float speed);

private:
    // Sound effect
    asw::Sample sound;

    // Texture
    asw::Texture texture;
};
