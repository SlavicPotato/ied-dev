#pragma once

#include "plugin.h"

namespace IED
{
	namespace PATHS
	{
		static constexpr auto IMGUI_INI                            = "Data\\SKSE\\Plugins\\" PLUGIN_NAME "_ImGui.ini";
		static constexpr auto SETTINGS                             = "Data\\SKSE\\Plugins\\IED\\Settings.json";
		static constexpr auto NODEMAP                              = "Data\\SKSE\\Plugins\\IED\\Nodes.json";
		static constexpr auto PROFILE_MANAGER_SLOT                 = "Data\\SKSE\\Plugins\\IED\\Profiles\\Slot";
		static constexpr auto PROFILE_MANAGER_CUSTOM               = "Data\\SKSE\\Plugins\\IED\\Profiles\\Custom";
		static constexpr auto PROFILE_MANAGER_EQUIPMENT_TRANSFORMS = "Data\\SKSE\\Plugins\\IED\\Profiles\\NodeOverrides";
		static constexpr auto PROFILE_MANAGER_FORM_FILTER          = "Data\\SKSE\\Plugins\\IED\\Profiles\\FormFilters";
		static constexpr auto PROFILE_MANAGER_VARIABLES            = "Data\\SKSE\\Plugins\\IED\\Profiles\\Variables";
		static constexpr auto PROFILE_MANAGER_OUTFIT               = "Data\\SKSE\\Plugins\\IED\\Profiles\\Outfit";
		static constexpr auto PROFILE_MANAGER_OUTFIT_FORM_LIST     = "Data\\SKSE\\Plugins\\IED\\Profiles\\OutfitFormList";
		static constexpr auto PROFILE_MANAGER_KEYBIND              = "Data\\SKSE\\Plugins\\IED\\Profiles\\Keybind";
		static constexpr auto PROFILE_MANAGER_MODEL                = "Data\\SKSE\\Plugins\\IED\\Assets\\Models";
		static constexpr auto PROFILE_MANAGER_STYLES               = "Data\\SKSE\\Plugins\\IED\\UI\\Styles";
		static constexpr auto EXPORTS                              = "Data\\SKSE\\Plugins\\IED\\Exports";
		static constexpr auto DEFAULT_CONFIG                       = "Data\\SKSE\\Plugins\\IED\\DefaultConfig.json";
		static constexpr auto DEFAULT_CONFIG_USER                  = "Data\\SKSE\\Plugins\\IED\\DefaultConfigUser.json";
		static constexpr auto LOCALIZATION                         = "Data\\SKSE\\Plugins\\IED\\Localization";
		static constexpr auto FONT_META                            = "Data\\SKSE\\Plugins\\IED\\Fonts\\meta.json";
		static constexpr auto FONT_META_USER_PATH                  = "Data\\SKSE\\Plugins\\IED\\Fonts\\user";
		static constexpr auto EXTRA_NODES                          = "Data\\SKSE\\Plugins\\IED\\SkeletonExtensions\\ExtraGearNodes";
		static constexpr auto CONVERT_NODES                        = "Data\\SKSE\\Plugins\\IED\\SkeletonExtensions\\ConvertNodes";
		static constexpr auto NODE_MONITOR                         = "Data\\SKSE\\Plugins\\IED\\NodeMonitor";
		static constexpr auto ADDITIONAL_CME_NODES                 = "Data\\SKSE\\Plugins\\IED\\AdditionalCMENodes";
		static constexpr auto KEY_BINDS                            = "Data\\SKSE\\Plugins\\IED\\KeyBinds.json";
	}
}
