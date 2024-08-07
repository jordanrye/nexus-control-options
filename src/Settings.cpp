#include <filesystem>
#include <fstream>

#include "keybinds/Keybinds.h"

#include "Settings.h"

namespace Settings
{
    std::mutex Mutex;
	std::filesystem::path SettingsPath;
    json Settings = json::object();

    void Load(const std::filesystem::path& aPath)
    {
		SettingsPath = aPath;

		if (!std::filesystem::exists(SettingsPath))
		{
			// Default keybinds
			MoveForwardKeybind.Key = MapVirtualKeyA('W', MAPVK_VK_TO_VSC);
			DodgeKeybind.Key = MapVirtualKeyA('V', MAPVK_VK_TO_VSC);
			JumpKeybind.Key = MapVirtualKeyA(VK_SPACE, MAPVK_VK_TO_VSC);
			ZoomOutKeybind.Key = MapVirtualKeyA(VK_NEXT, MAPVK_VK_TO_VSC);
			return;
		}

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
			if (!Settings["MOVE_FORWARD_KEY"].is_null()) { Settings["MOVE_FORWARD_KEY"].get_to(MoveForwardKeybind.Key); }
			if (!Settings["MOVE_FORWARD_ALT"].is_null()) { Settings["MOVE_FORWARD_ALT"].get_to(MoveForwardKeybind.Alt); }
			if (!Settings["MOVE_FORWARD_CTRL"].is_null()) { Settings["MOVE_FORWARD_CTRL"].get_to(MoveForwardKeybind.Ctrl); }
			if (!Settings["MOVE_FORWARD_SHIFT"].is_null()) { Settings["MOVE_FORWARD_SHIFT"].get_to(MoveForwardKeybind.Shift); }
			
			if (!Settings["DODGE_KEY"].is_null()) { Settings["DODGE_KEY"].get_to(DodgeKeybind.Key); }
			if (!Settings["DODGE_ALT"].is_null()) { Settings["DODGE_ALT"].get_to(DodgeKeybind.Alt); }
			if (!Settings["DODGE_CTRL"].is_null()) { Settings["DODGE_CTRL"].get_to(DodgeKeybind.Ctrl); }
			if (!Settings["DODGE_SHIFT"].is_null()) { Settings["DODGE_SHIFT"].get_to(DodgeKeybind.Shift); }

			if (!Settings["JUMP_KEY"].is_null()) { Settings["JUMP_KEY"].get_to(JumpKeybind.Key); }
			if (!Settings["JUMP_ALT"].is_null()) { Settings["JUMP_ALT"].get_to(JumpKeybind.Alt); }
			if (!Settings["JUMP_CTRL"].is_null()) { Settings["JUMP_CTRL"].get_to(JumpKeybind.Ctrl); }
			if (!Settings["JUMP_SHIFT"].is_null()) { Settings["JUMP_SHIFT"].get_to(JumpKeybind.Shift); }

			if (!Settings["ABOUT_FACE_KEY"].is_null()) { Settings["ABOUT_FACE_KEY"].get_to(AboutFaceKeybind.Key); }
			if (!Settings["ABOUT_FACE_ALT"].is_null()) { Settings["ABOUT_FACE_ALT"].get_to(AboutFaceKeybind.Alt); }
			if (!Settings["ABOUT_FACE_CTRL"].is_null()) { Settings["ABOUT_FACE_CTRL"].get_to(AboutFaceKeybind.Ctrl); }
			if (!Settings["ABOUT_FACE_SHIFT"].is_null()) { Settings["ABOUT_FACE_SHIFT"].get_to(AboutFaceKeybind.Shift); }

			if (!Settings["DODGE_JUMP_KEY"].is_null()) { Settings["DODGE_JUMP_KEY"].get_to(DodgeJumpKeybind.Key); }
			if (!Settings["DODGE_JUMP_ALT"].is_null()) { Settings["DODGE_JUMP_ALT"].get_to(DodgeJumpKeybind.Alt); }
			if (!Settings["DODGE_JUMP_CTRL"].is_null()) { Settings["DODGE_JUMP_CTRL"].get_to(DodgeJumpKeybind.Ctrl); }
			if (!Settings["DODGE_JUMP_SHIFT"].is_null()) { Settings["DODGE_JUMP_SHIFT"].get_to(DodgeJumpKeybind.Shift); }

			if (!Settings["MOVE_ABOUT_FACE_KEY"].is_null()) { Settings["MOVE_ABOUT_FACE_KEY"].get_to(MoveAboutFaceKeybind.Key); }
			if (!Settings["MOVE_ABOUT_FACE_ALT"].is_null()) { Settings["MOVE_ABOUT_FACE_ALT"].get_to(MoveAboutFaceKeybind.Alt); }
			if (!Settings["MOVE_ABOUT_FACE_CTRL"].is_null()) { Settings["MOVE_ABOUT_FACE_CTRL"].get_to(MoveAboutFaceKeybind.Ctrl); }
			if (!Settings["MOVE_ABOUT_FACE_SHIFT"].is_null()) { Settings["MOVE_ABOUT_FACE_SHIFT"].get_to(MoveAboutFaceKeybind.Shift); }

			if (!Settings["ZOOM_OUT_KEY"].is_null()) { Settings["ZOOM_OUT_KEY"].get_to(ZoomOutKeybind.Key); }
			if (!Settings["ZOOM_OUT_ALT"].is_null()) { Settings["ZOOM_OUT_ALT"].get_to(ZoomOutKeybind.Alt); }
			if (!Settings["ZOOM_OUT_CTRL"].is_null()) { Settings["ZOOM_OUT_CTRL"].get_to(ZoomOutKeybind.Ctrl); }
			if (!Settings["ZOOM_OUT_SHIFT"].is_null()) { Settings["ZOOM_OUT_SHIFT"].get_to(ZoomOutKeybind.Shift); }

			if (!Settings["AUTO_ADJUST_ZOOM_ENABLED"].is_null()) { Settings["AUTO_ADJUST_ZOOM_ENABLED"].get_to(AutoAdjustZoomEnabled); }

			if (!Settings["HOLD_DOUBLE_CLICK_KEY"].is_null()) { Settings["HOLD_DOUBLE_CLICK_KEY"].get_to(HoldDoubleClickKeybind.Key); }
			if (!Settings["HOLD_DOUBLE_CLICK_ALT"].is_null()) { Settings["HOLD_DOUBLE_CLICK_ALT"].get_to(HoldDoubleClickKeybind.Alt); }
			if (!Settings["HOLD_DOUBLE_CLICK_CTRL"].is_null()) { Settings["HOLD_DOUBLE_CLICK_CTRL"].get_to(HoldDoubleClickKeybind.Ctrl); }
			if (!Settings["HOLD_DOUBLE_CLICK_SHIFT"].is_null()) { Settings["HOLD_DOUBLE_CLICK_SHIFT"].get_to(HoldDoubleClickKeybind.Shift); }

			if (!Settings["SET_DOUBLE_CLICK_KEY"].is_null()) { Settings["SET_DOUBLE_CLICK_KEY"].get_to(SetDoubleClickKeybind.Key); }
			if (!Settings["SET_DOUBLE_CLICK_ALT"].is_null()) { Settings["SET_DOUBLE_CLICK_ALT"].get_to(SetDoubleClickKeybind.Alt); }
			if (!Settings["SET_DOUBLE_CLICK_CTRL"].is_null()) { Settings["SET_DOUBLE_CLICK_CTRL"].get_to(SetDoubleClickKeybind.Ctrl); }
			if (!Settings["SET_DOUBLE_CLICK_SHIFT"].is_null()) { Settings["SET_DOUBLE_CLICK_SHIFT"].get_to(SetDoubleClickKeybind.Shift); }
		}
		else
		{
			MoveForwardKeybind.Key = MapVirtualKeyA('W', MAPVK_VK_TO_VSC);
			DodgeKeybind.Key = MapVirtualKeyA('V', MAPVK_VK_TO_VSC);
			JumpKeybind.Key = MapVirtualKeyA(VK_SPACE, MAPVK_VK_TO_VSC);
			ZoomOutKeybind.Key = MapVirtualKeyA(VK_NEXT, MAPVK_VK_TO_VSC);
		}
    }

    void Save()
    {
		Settings["MOVE_FORWARD_KEY"] = MoveForwardKeybind.Key;
		Settings["MOVE_FORWARD_ALT"] = MoveForwardKeybind.Alt;
		Settings["MOVE_FORWARD_CTRL"] = MoveForwardKeybind.Ctrl;
		Settings["MOVE_FORWARD_SHIFT"] = MoveForwardKeybind.Shift;
		
		Settings["DODGE_KEY"] = DodgeKeybind.Key;
		Settings["DODGE_ALT"] = DodgeKeybind.Alt;
		Settings["DODGE_CTRL"] = DodgeKeybind.Ctrl;
		Settings["DODGE_SHIFT"] = DodgeKeybind.Shift;

		Settings["JUMP_KEY"] = JumpKeybind.Key;
		Settings["JUMP_ALT"] = JumpKeybind.Alt;
		Settings["JUMP_CTRL"] = JumpKeybind.Ctrl;
		Settings["JUMP_SHIFT"] = JumpKeybind.Shift;

		Settings["ABOUT_FACE_KEY"] = AboutFaceKeybind.Key;
		Settings["ABOUT_FACE_ALT"] = AboutFaceKeybind.Alt;
		Settings["ABOUT_FACE_CTRL"] = AboutFaceKeybind.Ctrl;
		Settings["ABOUT_FACE_SHIFT"] = AboutFaceKeybind.Shift;

		Settings["DODGE_JUMP_KEY"] = DodgeJumpKeybind.Key;
		Settings["DODGE_JUMP_ALT"] = DodgeJumpKeybind.Alt;
		Settings["DODGE_JUMP_CTRL"] = DodgeJumpKeybind.Ctrl;
		Settings["DODGE_JUMP_SHIFT"] = DodgeJumpKeybind.Shift;

		Settings["MOVE_ABOUT_FACE_KEY"] = MoveAboutFaceKeybind.Key;
		Settings["MOVE_ABOUT_FACE_ALT"] = MoveAboutFaceKeybind.Alt;
		Settings["MOVE_ABOUT_FACE_CTRL"] = MoveAboutFaceKeybind.Ctrl;
		Settings["MOVE_ABOUT_FACE_SHIFT"] = MoveAboutFaceKeybind.Shift;

		Settings["ZOOM_OUT_KEY"] = ZoomOutKeybind.Key;
		Settings["ZOOM_OUT_ALT"] = ZoomOutKeybind.Alt;
		Settings["ZOOM_OUT_CTRL"] = ZoomOutKeybind.Ctrl;
		Settings["ZOOM_OUT_SHIFT"] = ZoomOutKeybind.Shift;

		Settings["AUTO_ADJUST_ZOOM_ENABLED"] = AutoAdjustZoomEnabled;

		Settings["HOLD_DOUBLE_CLICK_KEY"] = HoldDoubleClickKeybind.Key;
		Settings["HOLD_DOUBLE_CLICK_ALT"] = HoldDoubleClickKeybind.Alt;
		Settings["HOLD_DOUBLE_CLICK_CTRL"] = HoldDoubleClickKeybind.Ctrl;
		Settings["HOLD_DOUBLE_CLICK_SHIFT"] = HoldDoubleClickKeybind.Shift;

		Settings["SET_DOUBLE_CLICK_KEY"] = SetDoubleClickKeybind.Key;
		Settings["SET_DOUBLE_CLICK_ALT"] = SetDoubleClickKeybind.Alt;
		Settings["SET_DOUBLE_CLICK_CTRL"] = SetDoubleClickKeybind.Ctrl;
		Settings["SET_DOUBLE_CLICK_SHIFT"] = SetDoubleClickKeybind.Shift;

		Mutex.lock();
		{
			std::ofstream file(SettingsPath);
			file << Settings.dump(1, '\t') << std::endl;
			file.close();
		}
		Mutex.unlock();
    }

	void KeybindButton(std::string keybindName, Keybind& keybind, std::string keybindTooltip)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::PaddedText(keybindName.c_str(), 6.0F, 4.0F);
		ImGui::TableNextColumn();
		if (ImGui::Button(std::string(Keybinds::KeybindToString(keybind) + "##" + keybindName).c_str()))
		{
			ImGui::OpenPopup(std::string("Set Keybind: " + keybindName).c_str(), ImGuiPopupFlags_AnyPopupLevel);
		}
		ImGui::TooltipGeneric(keybindTooltip.c_str());
		ImGui::TableNextColumn();

		KeybindModal(keybindName, keybind);
	}

	void KeybindModal(std::string keybindName, Keybind& keybind)
	{
		if (ImGui::BeginPopupModal(std::string("Set Keybind: " + keybindName).c_str()))
		{
			bool closeModal = false;
			isSettingKeybind = true;

			if (CurrentKeybind == Keybind{})
			{
				CurrentKeybind = keybind;
			}
			ImGui::Text(Keybinds::KeybindToString(CurrentKeybind).c_str());

			if (ImGui::Button("Unbind"))
			{
				keybind = {};
				closeModal = true;
			}

			ImGui::SameLine(); ImGui::Spacing(); 
			ImGui::SameLine(); ImGui::Spacing(); 
			ImGui::SameLine(); ImGui::Spacing();

			ImGui::SameLine();
			if (ImGui::Button("Accept"))
			{
				keybind = CurrentKeybind;
				closeModal = true;
			}
			
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				closeModal = true;
			}

			if (closeModal)
			{
				CurrentKeybind = Keybind{};
				isSettingKeybind = false;
				Save();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void SettingToggle(std::string settingName, bool& setting, std::string settingTooltip)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::PaddedText("Auto-Adjust Zoom", 6.0F, 4.0F);
		ImGui::TableNextColumn();
		if (ImGui::Checkbox(std::string("Enabled##" + settingName).c_str(), &setting))
		{
			Save();
		}
		ImGui::TooltipGeneric(settingTooltip.c_str());
		ImGui::TableNextColumn();
	}

	void SetDoubleClickModal(std::string modalName)
	{
		if (ImGui::BeginPopupModal(modalName.c_str()))
		{
			bool closeModal = false;

			if (!isSettingDoubleClick)
			{
				GetCursorPos(&doubleClickCursorPos);
			}

			isSettingDoubleClick = true;
			isDoubleClickActive = false;

			ImGui::InputFloat(std::string("seconds##" + modalName).c_str(), &doubleClickInterval, 0.25F, 0.25F, "%.2f");

			if (doubleClickInterval < 0.0F)
			{
				doubleClickInterval = 0.0F;
			}

			if (ImGui::Button(std::string("Start##" + modalName).c_str()))
			{
				isDoubleClickActive = true;
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
				doubleClickInterval = 0.75F;
				closeModal = true;
			}

			if (closeModal)
			{
				isSettingDoubleClick = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	/* Set Keybind */
	Keybind CurrentKeybind {};
	bool isSettingKeybind = false;

	/* Keybinds & Settings */
	Keybind MoveForwardKeybind {};
	Keybind DodgeKeybind {};
	Keybind JumpKeybind {};
	Keybind AboutFaceKeybind {};
	Keybind DodgeJumpKeybind {};
	Keybind MoveAboutFaceKeybind {};
	Keybind ZoomOutKeybind {};
	bool AutoAdjustZoomEnabled = false;
	Keybind HoldDoubleClickKeybind {};
	Keybind SetDoubleClickKeybind {};
	
	/* Toggle Double-Click */
	bool isSettingDoubleClick = false;
	bool isDoubleClickActive = false;
	float doubleClickInterval = 0.75F;
	POINT doubleClickCursorPos = { 0 };
} // namespace Settings
