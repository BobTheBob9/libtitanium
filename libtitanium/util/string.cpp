#include "string.hpp"

#include <libtitanium/memory/mem_core.hpp>
#include <libtitanium/dev/tests.hpp>
#include <libtitanium/logger/logger.hpp>
#include <libtitanium/util/maths.hpp>

#include <string.h>

namespace util::string
{
    // NOTE: as a general rule, prefer mapping these directly to string.h functions as they'll generally be way more optimised than any implementation we can do in pure c++
    int LengthOfCString( const char *const pszString )
    {
        return strlen( pszString );
    }

    int LengthOfCStringWithTerminator( const char *const pszString )
    {
        return strlen( pszString ) + 1;
    }

    int LengthOfSpanString( const util::data::Span<char> spszString )
    {
        return strlen( spszString.pData );
    }

    void CopyTo( const char *const pszSource, util::data::Span<char> spszDestinationBuffer )
    {
        const int nCopySize = util::maths::Min<int>( util::string::LengthOfCStringWithTerminator( pszSource ), spszDestinationBuffer.nLength );
        memcpy( spszDestinationBuffer.pData, pszSource, nCopySize );
    }

    void ConcatinateTo( const char* const pszSource, util::data::Span<char> spszDestinationBuffer )
    {
        const int nDestinationCopyBegin = util::string::LengthOfSpanString( spszDestinationBuffer );
        const int nSourceLength = util::string::LengthOfCStringWithTerminator( pszSource );
        int nCopySize = util::maths::Min<int>( nDestinationCopyBegin + nSourceLength, spszDestinationBuffer.nLength - 1 );

        util::data::Span<char> spszEndingBuffer = util::data::Span<char>::Offset( &spszDestinationBuffer, nCopySize );
        util::string::CopyTo( pszSource, spszEndingBuffer );
    }

    // NOTE: these work because all lowercase ascii chars are +32 from uppercase
    void ToLowercase( char *const pszString )
    {
        for ( char * pszStringIterator = pszString; *pszStringIterator; pszStringIterator++ )
        {
            *pszStringIterator += 32 * ( *pszStringIterator >= 'A' && *pszStringIterator <= 'Z' );
        }
    }

    void ToUppercase( char *const pszString )
    {
        for ( char * pszStringIterator = pszString; *pszStringIterator; pszStringIterator++ )
        {
            *pszStringIterator -= 32 * ( *pszStringIterator >= 'a' && *pszStringIterator <= 'z' );
        }
    }

    bool CStringsEqual( const char *const pszFirstString, const char *const pszSecondString )
    {
        return !strcmp( pszFirstString, pszSecondString );
    }

    bool CStringStartsWith( const char *const pszString, const char *const pszStringToStartWith )
    {
        return !strncmp( pszString, pszStringToStartWith, LengthOfCString( pszStringToStartWith ) );
    }

    // experimental simd stuff
    // TODO: profile, check if these are actually at all faster than non-simd since they swap between vector and scalar quite often
    /*
    char * AllocForSIMD( const size_t nChars )
    {
        // the + and - 1s here are to force rounding up (cxx diving truncates towards zero, so rounds down with unsigneds), so trying to allocate 18 chars actually allocates 32
        constexpr int ALIGNMENT = 16;
        size_t nSize = ALIGNMENT * ( ( nChars - 1 ) / ( ALIGNMENT + 1 ) );

        char * pResult = memory::alloc_nT<char>( nSize );
        memset( pResult, 0, nSize );

        return pResult;
    }

        
    int Length_SSE( char *const pa16szStringToCheck )
    {
        int nChars = 0;
        for ( char * pszStringIterator = pa16szStringToCheck; *pszStringIterator; pszStringIterator += 16 )
        {
            const simde__m128i simdCurrentChars = simde_mm_load_si128( ( simde__m128i * )pszStringIterator );
            const simde__m128i simdIsNullchar = simde_mm_cmpeq_epi8( simdCurrentChars, simde_mm_setzero_si128() );

            const u16 nBitmask = simde_mm_movemask_epi8( simdIsNullchar ); // get a bitmask out of it, where each set bit == nonzero char
            // TODO: BitScanForward or equivalent here, and remove std dependency
            //nChars += std::popcount<u16>( ~nBitmask ); // count number of nonzero bits
            nChars += __builtin_popcount( ~nBitmask ) - 16; // __builtin_popcount takes an int32, so -16 bytes that will always be 0

            if ( nBitmask )
                break;
        }

        return nChars;
    }


    void CopyTo_SSE( const char *const pa16szSource, char *const pa16szDestinationBuffer )
    {
        // NOTE: we don't have any kind of condition in this loop as we should only break after we've copied the nullchar
        for ( int i = 0;; i += 16 )
        {
            const simde__m128i simdCurrentChars = simde_mm_load_si128( ( simde__m128i * )( pa16szSource + i ) );
            simde_mm_storeu_si128( ( simde__m128i * )( pa16szDestinationBuffer + i ), simdCurrentChars ); // copy to buffer

            // check for nullchars, break if any are found
            const simde__m128i simdIsNullchar = simde_mm_cmpeq_epi8( simdCurrentChars, simde_mm_setzero_si128() );
            if ( simde_mm_movemask_epi8( simdIsNullchar ) ) // check if mask is nonzero
                break;
        }
    } 

    void ToLowercase_SSE( char *const pa16szStringToConvert )
    {
        // TODO: could we make this a compiletime constant? unsure
        const simde__m128i simdAsciiA = simde_mm_set1_epi8( 'A' );
        const simde__m128i simdAsciiZ = simde_mm_set1_epi8( 'Z' + 1 ); // + 1 for inclusive range in less than check later

        const simde__m128i simdLowercaseConversionDiff = simde_mm_set1_epi8( 32 ); // diff between lowercase and uppercase ascii chars

        // NOTE: we check if *pszStringIterator == nullchar each loop, HOWEVER
        //       this is only for if the first char of the iteration == nullchar, we do a manual check AFTER each iteration for nullchar
        for ( char * pszStringIterator = pa16szStringToConvert; *pszStringIterator; pszStringIterator += 16 )
        {
            const simde__m128i simdCurrentChars = simde_mm_load_si128( ( const simde__m128i * )pszStringIterator );

            // check if chars are within uppercase range
            // each byte of these comparisons are set to 0xFF if true, or 0x0 if false, this is important for bitmasker-y later
            const simde__m128i simdGreaterThanA = simde_mm_cmpgt_epi8( simdCurrentChars, simdAsciiA );
            const simde__m128i simdLessThanZ = simde_mm_cmplt_epi8( simdCurrentChars, simdAsciiZ );

            // create a bitmask by &-ing together the results of the greater than and less than comparisons
            // the results of the previous comparisons are either true == 0xFF or false == 0x0, &-ing together these will only result in true if both are true
            // i.e. if char > A and char < Z!
            const simde__m128i simdValidCharsMask = simde_mm_and_si128( simdGreaterThanA, simdLessThanZ );

            // make a mask to add values to our vector
            // we & our difference (32) with our previous comparison, so we add 32 to all valid chars, and 0 to all invalid ones
            // then add to the char values!
            const simde__m128i simdAddMask = simde_mm_and_si128( simdValidCharsMask, simdLowercaseConversionDiff );
            const simde__m128i simdConvertedChars = simde_mm_add_epi8( simdCurrentChars, simdAddMask );

            // store the result back in the string we're modifying
            simde_mm_storeu_si128( ( simde__m128i * )pszStringIterator, simdConvertedChars );

            // finally, we need to check if there are any nullchars in the current chunk, so we can exit
            const simde__m128i simdIsNullchar = simde_mm_cmpeq_epi8( simdCurrentChars, simde_mm_setzero_si128() );
            if (  simde_mm_movemask_epi8( simdIsNullchar ) ) // check if mask is nonzero
                break;
        }
    }

    void ToUppercase_SSE( char *const pa16szStringToConvert )
    {
        // see ToLowercase_SSE for comments/explanation
        const simde__m128i simdAsciia = simde_mm_set1_epi8( 'a' );
        const simde__m128i simdAsciiz = simde_mm_set1_epi8( 'z' + 1 );

        const simde__m128i simdUppercaseConversionDiff = simde_mm_set1_epi8( 32 );

        for ( char * pszStringIterator = pa16szStringToConvert; *pszStringIterator; pszStringIterator += 16 )
        {
            const simde__m128i simdCurrentChars = simde_mm_load_si128( ( simde__m128i * )pszStringIterator );

            const simde__m128i simdGreaterThana = simde_mm_cmpgt_epi8( simdCurrentChars, simdAsciia );
            const simde__m128i simdLessThanz = simde_mm_cmplt_epi8( simdCurrentChars, simdAsciiz );

            const simde__m128i simdValidCharsMask = simde_mm_and_si128( simdGreaterThana, simdLessThanz );

            const simde__m128i simdSubtractMask = simde_mm_and_si128( simdValidCharsMask, simdUppercaseConversionDiff );
            const simde__m128i simdConvertedChars = simde_mm_sub_epi8( simdCurrentChars, simdSubtractMask );

            simde_mm_storeu_si128( ( simde__m128i * )pszStringIterator, simdConvertedChars );

            const simde__m128i simdIsNullchar = simde_mm_cmpeq_epi8( simdCurrentChars, simde_mm_setzero_si128() );
            if ( simde_mm_movemask_epi8( simdIsNullchar ) )
                break;
        }
    }
    */
};

#if HAS_TESTS
    TEST( string )
    {
        TEST_EXPECT( util::string::CStringStartsWith( "renderer:showfps", "renderer" ) );
        return true;
    }
#endif // #if HAS_TESTS
