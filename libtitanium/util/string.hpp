#pragma once

#include <libtitanium/util/numerics.hpp>
#include <libtitanium/util/data/span.hpp>

/*

Functions for more easily performing common string operations

*/
namespace util::string
{
    // TODO: these kind of suck
    // TODO: would be nice to just generally abandon c strings, and move to span-based stuff exclusively
    //       unfortunately though, everything else uses c strings! (imgui especially :c which we call with c-strings alot of times per frame)
    //       per haps the solution to this is to make a new ui framework Clueless

    int LengthOfCString( const char *const pszString );
    int LengthOfCStringWithTerminator( const char *const pszString );
    int LengthOfSpanString( const util::data::Span<char> spszString );
    void CopyTo( const char *const pszSource, util::data::Span<char> spszDestinationBuffer );
    void ConcatinateTo( const char* const pszSource, util::data::Span<char> spszDestinationBuffer );
    void ToLowercase( char *const pszString );
    void ToUppercase( char *const pszString );
    bool CStringsEqual( const char *const pszFirstString, const char *const pszSecondString );
    bool CStringStartsWith( const char *const pszFirstString, const char *const pszSecondString );
};
