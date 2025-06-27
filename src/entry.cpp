#include <Windows.h>
#include <d3d11.h>
#include <filesystem>
#include <future>
#include <fstream>
#include <mutex>
#include <thread>
#include <vector>

#include "nexus/Nexus.h"
#include "mumble/Mumble.h"
#include "keybinds/keybinds.h"
#include "imgui/imgui.h"

#include "shared.h"
#include "settings.h"
#include "tasks.h"
#include "version.h"

void OnMumbleIdentityUpdated(void* aEventArgs);
void AddonLoad(AddonAPI* aApi);
void AddonUnload();
void AddonRender();
void AddonOptions();

static AddonDefinition AddonDef{};

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH: 
			hSelf = hModule; 
			break;
		case DLL_PROCESS_DETACH: 
			break;
		case DLL_THREAD_ATTACH: 
			break;
		case DLL_THREAD_DETACH: 
			break;
	}
	
	return TRUE;
}

extern "C" __declspec(dllexport) AddonDefinition * GetAddonDef()
{
	AddonDef.Signature = 37;
	AddonDef.APIVersion = NEXUS_API_VERSION;
	AddonDef.Name = "Extended Control Options";
	AddonDef.Version.Major = V_MAJOR;
	AddonDef.Version.Minor = V_MINOR;
	AddonDef.Version.Build = V_BUILD;
	AddonDef.Version.Revision = V_REVISION;
	AddonDef.Author = "Jordan";
	AddonDef.Description = "Provides additional controls and macros not available via the in-game Control Options menu.";
	AddonDef.Load = AddonLoad;
	AddonDef.Unload = AddonUnload;
	AddonDef.Flags = EAddonFlags_None;
	AddonDef.Provider = EUpdateProvider_GitHub;
	AddonDef.UpdateLink = "https://github.com/jordanrye/nexus-control-options";

	return &AddonDef;
}

void OnMumbleIdentityUpdated(void* aEventArgs)
{
	MumbleIdentity = (Mumble::Identity*)aEventArgs;
}

void AddonLoad(AddonAPI* aApi)
{
	APIDefs = aApi;
	ImGui::SetCurrentContext((ImGuiContext*)APIDefs->ImguiContext);
	ImGui::SetAllocatorFunctions((void* (*)(size_t, void*))APIDefs->ImguiMalloc, (void(*)(void*, void*))APIDefs->ImguiFree); // on imgui 1.80+

	NexusLink = (NexusLinkData*)APIDefs->DataLink.Get("DL_NEXUS_LINK");
	MumbleLink = (Mumble::Data*)APIDefs->DataLink.Get("DL_MUMBLE_LINK");

	DXGI_SWAP_CHAIN_DESC desc{};
    ((IDXGISwapChain*)APIDefs->SwapChain)->GetDesc(&desc);
    hClient = desc.OutputWindow;

	APIDefs->Events.Subscribe("EV_MUMBLE_IDENTITY_UPDATED", OnMumbleIdentityUpdated);

	APIDefs->Renderer.Register(ERenderType_Render, AddonRender);
	APIDefs->Renderer.Register(ERenderType_OptionsRender, AddonOptions);

	APIDefs->InputBinds.RegisterWithString("KB_CO_DODGE_JUMP", Tasks::DodgeJump, "(null)");
	APIDefs->InputBinds.RegisterWithString("KB_CO_MOVE_ABOUT_FACE", Tasks::MoveAboutFace, "(null)");
	APIDefs->InputBinds.RegisterWithString("KB_CO_HOLD_DOUBLE_CLICK", Tasks::HoldDoubleClick, "(null)");
	APIDefs->InputBinds.RegisterWithString("KB_CO_TOGGLE_DOUBLE_CLICK", Tasks::ToggleDoubleClick, "(null)");
	APIDefs->InputBinds.RegisterWithString("KB_CO_MANUAL_ADJUST_ZOOM", Tasks::ManualAdjustZoom, "(null)");

	APIDefs->Localization.Set("KB_CO_DODGE_JUMP", "en", "Dodge-Jump");
	APIDefs->Localization.Set("KB_CO_MOVE_ABOUT_FACE", "en", "Move About Face");
	APIDefs->Localization.Set("KB_CO_HOLD_DOUBLE_CLICK", "en", "Hold to Double-Click");
	APIDefs->Localization.Set("KB_CO_TOGGLE_DOUBLE_CLICK", "en", "Toggle Double-Click");
	APIDefs->Localization.Set("KB_CO_MANUAL_ADJUST_ZOOM", "en", "Manually Adjust Zoom");

	std::filesystem::create_directory(APIDefs->Paths.GetAddonDirectory("ControlOptions"));
	Settings::Load(APIDefs->Paths.GetAddonDirectory("ControlOptions/settings.json"));
}

void AddonUnload()
{
	APIDefs->InputBinds.Deregister("KB_CO_DODGE_JUMP");
	APIDefs->InputBinds.Deregister("KB_CO_MOVE_ABOUT_FACE");
	APIDefs->InputBinds.Deregister("KB_CO_HOLD_DOUBLE_CLICK");
	APIDefs->InputBinds.Deregister("KB_CO_TOGGLE_DOUBLE_CLICK");
	APIDefs->InputBinds.Deregister("KB_CO_MANUAL_ADJUST_ZOOM");

	APIDefs->Renderer.Deregister(AddonOptions);
	APIDefs->Renderer.Deregister(AddonRender);

	APIDefs->Events.Unsubscribe("EV_MUMBLE_IDENTITY_UPDATED", OnMumbleIdentityUpdated);

	hClient = nullptr;

	MumbleLink = nullptr;
	NexusLink = nullptr;

	Settings::Save();
}

void AddonRender()
{
	std::future<void> taskAutoAdjustZoom = std::async(std::launch::async, Tasks::AutoAdjustZoom);
	std::future<void> taskPerformDoubleClick = std::async(std::launch::async, Tasks::PerformDoubleClick);

	taskAutoAdjustZoom.wait();
	taskPerformDoubleClick.wait();
}

void AddonOptions()
{
	typedef enum
	{
		ESettingsView_Movement,
		ESettingsView_Camera,
		ESettingsView_Utilities
	} ESettingsView;

	std::string helpStr = "Keybind dependencies can be set in the\n'Game Keybinds' tab in Nexus.";

	// Navigation view
	static int selected = 0;
	{
		if (ImGui::BeginChild("##", ImVec2(150, 0), true))
		{
			if (ImGui::SelectablePadded("Movement", (selected == ESettingsView_Movement), 4.0F, 4.0F))
			{
				selected = ESettingsView_Movement;
			}
			if (ImGui::Selectable("Camera", selected == ESettingsView_Camera))
			{
				selected = ESettingsView_Camera;
			}
			if (ImGui::Selectable("Utilities", selected == ESettingsView_Utilities))
			{
				selected = ESettingsView_Utilities;
			}
		}
		ImGui::EndChild();
	}
	ImGui::SameLine();

	// Content view
	{
		ImGui::BeginGroup();
		if (ImGui::BeginChild("ItemView", ImVec2(0, 0)))
		{
			switch (selected)
			{
				case ESettingsView_Movement:
					if (ImGui::BeginTable("##Movement", 1, ImGuiTableFlags_BordersInnerH))
					{
						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						bool isBoundMoveForward = APIDefs->GameBinds.IsBound(EGameBinds_MoveForward);
						bool isBoundAboutFace = APIDefs->GameBinds.IsBound(EGameBinds_MoveAboutFace);
						if (isBoundMoveForward && isBoundAboutFace)
						{
							ImGui::TextWrappedPadded("Move About Face has relevant keybinds set.", 0.0F, 4.0F);
						}
						else if (isBoundMoveForward)
						{
							ImGui::TextWrappedPadded("Move About Face requires 'About Face' keybind.", 0.0F, 4.0F);
						}
						else if (isBoundAboutFace)
						{
							ImGui::TextWrappedPadded("Move About Face requires 'Move Forward' keybind.", 0.0F, 4.0F);
						}
						else
						{
							ImGui::TextWrappedPadded("Move About Face requires 'Move Forward' and 'About Face' keybinds.", 0.0F, 4.0F);
						}
						ImGui::TooltipGeneric("Hold to move your character backwards\nwithout rotating the camera.");
						ImGui::SameLine();
						ImGui::Text("(?)");
						ImGui::TooltipGeneric(helpStr.c_str());

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						bool isBoundDodge = APIDefs->GameBinds.IsBound(EGameBinds_MoveDodge);
						bool isBoundJump = APIDefs->GameBinds.IsBound(EGameBinds_MoveJump_SwimUp_FlyUp);
						if (isBoundDodge && isBoundJump)
						{
							ImGui::TextWrappedPadded("Dodge-Jump has relevant keybinds set.", 0.0F, 4.0F);
						}
						else if (isBoundDodge)
						{
							ImGui::TextWrappedPadded("Dodge-Jump requires 'Jump' keybind", 0.0F, 4.0F);
						}
						else if (isBoundJump)
						{
							ImGui::TextWrappedPadded("Dodge-Jump requires 'Dodge' keybind.", 0.0F, 4.0F);
						}
						else
						{
							ImGui::TextWrappedPadded("Dodge-Jump requires 'Dodge' and 'Jump' keybinds.", 0.0F, 4.0F);
						}
						ImGui::TooltipGeneric("Perform the dodge and jump actions\nsimultaneously.");
						ImGui::SameLine();
						ImGui::Text("(?)");
						ImGui::TooltipGeneric(helpStr.c_str());

						ImGui::EndTable();
					}
					break;
				case ESettingsView_Camera:
					if (ImGui::BeginTable("##Camera", 1, ImGuiTableFlags_BordersInnerH))
					{
						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						bool isBoundZoomOut = APIDefs->GameBinds.IsBound(EGameBinds_CameraZoomOut);
						if (isBoundZoomOut)
						{
							ImGui::TextWrappedPadded("Manually Adjust Zoom has relevant keybinds set.", 0.0F, 4.0F);
						}
						else
						{
							ImGui::TextWrappedPadded("Manually Adjust Zoom requires 'Zoom Out' keybind.", 0.0F, 4.0F);
						}
						ImGui::TooltipGeneric("Manually zoom your camera out to the\nmaximum distance.");
						ImGui::SameLine();
						ImGui::Text("(?)");
						ImGui::TooltipGeneric(helpStr.c_str());

						ImGui::TableNextRow();
						ImGui::TableNextColumn();

						if (isBoundZoomOut)
						{
							ImGui::TextWrappedPadded("Auto-Adjust Zoom has relevant keybinds set.", 0.0F, 4.0F);
						}
						else
						{
							ImGui::TextWrappedPadded("Auto-Adjust Zoom requires 'Zoom Out' keybind.", 0.0F, 4.0F);
						}
						ImGui::TooltipGeneric("Automatically zoom out your camera out\nwhen the field-of-view changes or you\nchange map.");
						ImGui::SameLine();
						ImGui::Text("(?)");
						ImGui::TooltipGeneric(helpStr.c_str());

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						if (ImGui::Checkbox("Auto-Adjust Zoom (FoV Change)", &Settings::AutoAdjustZoomFOV))
						{
							Settings::Save();
						}
						ImGui::TooltipGeneric("Automatically zoom out your camera when\nthe field-of-view changes.");

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						if (ImGui::Checkbox("Auto-Adjust Zoom (Map Change)", &Settings::AutoAdjustZoomMap))
						{
							Settings::Save();
						}
						ImGui::TooltipGeneric("Automatically zoom out your camera when\nyou change map.");

						ImGui::EndTable();
					}
					break;
				case ESettingsView_Utilities:
					if (ImGui::BeginTable("##Utilities", 1, ImGuiTableFlags_BordersInnerH))
					{
						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::TextWrappedPadded("Hold to Double-Click", 0.0F, 4.0F);
						ImGui::TooltipGeneric("Hold keybind to repeatedly double-click\nat your cursor's current position.");

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::TextWrappedPadded("Toggle Double-Click", 0.0F, 4.0F);
						ImGui::TooltipGeneric("Toggle macro to repeatedly double-click\nat a configured position and interval.");

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::TextWrappedPadded("Double-Click Default Interval", 0.0F, 4.0F);
						ImGui::TooltipGeneric("Sets the default interval for 'Hold to\nDouble-Click' and 'Toggle Double-Click.'");
						ImGui::PushItemWidth(150.0F);
						ImGui::InputFloat("seconds##DoubleClickDefaultInterval", &Settings::DoubleClickDefaultInterval, 0.05F, 0.25F, "%.2f");
						ImGui::PopItemWidth();

						ImGui::EndTable();
					}
					break;
				default:
					break;
			}
		}
		ImGui::EndChild();
		ImGui::EndGroup();
	}
}
