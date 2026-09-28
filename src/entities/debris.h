/*
 * Debris
 * A collision object that can be crashed into
 * A.D.S. Games
 */
#pragma once

#include <asw/asw.h>

#include "./robot.h"

class Debris : public asw::game::GameObject {
public:
    // Constructor
    Debris(asw::Texture sprite, asw::Sample sound, const asw::Vec2<float>& position,
        const float damage, const float motionMultiplier = 1.0f, const float acceleration = 0.0f,
        const float size = -1);

    // Logic override
    void logic(const float motion, Robot* robot, const float deltaTime);

    // Draw through the camera
    using asw::game::GameObject::draw;
    void draw(const asw::Camera& camera) const;

private:
    // Motion multiplier
    float motionMultiplier;
    float acceleration;
    float damage;

    // Texture
    asw::Texture texture;

    // Sound
    asw::Sample sound;
};
