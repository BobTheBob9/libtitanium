#pragma once

#include <libtitanium/util/numerics.hpp>
#include <libtitanium/util/maths.hpp>
#include <libtitanium/util/data/span.hpp>

namespace util
{
    struct CommandArgs
    {
        enum class EBufferSource
        {
            SYSTEM_COPY,
            USER_COPY,
            __MAX_COPY, // all copied buffer sources are less than this
            
            USER_VIEW
        };

        EBufferSource m_eBufferSource;

        // buffer holding all args
        util::data::Span<char> m_sBuffer;
        u32 m_nArgumentStrings;

        static void CreateFromSystemWithAlloc( CommandArgs *const pcaCommandArgs, const int nArgs, const char *const *const ppszArgs );

        /*
         *  For when the buffer you're passing to this function is created by you, and so you control its lifetime
         *  CommandArgs structs created from this function allocate no memory, instead just using the pointer passed to this function as their buffer
         *  This function destructively replaces separators between arguments with null pointers, use CreateFromBufferWithAlloc to create CommandArgs non-destructively, with a memory allocation
         */
        static void CreateFromBufferDestructive( CommandArgs *const pcaCommandArgs );

        static void CreateFromBufferWithAlloc( CommandArgs *const pcaCommandArgs );

        /*
         *  Gets the next argument given the current argument in the argument buffer, if no arguments are left, returns nullptr
         *  if pszCurrentArgument is nullptr, returns the first argument in the buffer
         */
        static const char * GetNextArgument( CommandArgs *const pcaCommandArgs, const char *const pszCurrentArgument );

        /*
         *  Finds the value of the argument passed into the function, i.e. the string immediately after it
         */
        static const char * GetArgumentValue( CommandArgs *const pcaCommandArgs, const char *const pszArgumentToFind );

        struct R_FindArgumentPair
        {
            bool bFound;

            const char * pszKey;
            const char * pszValue;
        };

        static R_FindArgumentPair GetNextArgumentPairByWildcard( CommandArgs *const pcaCommandArgs, const char *const pszSearchString, const size_t nSearchStringLength, const char *const pszCurrentArgument );

        static bool HasArgument( CommandArgs *const pcaCommandArgs, const char *const pszArgumentToFind );

        static void FreeMembers( CommandArgs *const pcaCommandArgs );
    };
}
