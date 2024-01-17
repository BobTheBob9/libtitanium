#include "config.hpp"


#include <stdio.h>

#include <libtitanium/util/data/stringbuf.hpp>
#include <libtitanium/util/string.hpp>
#include <libtitanium/memory/mem_core.hpp>
#include <libtitanium/util/data/span.hpp>
#include <libtitanium/util/data/span_dynamic.hpp>

namespace config
{
    ENUM_FLAGS( EFVarSetFlags );
    ENUM_FLAGS( EFVarUsageFlags );

    static util::data::SpanDynamic<Var *> s_vpcvarVars;

    void VarBool_ToString( const void *const pCvarPointer, util::data::Span<char> o_spszOutputBuffer )
    {
        util::string::CopyTo( *static_cast<const bool *const>( pCvarPointer ) ? "true" : "false", o_spszOutputBuffer );
    }

    void VarBool_SuggestValues( const void *const pCvarPointer, const char *const pszIncompleteValue, util::data::Span<util::data::StringBuf<32>> o_sspszOutputBuffer )
    {
        (void)pCvarPointer;

        constexpr const char *const BOOL_VALUES[] { "true", "false" };

        uint nOutputIndex = 0;
        for ( uint i = 0; i < sizeof( BOOL_VALUES ) / sizeof( char * ) && nOutputIndex < o_sspszOutputBuffer.nLength; i++ )
        {
            if ( util::string::CStringStartsWith( BOOL_VALUES[ i ], pszIncompleteValue ) )
            {
                o_sspszOutputBuffer.pData[ nOutputIndex++ ] = BOOL_VALUES[ i ];
            }
        }
    }

    void VarBool_SetFromString( void *const pCvarPointer, const char *const pszValue )
    {
        util::data::StringBuf<32> pszLowercaseValue = pszValue;
        util::string::ToLowercase( pszLowercaseValue.m_szStr );

        *static_cast<bool* const>( pCvarPointer ) = util::string::CStringsEqual( pszLowercaseValue, "true" ) || atoi( pszLowercaseValue ) > 0;
    }



    void VarFloat_ToString( const void *const pCvarPointer, util::data::Span<char> o_spszOutputBuffer )
    {
        char szStringified[32];
        (void)gcvt( *static_cast<const float *const>( pCvarPointer ), 3, szStringified );
        util::string::CopyTo( szStringified, o_spszOutputBuffer );
    }

    void VarFloat_SuggestValues( const void *const pCvarPointer, const char *const pszIncompleteValue, util::data::Span<util::data::StringBuf<32>> o_sspszOutputBuffer )
    {
        (void)pCvarPointer;
        (void)pszIncompleteValue;
        (void)o_sspszOutputBuffer;
    }

    void VarFloat_SetFromString( void *const pCvarPointer, const char *const pszValue )
    {
        *static_cast<float *const>( pCvarPointer ) = atof( pszValue );
    }


    Var * RegisterVar( const char *const pszName, const EFVarUsageFlags efUsage, Var::SetFuncs setFuncs, void *const pValue )
    {
        Var * pCvar = memory::alloc_nT<Var>( 1 );
        *pCvar = {
            .szName {},
            .efUsageFlags = efUsage,
            .setFuncs = setFuncs,
            .pValue = pValue
        };

        util::string::CopyTo( pszName, util::data::Span<char>( sizeof( pCvar->szName ), pCvar->szName ) );

        util::data::SpanDynamic<Var *>::AppendTo( &s_vpcvarVars, pCvar );
        return pCvar;
    }

    Var * FindVar( const char *const pszVarName )
    {
        for ( uint i = 0; i < s_vpcvarVars.sData.nLength; i++ )
        {
            if ( util::string::CStringsEqual( pszVarName, s_vpcvarVars.sData.pData[ i ]->szName ) )
            {
                return s_vpcvarVars.sData.pData[ i ];
            }
        }

        return nullptr;
    }

    void FindVarsStartingWith( const char *const pszVarSearchString, util::data::Span<Var *> o_spcvarVars )
    {
        uint nFoundVars = 0;
        for ( uint i = 0; i < s_vpcvarVars.sData.nLength && nFoundVars < o_spcvarVars.nLength; i++ )
        {
            if ( util::string::CStringStartsWith( s_vpcvarVars.sData.pData[ i ]->szName, pszVarSearchString ) )
            {
                o_spcvarVars.pData[ nFoundVars++ ] = s_vpcvarVars.sData.pData[ i ];
            }
        }
    }

    // TODO: this sucks, and doesn't free everything
    void FreeVars()
    {
        for ( uint i = 0; i < s_vpcvarVars.sData.nLength; i++ )
        {
            memory::free( s_vpcvarVars.sData.pData[ i ] );
        }

        util::data::SpanDynamic<Var *>::SetLength( &s_vpcvarVars, 0 );
    }
};
