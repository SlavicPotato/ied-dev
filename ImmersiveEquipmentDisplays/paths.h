#pragma once

#include "plugin.h"

namespace IED
{
	namespace PATHS
	{
		inline static constexpr auto IMGUI_INI                            = "Data\\SKSE\\Plugins\\" PLUGIN_NAME "_ImGui.ini";
		inline static constexpr auto SETTINGS                             = "Data\\SKSE\\Plugins\\IED\\Settings.json";
		inline static constexpr auto NODEMAP                              = "Data\\SKSE\\Plugins\\IED\\Nodes.json";
		inline static constexpr auto PROFILE_MANAGER_SLOT                 = "Data\\SKSE\\Plugins\\IED\\Profiles\\Slot";
		inline static constexpr auto PROFILE_MANAGER_CUSTOM               = "Data\\SKSE\\Plugins\\IED\\Profiles\\Custom";
		inline static constexpr auto PROFILE_MANAGER_EQUIPMENT_TRANSFORMS = "Data\\SKSE\\Plugins\\IED\\Profiles\\NodeOverrides";
		inline static constexpr auto PROFILE_MANAGER_FORM_FILTER          = "Data\\SKSE\\Plugins\\IED\\Profiles\\FormFilters";
		inline static constexpr auto PROFILE_MANAGER_VARIABLES            = "Data\\SKSE\\Plugins\\IED\\Profiles\\Variables";
		inline static constexpr auto PROFILE_MANAGER_MODEL                = "Data\\SKSE\\Plugins\\IED\\Assets\\Models";
		inline static constexpr auto EXPORTS                              = "Data\\SKSE\\Plugins\\IED\\Exports";
		inline static constexpr auto DEFAULT_CONFIG                       = "Data\\SKSE\\Plugins\\IED\\DefaultConfig.json";
		inline static constexpr auto DEFAULT_CONFIG_USER                  = "Data\\SKSE\\Plugins\\IED\\DefaultConfigUser.json";
		inline static constexpr auto LOCALIZATION                         = "Data\\SKSE\\Plugins\\IED\\Localization";
		inline static constexpr auto FONT_META                            = "Data\\SKSE\\Plugins\\IED\\Fonts\\meta.json";
		inline static constexpr auto FONT_META_USER_PATH                  = "Data\\SKSE\\Plugins\\IED\\Fonts\\user";
		inline static constexpr auto EXTRA_NODES                          = "Data\\SKSE\\Plugins\\IED\\SkeletonExtensions\\ExtraGearNodes";
		inline static constexpr auto CONVERT_NODES                        = "Data\\SKSE\\Plugins\\IED\\SkeletonExtensions\\ConvertNodes";
		inline static constexpr auto NODE_MONITOR                         = "Data\\SKSE\\Plugins\\IED\\NodeMonitor";
		inline static constexpr auto ADDITIONAL_CME_NODES                 = "Data\\SKSE\\Plugins\\IED\\AdditionalCMENodes";
	}
}
