#include "tests.hpp"

#include <libtitanium/util/data/span_dynamic.hpp>
#include <libtitanium/logger/logger.hpp>

#if HAS_TESTS
namespace dev::tests
{
    struct TestEntry
    {
        const char * pszTestName;
        FnTest fnTest;
    };

    static util::data::SpanDynamic<TestEntry> s_vTests;

    void AddTest( const char *const pszTestName, const FnTest fnTest )
    {
        util::data::SpanDynamic<TestEntry>::AppendTo( &s_vTests, { .pszTestName = pszTestName, .fnTest = fnTest } );
    }

    bool RunTests()
    {
        for ( uint i = 0; i < s_vTests.sData.nLength; i++ )
        {
            logger::Info( "Running test %s" ENDL, s_vTests.sData.pData[ i ].pszTestName );

            if ( !s_vTests.sData.pData[ i ].fnTest() )
            {
                return false;
            }
        }

        return true;
    }

    void CleanupTests()
    {
        util::data::SpanDynamic<TestEntry>::SetLength( &s_vTests, 0 );
    }
}
#endif // #if HAS_TESTS
