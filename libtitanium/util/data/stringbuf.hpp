#pragma once

#include <libtitanium/util/numerics.hpp>

#include <stdio.h>
#include <stdarg.h>

namespace util::data
{
    template <u64 TMaxSize>
    struct StringBuf
    {
        char m_szStr[ TMaxSize ];

        StringBuf();
        StringBuf( const char *const pszFmt, ... );

        char * ToCStr();
        const char * ToConstCStr() const;
        operator char * ();
        operator const char * () const;
    };



    template <u64 TMaxSize>
    StringBuf<TMaxSize>::StringBuf()
    {
        m_szStr[ 0 ] = '\0';
    }

    template <u64 TMaxSize>
    StringBuf<TMaxSize>::StringBuf( const char *const pszFmt, ... )
    {
        // todo: should this assert if we hit the buffer? (i.e. when output truncated)
        // i don't like manually doing this, having some kind of macro would be nice
        va_list vargs;
        va_start( vargs, pszFmt );
        (void)vsnprintf( m_szStr, TMaxSize, pszFmt, vargs );
        va_end( vargs );
    }

    template <u64 TMaxSize>
    char * StringBuf<TMaxSize>::ToCStr() { return m_szStr; }
    template <u64 TMaxSize>
    const char * StringBuf<TMaxSize>::ToConstCStr() const { return m_szStr; }
    template <u64 TMaxSize>
    StringBuf<TMaxSize>::operator char * () { return ToCStr(); }
    template <u64 TMaxSize>
    StringBuf<TMaxSize>::operator const char * () const { return ToConstCStr(); }
}
