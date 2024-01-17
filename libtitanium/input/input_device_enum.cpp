#include "input_device.hpp"

#include <SDL_mouse.h>
#include <SDL_scancode.h>
#include <libtitanium/util/static_array.hpp>
#include <libtitanium/util/string.hpp>

#include <SDL_gamecontroller.h>

namespace input
{
    // TODO: axis? axises? axes? idk
    const char *const szEKeyboardMouseAxisToString[] {
        "none", // EKeyboardMouseAxis::NONE

        "mouse_move_horizontal", // EKeyboardMouseAxis::MOUSE_MOVE_X
        "mouse_move_vertical", // EKeyboardMouseAxis::MOUSE_MOVE_Y
        "mouse_wheel" // EKeyboardMouseAxis::MOUSE_WHEEL
    };
    static_assert( util::StaticArray_Length( szEKeyboardMouseAxisToString ) == static_cast<int>( EKeyboardMouseAxis::_COUNT ) );

    EKeyboardMouseAxis EKeyboardMouseAxisFromString( const char *const pszAxis )
    {
        for ( uint i = 0; i < util::StaticArray_Length( szEKeyboardMouseAxisToString ); i++ )
        {
            if ( util::string::CStringsEqual( pszAxis, szEKeyboardMouseAxisToString[ i ] ) )
            {
                return static_cast<EKeyboardMouseAxis>( i );
            }
        }

        return EKeyboardMouseAxis::NONE;
    }

    const char * EKeyboardMouseAxisToString( const EKeyboardMouseAxis eAxis )
    {
        if ( eAxis >= EKeyboardMouseAxis::_COUNT )
        {
            return szEKeyboardMouseAxisToString[ static_cast<int>( EKeyboardMouseAxis::NONE ) ];
        }

        return szEKeyboardMouseAxisToString[ static_cast<int>( eAxis ) ];
    }


    const char *const szEKeyboardAndMouseButtonToString[] {
        "none", // EKeyboardMouseButton::NONE

        "mouse_left", // EKeyboardMouseButton::MOUSE_LEFT
        "mouse_right", // EKeyboardMouseButton::MOUSE_RIGHT
        "mouse_middle", // EKeyboardMouseButton::MOUSE_MIDDLE
        "mouse_ex1", // EKeyboardMouseButton::MOUSE_EX1
        "mouse_ex2", // EKeyboardMouseButton::MOUSE_EX2

        "kb_a", // EKeyboardMouseButton::KEYBOARD_A
        "kb_b", // EKeyboardMouseButton::KEYBOARD_B
        "kb_c", // EKeyboardMouseButton::KEYBOARD_C
        "kb_d", // EKeyboardMouseButton::KEYBOARD_D
        "kb_e", // EKeyboardMouseButton::KEYBOARD_E
        "kb_f", // EKeyboardMouseButton::KEYBOARD_F
        "kb_g", // EKeyboardMouseButton::KEYBOARD_G
        "kb_h", // EKeyboardMouseButton::KEYBOARD_H
        "kb_i", // EKeyboardMouseButton::KEYBOARD_I
        "kb_j", // EKeyboardMouseButton::KEYBOARD_J
        "kb_k", // EKeyboardMouseButton::KEYBOARD_K
        "kb_l", // EKeyboardMouseButton::KEYBOARD_L
        "kb_m", // EKeyboardMouseButton::KEYBOARD_M
        "kb_n", // EKeyboardMouseButton::KEYBOARD_N
        "kb_o", // EKeyboardMouseButton::KEYBOARD_O
        "kb_p", // EKeyboardMouseButton::KEYBOARD_P
        "kb_q", // EKeyboardMouseButton::KEYBOARD_Q
        "kb_r", // EKeyboardMouseButton::KEYBOARD_R
        "kb_s", // EKeyboardMouseButton::KEYBOARD_S
        "kb_t", // EKeyboardMouseButton::KEYBOARD_T
        "kb_u", // EKeyboardMouseButton::KEYBOARD_I
        "kb_v", // EKeyboardMouseButton::KEYBOARD_V
        "kb_w", // EKeyboardMouseButton::KEYBOARD_W
        "kb_x", // EKeyboardMouseButton::KEYBOARD_X
        "kb_y", // EKeyboardMouseButton::KEYBOARD_Y
        "kb_z", // EKeyboardMouseButton::KEYBOARD_Z
        "kb_1", // EKeyboardMouseButton::KEYBOARD_1
        "kb_2", // EKeyboardMouseButton::KEYBOARD_2
        "kb_3", // EKeyboardMouseButton::KEYBOARD_3
        "kb_4", // EKeyboardMouseButton::KEYBOARD_4
        "kb_5", // EKeyboardMouseButton::KEYBOARD_5
        "kb_6", // EKeyboardMouseButton::KEYBOARD_6
        "kb_7", // EKeyboardMouseButton::KEYBOARD_7
        "kb_8", // EKeyboardMouseButton::KEYBOARD_8
        "kb_9", // EKeyboardMouseButton::KEYBOARD_9
        "kb_0", // EKeyboardMouseButton::KEYBOARD_0

        "kb_uparrow", // EKeyboardMouseButton::KEYBOARD_UPARROW
        "kb_downarrow", // EKeyboardMouseButton::KEYBOARD_DOWNARROW
        "kb_leftarrow", // EKeyboardMouseButton::KEYBOARD_LEFTARROW
        "kb_rightarrow", // EKeyboardMouseButton::KEYBOARD_RIGHTARROW

        "kb_enter", // EKeyboardMouseButton::KEYBOARD_ENTER
        "kb_escape", // EKeyboardMouseButton::KEYBOARD_ESCAPE
        "kb_backspace", // EKeyboardMouseButton::KEYBOARD_BACKSPACE
        "kb_tab", // EKeyboardMouseButton::KEYBOARD_TAB
        "kb_space", // EKeyboardMouseButton::KEYBOARD_SPACE

        "kb_minus", // EKeyboardMouseButton::KEYBOARD_MINUS
        "kb_equals", // EKeyboardMouseButton::KEYBOARD_EQUALS
        "kb_leftbracket", // EKeyboardMouseButton::KEYBOARD_LEFTBRACKET
        "kb_rightbracket", // EKeyboardMouseButton::KEYBOARD_RIGHTBRACKET
        "kb_backslash", // EKeyboardMouseButton::KEYBOARD_BACKSLASH
        "kb_semicolon", // EKeyboardMouseButton::KEYBOARD_SEMICOLON
        "kb_apostrophe", // EKeyboardMouseButton::KEYBOARD_APOSTROPHE
        "kb_comma", // EKeyboardMouseButton::KEYBOARD_COMMA
        "kb_fullstop", // EKeyboardMouseButton::KEYBOARD_FULLSTOP
        "kb_forwardslash", // EKeyboardMouseButton::KEYBOARD_FORWARDSLASH
        "kb_grave", // EKeyboardMouseButton::KEYBOARD_GRAVE

        "kb_f1", // EKeyboardMouseButton::KEYBOARD_F1
        "kb_f2", // EKeyboardMouseButton::KEYBOARD_F2
        "kb_f3", // EKeyboardMouseButton::KEYBOARD_F3
        "kb_f4", // EKeyboardMouseButton::KEYBOARD_F4
        "kb_f5", // EKeyboardMouseButton::KEYBOARD_F5
        "kb_f6", // EKeyboardMouseButton::KEYBOARD_F6
        "kb_f7", // EKeyboardMouseButton::KEYBOARD_F7
        "kb_f8", // EKeyboardMouseButton::KEYBOARD_F8
        "kb_f9", // EKeyboardMouseButton::KEYBOARD_F9
        "kb_f10", // EKeyboardMouseButton::KEYBOARD_F10
        "kb_f11", // EKeyboardMouseButton::KEYBOARD_F11
        "kb_f12", // EKeyboardMouseButton::KEYBOARD_F12
        "kb_f13", // EKeyboardMouseButton::KEYBOARD_F13
        "kb_f14", // EKeyboardMouseButton::KEYBOARD_F14
        "kb_f15", // EKeyboardMouseButton::KEYBOARD_F15
        "kb_f16", // EKeyboardMouseButton::KEYBOARD_F16
        "kb_f17", // EKeyboardMouseButton::KEYBOARD_F17
        "kb_f18", // EKeyboardMouseButton::KEYBOARD_F18
        "kb_f19", // EKeyboardMouseButton::KEYBOARD_F19
        "kb_f20", // EKeyboardMouseButton::KEYBOARD_F20
        "kb_f21", // EKeyboardMouseButton::KEYBOARD_F21
        "kb_f22", // EKeyboardMouseButton::KEYBOARD_F22
        "kb_f23", // EKeyboardMouseButton::KEYBOARD_F23
        "kb_f24", // EKeyboardMouseButton::KEYBOARD_F24

        "kb_capslock", // EKeyboardMouseButton::KEYBOARD_CAPSLOCK
        "kb_ctrl", // EKeyboardMouseButton::KEYBOARD_LEFTCTRL
        "kb_rightctrl", // EKeyboardMouseButton::KEYBOARD_RIGHTCTRL
        "kb_shift", // EKeyboardMouseButton::KEYBOARD_LEFTSHIFT
        "kb_rightshift", // EKeyboardMouseButton::KEYBOARD_RIGHTSHIFT
        "kb_alt", // EKeyboardMouseButton::KEYBOARD_LEFTALT
        "kb_rightalt", // EKeyboardMouseButton::KEYBOARD_RIGHTALT
        "kb_super" // EKeyboardMouseButton::KEYBOARD_SUPER
    };
    static_assert( util::StaticArray_Length( szEKeyboardAndMouseButtonToString ) == static_cast<int>( EKeyboardMouseButton::_COUNT ) );

    int sdlEMouseButtonToSDLBitmask[] {
        0, // EKeyboardAndMouseButton::NONE

        SDL_BUTTON_LMASK, // EKeyboardAndMouseButton::MOUSE_LEFT
        SDL_BUTTON_RMASK, // EKeyboardAndMouseButton::MOUSE_RIGHT
        SDL_BUTTON_MMASK, // EKeyboardAndMouseButton::MOUSE_MIDDLE
        SDL_BUTTON_X1MASK, // EKeyboardAndMouseButton::MOUSE_EX1
        SDL_BUTTON_X2MASK  // EKeyboardAndMouseButton::MOUSE_EX2
    };

    static_assert( util::StaticArray_Length( sdlEMouseButtonToSDLBitmask ) == static_cast<int>( EKeyboardMouseButton::_MOUSE_MAX ) );

    SDL_Scancode sdlEKeyboardButtonToSDL[] {
        SDL_SCANCODE_UNKNOWN, // EKeyboardAndMouseButton::NONE

        SDL_SCANCODE_UNKNOWN, // EKeyboardMouseButton::MOUSE_LEFT
        SDL_SCANCODE_UNKNOWN, // EKeyboardMouseButton::MOUSE_RIGHT
        SDL_SCANCODE_UNKNOWN, // EKeyboardMouseButton::MOUSE_MIDDLE
        SDL_SCANCODE_UNKNOWN, // EKeyboardMouseButton::MOUSE_EX1
        SDL_SCANCODE_UNKNOWN, // EKeyboardMouseButton::MOUSE_EX2

        SDL_SCANCODE_A, // EKeyboardMouseButton::KEYBOARD_A
        SDL_SCANCODE_B, // EKeyboardMouseButton::KEYBOARD_B
        SDL_SCANCODE_C, // EKeyboardMouseButton::KEYBOARD_C
        SDL_SCANCODE_D, // EKeyboardMouseButton::KEYBOARD_D
        SDL_SCANCODE_E, // EKeyboardMouseButton::KEYBOARD_E
        SDL_SCANCODE_F, // EKeyboardMouseButton::KEYBOARD_F
        SDL_SCANCODE_G, // EKeyboardMouseButton::KEYBOARD_G
        SDL_SCANCODE_H, // EKeyboardMouseButton::KEYBOARD_H
        SDL_SCANCODE_I, // EKeyboardMouseButton::KEYBOARD_I
        SDL_SCANCODE_J, // EKeyboardMouseButton::KEYBOARD_J
        SDL_SCANCODE_K, // EKeyboardMouseButton::KEYBOARD_K
        SDL_SCANCODE_L, // EKeyboardMouseButton::KEYBOARD_L
        SDL_SCANCODE_M, // EKeyboardMouseButton::KEYBOARD_M
        SDL_SCANCODE_N, // EKeyboardMouseButton::KEYBOARD_N
        SDL_SCANCODE_O, // EKeyboardMouseButton::KEYBOARD_O
        SDL_SCANCODE_P, // EKeyboardMouseButton::KEYBOARD_P
        SDL_SCANCODE_Q, // EKeyboardMouseButton::KEYBOARD_Q
        SDL_SCANCODE_R, // EKeyboardMouseButton::KEYBOARD_R
        SDL_SCANCODE_S, // EKeyboardMouseButton::KEYBOARD_S
        SDL_SCANCODE_T, // EKeyboardMouseButton::KEYBOARD_T
        SDL_SCANCODE_U, // EKeyboardMouseButton::KEYBOARD_U
        SDL_SCANCODE_V, // EKeyboardMouseButton::KEYBOARD_V
        SDL_SCANCODE_W, // EKeyboardMouseButton::KEYBOARD_W
        SDL_SCANCODE_X, // EKeyboardMouseButton::KEYBOARD_X
        SDL_SCANCODE_Y, // EKeyboardMouseButton::KEYBOARD_Y
        SDL_SCANCODE_Z, // EKeyboardMouseButton::KEYBOARD_Z
        SDL_SCANCODE_1, // EKeyboardMouseButton::KEYBOARD_1
        SDL_SCANCODE_2, // EKeyboardMouseButton::KEYBOARD_2
        SDL_SCANCODE_3, // EKeyboardMouseButton::KEYBOARD_3
        SDL_SCANCODE_4, // EKeyboardMouseButton::KEYBOARD_4
        SDL_SCANCODE_5, // EKeyboardMouseButton::KEYBOARD_5
        SDL_SCANCODE_6, // EKeyboardMouseButton::KEYBOARD_6
        SDL_SCANCODE_7, // EKeyboardMouseButton::KEYBOARD_7
        SDL_SCANCODE_8, // EKeyboardMouseButton::KEYBOARD_8
        SDL_SCANCODE_9, // EKeyboardMouseButton::KEYBOARD_9
        SDL_SCANCODE_0, // EKeyboardMouseButton::KEYBOARD_0

        SDL_SCANCODE_UP, // EKeyboardMouseButton::KEYBOARD_UPARROW
        SDL_SCANCODE_DOWN, // EKeyboardMouseButton::KEYBOARD_DOWNARROW
        SDL_SCANCODE_LEFT, // EKeyboardMouseButton::KEYBOARD_LEFTARROW
        SDL_SCANCODE_RIGHT, // EKeyboardMouseButton::KEYBOARD_RIGHTARROW

        SDL_SCANCODE_RETURN, // EKeyboardMouseButton::KEYBOARD_ENTER
        SDL_SCANCODE_ESCAPE, // EKeyboardMouseButton::KEYBOARD_ESCAPE
        SDL_SCANCODE_BACKSPACE, // EKeyboardMouseButton::KEYBOARD_BACKSPACE
        SDL_SCANCODE_TAB, // EKeyboardMouseButton::KEYBOARD_TAB
        SDL_SCANCODE_SPACE, // EKeyboardMouseButton::KEYBOARD_SPACE

        SDL_SCANCODE_MINUS, // EKeyboardMouseButton::KEYBOARD_MINUS
        SDL_SCANCODE_EQUALS, // EKeyboardMouseButton::KEYBOARD_EQUALS
        SDL_SCANCODE_LEFTBRACKET, // EKeyboardMouseButton::KEYBOARD_LEFTBRACKET
        SDL_SCANCODE_RIGHTBRACKET, // EKeyboardMouseButton::KEYBOARD_RIGHTBRACKET
        SDL_SCANCODE_BACKSLASH, // EKeyboardMouseButton::KEYBOARD_BACKSLASH
        SDL_SCANCODE_SEMICOLON, // EKeyboardMouseButton::KEYBOARD_SEMICOLON
        SDL_SCANCODE_APOSTROPHE, // EKeyboardMouseButton::KEYBOARD_APOSTROPHE
        SDL_SCANCODE_COMMA, // EKeyboardMouseButton::KEYBOARD_COMMA
        SDL_SCANCODE_PERIOD, // EKeyboardMouseButton::KEYBOARD_FULLSTOP
        SDL_SCANCODE_SLASH, // EKeyboardMouseButton::KEYBOARD_FORWARDSLASH
        SDL_SCANCODE_GRAVE, // EKeyboardMouseButton::KEYBOARD_GRAVE

        SDL_SCANCODE_F1, // EKeyboardAndMouseButton::KEYBOARD_F1
        SDL_SCANCODE_F2, // EKeyboardAndMouseButton::KEYBOARD_F2
        SDL_SCANCODE_F3, // EKeyboardAndMouseButton::KEYBOARD_F3
        SDL_SCANCODE_F4, // EKeyboardAndMouseButton::KEYBOARD_F4
        SDL_SCANCODE_F5, // EKeyboardAndMouseButton::KEYBOARD_F5
        SDL_SCANCODE_F6, // EKeyboardAndMouseButton::KEYBOARD_F6
        SDL_SCANCODE_F7, // EKeyboardAndMouseButton::KEYBOARD_F7
        SDL_SCANCODE_F8, // EKeyboardAndMouseButton::KEYBOARD_F8
        SDL_SCANCODE_F9, // EKeyboardAndMouseButton::KEYBOARD_F9
        SDL_SCANCODE_F10, // EKeyboardAndMouseButton::KEYBOARD_F10
        SDL_SCANCODE_F11, // EKeyboardAndMouseButton::KEYBOARD_F11
        SDL_SCANCODE_F12, // EKeyboardAndMouseButton::KEYBOARD_F12
        SDL_SCANCODE_F13, // EKeyboardAndMouseButton::KEYBOARD_F13
        SDL_SCANCODE_F14, // EKeyboardAndMouseButton::KEYBOARD_F14
        SDL_SCANCODE_F15, // EKeyboardAndMouseButton::KEYBOARD_F15
        SDL_SCANCODE_F16, // EKeyboardAndMouseButton::KEYBOARD_F16
        SDL_SCANCODE_F17, // EKeyboardAndMouseButton::KEYBOARD_F17
        SDL_SCANCODE_F18, // EKeyboardAndMouseButton::KEYBOARD_F18
        SDL_SCANCODE_F19, // EKeyboardAndMouseButton::KEYBOARD_F19
        SDL_SCANCODE_F20, // EKeyboardAndMouseButton::KEYBOARD_F20
        SDL_SCANCODE_F21, // EKeyboardAndMouseButton::KEYBOARD_F21
        SDL_SCANCODE_F22, // EKeyboardAndMouseButton::KEYBOARD_F22
        SDL_SCANCODE_F23, // EKeyboardAndMouseButton::KEYBOARD_F23
        SDL_SCANCODE_F24, // EKeyboardAndMouseButton::KEYBOARD_F24

        SDL_SCANCODE_CAPSLOCK, // EKeyboardMouseButton::KEYBOARD_CAPSLOCK
        SDL_SCANCODE_LCTRL, // EKeyboardMouseButton::KEYBOARD_LEFTCTRL
        SDL_SCANCODE_RCTRL, // EKeyboardMouseButton::KEYBOARD_RIGHTCTRL
        SDL_SCANCODE_LSHIFT, // EKeyboardMouseButton::KEYBOARD_LEFTSHIFT
        SDL_SCANCODE_RSHIFT, // EKeyboardMouseButton::KEYBOARD_RIGHTSHIFT
        SDL_SCANCODE_LALT, // EKeyboardMouseButton::KEYBOARD_LEFTALT
        SDL_SCANCODE_RALT, // EKeyboardMouseButton::KEYBOARD_RIGHTALT
        SDL_SCANCODE_LGUI // EKeyboardMouseButton::KEYBOARD_SUPER
    };
    static_assert( util::StaticArray_Length( sdlEKeyboardButtonToSDL ) == static_cast<int>( EKeyboardMouseButton::_COUNT ) );

    EKeyboardMouseButton EKeyboardMouseButton_FromString( const char *const pszAxis )
    {
        for ( uint i = 0; i < util::StaticArray_Length( szEKeyboardAndMouseButtonToString ); i++ )
        {
            if ( util::string::CStringsEqual( pszAxis, szEKeyboardAndMouseButtonToString[ i ] ) )
            {
                return static_cast<EKeyboardMouseButton>( i );
            }
        }

        return EKeyboardMouseButton::NONE;
    }

    const char * EKeyboardMouseButton_ToString( const EKeyboardMouseButton eButton )
    {
        if ( eButton >= EKeyboardMouseButton::_COUNT )
        {
            return szEKeyboardAndMouseButtonToString[ static_cast<int>( EKeyboardMouseButton::NONE ) ];
        }

        return szEKeyboardAndMouseButtonToString[ static_cast<int>( eButton ) ];
    }

    bool EKeyboardMouseButton_IsMouseInput( const EKeyboardMouseButton eButton )
    {
        return eButton < EKeyboardMouseButton::_MOUSE_MAX;
    }

    SDL_Scancode EKeyboardMouseButton_ToSDLKeyboardScancode( const EKeyboardMouseButton eButton )
    {
        if ( eButton >= EKeyboardMouseButton::_COUNT )
        {
            return sdlEKeyboardButtonToSDL[ static_cast<int>( EKeyboardMouseButton::NONE ) ];
        }

        return sdlEKeyboardButtonToSDL[ static_cast<int>( eButton ) ];
    }

    uint EKeyboardMouseButton_ToSDLMouseButtonBitmask( const EKeyboardMouseButton eButton )
    {
        if ( eButton >= EKeyboardMouseButton::_MOUSE_MAX )
        {
            return sdlEMouseButtonToSDLBitmask[ static_cast<int>( EKeyboardMouseButton::NONE ) ];
        }

        return sdlEMouseButtonToSDLBitmask[ static_cast<int>( eButton ) ];
    }


    const char *const szEControllerAxisToString[] {
        "none", // EControllerAxis::NONE

        "leftstick_x", // EControllerAxis::LEFTSTICK_X
        "leftstick_y", // EControllerAxis::LEFTSTICK_Y
        "rightstick_x", // EControllerAxis::RIGHTSTICK_X
        "rightstick_y", // EControllerAxis::RIGHTSTICK_Y
        "trigger_left", // EControllerAxis::TRIGGER_LEFT
        "trigger_right" // EControllerAxis::TRIGGER_RIGHT
    };
    static_assert( util::StaticArray_Length( szEControllerAxisToString ) == static_cast<int>( EControllerAxis::_COUNT ) );

    EControllerAxis EControllerAxisFromString( const char *const pszAxis )
    {
        for ( uint i = 0; i < util::StaticArray_Length( szEControllerAxisToString ); i++ )
        {
            if ( util::string::CStringsEqual( pszAxis, szEControllerAxisToString[ i ] ) )
            {
                return static_cast<EControllerAxis>( i );
            }
        }

        return EControllerAxis::NONE;
    }

    const char * EControllerAxisToString( const EControllerAxis eAxis )
    {
        if ( eAxis >= EControllerAxis::_COUNT )
        {
            return szEControllerAxisToString[ static_cast<int>( EControllerAxis::NONE ) ];
        }

        return szEControllerAxisToString[ static_cast<int>( eAxis ) ];
    }

    SDL_GameControllerAxis sdlEControllerAxisToSDLAxis[] {
        SDL_CONTROLLER_AXIS_INVALID,

        SDL_CONTROLLER_AXIS_LEFTX,
        SDL_CONTROLLER_AXIS_LEFTY,
        SDL_CONTROLLER_AXIS_RIGHTX,
        SDL_CONTROLLER_AXIS_RIGHTY,
        SDL_CONTROLLER_AXIS_TRIGGERLEFT,
        SDL_CONTROLLER_AXIS_TRIGGERRIGHT
    };

    SDL_GameControllerAxis EControllerAxisToSDLAxis( const EControllerAxis eControllerAxis )
    {
        return sdlEControllerAxisToSDLAxis[ static_cast<int>( eControllerAxis ) ];
    }

    const char *const szEControllerButtonToString[] {
        "none", // EControllerButton::NONE,

        "a", // EControllerButton::A
        "b", // EControllerButton::B
        "x", // EControllerButton::X
        "y", // EControllerButton::Y
        "start", // EControllerButton::START
        "select", // EControllerButton::SELECT
        "guide", // EControllerButton::GUIDE
        "leftstick_in", // EControllerButton::LEFSTICK_IN
        "rightstick_in", // EControllerButton::RIGHTSTICK_IN
        "leftbumper", // EControllerButton::LEFTBUMPER
        "rightbumper", // EControllerButton::RIGHTBUMPER
        "dpad_up", // EControllerButton::DPAD_UP
        "dpad_down", // EControllerButton::DPAD_DOWN
        "dpad_left", // EControllerButton::DPAD_LEFT
        "dpad_right" // EControllerButton::DPAD_RIGHT
    };

    static_assert( util::StaticArray_Length( szEControllerButtonToString ) == static_cast<int>( EControllerButton::_COUNT ) );

    EControllerButton EControllerButtonFromString( const char *const pszButton )
    {
        for ( uint i = 0; i < util::StaticArray_Length( szEControllerButtonToString ); i++ )
        {
            if ( util::string::CStringsEqual( pszButton, szEControllerButtonToString[ i ] ) )
            {
                return static_cast<EControllerButton>( i );
            }
        }

        return EControllerButton::NONE;
    }

    const char * EControllerButtonToString( const EControllerButton eButton )
    {
        if ( eButton >= EControllerButton::_COUNT )
        {
            return szEControllerButtonToString[ static_cast<int>( EControllerButton::NONE ) ];
        }

        return szEControllerButtonToString[ static_cast<int>( eButton ) ];
    }

    SDL_GameControllerButton sdlEControllerButtonToSDLButton[] {
        SDL_CONTROLLER_BUTTON_INVALID,

        SDL_CONTROLLER_BUTTON_A,
        SDL_CONTROLLER_BUTTON_B,
        SDL_CONTROLLER_BUTTON_X,
        SDL_CONTROLLER_BUTTON_Y,
        SDL_CONTROLLER_BUTTON_START,
        SDL_CONTROLLER_BUTTON_BACK,
        SDL_CONTROLLER_BUTTON_GUIDE,
        SDL_CONTROLLER_BUTTON_LEFTSTICK,
        SDL_CONTROLLER_BUTTON_RIGHTSTICK,
        SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
        SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
        SDL_CONTROLLER_BUTTON_DPAD_UP,
        SDL_CONTROLLER_BUTTON_DPAD_DOWN,
        SDL_CONTROLLER_BUTTON_DPAD_LEFT,
        SDL_CONTROLLER_BUTTON_DPAD_RIGHT
    };

    static_assert( util::StaticArray_Length( sdlEControllerButtonToSDLButton ) == static_cast<int>( EControllerButton::_COUNT ) );

    SDL_GameControllerButton EControllerButtonToSDLButton( const EControllerButton eControllerButton )
    {
        return sdlEControllerButtonToSDLButton[ static_cast<int>( eControllerButton ) ];
    }

}
