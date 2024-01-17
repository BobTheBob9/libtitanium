#pragma once

#include <libtitanium/util/data/span.hpp>
#include <libtitanium/util/maths.hpp>

#include <SDL_gamecontroller.h>
#include <SDL_events.h>

namespace input
{
    enum class EKeyboardMouseAxis
    {
        NONE = 0,

        MOUSE_MOVE_X,
        MOUSE_MOVE_Y,
        MOUSE_WHEEL,

        _COUNT
    };

    EKeyboardMouseAxis EKeyboardMouseAxisFromString( const char *const pszAxis );
    const char * EKeyboardMouseAxisToString( const EKeyboardMouseAxis eAxis );

    enum class EKeyboardMouseButton
    {
        NONE = 0,

        MOUSE_LEFT,
        MOUSE_RIGHT,
        MOUSE_MIDDLE,
        MOUSE_EX1,
        MOUSE_EX2,

        _MOUSE_MAX,

        KEYBOARD_A = _MOUSE_MAX,
        KEYBOARD_B,
        KEYBOARD_C,
        KEYBOARD_D,
        KEYBOARD_E,
        KEYBOARD_F,
        KEYBOARD_G,
        KEYBOARD_H,
        KEYBOARD_I,
        KEYBOARD_J,
        KEYBOARD_K,
        KEYBOARD_L,
        KEYBOARD_M,
        KEYBOARD_N,
        KEYBOARD_O,
        KEYBOARD_P,
        KEYBOARD_Q,
        KEYBOARD_R,
        KEYBOARD_S,
        KEYBOARD_T,
        KEYBOARD_U,
        KEYBOARD_V,
        KEYBOARD_W,
        KEYBOARD_X,
        KEYBOARD_Y,
        KEYBOARD_Z,
        KEYBOARD_1,
        KEYBOARD_2,
        KEYBOARD_3,
        KEYBOARD_4,
        KEYBOARD_5,
        KEYBOARD_6,
        KEYBOARD_7,
        KEYBOARD_8,
        KEYBOARD_9,
        KEYBOARD_0,

        KEYBOARD_UPARROW,
        KEYBOARD_DOWNARROW,
        KEYBOARD_LEFTARROW,
        KEYBOARD_RIGHTARROW,

        KEYBOARD_ENTER,
        KEYBOARD_ESCAPE,
        KEYBOARD_BACKSPACE,
        KEYBOARD_TAB,
        KEYBOARD_SPACE,

        KEYBOARD_MINUS,
        KEYBOARD_EQUALS,
        KEYBOARD_LEFTBRACKET,
        KEYBOARD_RIGHTBRACKET,
        KEYBOARD_BACKSLASH,
        KEYBOARD_SEMICOLON,
        KEYBOARD_APOSTROPHE,
        KEYBOARD_COMMA,
        KEYBOARD_FULLSTOP,
        KEYBOARD_FORWARDSLASH,
        KEYBOARD_GRAVE,

        KEYBOARD_F1,
        KEYBOARD_F2,
        KEYBOARD_F3,
        KEYBOARD_F4,
        KEYBOARD_F5,
        KEYBOARD_F6,
        KEYBOARD_F7,
        KEYBOARD_F8,
        KEYBOARD_F9,
        KEYBOARD_F10,
        KEYBOARD_F11,
        KEYBOARD_F12,
        KEYBOARD_F13,
        KEYBOARD_F14,
        KEYBOARD_F15,
        KEYBOARD_F16,
        KEYBOARD_F17,
        KEYBOARD_F18,
        KEYBOARD_F19,
        KEYBOARD_F20,
        KEYBOARD_F21,
        KEYBOARD_F22,
        KEYBOARD_F23,
        KEYBOARD_F24,

        KEYBOARD_CAPSLOCK,
        KEYBOARD_LEFTCTRL,
        KEYBOARD_RIGHTCTRL,
        KEYBOARD_LEFTSHIFT,
        KEYBOARD_RIGHTSHIFT,
        KEYBOARD_LEFTALT,
        KEYBOARD_RIGHTALT,
        KEYBOARD_SUPER,

        _COUNT
    };

    EKeyboardMouseButton EKeyboardMouseButton_FromString( const char *const pszButton );
    const char * EKeyboardMouseButton_ToString( const EKeyboardMouseButton eButton );
    bool EKeyboardMouseButton_IsMouseInput( const EKeyboardMouseButton eButton );
    SDL_Scancode EKeyboardMouseButton_ToSDLKeyboardScancode( const EKeyboardMouseButton eButton );

    enum class EControllerAxis
    {
        NONE = 0,

        LEFTSTICK_X,
        LEFTSTICK_Y,
        RIGHTSTICK_X,
        RIGHTSTICK_Y,
        TRIGGER_LEFT,
        TRIGGER_RIGHT,

        _COUNT
    };

    EControllerAxis EControllerAxisFromString( const char *const pszAxis );
    const char * EControllerAxisToString( const EControllerAxis eAxis );
    SDL_GameControllerAxis EControllerAxisToSDLAxis( const EControllerAxis eControllerAxis );

    enum class EControllerButton
    {
        NONE = 0,

        A,
        B,
        X,
        Y,
        START,
        SELECT,
        GUIDE,
        LEFTSTICK_IN,
        RIGHTSTICK_IN,
        LEFTBUMPER,
        RIGHTBUMPER,
        DPAD_UP,
        DPAD_DOWN,
        DPAD_LEFT,
        DPAD_RIGHT,

        _COUNT
    };

    EControllerButton EControllerButtonFromString( const char *const pszButton );
    const char * EControllerButtonAxisToString( const EControllerButton eButton );
    SDL_GameControllerButton EControllerButtonToSDLButton( const EControllerButton eControllerButton );
    uint EKeyboardMouseButton_ToSDLMouseButtonBitmask( const EKeyboardMouseButton eButton );

    enum class EInputDeviceType
    {
        INVALID = 0,
        KEYBOARD_MOUSE,
        CONTROLLER
    };

    struct KeyboardMouseData
    {
        util::maths::Vec2<i32> vMouseMovement;
        float flScrollMovement;
    };

    struct InputDevice
    {
        EInputDeviceType eDeviceType;

        union {
            SDL_GameController * pSDLGameController;
            KeyboardMouseData keyboardAndMouseData;
        };
    };
    void InputDevice_InitialiseKeyboard( InputDevice *const pInputDevice );

    struct AnalogueBinding
    {
        EKeyboardMouseButton eKBButtonPos;
        EKeyboardMouseButton eKBButtonNeg;
        EKeyboardMouseAxis eKBAxis;
        EControllerButton eControllerButtonPos;
        EControllerButton eControllerButtonNeg;
        EControllerAxis eControllerAxis;
    };

    struct DigitalBinding
    {
        EKeyboardMouseButton eKBButton;
        EKeyboardMouseAxis eKBAxis;
        EControllerButton eControllerButton;
        EControllerAxis eControllerAxis;
    };

    void SetupSDL();
    bool ProcessSDLInputEvent( const SDL_Event *const pSdlEvent, util::data::Span<InputDevice> sInputDevices );
    void ProcessAnalogueActions( util::data::Span<InputDevice> sMulticastInputDevices, const util::data::Span<AnalogueBinding> sInputBindings, util::data::Span<f32> o_snAnalogueActionState, const float flsecFrameTime );
    void ProcessDigitalActions( util::data::Span<InputDevice> sMulticastInputDevices, const util::data::Span<DigitalBinding> sInputBindings, util::data::Span<u8> o_snDigitalActionState );
    void PostProcess( util::data::Span<InputDevice> sInputDevices );
}
