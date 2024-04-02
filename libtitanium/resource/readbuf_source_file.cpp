#include "readbuf.hpp"

#include <stdio.h>
#include <string.h>

#include <libtitanium/memory/mem_core.hpp>

namespace resource
{
    #define BUF_FILEHANDLE( pReadBuf ) reinterpret_cast<FILE *>( pReadBuf->pSourceSpecificData )

    bool ReadBufSource_File_CreateReadBuf( ReadBuf *const pReadBuf, const util::data::Span<char> filePath, jmp_buf longjmpToOnFailure )
    {
        // bleh: convert span string to c-style nullterminated string for fopen
        // TODO: utility for this (also string rewrite)
        // TODO: also when we have threadlocal temp/scratch allocators use those for this

        char * szPath = memory::alloc_nT<char>( filePath.nLength + 1 );
        szPath[ filePath.nLength ] = '\0';
        memcpy( szPath, filePath.pData, filePath.nLength );

        FILE * r_pFileHandle = fopen( szPath, "rb" );
        pReadBuf->sourceReadFuncs = ReadBufSourceFuncs_File;
        //pReadBuf->longjmpToOnFailure = longjmpToOnFailure;
        pReadBuf->pSourceSpecificData = r_pFileHandle;

        memory::free( szPath );

        return r_pFileHandle != nullptr;
    }

    u64 ReadBufSource_File_GetSize( ReadBuf *const pReadBuf )
    {
        FILE *const pFileHandle = BUF_FILEHANDLE( pReadBuf );
        const u64 oldPos = ftell( pFileHandle );
        fseek( pFileHandle, 0, SEEK_END );

        const u64 r_size = ftell( pFileHandle );
        fseek( pFileHandle, oldPos, SEEK_SET );

        return r_size;
    }

    #define IMPL_READBUF_READ_SINGLE( type, func ) ReadBufSourceReadResult<type> func( ReadBuf *const pReadBuf ) \
    { \
        type r_data; \
        size_t success = fread( &r_data, sizeof( type ), 1, BUF_FILEHANDLE( pReadBuf ) ); \
        return { .success = success != 0, .value = r_data }; \
    }

    IMPL_READBUF_READ_SINGLE( byte, ReadBufSource_File_GetByte );
    IMPL_READBUF_READ_SINGLE( u8, ReadBufSource_File_GetU8 );
    IMPL_READBUF_READ_SINGLE( i8, ReadBufSource_File_GetI8 );
    IMPL_READBUF_READ_SINGLE( u16, ReadBufSource_File_GetU16 );
    IMPL_READBUF_READ_SINGLE( i16, ReadBufSource_File_GetI16 );
    IMPL_READBUF_READ_SINGLE( u32, ReadBufSource_File_GetU32 );
    IMPL_READBUF_READ_SINGLE( i32, ReadBufSource_File_GetI32 );
    IMPL_READBUF_READ_SINGLE( u64, ReadBufSource_File_GetU64 );
    IMPL_READBUF_READ_SINGLE( i64, ReadBufSource_File_GetI64 );
    IMPL_READBUF_READ_SINGLE( f32, ReadBufSource_File_GetF32 );
    IMPL_READBUF_READ_SINGLE( f64, ReadBufSource_File_GetF64 );

    bool ReadBufSource_File_GetBytes( ReadBuf *const pReadBuf, util::data::Span<byte> o_data )
    {
        return fread( o_data.pData, o_data.nLength, 1, BUF_FILEHANDLE( pReadBuf ) ) != 0;
    }

    void ReadBufSource_File_CleanUp( ReadBuf *const pReadBuf )
    {
        // this can fail if the handle is invalid, we don't really care if it does
        fclose( BUF_FILEHANDLE( pReadBuf ) );
    }
}
