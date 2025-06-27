#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

#include "keybinds/keybinds.h"

#include "tasks.h"
#include "settings.h"
#include "resource.h"

namespace
{
	static bool isTimeoutElapsed(std::chrono::system_clock::duration timeout)
	{
		return (timeout < std::chrono::system_clock::now().time_since_epoch());
	}

	static bool isValidGameState()
	{
		if (NexusLink && MumbleLink && MumbleIdentity)
		{
			if (NexusLink->IsGameplay && MumbleLink->Context.IsGameFocused && !MumbleLink->Context.IsMapOpen && !MumbleLink->Context.IsTextboxFocused)
			{
				return true;
			}
		}
		return false;
	}
} // namespace

namespace Tasks
{
	/* State */
	bool isManualAdjustZoom = false;

	/* Textures */
	Texture* TexDblClk_0 = nullptr;
	Texture* TexDblClk_1 = nullptr;
	Texture* TexDblClk_2 = nullptr;
	Texture* TexDblClk_3 = nullptr;
	Texture* TexDblClk_4 = nullptr;

	void DodgeJump(const char* aIdentifier, bool aIsRelease)
	{
		if ((strcmp(aIdentifier, "KB_CO_DODGE_JUMP") == 0) && !aIsRelease)
		{
			if (isValidGameState())
			{
				if (Mumble::EMountIndex::None == MumbleLink->Context.MountIndex)
				{
					APIDefs->GameBinds.InvokeAsync(EGameBinds_MoveJump_SwimUp_FlyUp, 0);
					APIDefs->GameBinds.InvokeAsync(EGameBinds_MoveDodge, 0);
				}
				else
				{
					APIDefs->GameBinds.InvokeAsync(EGameBinds_MoveJump_SwimUp_FlyUp, 0);
				}
			}
		}
	}

	void MoveAboutFace(const char* aIdentifier, bool aIsRelease)
	{
		if (strcmp(aIdentifier, "KB_CO_MOVE_ABOUT_FACE") == 0)
		{
			if (!aIsRelease && isValidGameState())
			{
				// hold camera
				Keybinds::LMouseButtonDown(hClient);

				// start moving forward
				APIDefs->GameBinds.PressAsync(EGameBinds_MoveForward);

				// turn character about face
				APIDefs->GameBinds.InvokeAsync(EGameBinds_MoveAboutFace, 0);
			}
			else if (aIsRelease || !isValidGameState())
			{
				// turn character about face
				Keybinds::RMouseButtonDown(hClient);
				Keybinds::RMouseButtonUp(hClient);

				// stop moving forward
				APIDefs->GameBinds.ReleaseAsync(EGameBinds_MoveForward);

				// release camera
				Keybinds::LMouseButtonUp(hClient);
			}
		}
	}

	void ManualAdjustZoom(const char* aIdentifier, bool aIsRelease)
	{
		if (((strcmp(aIdentifier, "KB_CO_MANUAL_ADJUST_ZOOM") == 0) && !aIsRelease))
		{
			if (isValidGameState())
			{
				isManualAdjustZoom = true;
			}
		}
	}

	void AutoAdjustZoom()
	{
		const auto delay = std::chrono::milliseconds(50);
		static auto nextTick = std::chrono::system_clock::now().time_since_epoch();
		static int zoomTicks = 0;

		static float previousFOV = 0;
		static int previousMapID = 0;

		if (Settings::AutoAdjustZoomFOV)
		{
			if (previousFOV < MumbleIdentity->FOV)
			{
				zoomTicks += 3;
			}

			previousFOV = MumbleIdentity->FOV;
		}

		if (Settings::AutoAdjustZoomMap)
		{
			if (previousMapID != MumbleIdentity->MapID)
			{
				zoomTicks += 12;
			}

			previousMapID = MumbleIdentity->MapID;
		}

		if (isManualAdjustZoom)
		{
			zoomTicks += 12;

			isManualAdjustZoom = false;
		}

		if (zoomTicks && isTimeoutElapsed(nextTick))
		{
			if (isValidGameState())
			{
				APIDefs->GameBinds.InvokeAsync(EGameBinds_CameraZoomOut, 50);
				zoomTicks--;

				nextTick = std::chrono::system_clock::now().time_since_epoch() + delay;
			}
		}
	}

	void HoldDoubleClick(const char* aIdentifier, bool aIsRelease)
	{
		if (strcmp(aIdentifier, "KB_CO_HOLD_DOUBLE_CLICK") == 0)
		{
			if (isValidGameState())
			{
				if (!aIsRelease)
				{
					Settings::isDoubleClickActive = true;
					Settings::isDoubleClickPosFixed = false;
					Settings::doubleClickInterval = Settings::DoubleClickDefaultInterval;
					Settings::doubleClickTexIt = 0U;
				}
				else
				{
					Settings::isDoubleClickActive = false;
				}
			}
			else
			{
				// manually deactivate double-click if game state becomes invalid
				Settings::isDoubleClickActive = false;
			}
		}
	}

	void ToggleDoubleClick(const char* aIdentifier, bool aIsRelease)
	{
		if (((strcmp(aIdentifier, "KB_CO_TOGGLE_DOUBLE_CLICK") == 0) && !aIsRelease))
		{
			if (isValidGameState())
			{
				if (!Settings::isDoubleClickActive)
				{
					// activate double-click modal
					Settings::isSettingDoubleClick = true;
					Settings::doubleClickKeybindId = std::string(aIdentifier);
					Settings::doubleClickInterval = Settings::DoubleClickDefaultInterval;
					Settings::doubleClickTexIt = 0U;
				}
				else
				{
					// deactivate double-click
					Settings::isDoubleClickActive = false;
				}
			}
		}
	}

	void PerformDoubleClick()
	{
		static auto timeout = std::chrono::system_clock::now().time_since_epoch();
		static auto texTimeout = std::chrono::system_clock::now().time_since_epoch();
		static bool isDoubleClickTexAnim = false;

		if (Settings::isDoubleClickActive)
		{
			if (isValidGameState())
			{
				if (isTimeoutElapsed(timeout))
				{
					if (!Settings::isDoubleClickPosFixed)
					{
						// double-click at current cursor position
						Keybinds::LMouseButtonDblClk(hClient);
						Keybinds::LMouseButtonUp(hClient);
					}
					else
					{
						// get current cursor position
						POINT CursorPos;
						GetCursorPos(&CursorPos);

						// go to configured cursor position and double-click
						SetCursorPos(Settings::doubleClickCursorPos.x, Settings::doubleClickCursorPos.y);
						Keybinds::LMouseButtonDblClk(hClient);
						Keybinds::LMouseButtonUp(hClient);

						// set previous cursor position
						SetCursorPos(CursorPos.x, CursorPos.y);
					}

					// set timeout interval
					auto doubleClickIntervalMs = std::chrono::milliseconds(static_cast<int>(Settings::doubleClickInterval * 1000));
					timeout = std::chrono::system_clock::now().time_since_epoch() + doubleClickIntervalMs;

					// set animation
					isDoubleClickTexAnim = true;
				}

				// render indicators
				if (nullptr != TexDblClk_0 && nullptr != TexDblClk_0->Resource &&
					nullptr != TexDblClk_1 && nullptr != TexDblClk_1->Resource &&
					nullptr != TexDblClk_2 && nullptr != TexDblClk_2->Resource &&
					nullptr != TexDblClk_3 && nullptr != TexDblClk_3->Resource &&
					nullptr != TexDblClk_4 && nullptr != TexDblClk_4->Resource)
				{
					// get cursor position
					POINT CursorPos = { 0 };
					if (Settings::isDoubleClickPosFixed) {
						CursorPos = Settings::doubleClickCursorPos;
					}
					else {
						GetCursorPos(&CursorPos);
					}
					ScreenToClient(hClient, &CursorPos);

					// get curr texture
					Texture* currTex = nullptr;
					auto nextTexIt = []() { 
						if (isDoubleClickTexAnim && isTimeoutElapsed(texTimeout)) {
							Settings::doubleClickTexIt++;
							texTimeout = std::chrono::system_clock::now().time_since_epoch()
								+ std::chrono::milliseconds(100);
						}
					};
					switch (Settings::doubleClickTexIt)
					{
					case 0:
						currTex = TexDblClk_0;
						nextTexIt();
						break;
					case 1:
						currTex = TexDblClk_1;
						nextTexIt();
						break;
					case 2:
						currTex = TexDblClk_2;
						nextTexIt();
						break;
					case 3:
						currTex = TexDblClk_3;
						nextTexIt();
						break;
					case 4:
						currTex = TexDblClk_4;
						Settings::doubleClickTexIt = 0U;
						isDoubleClickTexAnim = false;
						break;
					default:
						// invalid texture id
						Settings::doubleClickTexIt = 0U;
						break;
					}

					// render double-click indicator
					if (nullptr != currTex)
					{
						if (ImGui::Begin("Double-Click", (bool*)0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoInputs))
						{
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.f, 0.f });
							ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f, 0.f });
							ImGui::SetWindowPos(ImVec2((CursorPos.x - (currTex->Width / 2) - 4), (CursorPos.y - (currTex->Height / 2) - 22)));
							ImGui::Image(currTex->Resource, ImVec2(currTex->Width, currTex->Height));
							ImGui::PopStyleVar(2);
						}
						ImGui::End();

						// render timeout indicator
						if (Settings::isDoubleClickPosFixed)
						{
							if (ImGui::Begin("Timeout", (bool*)0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoInputs))
							{
								ImGui::SetWindowPos(ImVec2((CursorPos.x + (currTex->Width / 2)), (CursorPos.y - (currTex->Height / 2))));
								auto countdown = std::chrono::duration<double>(timeout).count() - std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
								std::stringstream ss;
								ss << std::fixed << std::setprecision(2) << countdown << "s (press \'" << APIDefs->Localization.Translate("KB_CO_TOGGLE_DOUBLE_CLICK") << "\' keybind to stop)";
								ImGui::Text(ss.str().c_str());
							}
							ImGui::End();
						}
					}
				}
				else
				{
					// load textures
					TexDblClk_0 = APIDefs->Textures.GetOrCreateFromResource("RES_TEX_DBLCLK_0", RES_TEX_DBLCLK_0, hSelf);
					TexDblClk_1 = APIDefs->Textures.GetOrCreateFromResource("RES_TEX_DBLCLK_1", RES_TEX_DBLCLK_1, hSelf);
					TexDblClk_2 = APIDefs->Textures.GetOrCreateFromResource("RES_TEX_DBLCLK_2", RES_TEX_DBLCLK_2, hSelf);
					TexDblClk_3 = APIDefs->Textures.GetOrCreateFromResource("RES_TEX_DBLCLK_3", RES_TEX_DBLCLK_3, hSelf);
					TexDblClk_4 = APIDefs->Textures.GetOrCreateFromResource("RES_TEX_DBLCLK_4", RES_TEX_DBLCLK_4, hSelf);
				}
			}
		}
		else if (Settings::isSettingDoubleClick)
		{
			// render double-click modal
			std::string modalName = "Toggle Double-Click";
			ImGui::OpenPopup(modalName.c_str(), ImGuiPopupFlags_AnyPopupLevel);
			Settings::ToggleDoubleClickModal(modalName);
		}
	}

} // namespace Tasks
