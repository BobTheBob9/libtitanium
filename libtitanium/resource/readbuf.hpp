#pragma once

#include <setjmp.h>

#include <libtitanium/util/numerics.hpp>
#include <libtitanium/util/data/span.hpp>

namespace resource
{
    template <typename T> struct ReadBufSourceReadResult { bool success; T value; };

    struct ReadBuf
    {
        jmp_buf longjmpToOnFailure;
        void * pSourceSpecificData; // actual state of the reader backend, e.g. a filehandle, a pointer + length + index, etc


        struct ReadBufSourceFuncs
        {
            u64 (*GetSize)( ReadBuf *const pReadBuf );

            ReadBufSourceReadResult<byte> (*GetByte)( ReadBuf *const pReadBuf );
            ReadBufSourceReadResult<u8> (*GetU8)( ReadBuf *const pReadBuf );
            ReadBufSourceReadResult<i8> (*GetI8)( ReadBuf *const pReadBuf );
            ReadBufSourceReadResult<u16> (*GetU16)( ReadBuf *const pReadBuf );
            ReadBufSourceReadResult<i16> (*GetI16)( ReadBuf *const pReadBuf );
            ReadBufSourceReadResult<u32> (*GetU32)( ReadBuf *const pReadBuf );
            ReadBufSourceReadResult<i32> (*GetI32)( ReadBuf *const pReadBuf );
            ReadBufSourceReadResult<u64> (*GetU64)( ReadBuf *const pReadBuf );
            ReadBufSourceReadResult<i64> (*GetI64)( ReadBuf *const pReadBuf );
            ReadBufSourceReadResult<f32> (*GetF32)( ReadBuf *const pReadBuf );
            ReadBufSourceReadResult<f64> (*GetF64)( ReadBuf *const pReadBuf );
            bool (*GetBytes)( ReadBuf *const pReadBuf, util::data::Span<byte> o_data );
            void (*CleanUp)( ReadBuf *const pReadBuf );
        } sourceReadFuncs;

        static u64 GetSize( ReadBuf *const pReadBuf );

        static byte GetByte( ReadBuf *const pReadBuf );
        static u8 GetU8( ReadBuf *const pReadBuf );
        static i8 GetI8( ReadBuf *const pReadBuf );
        static u16 GetU16( ReadBuf *const pReadBuf );
        static i16 GetI16( ReadBuf *const pReadBuf );
        static u32 GetU32( ReadBuf *const pReadBuf );
        static i32 GetI32( ReadBuf *const pReadBuf );
        static u64 GetU64( ReadBuf *const pReadBuf );
        static i64 GetI64( ReadBuf *const pReadBuf );
        static f32 GetF32( ReadBuf *const pReadBuf );
        static f64 GetF64( ReadBuf *const pReadBuf );
        static void GetBytes( ReadBuf *const pReadBuf, util::data::Span<byte> o_data );
    };

    bool ReadBufSource_File_CreateReadBuf( ReadBuf *const pReadBuf, const util::data::Span<char> filePath, jmp_buf longjmpToOnFailure );

    u64 ReadBufSource_File_GetSize( ReadBuf *const pReadBuf );
    ReadBufSourceReadResult<byte> ReadBufSource_File_GetByte( ReadBuf *const pReadBuf );
    ReadBufSourceReadResult<u8> ReadBufSource_File_GetU8( ReadBuf *const pReadBuf );
    ReadBufSourceReadResult<i8> ReadBufSource_File_GetI8( ReadBuf *const pReadBuf );
    ReadBufSourceReadResult<u16> ReadBufSource_File_GetU16( ReadBuf *const pReadBuf );
    ReadBufSourceReadResult<i16> ReadBufSource_File_GetI16( ReadBuf *const pReadBuf );
    ReadBufSourceReadResult<u32> ReadBufSource_File_GetU32( ReadBuf *const pReadBuf );
    ReadBufSourceReadResult<i32> ReadBufSource_File_GetI32( ReadBuf *const pReadBuf );
    ReadBufSourceReadResult<u64> ReadBufSource_File_GetU64( ReadBuf *const pReadBuf );
    ReadBufSourceReadResult<i64> ReadBufSource_File_GetI64( ReadBuf *const pReadBuf );
    ReadBufSourceReadResult<f32> ReadBufSource_File_GetF32( ReadBuf *const pReadBuf );
    ReadBufSourceReadResult<f64> ReadBufSource_File_GetF64( ReadBuf *const pReadBuf );
    bool ReadBufSource_File_GetBytes( ReadBuf *const pReadBuf, util::data::Span<byte> o_data );
    void ReadBufSource_File_CleanUp( ReadBuf *const pReadBuf );

    constexpr ReadBuf::ReadBufSourceFuncs ReadBufSourceFuncs_File = {
        .GetSize = ReadBufSource_File_GetSize,
        .GetByte = ReadBufSource_File_GetByte,
        .GetU8 = ReadBufSource_File_GetU8,
        .GetI8 = ReadBufSource_File_GetI8,
        .GetU16 = ReadBufSource_File_GetU16,
        .GetI16 = ReadBufSource_File_GetI16,
        .GetU32 = ReadBufSource_File_GetU32,
        .GetI32 = ReadBufSource_File_GetI32,
        .GetU64 = ReadBufSource_File_GetU64,
        .GetI64 = ReadBufSource_File_GetI64,
        .GetF32 = ReadBufSource_File_GetF32,
        .GetF64 = ReadBufSource_File_GetF64,
        .GetBytes = ReadBufSource_File_GetBytes,
        .CleanUp = ReadBufSource_File_CleanUp
    };
};
