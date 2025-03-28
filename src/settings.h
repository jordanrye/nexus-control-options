#ifndef SETTINGS_H
#define SETTINGS_H

#include <mutex>

#include "imgui/imgui.h"
#include "nlohmann/json.hpp"

#include "shared.h"

using json = nlohmann::json;

namespace Settings
{
	extern std::mutex Mutex;
	extern std::filesystem::path SettingsPath;
	extern json Settings;

	void Load(const std::filesystem::path& aPath);
	void Save();

	void ToggleDoubleClickModal(std::string modalName);

	/* Set Keybind */
	extern Keybind CurrentKeybind;
	extern bool isSettingKeybind;

	/* Settings */
	extern bool AutoAdjustZoomFOV;
	extern bool AutoAdjustZoomMap;
	extern float DoubleClickDefaultInterval;

	/* Toggle Double-Click */
	extern bool isSettingDoubleClick;
	extern bool isDoubleClickActive;
	extern bool isDoubleClickPosFixed;
	extern float doubleClickInterval;
	extern UINT doubleClickTexIt;
	extern std::string doubleClickKeybindId;
	extern POINT doubleClickCursorPos;
} // namespace Settings

namespace ImGui
{
	static ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
	{
		return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}
	
	static bool Tooltip()
	{
		bool hovered = ImGui::IsItemHovered();
		if (hovered)
		{
			ImGui::BeginTooltip();
		}
		return hovered;
	}

	static void TooltipGeneric(const char* str, ...)
	{
		if (ImGui::Tooltip())
		{
			ImGui::Text(str);
			ImGui::EndTooltip();
		}
	}

	static void TextPadded(const char* str, float paddingX, float paddingY)
	{
		ImVec2 textSize = ImGui::CalcTextSize(str);
		ImVec2 cursorStart = ImGui::GetCursorPos();
		ImGui::InvisibleButton("##TextPadded", textSize + ImVec2(paddingX * 2, paddingY * 2));
		ImVec2 cursorFinal = ImGui::GetCursorPos();
		ImGui::SetCursorPos(cursorStart + ImVec2(paddingX, paddingY));
		ImGui::Text(str);
		ImGui::SetCursorPos(cursorFinal);
	}

	static void TextWrappedPadded(const char* str, float paddingX, float paddingY)
	{
		ImVec2 textSize = ImGui::CalcTextSize(str);
		ImVec2 cursorStart = ImGui::GetCursorPos();
		ImGui::InvisibleButton("##TextWrappedPadded", textSize + ImVec2(paddingX * 2, paddingY * 2));
		ImVec2 cursorFinal = ImGui::GetCursorPos();
		ImGui::SetCursorPos(cursorStart + ImVec2(paddingX, paddingY));
		ImGui::TextWrapped(str);
		ImGui::SetCursorPos(cursorFinal);
	}

	static bool SelectablePadded(const char* label, bool selected, float paddingX, float paddingY)
	{
		//ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {paddingX, paddingY});
		return ImGui::Selectable(label, selected);
		//ImGui::PopStyleVar(1);
	}

} // namespace ImGui

#endif /* SETTINGS_H */
