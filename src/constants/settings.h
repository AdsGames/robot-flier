/*
 * Game settings management
 * A.D.S. Games
 */
#pragma once

#include <string>

enum class ParticleType : int {
    Circle = 0,
    Square = 1,
    Pixel = 2,
    Off = 3,
};

enum class ScreenShake : int {
    None = 0,
    Low = 1,
    Medium = 2,
    High = 3,
};

enum class ControlMode : int {
    Xbox = 0,
    Keyboard = 1,
    Auto = 2,
};

class Settings {
public:
    // Persistent settings (saved to file)
    ParticleType particleType = ParticleType::Circle;
    bool sound = true;
    bool music = true;
    bool fullscreen = false;
    ScreenShake screenshake = ScreenShake::Medium;
    ControlMode controlMode = ControlMode::Xbox;

    // Runtime-only settings (set via CLI args, never persisted)
    bool debug = false;
    bool mega = false;
    bool supershake = false;
    bool christmas = false;

    // File I/O
    void load(const std::string& path = "assets/data/settings.dat");
    void save(const std::string& path = "assets/data/settings.dat") const;

    // Args parsing (for CLI options)
    void parseArgs(int argc, char* argv[]);

    // Apply audio/display settings to the engine
    void applyAudio() const;
    void applyFullscreen() const;
    void apply() const;

    // Cycling helpers for the options menu
    void cycleParticleType();
    void cycleScreenShake();
    void cycleControlMode();

    // Convenience queries
    bool particlesEnabled() const
    {
        return particleType != ParticleType::Off;
    }
    int screenshakeMultiplier() const
    {
        return static_cast<int>(screenshake);
    }
};

extern Settings settings;
