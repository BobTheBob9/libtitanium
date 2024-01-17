#pragma once

#include <libtitanium/util/numerics.hpp>
#include <libtitanium/util/data/span.hpp>

#include <SDL.h>

namespace input
{
    /*
     *  NOTE: just noting down some terminology and other general notes on the input system here
     *  we have 2 types of inputs in this system, "real" inputs come from the OS as keycodes or analogue values
     *  "virtual" inputs are code-defined actions that can be bound to real inputs
     *
     *  so essentially, to start the "forward" action, we'll check the bindings for the virtual forward action
     *  then once we've found the real inputs it maps to, we'll check for any or all of them (depending on config),
     *  then fire the forward action if the right real inputs were given
     *
     *
     *  we don't place any kind of limitation on which types of real inputs can be bound to which types of virtual ones
     *  so, an analogue stick can set a boolean action, and vice versa, the way these work is configurable though
     *
     *
     *  so, the workflow for this system should pretty much end up as
     *  1) define all your virtual actions from code within an enum
     *  2) set up your bindings struct (assumedly using a convar, command, config file or whatever) for each action
     *  3) ask inputsystem of the state of each virtual action whenever necessary
     *
     *  TODO: we're primarily client/server, should we design this to support like, arbitrary data sources (e.g. the network)?
     *  could be nice to reuse api, but could also bloat things, might be too much responsibility for one system
     *
     *  TODO: work out supporting multiple controllers etc
     */

    constexpr f32 MAX_JOYSTICK = 1.f;
    constexpr f32 MIN_JOYSTICK = -MAX_JOYSTICK;
    constexpr uint SizeNeededForDigitalActions( const uint nActions ) { return ( 2 * nActions + 8 ) / 8; }

    void SetDigitalActionHeld( const util::data::Span<u8> snCurrentInputs, const uint nActionIndex );
    void SetDigitalActionPressed( const util::data::Span<u8> snCurrentInputs, const uint nActionIndex );
    void SetDigitalActionDepressed( const util::data::Span<u8> snCurrentInputs, const uint nActionIndex );
    void SetDigitalActionClear( const util::data::Span<u8> snCurrentInputs, const uint nActionIndex );

    bool DigitalActionHeld( const util::data::Span<u8> snCurrentInputs, const uint nActionIndex );
    bool DigitalActionPressed( const util::data::Span<u8> snCurrentInputs, const uint nActionIndex );
    bool DigitalActionDepressed( const util::data::Span<u8> snCurrentInputs, const uint nActionIndex );

    float AnalogueActionValue( const util::data::Span<f32> sflCurrentAnalogueInputs, const uint nActionIndex );
}
