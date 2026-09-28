/*
 * Robot
 * Spacetoaster!
 * A.D.S. Games
 * 12/01/2017
 */
#pragma once

#include <iostream>
#include <vector>

#include "../constants/controls.h"
#include "../constants/globals.h"

class Robot {
public:
    Robot();
    Robot(const asw::Vec2<float>& position);

    void logic(float deltaTime);
    void draw(const asw::Camera& camera);
    void drawOverlay(const asw::Camera& camera);

    // Getters
    int getHealth() const;
    void addHealth(int amount);

    const asw::Quad<float>& getTransform() const
    {
        return transform;
    }

    bool isOnGround() const;
    bool isAlive() const;
    bool hasBegun() const;

    // Invincibility
    bool isInvincible() const;
    float getInvincibleTimer() const;
    void setInvincibleTimer(float time);

    // Magnetic
    bool isMagnetic() const;
    float getMagneticTimer() const;
    void setMagneticTimer(float time);

private:
    // Transform
    asw::Quad<float> transform;

    // Robot specific
    float gravity { 1.6F };
    float speed { 0.0F };
    bool alive { true };
    float invincibleTimer { 0 };
    float magneticTimer { 0 };
    bool rocket { false };
    bool onGround { false };
    int health { 100 };

    // Wait for keypress
    bool keyPressed { false };

    // Images
    asw::Texture mainRobot;
    asw::Texture robotFire;
    asw::Texture robotInvincible;
    asw::Texture robotInvincibleFire;
    asw::Texture robotInvincibleTop;
    asw::Texture robotDie;
    asw::Texture christmasHat;

    // Sounds
    asw::Sample soundFlame;
    asw::Sample soundHitground;

    // Particle emitters
    asw::ParticleEmitter emitter_left;
    asw::ParticleEmitter emitter_right;
    asw::ParticleEmitter emitter_smoke;
};
