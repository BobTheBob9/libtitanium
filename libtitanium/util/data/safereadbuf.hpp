#pragma once

#include <concepts>
#include <type_traits>

#include <libtitanium/util/numerics.hpp>
#include <libtitanium/util/assert.hpp>
#include <libtitanium/util/data/span.hpp>

namespace util::data
{
    template <typename T>
    concept ISafeReadBuf = requires(T t)
    {
        { &T::Size } -> std::same_as< u32 ( T::* )() const >;

        { &T::SetPosition } -> std::same_as< void ( T::* )( const u32 nPosition ) >;

        { &T::ReadByte } -> std::same_as< byte ( T::* )() >;
        { &T::ReadU8 } -> std::same_as< u8 ( T::* )() >;
        { &T::ReadU16 } -> std::same_as< u16 ( T::* )() >;
        { &T::ReadU32 } -> std::same_as< u32 ( T::* )() >;
        { &T::ReadU64 } -> std::same_as< u64 ( T::* )() >;

        { &T::ReadBytes } -> std::same_as< const byte * ( T::* )( const u32 nLength ) >;
    };
    
    template <std::unsigned_integral TSize = u32> 
    class SafeReadBufBasic
    {
        TSize m_nPosition = 0;
        const util::data::Span<byte, TSize> m_Data;

    public:
        SafeReadBufBasic() = delete;
        SafeReadBufBasic( const Span<byte, TSize> data );
        SafeReadBufBasic( const TSize nElements, const byte *const pData );

        TSize Size() const;

        void SetPosition( const TSize nPosition ); 
        
        byte ReadByte();
        u8 ReadU8();
        u16 ReadU16();
        u32 ReadU32();
        u64 ReadU64();
        const byte* ReadBytes( const TSize nBytes );
    };
};

#include "safereadbufbasic_impl.inl"
