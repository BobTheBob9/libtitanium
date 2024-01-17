#pragma once

#define ENDL "\n"

#ifndef HAS_LOGGED_TEXT_BUF
    #define HAS_LOGGED_TEXT_BUF 1
#endif // #ifndef HAS_LOGGED_TEXT_BUF

#if HAS_LOGGED_TEXT_BUF
    extern char g_szLoggedTextBuffer[ 4096 ][ 50 ];
#endif // #if HAS_LOGGED_TEXT_BUF

namespace logger
{
    enum class ELogLevel
    {
        INFO,
        WARN,
        ERROR,
        ERROR_FATAL
    };

    void Log( const ELogLevel eLevel, const char *const pFmt, ... );

    void Info( const char *const pFmt, ... );
    void Warn( const char *const pFmt, ... );
    void Error( const char *const pFmt, ... );
};

#define LOG_CALL( func ) logger::Info( "Call: " #func " in " __FILE__ ENDL ); func
