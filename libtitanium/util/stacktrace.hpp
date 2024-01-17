#pragma once

#include <libtitanium/util/data/span.hpp>

namespace util
{
    struct StackMember
    {
        void * pAddress;
        char szFuncName[ 256 ];
    };

    void GetStacktrace( util::data::Span<StackMember> o_sStackTrace );
}
