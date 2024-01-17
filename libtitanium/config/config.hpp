#pragma once

#include <type_traits>

#include <libtitanium/util/numerics.hpp>
#include <libtitanium/util/enum_flags.hpp>
#include <libtitanium/util/data/span.hpp>
#include <libtitanium/util/data/stringbuf.hpp>

namespace config
{
    enum class EFVarSetFlags
    {
        NONE = 0,

        NON_CODE_SOURCE = ( 1 << 0 ), // This call isn't code-driven, it's from a user or game provided call
        ALLOW_SET_STARTUP = ( 1 << 1 ), // This call can set vars with ConfigVar::EFUsageFlags::STARTUP

        SKIP_ALL_CHECKS = ( 1 << 2 )
    };
    ENUM_FLAGS_DECLARE( EFVarSetFlags );
    
    enum class EFVarUsageFlags
    {
        NONE = 0,

        READONLY = ( 1 << 0 ), // The config var can be read, but can't be changed by users
        STARTUP = ( 1 << 1 ), // The config var can only be set during startup (e.g. through startup args, or cfg on startup)
        TEMPORARY = ( 1 << 2 ), // Hint that the config var is likely to be destroyed during the program's runtime other than shutdown (good for code-created cvars!!)
    };
    ENUM_FLAGS_DECLARE( EFVarUsageFlags );

    enum class EVarSetResult
    {
        SUCCESS,

        INVALID_READONLY,
        INVALID_STARTUP,
    };

    // interface for querying untyped config vars
    struct Var
    {
        char szName[ 128 ];
        EFVarUsageFlags efUsageFlags;

        struct SetFuncs
        {
            using FnVarToString = void(*)( const void *const pCvarPointer, util::data::Span<char> o_spszOutputBuffer );
            // TODO: should this even include pCvarPointer?
            using FnVarSuggestValues = void(*)( const void *const pCvarPointer, const char *const pszIncompleteValue, util::data::Span<util::data::StringBuf<32>> o_sspszOutputBuffer );
            using FnVarSetFromString = void(*)( void *const pCvarPointer, const char *const pszValue );

            FnVarToString fnToString;
            FnVarSuggestValues fnSuggestValues;
            FnVarSetFromString fnSetFromString;
        } setFuncs;

        bool bDirty = false;

        void * pValue;
    };

    void VarBool_ToString( const void *const pCvarPointer, util::data::Span<char> o_spszOutputBuffer );
    void VarBool_SuggestValues( const void *const pCvarPointer, const char *const pszIncompleteValue, util::data::Span<util::data::StringBuf<32>> o_sspszOutputBuffer );
    void VarBool_SetFromString( void *const pCvarPointer, const char *const pszValue );
    constexpr Var::SetFuncs VARF_BOOL = { .fnToString = VarBool_ToString, .fnSuggestValues = VarBool_SuggestValues, .fnSetFromString = VarBool_SetFromString };

    void VarFloat_ToString( const void *const pCvarPointer, util::data::Span<char> o_spszOutputBuffer );
    void VarFloat_SuggestValues( const void *const pCvarPointer, const char *const pszIncompleteValue, util::data::Span<util::data::StringBuf<32>> o_sspszOutputBuffer );
    void VarFloat_SetFromString( void *const pCvarPointer, const char *const pszValue );
    constexpr Var::SetFuncs VARF_FLOAT = { .fnToString = VarFloat_ToString, .fnSuggestValues = VarFloat_SuggestValues, .fnSetFromString = VarFloat_SetFromString };

    Var * RegisterVar( const char *const pszName, const EFVarUsageFlags efUsage, Var::SetFuncs setFuncs, void *const pValue );
    Var * FindVar( const char *const pszVarName );
    void FindVarsStartingWith( const char *const pszVarSearchString, util::data::Span<Var *> o_spcvarVars );
    void FreeVars();
};
