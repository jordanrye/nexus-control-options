#ifndef SETTINGS_H
#define SETTINGS_H

#include <mutex>

#include "imgui/imgui.h"
#include "imgui_extensions.h"
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

#endif /* SETTINGS_H */
