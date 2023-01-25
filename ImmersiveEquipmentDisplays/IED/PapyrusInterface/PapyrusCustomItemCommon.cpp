#include "pch.h"

#include "PapyrusCommon.h"
#include "PapyrusCustomItemCommon.h"

#include "IED/ConfigCommon.h"
#include "IED/Controller/Controller.h"
#include "IED/Data.h"
#include "IED/Main.h"

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

				if (!ValidateKey(key))
				{
					return {};
				}

				return {
					std::move(key),
					a_name.c_str()
				};
			}

			void QueueReset(
				Game::FormID             a_target,
				ConfigClass              a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name)
			{
				switch (a_class)
				{
				case ConfigClass::Actor:
					Initializer::GetController()->QueueResetCustom(
						a_target,
						a_class,
						a_key,
						a_name);
					break;
				case ConfigClass::NPC:
					Initializer::GetController()->QueueResetCustomNPC(
						a_target,
						a_class,
						a_key,
						a_name);
					break;
				case ConfigClass::Race:
					Initializer::GetController()->QueueResetCustomRace(
						a_target,
						a_class,
						a_key,
						a_name);
					break;
				}
			}

			void QueueReset(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key)
			{
				switch (a_class)
				{
				case ConfigClass::Actor:
					Initializer::GetController()->QueueResetCustom(
						a_target,
						a_class,
						a_key);
					break;
				case ConfigClass::NPC:
					Initializer::GetController()->QueueResetCustomNPC(
						a_target,
						a_class,
						a_key);
					break;
				case ConfigClass::Race:
					Initializer::GetController()->QueueResetCustomRace(
						a_target,
						a_class,
						a_key);
					break;
				}
			}

			void QueueReset(
				Game::FormID      a_target,
				Data::ConfigClass a_class)
			{
				switch (a_class)
				{
				case ConfigClass::Actor:
					Initializer::GetController()->QueueResetCustom(
						a_target,
						a_class);
					break;
				case ConfigClass::NPC:
					Initializer::GetController()->QueueResetCustomNPC(
						a_target,
						a_class);
					break;
				case ConfigClass::Race:
					Initializer::GetController()->QueueResetCustomRace(
						a_target,
						a_class);
					break;
				}
			}

			void QueueTransformUpdate(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name)
			{
				switch (a_class)
				{
				case ConfigClass::Actor:
					Initializer::GetController()->QueueUpdateTransformCustom(
						a_target,
						a_class,
						a_key,
						a_name);
					break;
				case ConfigClass::NPC:
					Initializer::GetController()->QueueUpdateTransformCustomNPC(
						a_target,
						a_class,
						a_key,
						a_name);
					break;
				case ConfigClass::Race:
					Initializer::GetController()->QueueUpdateTransformCustomRace(
						a_target,
						a_class,
						a_key,
						a_name);
					break;
				}
			}

			void QueueEvaluate(
				Game::FormID      a_target,
				Data::ConfigClass a_class)
			{
				switch (a_class)
				{
				case ConfigClass::Actor:
					Initializer::GetController()->QueueEvaluate(
						a_target,
						ControllerUpdateFlags::kNone);
					break;
				case ConfigClass::NPC:
					Initializer::GetController()->QueueEvaluateNPC(
						a_target,
						ControllerUpdateFlags::kNone);
					break;
				case ConfigClass::Race:
					Initializer::GetController()->QueueEvaluateRace(
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
				auto& store = Initializer::GetController()->GetActiveConfig();

				switch (a_class)
				{
				case ConfigClass::Actor:
					return store.custom.GetActorData();
				case ConfigClass::NPC:
					return store.custom.GetNPCData();
				case ConfigClass::Race:
					return store.custom.GetRaceData();
				default:
					HALT("FIXME");
				}
			}

			configCustomEntry_t* LookupConfig(
				Game::FormID             a_target,
				ConfigClass              a_class,
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

			std::optional<NiPoint3> GetNiPoint3(VMArray<float>& a_in)
			{
				if (a_in.Length() != 3)
				{
					return {};
				}

				NiPoint3 result(NiPoint3::noinit_arg_t{});

				for (std::uint32_t i = 0; i < 3; i++)
				{
					a_in.Get(std::addressof(result[i]), i);
				}

				return result;
			}

		}
	}
}