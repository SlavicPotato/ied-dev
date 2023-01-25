#ifndef PCH_H
#define PCH_H

#include <ext/ICommon.h>

#include <skse64/FormTraits.h>

#include <ext/GameHandlesExtra.h>

#include <ext/BGSLensFlare.h>
#include <ext/BSAnimationGraphManager.h>
#include <ext/BSLight.h>
#include <ext/Calendar.h>
#include <ext/Events.h>
#include <ext/GameCommon.h>
#include <ext/Hash.h>
#include <ext/IDebugLog.h>
#include <ext/ILUID.h>
#include <ext/IOS.h>
#include <ext/IPluginInfo.h>
#include <ext/ISerializationBase.h>
#include <ext/ITaskPool.h>
#include <ext/LightCreateParams.h>
#include <ext/Model.h>
#include <ext/Node.h>
#include <ext/PluginInterfaceSDS.h>
#include <ext/SKSEMessagingEvents.h>
#include <ext/SKSESerializationEvents.h>
#include <ext/ShaderReferenceEffect.h>
#include <ext/ShadowSceneNode.h>
#include <ext/StringCache.h>
#include <ext/Vector.h>
#include <ext/WeaponAnimationGraphManagerHolder.h>
#include <ext/compiletime-strings.h>
#include <ext/hkaSkeleton.h>

#include <ext/stl_boost_serialization_containers.h>
#include <ext/stl_comparison.h>
#include <ext/stl_csr.h>
#include <ext/stl_error.h>
#include <ext/stl_flat_map.h>
#include <ext/stl_flat_set.h>
#include <ext/stl_math.h>
#include <ext/stl_mutex.h>
#include <ext/stl_str_helpers.h>
#include <ext/str_conv.h>

#if defined(IED_USE_MIMALLOC_COLLECTOR) || defined(IED_USE_MIMALLOC_SIMCOMPONENT)
#	include <ext/stl_allocator_mi.h>
#endif

#include <skse64/BipedObject.h>
#include <skse64/FormTraits.h>
#include <skse64/GameAudio.h>
#include <skse64/GameCamera.h>
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
#include <skse64/NiLight.h>
#include <skse64/NiNodes.h>
#include <skse64/NiProperties.h>
#include <skse64/NiRenderer.h>
#include <skse64/NiSerialization.h>
#include <skse64/PapyrusNativeFunctions.h>
#include <skse64/PluginAPI.h>

#include <algorithm>
#include <bitset>
#include <execution>
#include <filesystem>
#include <functional>
#include <list>
#include <memory>
#include <numbers>
#include <regex>
#include <shared_mutex>
#include <source_location>
#include <string>

#include <vector>

#include <DirectXCollision.h>
#include <DirectXMath.h>

#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/SimpleMath.h>
#include <DirectXTK/VertexTypes.h>

#include <LinearMath/btMatrix3x3.h>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btTransform.h>
#include <LinearMath/btVector3.h>

#include "Common/BulletExtensions.h"

#include <d3d11.h>
#include <dxgi1_5.h>
#include <wrl/client.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/version.h>

#include <json/json.h>

#include <boost/locale.hpp>
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

#define IMGUI_DEFINE_MATH_OPERATORS

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

#define IED_ENABLE_I3DI

#if (defined(IED_USE_MIMALLOC_COLLECTOR) || defined(IED_USE_MIMALLOC_SIMCOMPONENT) || defined(SKMP_SC_USE_MIMALLOC) || SKMP_CUSTOM_ALLOCATOR == 1)
#	define IED_MIMALLOC_IN_USE 1
#endif

using IPluginInfoA = IPluginInfo<stl::fixed_string>;

#endif  // PCH_H
