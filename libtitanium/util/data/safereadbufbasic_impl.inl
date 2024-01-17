namespace util::data
{
    template <std::unsigned_integral TSize> 
    SafeReadBufBasic<TSize>::SafeReadBufBasic( const Span<byte, TSize> data ) : m_Data( data ) {}
    template <std::unsigned_integral TSize> 
    SafeReadBufBasic<TSize>::SafeReadBufBasic( const TSize nElements, const byte *const pData ) : m_Data( util::data::Span<byte, TSize>( nElements, pData ) ) {}

    template <std::unsigned_integral TSize> 
    TSize SafeReadBufBasic<TSize>::Size() const
    {
        return m_Data.m_nElements;
    }


    template <std::unsigned_integral TSize> 
    void SafeReadBufBasic<TSize>::SetPosition( const TSize nPosition )
    {
        assert::Debug( nPosition < Size() );
        m_nPosition = nPosition;
    }


    template <std::unsigned_integral TSize> 
    byte SafeReadBufBasic<TSize>::ReadByte()
    {
        assert::Release( m_nPosition + sizeof( byte ) < Size() );

        byte value = m_Data.m_pData[ m_nPosition ];
        m_nPosition += sizeof( byte );

        return value;
    }

    template <std::unsigned_integral TSize> 
    u16 SafeReadBufBasic<TSize>::ReadU16()
    {
        assert::Release( m_nPosition + sizeof( u16 ) < Size() );

        u16 value = m_Data.m_pData[ m_nPosition ];
        m_nPosition += sizeof( u16 );

        return value;
    }

    template <std::unsigned_integral TSize> 
    u32 SafeReadBufBasic<TSize>::ReadU32()
    {
        assert::Release( m_nPosition + sizeof( u32 ) < Size() );

        u32 value = m_Data.m_pData[ m_nPosition ];
        m_nPosition += sizeof( u32 );

        return value;
    }

    template <std::unsigned_integral TSize> 
    u64 SafeReadBufBasic<TSize>::ReadU64()
    {
        assert::Release( m_nPosition + sizeof( u64 ) < Size() );

        u64 value = m_Data.m_pData[ m_nPosition ];
        m_nPosition += sizeof( u64 );

        return value;
    }

    template <std::unsigned_integral TSize> 
    const byte* SafeReadBufBasic<TSize>::ReadBytes( const TSize nBytes )
    {
        assert::Release( m_nPosition + nBytes * sizeof( byte ) < Size() );
        
        const byte* data = m_Data.m_pData + m_nPosition;
        m_nPosition += m_nPosition;

        return data;
    }
}
