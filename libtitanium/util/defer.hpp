#pragma once

// this is a pretty c++-y (unfortunately) solution to a defer function, similar to what's in stuff like go
// it's also very macro-y, bleh
// unsure if i'll keep this but the goal here is to abstract away any of the c++-y-ness of this system to outside code, so we have something that feels like a c-ish defer keyword
// only issue with this solution is no "if ( ... ) defer( ... );", since the defer will call immediately due to leaving scope, unsure if that's ok
// TODO: need to check what asm this generates, if it doesn't just generate a normal call at the end of the function i'll be pissed!

namespace util
{
    // TODO: check if actually callable
    template <typename TFunc> class __DeferCallToScopeExit
    {
    private:
        TFunc fnDeferFunc;

    public:
        __DeferCallToScopeExit( TFunc fnDeferFunc )
        {
            this->fnDeferFunc = fnDeferFunc;
        }

        ~__DeferCallToScopeExit()
        {
            ( *fnDeferFunc )();
        }
    };
}

#define __DEFER_CONCAT_INNER( f, s ) f##s
#define __DEFER_CONCAT( f, s ) __DEFER_CONCAT_INNER( f, s )

// usage: defer( memory::free( pSomeThing ) );
#define defer( func ) auto __DEFER_CONCAT( __defer_func, __LINE__ ) = [&](){ func; }; util::__DeferCallToScopeExit<decltype( __DEFER_CONCAT( __defer_func, __LINE__ ) )*> __DEFER_CONCAT( __defer, __LINE__ )( &__DEFER_CONCAT( __defer_func, __LINE__ ) )
