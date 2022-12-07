#include "pch.h"

#include "ConfigINI.h"
#include "Data.h"
#include "FormCommon.h"
#include "LocaleData.h"
#include "NodeMap.h"
#include "StringHolder.h"

namespace IED
{
	namespace Data
	{
		IData IData::m_Instance;

		namespace ItemData
		{
			NodeDescriptor GetDefaultSlotNode(ObjectSlot a_slot)
			{
				switch (a_slot)
				{
				case ObjectSlot::k1HSword:
					return { BSStringHolder::NINODE_SWORD, NodeDescriptorFlags::kManaged };
				case ObjectSlot::k1HSwordLeft:
					return { BSStringHolder::NINODE_SWORD_LEFT, NodeDescriptorFlags::kManaged };
				case ObjectSlot::k1HAxe:
					return { BSStringHolder::NINODE_AXE, NodeDescriptorFlags::kManaged };
				case ObjectSlot::k1HAxeLeft:
					return { BSStringHolder::NINODE_AXE_LEFT, NodeDescriptorFlags::kManaged };
				case ObjectSlot::k2HSword:
					return { BSStringHolder::NINODE_WEAPON_BACK, NodeDescriptorFlags::kManaged };
				case ObjectSlot::k2HSwordLeft:
					return { BSStringHolder::NINODE_SWORD_LEFT, NodeDescriptorFlags::kManaged };
				case ObjectSlot::k2HAxe:
					return { BSStringHolder::NINODE_WEAPON_BACK, NodeDescriptorFlags::kManaged };
				case ObjectSlot::k2HAxeLeft:
					return { BSStringHolder::NINODE_AXE_LEFT, NodeDescriptorFlags::kManaged };
				case ObjectSlot::kMace:
					return { BSStringHolder::NINODE_MACE, NodeDescriptorFlags::kManaged };
				case ObjectSlot::kMaceLeft:
					return { BSStringHolder::NINODE_MACE_LEFT, NodeDescriptorFlags::kManaged };
				case ObjectSlot::kDagger:
					return { BSStringHolder::NINODE_DAGGER, NodeDescriptorFlags::kManaged };
				case ObjectSlot::kDaggerLeft:
					return { BSStringHolder::NINODE_DAGGER_LEFT, NodeDescriptorFlags::kManaged };
				case ObjectSlot::kStaff:
					return { BSStringHolder::NINODE_STAFF, NodeDescriptorFlags::kManaged };
				case ObjectSlot::kStaffLeft:
					return { BSStringHolder::NINODE_STAFF_LEFT, NodeDescriptorFlags::kManaged };
				case ObjectSlot::kBow:
					return { BSStringHolder::NINODE_BOW, NodeDescriptorFlags::kManaged };
				case ObjectSlot::kCrossBow:
					return { BSStringHolder::NINODE_BOW, NodeDescriptorFlags::kManaged };
				case ObjectSlot::kShield:
					return { BSStringHolder::NINODE_SHIELD_BACK };
				case ObjectSlot::kTorch:
					return { BSStringHolder::NINODE_AXE_LEFT_REVERSE };
				case ObjectSlot::kAmmo:
					return { BSStringHolder::NINODE_QUIVER, NodeDescriptorFlags::kManaged };
				default:
					return {};
				}
			}
		}

		bool IData::PopulateRaceList()
		{
			m_Instance.m_raceList.clear();

			auto dh = DataHandler::GetSingleton();
			if (!dh)
				return false;

			for (auto& race : dh->races)
			{
				if (!race)
					continue;

				if (race->IsDeleted())
					continue;

				m_Instance.m_raceList.try_emplace(
					race->formID,
					LocaleData::ToUTF8(race->GetFullName()),
					LocaleData::ToUTF8(race->editorId.c_str()),
					race->data.raceFlags);
			}

			return true;
		}

		bool IData::PopulatePluginInfo()
		{
			return m_Instance.m_pluginInfo.Populate();
		}

		bool IData::PopulateMiscInfo()
		{
			auto player = *g_thePlayer;
			if (!player)
			{
				return false;
			}

			m_Instance.m_playerRefID = player->formID;

			auto base = player->baseForm;
			if (!base)
			{
				return false;
			}

			if (auto npc = base->As<TESNPC>())
			{
				m_Instance.m_playerBaseID = npc->formID;

				return true;
			}
			else
			{
				return false;
			}
		}

		static void FillDefaultBaseConfig(
			ObjectSlot                     a_slot,
			configSlotHolder_t::data_type& a_data)
		{
			auto targetNode = ItemData::GetDefaultSlotNode(a_slot);

			auto& nodeMapData = NodeMap::GetSingleton().GetData();

			auto it        = nodeMapData.find(targetNode.name);
			auto nodeFlags = it != nodeMapData.end() ?
			                     it->second.flags.value :
			                     NodeDescriptorFlags::kNone;

			for (auto& e : a_data())
			{
				e.targetNode.name  = targetNode.name;
				e.targetNode.flags = nodeFlags;
			}
		}

		std::unique_ptr<configSlotHolder_t::data_type> CreateDefaultSlotConfig(
			ObjectSlot a_slot)
		{
			auto r = std::make_unique<configSlotHolder_t::data_type>();

			FillDefaultBaseConfig(a_slot, *r);

			return r;
		}

		std::unique_ptr<configSlotHolderCopy_t::data_type> CreateDefaultSlotConfig(
			ObjectSlot  a_slot,
			ConfigClass a_class)
		{
			auto r = std::make_unique<configSlotHolderCopy_t::data_type>();

			FillDefaultBaseConfig(a_slot, r->second);

			return r;
		}

	}

}