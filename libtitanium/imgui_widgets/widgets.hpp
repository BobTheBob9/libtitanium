#pragma once

#include <imgui/imgui.h>

#include <libtitanium/util/data/span.hpp>
#include <libtitanium/util/data/stringbuf.hpp>

namespace imguiwidgets
{
    void Console( util::data::Span<char> spszConsoleInput, void * pCallbackUserData,
                  void ( *fnCommandHintCallback )( const util::data::Span<char> spszConsoleInput, const util::data::Span<util::data::StringBuf<128>> o_spszAutocompleteItems, void * pCallbackUserData ),
                  void ( *fnCommandCompletionCallback )( const util::data::Span<char> spszConsoleInput, void * pCallbackUserData ) );
    void ResourceProfiler();

    bool BeginDebugOverlay();
}
