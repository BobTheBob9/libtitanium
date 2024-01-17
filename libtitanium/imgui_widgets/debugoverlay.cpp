#include "imgui.h"

#include <libtitanium/config/config.hpp>

namespace imguiwidgets
{
    static bool s_bShowDebugOverlays = true;
    config::Var * pcvarShowDebugOverlays = config::RegisterVar( "dev::debugoverlays", config::EFVarUsageFlags::NONE, config::VARF_BOOL, &s_bShowDebugOverlays );

    bool BeginDebugOverlay()
    {
        if ( s_bShowDebugOverlays )
        {
            ImGui::SetNextWindowPos( ImVec2( 0.f, 0.f ) );
            ImGui::Begin( "Debug Overlay", nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking );

            return true;
        }
        else
        {
            return false;
        }
    }
}
