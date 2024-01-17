#include "input_actions.hpp"

#include <libtitanium/util/numerics.hpp>
#include <libtitanium/util/data/span.hpp>
#include <libtitanium/dev/tests.hpp>

namespace input
{
    /*
     *  NOTE: both analogue and digital actions are stored in simple spans (cont)
     *  digital actions are stored as 2 bits each in arrays of bytes, if the first bit is set, the button is being held down
     *  the second bit acts as a recency modifier, representing whether this is the first frame the current "held" state was taken
     *  i.e. if both the first and second bits are set, the button is held, and it's the first frame it's been held for (considered "pressed")
     *       if just the second bit is set, the button has just been let go of (considered "depressed")
     *
     *  analogue actions are stored as arrays of 16 bit signed ints (TODO: investigate using i8s here instead for bandwidth), which are converted to floats of range -1.0, 1.0 for code
     */

    bool GetDigitalActionHeldBit( const util::data::Span<u8> snCurrentInputs, const uint nActionIndex )
    {
        const int nActionBitIndex = ( nActionIndex % 4 ) * 2;
        return ( snCurrentInputs.pData[ nActionIndex / 4 ] & ( 0b11 << nActionBitIndex ) ) & ( 0b10 << nActionBitIndex );
    }

    bool GetDigitalActionModifierBit( const util::data::Span<u8> snCurrentInputs, const uint nActionIndex )
    {
        const int nActionBitIndex = ( nActionIndex % 4 ) * 2;
        return ( snCurrentInputs.pData[ nActionIndex / 4 ] & ( 0b11 << nActionBitIndex ) ) & ( 0b01 << nActionBitIndex );
    }

    bool DigitalActionHeld( const util::data::Span<u8> snCurrentInputs, const uint nActionIndex )
    {
        return GetDigitalActionHeldBit( snCurrentInputs, nActionIndex );
    }

    bool DigitalActionPressed( const util::data::Span<u8> snCurrentInputs, const uint nActionIndex )
    {
        return GetDigitalActionHeldBit( snCurrentInputs, nActionIndex ) && GetDigitalActionModifierBit( snCurrentInputs, nActionIndex );
    }

    bool DigitalActionDepressed( const util::data::Span<u8> snCurrentInputs, const uint nActionIndex )
    {
        return !GetDigitalActionHeldBit( snCurrentInputs, nActionIndex ) && GetDigitalActionModifierBit( snCurrentInputs, nActionIndex );
    }

    void SetDigitalActionBits( const util::data::Span<u8> snCurrentInputs, const uint nActionIndex, const u8 nSetBits )
    {
        const uint nBitIndex = ( nActionIndex % 4 ) * 2;
        snCurrentInputs.pData[ nActionIndex / 4 ] &= ~( 0b11 << nBitIndex ); // clear
        snCurrentInputs.pData[ nActionIndex / 4 ] |= nSetBits << nBitIndex;
    }

    void SetDigitalActionHeld( const util::data::Span<u8> snCurrentInputs, const uint nActionIndex )
    {
        SetDigitalActionBits( snCurrentInputs, nActionIndex, 0b10 );
    }

    void SetDigitalActionPressed( const util::data::Span<u8> snCurrentInputs, const uint nActionIndex )
    {
        SetDigitalActionBits( snCurrentInputs, nActionIndex, 0b11 );
    }

    void SetDigitalActionDepressed( const util::data::Span<u8> snCurrentInputs, const uint nActionIndex )
    {
        SetDigitalActionBits( snCurrentInputs, nActionIndex, 0b01 );
    }

    void SetDigitalActionClear( const util::data::Span<u8> snCurrentInputs, const uint nActionIndex )
    {
        SetDigitalActionBits( snCurrentInputs, nActionIndex, 0b00 );
    }

    float AnalogueActionValue( const util::data::Span<f32> sflCurrentAnalogueInputs, const uint nActionIndex )
    {
        return sflCurrentAnalogueInputs.pData[ nActionIndex ];
    }
}

#if HAS_TESTS
    #include <libtitanium/util/static_array.hpp>

    TEST( InputActions )
    {
        u8 nDigitalInputs[ input::SizeNeededForDigitalActions( 5 ) ] {};

        for ( uint i = 0; i < util::StaticArray_Length( nDigitalInputs ); i++ )
        {
            input::SetDigitalActionPressed( util::StaticArray_ToSpan( nDigitalInputs ), i );
            TEST_EXPECT( input::DigitalActionHeld( util::StaticArray_ToSpan( nDigitalInputs ), i ) );
            TEST_EXPECT( input::DigitalActionPressed( util::StaticArray_ToSpan( nDigitalInputs ), i ) );
            TEST_EXPECT( !input::DigitalActionDepressed( util::StaticArray_ToSpan( nDigitalInputs ), i ) );

            input::SetDigitalActionHeld( util::StaticArray_ToSpan( nDigitalInputs ), i );
            TEST_EXPECT( input::DigitalActionHeld( util::StaticArray_ToSpan( nDigitalInputs ), i ) );
            TEST_EXPECT( !input::DigitalActionPressed( util::StaticArray_ToSpan( nDigitalInputs ), i ) );
            TEST_EXPECT( !input::DigitalActionDepressed( util::StaticArray_ToSpan( nDigitalInputs ), i ) );

            input::SetDigitalActionDepressed( util::StaticArray_ToSpan( nDigitalInputs ), i );
            TEST_EXPECT( input::DigitalActionDepressed( util::StaticArray_ToSpan( nDigitalInputs ), i ) );
            TEST_EXPECT( !input::DigitalActionHeld( util::StaticArray_ToSpan( nDigitalInputs ), i ) );
            TEST_EXPECT( !input::DigitalActionPressed( util::StaticArray_ToSpan( nDigitalInputs ), i ) );

            input::SetDigitalActionClear( util::StaticArray_ToSpan( nDigitalInputs ), i );
            TEST_EXPECT( !input::DigitalActionDepressed( util::StaticArray_ToSpan( nDigitalInputs ), i ) );
            TEST_EXPECT( !input::DigitalActionHeld( util::StaticArray_ToSpan( nDigitalInputs ), i ) );
            TEST_EXPECT( !input::DigitalActionPressed( util::StaticArray_ToSpan( nDigitalInputs ), i ) );
        }

        return true;
    }
#endif // #if HAS_TESTS
