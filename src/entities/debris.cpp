#include "debris.h"

#include "../constants/globals.h"

// Constructor
Debris::Debris(asw::Texture sprite, asw::Sample sound, const asw::Vec2<float>& position,
    const float damage, const float motionMultiplier, const float acceleration, const float size)
    : asw::game::GameObject()
    , motionMultiplier(motionMultiplier)
    , acceleration(acceleration)
    , damage(damage)
    , texture(sprite)
    , sound(sound)
{
    transform.position = position;

    if (size != -1) {
        transform.size.y = size * 8;
        transform.size.x = size * 10;
    } else {
        transform.size.x = static_cast<float>(sprite->w);
        transform.size.y = static_cast<float>(sprite->h);
    }
}

// Logic
void Debris::logic(const float motion, Robot* robot, const float deltaTime)
{
    GameObject::update(deltaTime);

    // Move across screen
    transform.position.x -= motion * motionMultiplier;
    motionMultiplier += acceleration;

    // Allow for some padding (since we use bounding box)
    const auto collisionBuffer = transform.size.y / 3.0F;

    // Collide with robot
    const auto offset = asw::Quad<float>(
        collisionBuffer, collisionBuffer, -collisionBuffer * 2.0F, -collisionBuffer * 2.0F);

    if (this->alive && !robot->isInvincible()
        && transform.collides(robot->getTransform() + offset)) {
        // Hurt robot
        robot->addHealth(-damage);

        // Shake it up
        screenshake += damage * 4;

        // Play sound
        asw::sound::play(sound);

        // Get hit
        this->alive = false;
        stats[STAT_DEBRIS] += 1;
    }

    // Offscreen
    if (this->alive && transform.position.x + transform.size.x < 0) {
        this->alive = false;
    }
}

void Debris::draw()
{
    if (this->alive) {
        asw::draw::stretch_sprite(texture, transform);
    }
}