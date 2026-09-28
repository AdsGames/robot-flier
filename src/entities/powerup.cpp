#include "powerup.h"

// Constructor
Powerup::Powerup(asw::Texture sprite, asw::Sample sound, const asw::Vec2<float>& position,
    float timerLength, const int type)
    : type(type)
    , timerLength(timerLength)
    , sound(sound)
    , texture(sprite)
{
    transform.position = position;
    transform.size.x = static_cast<float>(sprite->w);
    transform.size.y = static_cast<float>(sprite->h);
}

// Logic loop!
void Powerup::logic(const float motion, Robot* robot)
{
    transform.position.x -= motion;

    if (this->alive && transform.collides(robot->getTransform())) {
        stats[STAT_POWERUPS] += 1;

        if (type == 1) {
            robot->setInvincibleTimer(timerLength);
        } else {
            robot->setMagneticTimer(timerLength);
        }

        asw::sound::play(sound);

        this->alive = false;
    }

    if (this->alive && transform.position.x + transform.size.x < 0) {
        this->alive = false;
    }
}

void Powerup::draw(const asw::Camera& camera) const
{
    if (this->alive) {
        asw::draw::sprite(texture, camera.world_to_screen(transform.position));
    }
}