#pragma once

#include <math.h>

#include <libtitanium/util/numerics.hpp>
#include <libtitanium/util/data/span.hpp>

namespace util
{
    /*
     * MEGA EXPERIMENTAL!
     * This defines 64-bit fixed-point decimals to be 17/47 bit signed ints, so 17 bits left of the decimal, 47 on the right
     * ( you can figure out the number of full digits (i.e. full 0-9 range) with the formula floor(log10(2^bits))-1, remove the -1 for non-full digits )
     * this gives us a range of -65536 to 65536 with 13 decimal points of precision!
     * this does mean the actual representation of these are weird, 0.3 == 300000000000, or 5500.475 == 55004750000000000
     *
     * TODO: theoretically (at least according to my brain), addition and subtraction should like, just work
     *       multiplication/div won't just work though
     *       like, 300000000000 * 300000000000 is not the same as 0.3 * 0.3 unfortunately
     *
     *       if mult/div works though, all we need is literals, tostring and conversion to/from decimal and normal integers
     *       and then it should all just like, work? maybe
     *
     * possible reading material: https://vanhunteradams.com/FixedPoint/FixedPoint.html
     * slightly unfortunate thing here is that they're on 32-bit numbers so they can just like, combine 2 32s to do 64-bit maths for mult, we can't do that if we're on 64 bit numbers (128 bit is... not quite as easy as 64)
     *
     * the intention here is determinism, don't want to deal with floating point imprecision making movement slightly different on different framerates, for instance
     * also means we have very fixed ranges, don't need to deal with the loose "oh it just gets worse as you go further out c:" ranges of floats, would rather have a very fixed range instead, where everything works the same at all values (unless you overflow!)
     */

    typedef decimal64_t = int64_t;

    constexpr int DECIMAL_BITS_LEFT = 17;
    constexpr int DECIMAL_BITS_RIGHT = 47;
    static_assert( DECIMAL_BITS_LEFT + DECIMAL_BITS_RIGHT == sizeof( decimal64_t ) * 8 );

    constexpr int DECIMAL_DIGITS_RIGHT = floor( log10( pow( 2, DECIMAL_BITS_RIGHT ) ) ) - 1;

    constexpr decimal64_t decimal_make( int32_t left, uint64_t right )
    {
        return ( left * pow( 10, DECIMAL_DIGITS_RIGHT - 1 ) ) + ( right * pow( 10, DECIMAL_DIGITS_RIGHT - 2 ) );
    }

    constexpr decimal64_t decimal_extractleft( decimal64_t num )
    {
        return num / pow( 10, DECIMAL_DIGITS_RIGHT - 1 );
    }

    constexpr decimal64_t decimal_extractright( decimal64_t num )
    {
        // isolate the left, then subtract it from the whole number to get the right
        // so, 1.3 => 1.3 - 1 == .3
        decimal64_t left = decimal_extractleft( num );
        return num - ( left * pow( 10, DECIMAL_DIGITS_RIGHT - 1 ) );
    }

    constexpr decimal64_t decimal_mult( decimal64_t first, decimal64_t second )
    {
        // multiplying fixed point decimals is weird somewhat
        // take 5*3.2, for example:
        // 5*3200000000000 is not the same as 5*3.2, so we can't just multiply these numbers together
        // to actually do this, we need to do it in 2 steps, so we want to do ( 5 * 3 ) + ( 5 / ( 1 / 0.2 ) ) (or 5*3+5/5,so 16)

        return ( first * decimal_extractleft( second ) ) + ( first / ( decimal_make( 1, 0 ) / decimal_extractright( second ) ) );
    }

    constexpr decimal64_t decimal_div( decimal64_t first, decimal64_t second )
    {
        // this has the same problems as multiplying fixed point decimals, and works similarly
        // 5/3.2 == 5/3 - i dunno
        // NOTE! 5/3.2 == 5/3 - 5/48! so probably figure out how we can get 1/48th from 0.2?
    }

    void ToString( char szBuf[] ) // At most, the string representation of a number will be 29 chars long (5 left, 13 right + decimal place)
    {

    }
}
