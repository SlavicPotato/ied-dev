#include "pch.h"

#include "PapyrusCommon.h"
#include "PapyrusCustomItemCommon.h"

#include "../ConfigOverrideCommon.h"
#include "../Controller/Controller.h"
#include "../Data.h"
#include "../Main.h"

namespace IED
{
	namespace Papyrus
	{
		namespace Custom
		{
			using namespace Data;

			keyPair_t GetKeys(
				const BSFixedString& a_key,
				const BSFixedString& a_name)
			{
				if (!ValidateString(a_key) ||
				    !ValidateString(a_name))
				{
					return {};
				}

				stl::fixed_string key(a_key.c_str());

				if (key == StringHolder::GetSingleton().IED)
				{
					return {};
				}

				if (!IData::GetPluginInfo().GetLookupRef().contains(key))
				{
					return {};
				}

				return {
					std::move(key),
					a_name.c_str()
				};
			}

			void QueueReset(
				Game::FormID a_target,
				ConfigClass a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name)
			{
				switch (a_class)
				{
				case ConfigClass::Actor:
					g_controller->QueueResetCustom(
						a_target,
						a_class,
						a_key,
						a_name);
					break;
				case ConfigClass::NPC:
					g_controller->QueueResetCustomNPC(
						a_target,
						a_class,
						a_key,
						a_name);
					break;
				case ConfigClass::Race:
					g_controller->QueueResetCustomRace(
						a_target,
						a_class,
						a_key,
						a_name);
					break;
				}
			}

			void QueueReset(
				Game::FormID a_target,
				Data::ConfigClass a_class,
				const stl::fixed_string& a_key)
			{
				switch (a_class)
				{
				case ConfigClass::Actor:
					g_controller->QueueResetCustom(
						a_target,
						a_class,
						a_key);
					break;
				case ConfigClass::NPC:
					g_controller->QueueResetCustomNPC(
						a_target,
						a_class,
						a_key);
					break;
				case ConfigClass::Race:
					g_controller->QueueResetCustomRace(
						a_target,
						a_class,
						a_key);
					break;
				}
			}

			void QueueReset(
				Game::FormID a_target,
				Data::ConfigClass a_class)
			{
				switch (a_class)
				{
				case ConfigClass::Actor:
					g_controller->QueueResetCustom(
						a_target,
						a_class);
					break;
				case ConfigClass::NPC:
					g_controller->QueueResetCustomNPC(
						a_target,
						a_class);
					break;
				case ConfigClass::Race:
					g_controller->QueueResetCustomRace(
						a_target,
						a_class);
					break;
				}
			}

			void QueueTransformUpdate(
				Game::FormID a_target,
				Data::ConfigClass a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name)
			{
				switch (a_class)
				{
				case ConfigClass::Actor:
					g_controller->QueueUpdateTransformCustom(
						a_target,
						a_class,
						a_key,
						a_name);
					break;
				case ConfigClass::NPC:
					g_controller->QueueUpdateTransformCustomNPC(
						a_target,
						a_class,
						a_key,
						a_name);
					break;
				case ConfigClass::Race:
					g_controller->QueueUpdateTransformCustomRace(
						a_target,
						a_class,
						a_key,
						a_name);
					break;
				}
			}

			void QueueAttach(
				Game::FormID a_target,
				Data::ConfigClass a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name)
			{
				switch (a_class)
				{
				case ConfigClass::Actor:
					g_controller->QueueUpdateAttachCustom(
						a_target,
						a_class,
						a_key,
						a_name);
					break;
				case ConfigClass::NPC:
					g_controller->QueueUpdateAttachCustomNPC(
						a_target,
						a_class,
						a_key,
						a_name);
					break;
				case ConfigClass::Race:
					g_controller->QueueUpdateAttachCustomRace(
						a_target,
						a_class,
						a_key,
						a_name);
					break;
				}
			}

			void QueueEvaluate(
				Game::FormID a_target,
				Data::ConfigClass a_class)
			{
				switch (a_class)
				{
				case ConfigClass::Actor:
					g_controller->QueueEvaluate(
						a_target,
						ControllerUpdateFlags::kNone);
					break;
				case ConfigClass::NPC:
					g_controller->QueueEvaluateNPC(
						a_target,
						ControllerUpdateFlags::kNone);
					break;
				case ConfigClass::Race:
					g_controller->QueueEvaluateRace(
						a_target,
						ControllerUpdateFlags::kNone);
					break;
				}
			}

			Data::NodeDescriptor GetOrCreateNodeDescriptor(const BSFixedString& a_node)
			{
				if (!ValidateString(a_node))
				{
					return {};
				}

				stl::fixed_string node(a_node.c_str());

				const auto& data = NodeMap::GetSingleton().GetData();

				if (auto it = data.find(node); it != data.end())
				{
					return { std::move(node), it->second.flags };
				}
				else
				{
					return { std::move(node), Data::NodeDescriptorFlags::kPapyrus };
				}

				/*auto& extraData = nodeMap.GetExtraData();

				auto r = extraData.try_emplace(
					node,
					node,
					NodeDescriptorFlags::kPapyrus);

				return std::addressof(r.first->second);*/
			}

			configMapCustom_t& GetConfigMap(ConfigClass a_class) noexcept
			{
				auto& store = g_controller->GetConfigStore();

				switch (a_class)
				{
				case ConfigClass::Actor:
					return store.active.custom.GetActorData();
				case ConfigClass::NPC:
					return store.active.custom.GetNPCData();
				case ConfigClass::Race:
					return store.active.custom.GetRaceData();
				default:
					HALT("FIXME");
				}
			}

			configCustomEntry_t* LookupConfig(
				Game::FormID a_target,
				ConfigClass a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name)
			{
				auto& data = GetConfigMap(a_class);

				auto itd = data.find(a_target);
				if (itd == data.end())
				{
					return nullptr;
				}

				auto itp = itd->second.find(a_key);
				if (itp == itd->second.end())
				{
					return nullptr;
				}

				auto itv = itp->second.data.find(a_name);
				if (itv == itp->second.data.end())
				{
					return nullptr;
				}

				return std::addressof(itv->second);
			}

			bool GetNiPoint3(VMArray<float>& a_in, NiPoint3& a_out)
			{
				if (a_in.Length() != 3)
				{
					return false;
				}

				for (std::uint32_t i = 0; i < 3; i++)
				{
					a_in.Get(std::addressof(a_out[i]), i);
				}

				return true;
			}

		}
	}
}