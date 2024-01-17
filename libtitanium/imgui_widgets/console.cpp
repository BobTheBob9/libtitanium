#include "imgui.h"
#include "widgets.hpp"

#include <libtitanium/util/static_array.hpp>
#include <libtitanium/config/config.hpp>
#include <libtitanium/util/data/span.hpp>
#include <libtitanium/util/data/stringbuf.hpp>

namespace imguiwidgets
{
    // TODO: using void pointers for userdata sucks here, should be templated

    struct C_ConsoleUserData
    {
        char * pszSelectedEntry;
        int nSelectionChange;
    };

    int C_ConsoleInput( ImGuiInputTextCallbackData *const pimguiCallbackData )
    {
        C_ConsoleUserData * pCallbackUserData = static_cast<C_ConsoleUserData *>( pimguiCallbackData->UserData );
        if ( pimguiCallbackData->EventFlag == ImGuiInputTextFlags_CallbackCompletion )
        {
            pimguiCallbackData->BufDirty = true;
            strcpy( pimguiCallbackData->Buf, pCallbackUserData->pszSelectedEntry );
            pimguiCallbackData->BufTextLen = strlen( pimguiCallbackData->Buf );
            pimguiCallbackData->CursorPos = pimguiCallbackData->BufTextLen;
        }
        else if ( pimguiCallbackData->EventFlag == ImGuiInputTextFlags_CallbackHistory && ImGui::GetIO().KeyShift )
        {
            pCallbackUserData->nSelectionChange = pimguiCallbackData->EventKey == ImGuiKey_DownArrow ? -1 : 1;
        }

        return 0;
    }

    void Console( util::data::Span<char> spszConsoleInput, void * pCallbackUserData,
                  void ( *fnCommandHintCallback )( const util::data::Span<char> spszConsoleInput, const util::data::Span<util::data::StringBuf<128>> o_spszAutocompleteItems, void * pCallbackUserData ),
                  void ( *fnCommandCompletionCallback )( const util::data::Span<char> spszConsoleInput, void * pUserData ) )
    {
        if ( ImGui::Begin( "Developer Console" ) )
        {
            util::data::StringBuf<128> szAutocompleteItems[10];
            fnCommandHintCallback( spszConsoleInput, util::StaticArray_ToSpan( szAutocompleteItems ), pCallbackUserData );

            // TODO: do this in a dropdown
            ImGui::InputTextMultiline( "##ConsoleLog", util::data::StringBuf<1>().ToCStr(), 0, ImVec2( 0, -( ImGui::GetTextLineHeightWithSpacing() * 1.1 ) ), ImGuiInputTextFlags_ReadOnly );

            // TODO: do this in callback
            C_ConsoleUserData callbackUserData { .pszSelectedEntry = szAutocompleteItems[ 0 ].ToCStr() };
            if ( ImGui::InputText( "Input", spszConsoleInput.pData, spszConsoleInput.nLength, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, C_ConsoleInput, &callbackUserData ) )
            {
                fnCommandCompletionCallback( spszConsoleInput, pCallbackUserData );
                memset( spszConsoleInput.pData, '\0', spszConsoleInput.nLength );
            }

            ImVec2 vAutocompletePos = ImGui::GetItemRectMin();
            vAutocompletePos.y += ImGui::GetItemRectSize().y;

            if ( util::StaticArray_Length( szAutocompleteItems ) && *szAutocompleteItems[ 0 ].m_szStr )
            {
                ImGui::SetNextWindowPos( vAutocompletePos );
                ImGui::Begin( "ConsoleAutocomplete", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |   ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing );
                {
                    for ( uint i = 0; i < util::StaticArray_Length( szAutocompleteItems ) && *szAutocompleteItems[ i ].m_szStr; i++ )
                    {
                        ImGui::PushID( i );
                        if ( ImGui::Selectable( szAutocompleteItems[ i ].ToCStr(), false ) )
                        {
                            strcpy( spszConsoleInput.pData, szAutocompleteItems[ 0 ].ToCStr() );
                        }
                        ImGui::PopID();
                    }
                }
                ImGui::End();
            }
        }
        ImGui::End();
    }
}
