#ifndef IMGUI_EXTENSIONS_H
#define IMGUI_EXTENSIONS_H

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

namespace ImGui
{
    bool Tooltip();
    void TooltipGeneric(const char* str, ...);

    void TextPadded(const char* str, float paddingX, float paddingY);
    void TextWrappedPadded(const char* str, float paddingX, float paddingY);

    bool SelectablePadded(const char* label, bool selected, float paddingX, float paddingY);

    void BeginGroupPanel(const char* name, const ImVec2& size);
    void EndGroupPanel();

} // namespace ImGui

#endif /* IMGUI_EXTENSIONS_H */