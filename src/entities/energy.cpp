#include "energy.h"

#include "../constants/globals.h"

// Constructor
Energy::Energy(asw::Texture sprite, asw::Sample sound, const asw::Vec2<float>& position)
    : sound(sound)
    , texture(sprite)
{
    transform.position = position;
    transform.size.x = static_cast<float>(sprite->w);
    transform.size.y = static_cast<float>(sprite->h);
}

// Game logic
void Energy::logic(const float motion, Robot* robot)
{
    transform.position.x -= motion;

    if (this->alive && transform.collides(robot->getTransform())) {
        score += 5;
        stats[STAT_ENERGY] += 1;

        if (robot->getHealth() < 100) {
            robot->addHealth(1);
        }

        asw::sound::play(sound);

        this->alive = false;
    }

    if (this->alive && transform.position.x + transform.size.x < 0) {
        this->alive = false;
    }
}

void Energy::draw(const asw::Camera& camera) const
{
    if (this->alive) {
        asw::draw::sprite(texture, camera.world_to_screen(transform.position));
    }
}

// Move towards robot
void Energy::moveTowards(const asw::Vec2<float>& target, const float speed)
{
    transform.position = asw::util::lerp(transform.position, target, speed / 200.0F);
}
