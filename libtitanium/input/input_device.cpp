#include "input_device.hpp"
#include "input_actions.hpp"

#include <libtitanium/config/config.hpp>
#include <libtitanium/util/assert.hpp>
#include <libtitanium/util/maths.hpp>
#include <libtitanium/logger/logger.hpp>

// TODO: i'm wrapping this in TITANIUM_SDL, but honestly with how heavily this is tied to sdl stuff it might be better just to make a new file for it
#if TITANIUM_SDL
    #include <SDL.h>
    #include <SDL_mouse.h>
    #include <SDL_events.h>
    #include <SDL_keyboard.h>
    #include <SDL_gamecontroller.h>
#endif // #if TITANIUM_SDL

namespace input
{
    bool s_bEnableController = true; config::Var * pcvarEnableController = config::RegisterVar( "input::controllerenable", config::EFVarUsageFlags::NONE, config::VARF_FLOAT, &s_bEnableController );

    // TODO: should just be outputting i16s for analogue, so shouldn't be using float for this setting
    // maybe provide helpers for float deadzone => i16 deadzone?
    f32 s_flControllerDeadzone = 0.005; config::Var * pcvarControllerDeadzone = config::RegisterVar( "input::controllerdeadzone", config::EFVarUsageFlags::NONE, config::VARF_FLOAT, &s_flControllerDeadzone );

    // mirrors the source/quake default value for m_yaw and m_pitch, we prescale our mouse input by this so we can mirror idtech/source sensitivities
    constexpr float BASE_MOUSE_SENSITIVITY_SCALE = 0.022f;

    f32 s_flMouseYawScale = 1.f; config::Var * pcvarMouseYawScale = config::RegisterVar( "input::mouseyawscale", config::EFVarUsageFlags::NONE, config::VARF_FLOAT, &s_flMouseYawScale );
    f32 s_flMousePitchScale = 1.f; config::Var * pcvarMousePitchScale = config::RegisterVar( "input::mousepitchscale", config::EFVarUsageFlags::NONE, config::VARF_FLOAT, &s_flMousePitchScale );

    // value taken from winquake's joy_pitchsensitivity var
    constexpr float BASE_JOYSTICK_SENSITIVITY_SCALE = 150.f;

    // default value taken from winquake's joy_pitchsensitivity var
    f32 s_flJoystickScale = 1.f; config::Var * pcvarJoystickScale = config::RegisterVar( "input::joystickscale", config::EFVarUsageFlags::NONE, config::VARF_FLOAT, &s_flJoystickScale );

#if TITANIUM_SDL
    void SetupSDL()
    {
        SDL_SetRelativeMouseMode( SDL_TRUE );
    }

    void InputDevice_InitialiseKeyboard( InputDevice *const pInputDevice )
    {
        *pInputDevice = { .eDeviceType = EInputDeviceType::KEYBOARD_MOUSE, .pSDLGameController = nullptr };
    }

    bool FindKeyboardAndMouse( const InputDevice *const pCurrentValue, const InputDevice *const pUserComparator )
    {
        (void)pUserComparator;
        return pCurrentValue->eDeviceType == EInputDeviceType::KEYBOARD_MOUSE;
    }

    bool ProcessSDLInputEvent( const SDL_Event *const pSdlEvent, util::data::Span<InputDevice> sInputDevices )
    {
        InputDevice * pKeyboardAndMouse = util::data::Span<InputDevice>::Find( &sInputDevices, FindKeyboardAndMouse, nullptr );

        switch ( pSdlEvent->type )
        {
            case SDL_CONTROLLERDEVICEADDED:
            {
                SDL_GameController *const pSDLGameController = SDL_GameControllerOpen( pSdlEvent->cdevice.which );

                for ( uint i = 0; i < sInputDevices.nLength; i++ )
                {
                    if ( sInputDevices.pData[ i ].eDeviceType == EInputDeviceType::INVALID )
                    {
                        sInputDevices.pData[ i ] = { .eDeviceType = EInputDeviceType::CONTROLLER, .pSDLGameController = pSDLGameController };

                        logger::Info( "Connected new controller \"%s\" at index %i" ENDL, SDL_GameControllerName( pSDLGameController ), i );
                        return true;
                    }
                }

                logger::Info( "Tried to connect new joystick, but code has run out of controller slots! (%i available)" ENDL, sInputDevices.nLength );
                return true;
            }

            case SDL_CONTROLLERDEVICEREMOVED:
            {
                SDL_GameController *const pSDLGameController = SDL_GameControllerFromInstanceID( pSdlEvent->cdevice.which );

                for ( uint i = 0; i < sInputDevices.nLength; i++ )
                {
                    if ( sInputDevices.pData[ i ].eDeviceType == EInputDeviceType::CONTROLLER && sInputDevices.pData[ i ].pSDLGameController == pSDLGameController )
                    {
                        logger::Info( "Disconnecting controller \"%s\" from index %i" ENDL, SDL_GameControllerName( pSDLGameController ), i );
                        SDL_GameControllerClose( pSDLGameController );
                        sInputDevices.pData[ i ] = { .eDeviceType = EInputDeviceType::INVALID, .pSDLGameController = nullptr };
                        break;
                    }
                }

                // shouldn't be possible to hit
                return true;
            }

            case SDL_MOUSEMOTION:
            {
                if ( pKeyboardAndMouse && SDL_GetRelativeMouseMode() )
                {
                    pKeyboardAndMouse->keyboardAndMouseData.vMouseMovement.x += pSdlEvent->motion.xrel;
                    pKeyboardAndMouse->keyboardAndMouseData.vMouseMovement.y += -pSdlEvent->motion.yrel;
                }

                return true;
            }

            case SDL_MOUSEWHEEL:
            {
                if ( pKeyboardAndMouse )
                {
                    pKeyboardAndMouse->keyboardAndMouseData.flScrollMovement += pSdlEvent->wheel.preciseY;
                }

                return true;
            }

            default: return false;
        }
    }

    // TODO: consolidate input/axis status funcs into individual functions that we can use across both ProcessAnalogueActions and ProcessDigitalActions

    void ProcessAnalogueActions( util::data::Span<InputDevice> sMulticastInputDevices, const util::data::Span<AnalogueBinding> sInputBindings, util::data::Span<f32> o_snAnalogueActionState, const float flsecFrameTime )
    {
        assert::Debug( sInputBindings.nLength == o_snAnalogueActionState.nLength );
        memset( o_snAnalogueActionState.pData, 0, o_snAnalogueActionState.nLength * sizeof( f32 ) );

        for ( uint i = 0; i < sMulticastInputDevices.nLength; i++ )
        {
            InputDevice *const pInputDevice = &sMulticastInputDevices.pData[ i ];
            if ( pInputDevice->eDeviceType == EInputDeviceType::INVALID )
            {
                continue;
            }

            if ( pInputDevice->eDeviceType == EInputDeviceType::CONTROLLER && !s_bEnableController )
            {
                continue;
            }

            for ( uint j = 0; j < sInputBindings.nLength; j++ )
            {
                bool bScaleToFrameTime = true;
                f32 flValue = 0;

                if ( pInputDevice->eDeviceType == EInputDeviceType::KEYBOARD_MOUSE )
                {
                    if ( sInputBindings.pData[ j ].eKBAxis != EKeyboardMouseAxis::NONE )
                    {
                        switch ( sInputBindings.pData[ j ].eKBAxis )
                        {
                            case EKeyboardMouseAxis::MOUSE_WHEEL:
                            {
                                flValue += pInputDevice->keyboardAndMouseData.flScrollMovement;
                                bScaleToFrameTime = false;
                                break;
                            }

                            // TODO: these are busted
                            case EKeyboardMouseAxis::MOUSE_MOVE_X:
                            {
                                flValue += pInputDevice->keyboardAndMouseData.vMouseMovement.x * BASE_MOUSE_SENSITIVITY_SCALE * s_flMouseYawScale;
                                bScaleToFrameTime = false;
                                break;
                            }

                            case EKeyboardMouseAxis::MOUSE_MOVE_Y:
                            {
                                flValue += pInputDevice->keyboardAndMouseData.vMouseMovement.y * BASE_MOUSE_SENSITIVITY_SCALE * s_flMousePitchScale;
                                bScaleToFrameTime = false;
                                break;
                            }

                            default: break;
                        }
                    }
                    else
                    {
                        // TODO: mouse button and axis support
                        const u8* const pbSDLKeyStates = SDL_GetKeyboardState( nullptr );

                        EKeyboardMouseButton ePosButton = sInputBindings.pData[ j ].eKBButtonPos;
                        if ( EKeyboardMouseButton_IsMouseInput( ePosButton ) )
                        {
                            if ( SDL_GetMouseState( nullptr, nullptr ) & EKeyboardMouseButton_ToSDLMouseButtonBitmask( sInputBindings.pData[ i ].eKBButtonPos ) )
                            {
                                flValue += MAX_JOYSTICK;
                            }
                        }
                        else
                        {
                            if ( pbSDLKeyStates[ EKeyboardMouseButton_ToSDLKeyboardScancode( ePosButton ) ] )
                            {
                                flValue += MAX_JOYSTICK;
                            }
                        }

                        EKeyboardMouseButton eNegButton = sInputBindings.pData[ j ].eKBButtonNeg;
                        if ( EKeyboardMouseButton_IsMouseInput( eNegButton ) )
                        {
                            if ( SDL_GetMouseState( nullptr, nullptr ) & EKeyboardMouseButton_ToSDLMouseButtonBitmask( sInputBindings.pData[ i ].eKBButtonPos ) )
                            {
                                flValue += MIN_JOYSTICK;
                            }
                        }
                        else
                        {
                            if ( pbSDLKeyStates[ EKeyboardMouseButton_ToSDLKeyboardScancode( eNegButton ) ] )
                            {
                                flValue += MIN_JOYSTICK;
                            }
                        }
                    }
                }
                else if ( pInputDevice->eDeviceType == EInputDeviceType::CONTROLLER )
                {
                    if ( SDL_GameControllerGetButton( pInputDevice->pSDLGameController, EControllerButtonToSDLButton( sInputBindings.pData[ j ].eControllerButtonPos ) ) )
                    {
                        flValue += MAX_JOYSTICK;
                    }
                    else if ( SDL_GameControllerGetButton( pInputDevice->pSDLGameController, EControllerButtonToSDLButton(sInputBindings.pData[ j ].eControllerButtonNeg ) ) )
                    {
                        flValue += MIN_JOYSTICK;
                    }
                    else
                    {
                        SDL_GameControllerAxis sdlGameControllerAxis = EControllerAxisToSDLAxis( sInputBindings.pData[ j ].eControllerAxis );
                        int nValue = SDL_GameControllerGetAxis( pInputDevice->pSDLGameController, sdlGameControllerAxis );

                        if ( abs( nValue ) < maxof( i16 ) * s_flControllerDeadzone )
                        {
                            nValue = 0;
                        }
                        // SDL gives joystick y values as negative-up, we use positive-up y values for axis, so reverse them
                        else if ( sdlGameControllerAxis == SDL_CONTROLLER_AXIS_LEFTY || sdlGameControllerAxis == SDL_CONTROLLER_AXIS_RIGHTY )
                        {
                            nValue = -nValue;
                        }

                        flValue = static_cast<float>( nValue ) / static_cast<float>( maxof( i16 ) );
                    }

                }

                if ( fabs( flValue ) > fabs( o_snAnalogueActionState.pData[ j ] ) )
                {
                    if ( bScaleToFrameTime )
                    {
                        flValue *= flsecFrameTime * BASE_JOYSTICK_SENSITIVITY_SCALE * s_flJoystickScale;
                    }

                    o_snAnalogueActionState.pData[ j ] = flValue;
                }
            }
        }
    }

    void ProcessDigitalActions( util::data::Span<InputDevice> sMulticastInputDevices, const util::data::Span<DigitalBinding> sInputBindings, util::data::Span<u8> o_snDigitalActionState )
    {
        //assert::Debug( util::maths::WithinRange( sInputBindings.m_nElements, SizeNeededForDigitalActions( o_snDigitalActionState.m_nElements ), SizeNeededForDigitalActions( o_snDigitalActionState.m_nElements ) + 1 ) );

        for ( uint i = 0; i < sInputBindings.nLength; i++ )
        {
            bool bPreviouslyHeld = DigitalActionHeld( o_snDigitalActionState, i );

            bool bCurrentlyHeld = false;
            for ( uint j = 0; j < sMulticastInputDevices.nLength; j++ )
            {
                InputDevice *const pInputDevice = &sMulticastInputDevices.pData[ j ];
                if ( pInputDevice->eDeviceType == EInputDeviceType::INVALID )
                {
                    continue;
                }

                if ( pInputDevice->eDeviceType == EInputDeviceType::CONTROLLER && !s_bEnableController )
                {
                    continue;
                }

                if ( pInputDevice->eDeviceType == EInputDeviceType::KEYBOARD_MOUSE )
                {
                    // TODO: doesn't support using buttons if an axis is bound

                    if ( EKeyboardMouseButton_IsMouseInput( sInputBindings.pData[ i ].eKBButton ) )
                    {
                        bCurrentlyHeld = SDL_GetMouseState( nullptr, nullptr ) & EKeyboardMouseButton_ToSDLMouseButtonBitmask( sInputBindings.pData[ i ].eKBButton );
                    }
                    else
                    {
                        const u8* const pbSDLKeyStates = SDL_GetKeyboardState( nullptr );
                        bCurrentlyHeld = pbSDLKeyStates[ EKeyboardMouseButton_ToSDLKeyboardScancode( sInputBindings.pData[ i ].eKBButton ) ];
                    }
                }
                else if ( pInputDevice->eDeviceType == EInputDeviceType::CONTROLLER )
                {
                    if ( SDL_GameControllerGetButton( pInputDevice->pSDLGameController, EControllerButtonToSDLButton( sInputBindings.pData[ i ].eControllerButton ) ) )
                    {
                        bCurrentlyHeld = true;
                    }
                    else
                    {
                        i16 nRawValue = SDL_GameControllerGetAxis( pInputDevice->pSDLGameController, EControllerAxisToSDLAxis( sInputBindings.pData[ i ].eControllerAxis ) );
                        bCurrentlyHeld = abs( nRawValue ) >= MAX_JOYSTICK * s_flControllerDeadzone;
                    }
                }

                if ( bCurrentlyHeld )
                    break;
            }

            if ( bCurrentlyHeld )
            {
                if ( bPreviouslyHeld )
                {
                    SetDigitalActionHeld( o_snDigitalActionState, i );
                }
                else
                {
                    SetDigitalActionPressed( o_snDigitalActionState, i );
                }
            }
            else
            {
                if ( bPreviouslyHeld )
                {
                    SetDigitalActionDepressed( o_snDigitalActionState, i );
                }
                else
                {
                    SetDigitalActionClear( o_snDigitalActionState, i );
                }
            }
        }
    }

    void PostProcess( util::data::Span<InputDevice> sInputDevices )
    {
        for ( uint i = 0; i < sInputDevices.nLength; i++ )
        {
            if ( sInputDevices.pData[ i ].eDeviceType == EInputDeviceType::KEYBOARD_MOUSE )
            {
                // reset pending data we've processed already
                sInputDevices.pData[ i ].keyboardAndMouseData = {};
            }
        }
    }
#endif // #if TITANIUM_SDL
}
