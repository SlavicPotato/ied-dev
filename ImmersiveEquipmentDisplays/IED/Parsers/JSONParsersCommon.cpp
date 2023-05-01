#include "pch.h"

#include "JSONParsersCommon.h"

namespace IED
{
	namespace Serialization
	{
		using namespace Data;

		const char* SlotKeyParser::SlotToKey(ObjectSlot a_slot) noexcept
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

		ObjectSlot SlotKeyParser::KeyToSlot(const std::string& a_key) noexcept
		{
			switch (stl::fixed_string::key_type::compute_hash(a_key.c_str()))
			{
			case stl::fixed_string::make_hash("1h_sword"):
				return ObjectSlot::k1HSword;
			case stl::fixed_string::make_hash("1h_sword_l"):
				return ObjectSlot::k1HSwordLeft;
			case stl::fixed_string::make_hash("1h_axe"):
				return ObjectSlot::k1HAxe;
			case stl::fixed_string::make_hash("1h_axe_l"):
				return ObjectSlot::k1HAxeLeft;
			case stl::fixed_string::make_hash("2h_sword"):
				return ObjectSlot::k2HSword;
			case stl::fixed_string::make_hash("2h_sword_l"):
				return ObjectSlot::k2HSwordLeft;
			case stl::fixed_string::make_hash("2h_axe"):
				return ObjectSlot::k2HAxe;
			case stl::fixed_string::make_hash("2h_axe_l"):
				return ObjectSlot::k2HAxeLeft;
			case stl::fixed_string::make_hash("mace"):
				return ObjectSlot::kMace;
			case stl::fixed_string::make_hash("mace_l"):
				return ObjectSlot::kMaceLeft;
			case stl::fixed_string::make_hash("dagger"):
				return ObjectSlot::kDagger;
			case stl::fixed_string::make_hash("dagger_l"):
				return ObjectSlot::kDaggerLeft;
			case stl::fixed_string::make_hash("staff"):
				return ObjectSlot::kStaff;
			case stl::fixed_string::make_hash("staff_l"):
				return ObjectSlot::kStaffLeft;
			case stl::fixed_string::make_hash("bow"):
				return ObjectSlot::kBow;
			case stl::fixed_string::make_hash("crossbow"):
				return ObjectSlot::kCrossBow;
			case stl::fixed_string::make_hash("shield"):
				return ObjectSlot::kShield;
			case stl::fixed_string::make_hash("torch"):
				return ObjectSlot::kTorch;
			case stl::fixed_string::make_hash("ammo"):
				return ObjectSlot::kAmmo;
			default:
				return ObjectSlot::kNone;
			}
		}
	}
}