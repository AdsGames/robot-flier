#include "settings.h"

#include <asw/asw.h>
#include <cstring>
#include <fstream>

Settings settings;

void Settings::load(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open()) {
        save(path);
        return;
    }

    int pt;
    int snd;
    int mus;
    int fs;
    int ss;
    int cm;

    if (file >> pt >> snd >> mus >> fs >> ss >> cm) {
        particleType = static_cast<ParticleType>(pt);
        sound = (snd != 0);
        music = (mus != 0);
        fullscreen = (fs != 0);
        screenshake = static_cast<ScreenShake>(ss);
        controlMode = static_cast<ControlMode>(cm);
    }
}

void Settings::save(const std::string& path) const
{
    std::ofstream file(path);
    file << static_cast<int>(particleType) << " " << static_cast<int>(sound) << " "
         << static_cast<int>(music) << " " << static_cast<int>(fullscreen) << " "
         << static_cast<int>(screenshake) << " " << static_cast<int>(controlMode) << "\n";
}

void Settings::parseArgs(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "mega") == 0) {
            mega = true;
        } else if (strcmp(argv[i], "supershake") == 0) {
            supershake = true;
        } else if (strcmp(argv[i], "merrychristmas") == 0) {
            christmas = true;
        } else if (strcmp(argv[i], "debug") == 0) {
            debug = true;
        }
    }
}

void Settings::applyAudio() const
{
    asw::sound::set_sfx_volume(sound ? 1 : 0);
    if (music) {
        asw::sound::set_music_volume(1);
    } else {
        asw::sound::set_music_volume(0);
        asw::sound::stop_music();
    }
}

void Settings::applyFullscreen() const
{
    asw::display::set_fullscreen(fullscreen);
}

void Settings::apply() const
{
    applyAudio();
    applyFullscreen();
}

void Settings::cycleParticleType()
{
    particleType = static_cast<ParticleType>((static_cast<int>(particleType) + 1) % 4);
}

void Settings::cycleSound()
{
    sound = !sound;
}

void Settings::cycleMusic()
{
    music = !music;
}

void Settings::cycleFullscreen()
{
    fullscreen = !fullscreen;
}

void Settings::cycleScreenShake()
{
    screenshake = static_cast<ScreenShake>((static_cast<int>(screenshake) + 1) % 4);
}

void Settings::cycleControlMode()
{
    controlMode = static_cast<ControlMode>((static_cast<int>(controlMode) + 1) % 3);
}
