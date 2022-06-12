#ifndef PCH_H
#define PCH_H

#include <ext/Events.h>
#include <ext/GameCommon.h>
#include <ext/IDebugLog.h>
#include <ext/IOS.h>
#include <ext/IPluginInfo.h>
#include <ext/ISerializationBase.h>
#include <ext/ITaskPool.h>
#include <ext/Model.h>
#include <ext/Node.h>
#include <ext/PluginInterfaceSDS.h>
#include <ext/SKSEMessagingEvents.h>
#include <ext/SKSESerializationEvents.h>
#include <ext/StrHelpers.h>
#include <ext/Vector.h>
#include <ext/stl_boost_serialization_containers.h>
#include <ext/stl_map_sa.h>
#include <ext/stl_math.h>
#include <ext/stl_set_sa.h>
#include <ext/str_conv.h>

#include <skse64/BipedObject.h>
#include <skse64/FormTraits.h>
#include <skse64/GameAudio.h>
#include <skse64/GameEvents.h>
#include <skse64/GameExtraData.h>
#include <skse64/GameMenus.h>
#include <skse64/GameObjects.h>
#include <skse64/GameRTTI.h>
#include <skse64/GameStreams.h>
#include <skse64/GameThreads.h>
#include <skse64/NiControllers.h>
#include <skse64/NiExtraData.h>
#include <skse64/NiGeometry.h>
#include <skse64/NiNodes.h>
#include <skse64/NiSerialization.h>
#include <skse64/PapyrusNativeFunctions.h>
#include <skse64/PluginAPI.h>

#include <algorithm>
#include <bitset>
#include <chrono>
#include <execution>
#include <filesystem>
#include <functional>
#include <list>
#include <memory>
#include <numbers>
#include <regex>
#include <source_location>
#include <string>
#include <vector>

#include <d3d11.h>
#include <dxgi1_5.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <json/json.h>

#include <boost/serialization/array.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include <boost/locale.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

#include <ImGUI/imgui_impl_dx11.h>
#include <ImGUI/imgui_impl_win32.h>

namespace fs = std::filesystem;

#include "paths.h"
#include "plugin.h"
#include "skse.h"

//#define IED_ENABLE_STATS_G
//#define IED_ENABLE_STATS_T

#endif  // PCH_H
