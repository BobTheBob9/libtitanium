#include "readbuf.hpp"

#include <setjmp.h>
#include <libtitanium/util/assert.hpp>

namespace resource
{
    u64 ReadBuf::GetSize( ReadBuf *const pReadBuf )
    {
        // this can't throw/fail, assumedly
        return pReadBuf->sourceReadFuncs.GetSize( pReadBuf );
    }

    #define IMPL_READBUF_READ_SINGLE( type, func ) type ReadBuf::func( ReadBuf *const pReadBuf ) \
    { \
        ReadBufSourceReadResult<type> __ret = pReadBuf->sourceReadFuncs.func( pReadBuf ); \
        if ( !__ret.success ) \
        { \
            pReadBuf->sourceReadFuncs.CleanUp( pReadBuf ); \
            assert::Release( pReadBuf->longjmpToOnFailure ); /* ensures we don't continue on fail even if no longjmp provided */ \
            longjmp( pReadBuf->longjmpToOnFailure, maxof( int ) ); \
        } \
        \
        return __ret.value; \
    }

    IMPL_READBUF_READ_SINGLE( byte, GetByte );
    IMPL_READBUF_READ_SINGLE( u8, GetU8 );
    IMPL_READBUF_READ_SINGLE( i8, GetI8 );
    IMPL_READBUF_READ_SINGLE( u16, GetU16 );
    IMPL_READBUF_READ_SINGLE( i16, GetI16 );
    IMPL_READBUF_READ_SINGLE( u32, GetU32 );
    IMPL_READBUF_READ_SINGLE( i32, GetI32 );
    IMPL_READBUF_READ_SINGLE( u64, GetU64 );
    IMPL_READBUF_READ_SINGLE( i64, GetI64 );
    IMPL_READBUF_READ_SINGLE( f32, GetF32 );
    IMPL_READBUF_READ_SINGLE( f64, GetF64 );

    void ReadBuf::GetBytes( ReadBuf *const pReadBuf, util::data::Span<byte> o_data )
    {
        bool success = pReadBuf->sourceReadFuncs.GetBytes( pReadBuf, o_data );
        if ( !success )
        {
            pReadBuf->sourceReadFuncs.CleanUp( pReadBuf );
            assert::Release( pReadBuf->longjmpToOnFailure );
            longjmp( pReadBuf->longjmpToOnFailure, maxof( int ) );
        }
    }
}
