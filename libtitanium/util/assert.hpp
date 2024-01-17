#pragma once

#include <libtitanium/util/debug.hpp>
#include <libtitanium/logger/logger.hpp>

namespace assert
{
    // TODO: this whole file is eh, should rework

    /*
    
    Throw an error if condition is not met, even in release!

    */
    inline void Release( const bool bCondition, const char * const pMessageFormat = nullptr, ... )
    {
        // TODO: error
        if ( !bCondition ) [[ unlikely ]] 
        {
            logger::Info( "assertion failed: \"%s\"" ENDL, pMessageFormat ? pMessageFormat : "" );

            __builtin_trap();
            //exit( EXIT_FAILURE );
        }
    }

    /*
    
    Throw an error if condition is not met, only in debug/development!
    
    */
    inline void Debug( const bool bCondition, const char * const pMessageFormat = nullptr, ... )
    {
#if HAS_DEBUG
        // TODO: temp
        if ( !bCondition ) [[ unlikely ]]
        {
            logger::Info( "assertion failed: \"%s\"" ENDL, pMessageFormat );

            __builtin_trap();
            //exit( EXIT_FAILURE );
        }
#else // #ifdef NDEBUG
        (void)bCondition;
        (void)pMessageFormat;
#endif // #else // #ifndef NDEBUG
    }
};
