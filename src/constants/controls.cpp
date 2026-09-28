#include "controls.h"

#include <asw/asw.h>

void controls::bind()
{
    using asw::input::ControllerButton;
    using asw::input::ControllerButtonBinding;
    using asw::input::Key;
    using asw::input::KeyBinding;
    using asw::input::MouseButton;
    using asw::input::MouseButtonBinding;

    constexpr auto any = asw::input::ANY_CONTROLLER;

    asw::input::bind_action(FLY, KeyBinding { Key::W });
    asw::input::bind_action(FLY, KeyBinding { Key::Up });
    asw::input::bind_action(FLY, MouseButtonBinding { MouseButton::Left });
    asw::input::bind_action(FLY, ControllerButtonBinding { ControllerButton::A, any });
    asw::input::bind_action(FLY, ControllerButtonBinding { ControllerButton::LeftPaddle1, any });

    asw::input::bind_action(PAUSE, KeyBinding { Key::Escape });
    asw::input::bind_action(PAUSE, KeyBinding { Key::Space });
    asw::input::bind_action(PAUSE, MouseButtonBinding { MouseButton::Right });
    asw::input::bind_action(PAUSE, ControllerButtonBinding { ControllerButton::Start, any });

    asw::input::bind_action(CONFIRM, KeyBinding { Key::Return });
    asw::input::bind_action(CONFIRM, ControllerButtonBinding { ControllerButton::A, any });
    asw::input::bind_action(CONFIRM, ControllerButtonBinding { ControllerButton::Start, any });

    asw::input::bind_action(SCREENSHOT, KeyBinding { Key::F11 });
    asw::input::bind_action(SCREENSHOT, ControllerButtonBinding { ControllerButton::Y, any });
}
