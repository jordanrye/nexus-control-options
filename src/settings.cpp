#include <filesystem>
#include <fstream>

#include "keybinds/keybinds.h"

#include "settings.h"

namespace Settings
{
    std::mutex Mutex;
	std::filesystem::path SettingsPath;
    json Settings = json::object();

    void Load(const std::filesystem::path& aPath)
    {
		SettingsPath = aPath;

		Mutex.lock();
		{
			try
			{
				std::ifstream file(SettingsPath);
				Settings = json::parse(file);
				file.close();
			}
			catch (json::parse_error& ex)
			{
				APIDefs->Log(ELogLevel_WARNING, "Extended Control Options", "settings.json could not be parsed.");
				APIDefs->Log(ELogLevel_WARNING, "Extended Control Options", ex.what());
			}
		}
		Mutex.unlock();

		if (!Settings.is_null())
		{
			if (!Settings["AUTO_ADJUST_ZOOM_FOV"].is_null()) { Settings["AUTO_ADJUST_ZOOM_FOV"].get_to(AutoAdjustZoomFOV); }
			if (!Settings["AUTO_ADJUST_ZOOM_MAP"].is_null()) { Settings["AUTO_ADJUST_ZOOM_MAP"].get_to(AutoAdjustZoomMap); }
			if (!Settings["DBLCLK_DEFAULT_INTERVAL"].is_null()) { Settings["DBLCLK_DEFAULT_INTERVAL"].get_to(DoubleClickDefaultInterval); }
		}
    }

    void Save()
    {
		Settings["AUTO_ADJUST_ZOOM_FOV"] = AutoAdjustZoomFOV;
		Settings["AUTO_ADJUST_ZOOM_MAP"] = AutoAdjustZoomMap;
		Settings["DBLCLK_DEFAULT_INTERVAL"] = DoubleClickDefaultInterval;

		Mutex.lock();
		{
			std::ofstream file(SettingsPath);
			file << Settings.dump(1, '\t') << std::endl;
			file.close();
		}
		Mutex.unlock();
    }

	void ToggleDoubleClickModal(std::string modalName)
	{
		if (ImGui::BeginPopupModal(modalName.c_str()))
		{
			static bool isSetCursorPos = false;
			bool closeModal = false;

			if (!isSetCursorPos)
			{
				GetCursorPos(&doubleClickCursorPos);
				isSetCursorPos = true;
			}

			isDoubleClickActive = false;
			isDoubleClickPosFixed = false;

			ImGui::InputFloat(std::string("seconds##" + modalName).c_str(), &doubleClickInterval, 0.05F, 0.25F, "%.2f");

			if (doubleClickInterval < 0.0F)
			{
				doubleClickInterval = 0.0F;
			}

			if (ImGui::Button(std::string("Start##" + modalName).c_str()))
			{
				isDoubleClickActive = true;
				isDoubleClickPosFixed = true;
				closeModal = true;
			}

			ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine(); ImGui::Spacing();
			ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine(); ImGui::Spacing();
			ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine(); ImGui::Spacing();
			ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine(); ImGui::Spacing();
			ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine(); ImGui::Spacing();
			ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine(); ImGui::Spacing();
			ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine(); ImGui::Spacing();
			ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine(); ImGui::Spacing();

			ImGui::SameLine();
			if (ImGui::Button(std::string("Cancel##" + modalName).c_str()))
			{
				// set back to default value
				doubleClickInterval = DoubleClickDefaultInterval;
				closeModal = true;
			}

			if (closeModal)
			{
				isSettingDoubleClick = false;
				isSetCursorPos = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	/* Set Keybind */
	Keybind CurrentKeybind {};
	bool isSettingKeybind = false;

	/* Settings */
	bool AutoAdjustZoomFOV = false;
	bool AutoAdjustZoomMap = false;
	float DoubleClickDefaultInterval = 0.15F;
	
	/* Toggle Double-Click */
	bool isSettingDoubleClick = false;
	bool isDoubleClickActive = false;
	bool isDoubleClickPosFixed = false;
	float doubleClickInterval = 0.15F;
	UINT doubleClickTexIt = 0U;
	std::string doubleClickKeybindId = "(null)";
	POINT doubleClickCursorPos = { 0 };
} // namespace Settings
