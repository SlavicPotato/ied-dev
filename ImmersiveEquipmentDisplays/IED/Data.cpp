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

		NodeDescriptor ItemData::GetDefaultSlotNode(ObjectSlot a_slot)
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

		const char* GetSlotName(ObjectSlot a_slot)
		{
			switch (a_slot)
			{
			case ObjectSlot::k1HSword:
				return "One-Handed Sword";
			case ObjectSlot::k1HSwordLeft:
				return "One-Handed Sword Left";
			case ObjectSlot::k1HAxe:
				return "One-Handed Axe";
			case ObjectSlot::k1HAxeLeft:
				return "One-Handed Axe Left";
			case ObjectSlot::k2HSword:
				return "Two-Handed Sword";
			case ObjectSlot::k2HSwordLeft:
				return "Two-Handed Sword Left";
			case ObjectSlot::k2HAxe:
				return "Two-Handed Axe";
			case ObjectSlot::k2HAxeLeft:
				return "Two-Handed Axe Left";
			case ObjectSlot::kMace:
				return "Mace";
			case ObjectSlot::kMaceLeft:
				return "Mace Left";
			case ObjectSlot::kDagger:
				return "Dagger";
			case ObjectSlot::kDaggerLeft:
				return "Dagger Left";
			case ObjectSlot::kStaff:
				return "Staff";
			case ObjectSlot::kStaffLeft:
				return "Staff Left";
			case ObjectSlot::kBow:
				return "Bow";
			case ObjectSlot::kCrossBow:
				return "Crossbow";
			case ObjectSlot::kShield:
				return "Shield";
			case ObjectSlot::kTorch:
				return "Torch";
			case ObjectSlot::kAmmo:
				return "Ammo";
			default:
				return nullptr;
			}
		}

		const char* GetSlotName(ObjectSlotExtra a_slot)
		{
			switch (a_slot)
			{
			case ObjectSlotExtra::k1HSword:
				return "One-Handed Sword";
			case ObjectSlotExtra::k1HSwordLeft:
				return "One-Handed Sword Left";
			case ObjectSlotExtra::k1HAxe:
				return "One-Handed Axe";
			case ObjectSlotExtra::k1HAxeLeft:
				return "One-Handed Axe Left";
			case ObjectSlotExtra::k2HSword:
				return "Two-Handed Sword";
			case ObjectSlotExtra::k2HSwordLeft:
				return "Two-Handed Sword Left";
			case ObjectSlotExtra::k2HAxe:
				return "Two-Handed Axe";
			case ObjectSlotExtra::k2HAxeLeft:
				return "Two-Handed Axe Left";
			case ObjectSlotExtra::kMace:
				return "Mace";
			case ObjectSlotExtra::kMaceLeft:
				return "Mace Left";
			case ObjectSlotExtra::kDagger:
				return "Dagger";
			case ObjectSlotExtra::kDaggerLeft:
				return "Dagger Left";
			case ObjectSlotExtra::kStaff:
				return "Staff";
			case ObjectSlotExtra::kStaffLeft:
				return "Staff Left";
			case ObjectSlotExtra::kBow:
				return "Bow";
			case ObjectSlotExtra::kCrossBow:
				return "Crossbow";
			case ObjectSlotExtra::kShield:
				return "Shield";
			case ObjectSlotExtra::kTorch:
				return "Torch";
			case ObjectSlotExtra::kArmor:
				return "Armor";
			case ObjectSlotExtra::kAmmo:
				return "Ammo";
			case ObjectSlotExtra::kSpell:
				return "Spell";
			case ObjectSlotExtra::kSpellLeft:
				return "Spell Left";
			default:
				return nullptr;
			}
		}

		const char* GetObjectTypeName(ObjectType a_type)
		{
			switch (a_type)
			{
			case ObjectType::k1HSword:
				return "One-Handed Sword";
			case ObjectType::kDagger:
				return "Dagger";
			case ObjectType::k1HAxe:
				return "One-Handed Axe";
			case ObjectType::kMace:
				return "Mace";
			case ObjectType::kStaff:
				return "Staff";
			case ObjectType::k2HSword:
				return "Two-Handed Sword";
			case ObjectType::k2HAxe:
				return "Two-Handed Axe";
			case ObjectType::kBow:
				return "Bow";
			case ObjectType::kCrossBow:
				return "Crossbow";
			case ObjectType::kShield:
				return "Shield";
			case ObjectType::kTorch:
				return "Torch";
			case ObjectType::kAmmo:
				return "Ammo";
			default:
				return nullptr;
			}
		}

		const char* GetObjectTypeName(ObjectTypeExtra a_type)
		{
			switch (a_type)
			{
			case ObjectTypeExtra::k1HSword:
				return "One-Handed Sword";
			case ObjectTypeExtra::kDagger:
				return "Dagger";
			case ObjectTypeExtra::k1HAxe:
				return "One-Handed Axe";
			case ObjectTypeExtra::kMace:
				return "Mace";
			case ObjectTypeExtra::kStaff:
				return "Staff";
			case ObjectTypeExtra::k2HSword:
				return "Two-Handed Sword";
			case ObjectTypeExtra::k2HAxe:
				return "Two-Handed Axe";
			case ObjectTypeExtra::kBow:
				return "Bow";
			case ObjectTypeExtra::kCrossBow:
				return "Crossbow";
			case ObjectTypeExtra::kShield:
				return "Shield";
			case ObjectTypeExtra::kTorch:
				return "Torch";
			case ObjectTypeExtra::kArmor:
				return "Armor";
			case ObjectTypeExtra::kAmmo:
				return "Ammo";
			case ObjectTypeExtra::kSpell:
				return "Spell";
			default:
				return nullptr;
			}
		}

		const char* GetConfigClassName(ConfigClass a_class)
		{
			switch (a_class)
			{
			case ConfigClass::Actor:
				return "Actor";
			case ConfigClass::NPC:
				return "NPC";
			case ConfigClass::Race:
				return "Race";
			case ConfigClass::Global:
				return "Global";
			default:
				return nullptr;
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

		std::unique_ptr<configSlotHolder_t::data_type>
			CreateDefaultSlotConfig(ObjectSlot a_slot)
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