#include "pch.h"

#include "JSONParsersCommon.h"

namespace IED
{
	namespace Serialization
	{
		using namespace Data;

		SlotKeyParser::SlotKeyParser() :
			m_keyToSlot{
				{ "1h_sword", ObjectSlot::k1HSword },
				{ "1h_sword_l", ObjectSlot::k1HSwordLeft },
				{ "1h_axe", ObjectSlot::k1HAxe },
				{ "1h_axe_l", ObjectSlot::k1HAxeLeft },
				{ "2h_sword", ObjectSlot::k2HSword },
				{ "2h_sword_l", ObjectSlot::k2HSwordLeft },
				{ "2h_axe", ObjectSlot::k2HAxe },
				{ "2h_axe_l", ObjectSlot::k2HAxeLeft },
				{ "mace", ObjectSlot::kMace },
				{ "mace_l", ObjectSlot::kMaceLeft },
				{ "dagger", ObjectSlot::kDagger },
				{ "dagger_l", ObjectSlot::kDaggerLeft },
				{ "staff", ObjectSlot::kStaff },
				{ "staff_l", ObjectSlot::kStaffLeft },
				{ "bow", ObjectSlot::kBow },
				{ "crossbow", ObjectSlot::kCrossBow },
				{ "shield", ObjectSlot::kShield },
				{ "torch", ObjectSlot::kTorch },
				{ "ammo", ObjectSlot::kAmmo }
			}
		{
		}

		const char* SlotKeyParser::SlotToKey(ObjectSlot a_slot) const noexcept
		{
			switch (a_slot)
			{
			case ObjectSlot::k1HSword:
				return "1h_sword";
			case ObjectSlot::k1HSwordLeft:
				return "1h_sword_l";
			case ObjectSlot::k1HAxe:
				return "1h_axe";
			case ObjectSlot::k1HAxeLeft:
				return "1h_axe_l";
			case ObjectSlot::k2HSword:
				return "2h_sword";
			case ObjectSlot::k2HSwordLeft:
				return "2h_sword_l";
			case ObjectSlot::k2HAxe:
				return "2h_axe";
			case ObjectSlot::k2HAxeLeft:
				return "2h_axe_l";
			case ObjectSlot::kMace:
				return "mace";
			case ObjectSlot::kMaceLeft:
				return "mace_l";
			case ObjectSlot::kDagger:
				return "dagger";
			case ObjectSlot::kDaggerLeft:
				return "dagger_l";
			case ObjectSlot::kStaff:
				return "staff";
			case ObjectSlot::kStaffLeft:
				return "staff_l";
			case ObjectSlot::kBow:
				return "bow";
			case ObjectSlot::kCrossBow:
				return "crossbow";
			case ObjectSlot::kShield:
				return "shield";
			case ObjectSlot::kTorch:
				return "torch";
			case ObjectSlot::kAmmo:
				return "ammo";
			default:
				return nullptr;
			}
		}

		ObjectSlot SlotKeyParser::KeyToSlot(const std::string& a_key) const
		{
			auto it = m_keyToSlot.find(a_key);
			if (it != m_keyToSlot.end())
			{
				return it->second;
			}
			else
			{
				return ObjectSlot::kMax;
			}
		}
	}
}