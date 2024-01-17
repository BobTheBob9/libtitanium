#include "span_dynamic.hpp"

#include <libtitanium/logger/logger.hpp>
#include <libtitanium/dev/tests.hpp>

#if HAS_TESTS
    TEST( SpanDynamic )
    {
        util::data::SpanDynamic<uint> vInts {};

        for ( uint i = 0; i < 20; i++ )
        {
            LOG_CALL( util::data::SpanDynamic<uint>::AppendTo( &vInts, i ) );
            logger::Info( "vInts.nAllocatedLength = %i" ENDL, vInts.nAllocatedLength );
            TEST_EXPECT( vInts.sData.pData[ vInts.sData.nLength - 1 ] == i );
            TEST_EXPECT( vInts.sData.nLength == i + 1 );
        }

        for ( uint i = 0; i < vInts.sData.nLength; i++ )
        {
            TEST_EXPECT( vInts.sData.pData[ i ] == i );
        }

        const uint nLength = vInts.sData.nLength;
        for ( uint i = 0; i < nLength; i++ )
        {
            logger::Info( "%i %i" ENDL, i, vInts.sData.pData[ 0 ] );

            TEST_EXPECT( vInts.sData.pData[ 0 ] == i );
            util::data::SpanDynamic<uint>::RemoveAt( &vInts, 0 );
        }

        return true;
    }
#endif // #if HAS_TESTS
