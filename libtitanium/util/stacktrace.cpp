#include "stacktrace.hpp"

#include <libtitanium/util/assert.hpp>
#include <libtitanium/util/string.hpp>
#include <libtitanium/util/static_array.hpp>
#include <libtitanium/logger/logger.hpp>
#include <libtitanium/memory/mem_core.hpp>

#include <execinfo.h>

namespace util
{
    // TODO: unfortunately, no way to use c++23's stacktraces in clang, and glibc backtrace is... not ideal
    // no function names (need to call addr2line from binutils for them on linux), and not portable to my knowledge
    // TODO: actually good portable backtrace -_-

    void GetStacktrace( util::data::Span<StackMember> o_sStackTrace )
    {
        constexpr int MAX_STACKTRACE = 128;
        assert::Debug( o_sStackTrace.nLength < MAX_STACKTRACE );

        void * pCallstack[ MAX_STACKTRACE ];
        backtrace( pCallstack, o_sStackTrace.nLength );

        char ** ppszSymbols = backtrace_symbols( pCallstack, o_sStackTrace.nLength );
        // start at 1 so we don't include this function in the stacktrace
        for ( uint i = 1; i < o_sStackTrace.nLength; i++ )
        {
            if ( !pCallstack[ i ] )
                break;

            o_sStackTrace.pData[ i - 1 ].pAddress = pCallstack[ i ];
            util::string::CopyTo( ppszSymbols[ i ], StaticArray_ToSpan( o_sStackTrace.pData[ i - 1 ].szFuncName ) );
        }

        ::free( ppszSymbols );
    }

}
