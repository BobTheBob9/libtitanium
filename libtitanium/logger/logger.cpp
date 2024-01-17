#include "logger.hpp"
#include <stdarg.h>
#include <stdio.h>

#if HAS_LOGGED_TEXT_BUF
    char g_szLoggedTextBuffer[ 4096 ][ 50 ];
#endif // #if HAS_LOGGED_TEXT_BUF

void logger::Info( const char *const pFmt, ... )
{
    char szBuf[ 4096 ];
    va_list vargs;
    va_start( vargs, pFmt );
    {
        vsnprintf( szBuf, sizeof( szBuf ) - 1, pFmt, vargs );
    }
    va_end( vargs );

    printf( "%s", szBuf );
}
