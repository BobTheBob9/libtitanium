#include "commandline.hpp"

#include <libtitanium/logger/logger.hpp>
#include <libtitanium/util/assert.hpp>
#include <libtitanium/util/static_array.hpp>
#include <libtitanium/util/string.hpp>
#include <libtitanium/memory/mem_core.hpp>
#include <libtitanium/dev/tests.hpp>

#include <string.h>

namespace util
{
    void CommandArgs::CreateFromSystemWithAlloc( CommandArgs *const pcaCommandArgs, const int nArgs, const char *const *const ppszArgs )
    {
        // TODO: if we get a proper string copy function that takes the null terminator also, use this here
        // also
        // TODO update for new mem alloc stuff

        // Figure out the size of the buffer in pcaCommandArgs.m_sBuffer
        uint nBufSize = 0;
        for ( int i = 0; i < nArgs; i++ )
        {
            nBufSize += util::string::LengthOfCStringWithTerminator( ppszArgs[ i ] );
        }

        // create copy strings to the argument buffer
        size_t nBufIndex = 0;
        char *const pArgBuffer = memory::alloc_nT<char>( nBufSize );
        for ( int i = 0; i < nArgs; i++ )
        {
            size_t nCopyAmount = util::string::LengthOfCStringWithTerminator( ppszArgs[ i ] );
            memcpy( pArgBuffer + nBufIndex, ppszArgs[ i ], nCopyAmount );

            nBufIndex += nCopyAmount;
        }

        // copy to out command args struct
        pcaCommandArgs->m_eBufferSource = EBufferSource::SYSTEM_COPY;
        pcaCommandArgs->m_nArgumentStrings = nArgs;
        pcaCommandArgs->m_sBuffer = { .nLength = nBufSize, .pData = pArgBuffer };
    }

    /*
     *  For when the buffer you're passing to this function is created by you, and so you control its lifetime
     *  CommandArgs structs created from this function allocate no memory, instead just using the pointer passed to this function as their buffer
     *  This function destructively replaces separators between arguments with null pointers, use CreateFromBufferWithAlloc to create CommandArgs non-destructively, with a memory allocation
     */
    void CommandArgs::CreateFromBufferDestructive( CommandArgs *const pcaCommandArgs )
    {
        (void)pcaCommandArgs;
        // TODO: this
    }

    void CommandArgs::CreateFromBufferWithAlloc( CommandArgs *const pcaCommandArgs )
    {
        (void)pcaCommandArgs;
        // TODO: this
    }

    /*
     *  Gets the next argument given the current argument in the argument buffer, if no arguments are left, returns nullptr
     *  if pszCurrentArgument is nullptr, returns the first argument in the buffer
     */
    const char * CommandArgs::GetNextArgument( CommandArgs *const pcaCommandArgs, const char *const pszCurrentArgument )
    {
        if ( !pszCurrentArgument ) [[ unlikely ]]
        {
            return pcaCommandArgs->m_sBuffer.pData;
        }

        const char *const pBufUpperIndex = pcaCommandArgs->m_sBuffer.pData + pcaCommandArgs->m_sBuffer.nLength;
        assert::Debug( util::maths::WithinRange<const void *>( pszCurrentArgument, pcaCommandArgs->m_sBuffer.pData, pBufUpperIndex ) );

        // iterate from the current arg, to the next, then return it
        // if we exceed the argument buffer, return nullptr
        for ( const char * pszCharIterator = pszCurrentArgument + 1; pszCharIterator < pBufUpperIndex; pszCharIterator++ )
        {
            if ( !*( pszCharIterator - 1 ) ) 
            {
                return pszCharIterator;
            }
        }

        return nullptr;
    }

    /*
     *  Finds the value of the argument passed into the function, i.e. the string immediately after it
     */
    const char * CommandArgs::GetArgumentValue( CommandArgs *const pcaCommandArgs, const char *const pszArgumentToFind )
    { 
        // find the position of the argument passed in
        const char * pszArgIterator = nullptr;
        while ( ( pszArgIterator = GetNextArgument( pcaCommandArgs, pszArgIterator ) ) )
        {
            if ( !strcmp( pszArgIterator, pszArgumentToFind ) )
            {
                break;
            }
        }

        if ( !pszArgIterator )
        {
            return nullptr;
        }

        // return the value of the argument (string after the passed in argument), or nullptr if buffer is over
        return GetNextArgument( pcaCommandArgs, pszArgIterator );
    }

    /*
     *  
     */
    CommandArgs::R_FindArgumentPair CommandArgs::GetNextArgumentPairByWildcard( CommandArgs *const pcaCommandArgs, const char *const pszSearchString, const size_t nSearchStringLength, const char *const pszCurrentArgument )
    {
        const char * pszArgIterator = pszCurrentArgument;
        while ( ( pszArgIterator = GetNextArgument( pcaCommandArgs, pszArgIterator ) ) )
        {
            if ( !strncmp( pszArgIterator, pszSearchString, nSearchStringLength ) )
            {
                R_FindArgumentPair r_pair;
                r_pair.pszKey = pszArgIterator;
                r_pair.pszValue = GetNextArgument( pcaCommandArgs, pszArgIterator );
                r_pair.bFound = r_pair.pszValue != nullptr;

                return r_pair;
            }
        }

        return { .bFound = false };
    }

    bool CommandArgs::HasArgument( CommandArgs *const pcaCommandArgs, const char *const pszArgumentToFind )
    {
        const char * pszArgIterator = nullptr;
        while ( ( pszArgIterator = GetNextArgument( pcaCommandArgs, pszArgIterator ) ) )
        {
            if ( util::string::CStringsEqual( pszArgIterator, pszArgumentToFind ) )
            {
                return true;
            }
        }

        return false;
    }

    void CommandArgs::FreeMembers( CommandArgs *const pcaCommandArgs )
    {
        if ( pcaCommandArgs->m_eBufferSource < CommandArgs::EBufferSource::__MAX_COPY )
        {
            memory::free( pcaCommandArgs->m_sBuffer.pData );
        }
    }
}

#if HAS_TESTS
    TEST( Commandline )
    {
        util::CommandArgs caArgs;
        const char *const szCommandlineArgs[] {
            "hi", 
            "+connect", "localhost", 
            "wow", 
            "+sv_cheats", "1", 
            "+mp_enablematchending", "5", 
            "cool" 
        };
        
        util::CommandArgs::CreateFromSystemWithAlloc( &caArgs, util::StaticArray_Length( szCommandlineArgs ), szCommandlineArgs );
        
        TEST_EXPECT( caArgs.m_nArgumentStrings == util::StaticArray_Length( szCommandlineArgs ) );
        TEST_EXPECT( caArgs.m_eBufferSource == util::CommandArgs::CommandArgs::EBufferSource::SYSTEM_COPY );

        {
            const char * pszArgIterator = nullptr;
            int i = 0;
            while ( ( pszArgIterator = util::CommandArgs::GetNextArgument( &caArgs, pszArgIterator ) ) )
            {
                TEST_EXPECT( !strcmp( pszArgIterator, szCommandlineArgs[ i++ ] ) );
            }
        }

        TEST_EXPECT( !strcmp( util::CommandArgs::GetArgumentValue( &caArgs, "+sv_cheats" ), "1" ) );
        TEST_EXPECT( !strcmp( util::CommandArgs::GetArgumentValue( &caArgs, "+connect" ), "localhost" ) );

        {
            // find the values of all arguments prefixed with "+", e.g. for finding convar values
            util::CommandArgs::R_FindArgumentPair argIterator {};
            while ( ( argIterator = util::CommandArgs::GetNextArgumentPairByWildcard( &caArgs, "+", 1, argIterator.pszValue ) ).bFound )
            {
                TEST_EXPECT( !strcmp( util::CommandArgs::GetArgumentValue( &caArgs, argIterator.pszKey ), argIterator.pszValue ) );
            }
        }

        TEST_EXPECT( util::CommandArgs::GetArgumentValue( &caArgs, "cool" ) == nullptr );

        return true;
    }
#endif // #if HAS_TESTS
