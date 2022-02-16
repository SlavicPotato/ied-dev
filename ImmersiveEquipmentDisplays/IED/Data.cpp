#include "pch.h"

#include "Config.h"
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

		ObjectSlot ItemData::GetObjectSlot(TESObjectWEAP* a_form) noexcept
		{
			switch (a_form->gameData.type)
			{
			case TESObjectWEAP::GameData::kType_OneHandSword:
				return ObjectSlot::k1HSword;
			case TESObjectWEAP::GameData::kType_OneHandDagger:
				return ObjectSlot::kDagger;
			case TESObjectWEAP::GameData::kType_OneHandAxe:
				return ObjectSlot::k1HAxe;
			case TESObjectWEAP::GameData::kType_OneHandMace:
				return ObjectSlot::kMace;
			case TESObjectWEAP::GameData::kType_TwoHandSword:
				return ObjectSlot::k2HSword;
			case TESObjectWEAP::GameData::kType_TwoHandAxe:
				return ObjectSlot::k2HAxe;
			case TESObjectWEAP::GameData::kType_Bow:
				return ObjectSlot::kBow;
			case TESObjectWEAP::GameData::kType_Staff:
				return ObjectSlot::kStaff;
			case TESObjectWEAP::GameData::kType_CrossBow:
				return ObjectSlot::kCrossBow;
			default:
				return ObjectSlot::kMax;
			}
		}

		ObjectSlot ItemData::GetObjectSlotLeft(TESObjectWEAP* a_form) noexcept
		{
			switch (a_form->gameData.type)
			{
			case TESObjectWEAP::GameData::kType_OneHandSword:
				return ObjectSlot::k1HSwordLeft;
			case TESObjectWEAP::GameData::kType_OneHandAxe:
				return ObjectSlot::k1HAxeLeft;
			case TESObjectWEAP::GameData::kType_TwoHandSword:
				return ObjectSlot::k2HSwordLeft;
			case TESObjectWEAP::GameData::kType_TwoHandAxe:
				return ObjectSlot::k2HAxeLeft;
			case TESObjectWEAP::GameData::kType_OneHandDagger:
				return ObjectSlot::kDaggerLeft;
			case TESObjectWEAP::GameData::kType_OneHandMace:
				return ObjectSlot::kMaceLeft;
			case TESObjectWEAP::GameData::kType_Staff:
				return ObjectSlot::kStaffLeft;
			default:
				return ObjectSlot::kMax;
			}
		}

		ObjectSlot ItemData::GetObjectSlotLeft(TESObjectLIGH* a_form) noexcept
		{
			return a_form->CanCarry() ? ObjectSlot::kTorch : ObjectSlot::kMax;
		}

		ObjectSlot ItemData::GetObjectSlot(TESObjectARMO* a_form) noexcept
		{
			return a_form->IsShield() ? ObjectSlot::kShield : ObjectSlot::kMax;
		}

		ObjectSlot ItemData::GetObjectSlot(TESAmmo* a_form) noexcept
		{
			return ObjectSlot::kAmmo;
		}

		ObjectSlot ItemData::GetObjectSlot(TESForm* a_form) noexcept
		{
			switch (a_form->formType)
			{
			case TESObjectWEAP::kTypeID:
				return ItemData::GetObjectSlot(static_cast<TESObjectWEAP*>(a_form));
			case TESAmmo::kTypeID:
				return ItemData::GetObjectSlot(static_cast<TESAmmo*>(a_form));
			}

			return ObjectSlot::kMax;
		}

		ObjectSlot ItemData::GetObjectSlotLeft(TESForm* a_form) noexcept
		{
			switch (a_form->formType)
			{
			case TESObjectWEAP::kTypeID:
				return ItemData::GetObjectSlotLeft(static_cast<TESObjectWEAP*>(a_form));
			case TESObjectARMO::kTypeID:
				return ItemData::GetObjectSlot(static_cast<TESObjectARMO*>(a_form));
			case TESObjectLIGH::kTypeID:
				return ItemData::GetObjectSlotLeft(static_cast<TESObjectLIGH*>(a_form));
			}

			return ObjectSlot::kMax;
		}

		ObjectType ItemData::GetItemType(TESObjectARMO* a_form) noexcept
		{
			return a_form->IsShield() ? ObjectType::kShield : ObjectType::kMax;
		}

		ObjectType ItemData::GetItemType(TESObjectLIGH* a_form) noexcept
		{
			return a_form->CanCarry() ? ObjectType::kTorch : ObjectType::kMax;
		}

		ObjectType ItemData::GetItemType(TESAmmo* a_form) noexcept
		{
			return ObjectType::kAmmo;
		}

		ObjectType ItemData::GetItemType(TESObjectWEAP* a_form) noexcept
		{
			switch (a_form->gameData.type)
			{
			case TESObjectWEAP::GameData::kType_OneHandSword:
				return ObjectType::k1HSword;
			case TESObjectWEAP::GameData::kType_OneHandDagger:
				return ObjectType::kDagger;
			case TESObjectWEAP::GameData::kType_OneHandAxe:
				return ObjectType::k1HAxe;
			case TESObjectWEAP::GameData::kType_OneHandMace:
				return ObjectType::kMace;
			case TESObjectWEAP::GameData::kType_TwoHandSword:
				return ObjectType::k2HSword;
			case TESObjectWEAP::GameData::kType_TwoHandAxe:
				return ObjectType::k2HAxe;
			case TESObjectWEAP::GameData::kType_Bow:
				return ObjectType::kBow;
			case TESObjectWEAP::GameData::kType_Staff:
				return ObjectType::kStaff;
			case TESObjectWEAP::GameData::kType_CrossBow:
				return ObjectType::kCrossBow;
			default:
				return ObjectType::kMax;
			}
		}

		ObjectType ItemData::GetItemType(TESForm* a_form) noexcept
		{
			switch (a_form->formType)
			{
			case TESObjectWEAP::kTypeID:
				return ItemData::GetItemType(static_cast<TESObjectWEAP*>(a_form));
			case TESObjectARMO::kTypeID:
				return ItemData::GetItemType(static_cast<TESObjectARMO*>(a_form));
			case TESObjectLIGH::kTypeID:
				return ItemData::GetItemType(static_cast<TESObjectLIGH*>(a_form));
			case TESAmmo::kTypeID:
				return ItemData::GetItemType(static_cast<TESAmmo*>(a_form));
			}

			return ObjectType::kMax;
		}

		ObjectTypeExtra ItemData::GetItemTypeExtra(TESObjectARMO* a_form) noexcept
		{
			return a_form->IsShield() ? ObjectTypeExtra::kShield : ObjectTypeExtra::kArmor;
		}

		ObjectTypeExtra ItemData::GetItemTypeExtra(TESObjectLIGH* a_form) noexcept
		{
			return a_form->CanCarry() ? ObjectTypeExtra::kTorch : ObjectTypeExtra::kNone;
		}

		ObjectTypeExtra ItemData::GetItemTypeExtra(TESObjectWEAP* a_form) noexcept
		{
			switch (a_form->gameData.type)
			{
			case TESObjectWEAP::GameData::kType_OneHandSword:
				return ObjectTypeExtra::k1HSword;
			case TESObjectWEAP::GameData::kType_OneHandDagger:
				return ObjectTypeExtra::kDagger;
			case TESObjectWEAP::GameData::kType_OneHandAxe:
				return ObjectTypeExtra::k1HAxe;
			case TESObjectWEAP::GameData::kType_OneHandMace:
				return ObjectTypeExtra::kMace;
			case TESObjectWEAP::GameData::kType_TwoHandSword:
				return ObjectTypeExtra::k2HSword;
			case TESObjectWEAP::GameData::kType_TwoHandAxe:
				return ObjectTypeExtra::k2HAxe;
			case TESObjectWEAP::GameData::kType_Bow:
				return ObjectTypeExtra::kBow;
			case TESObjectWEAP::GameData::kType_Staff:
				return ObjectTypeExtra::kStaff;
			case TESObjectWEAP::GameData::kType_CrossBow:
				return ObjectTypeExtra::kCrossBow;
			default:
				return ObjectTypeExtra::kNone;
			}
		}

		ObjectTypeExtra ItemData::GetItemTypeExtra(TESForm* a_form) noexcept
		{
			switch (a_form->formType)
			{
			case TESObjectWEAP::kTypeID:
				return ItemData::GetItemTypeExtra(static_cast<TESObjectWEAP*>(a_form));
			case TESObjectARMO::kTypeID:
				return ItemData::GetItemTypeExtra(static_cast<TESObjectARMO*>(a_form));
			case TESObjectLIGH::kTypeID:
				return ItemData::GetItemTypeExtra(static_cast<TESObjectLIGH*>(a_form));
			case TESAmmo::kTypeID:
				return ObjectTypeExtra::kAmmo;
			case SpellItem::kTypeID:
				return ObjectTypeExtra::kSpell;
			default:
				return ObjectTypeExtra::kNone;
			}
		}

		ObjectSlotExtra ItemData::GetItemSlotExtra(TESObjectARMO* a_form) noexcept
		{
			return !a_form->IsShield() ? ObjectSlotExtra::kArmor : ObjectSlotExtra::kNone;
		}

		ObjectSlotExtra ItemData::GetItemSlotExtra(TESObjectWEAP* a_form) noexcept
		{
			switch (a_form->gameData.type)
			{
			case TESObjectWEAP::GameData::kType_OneHandSword:
				return ObjectSlotExtra::k1HSword;
			case TESObjectWEAP::GameData::kType_OneHandDagger:
				return ObjectSlotExtra::kDagger;
			case TESObjectWEAP::GameData::kType_OneHandAxe:
				return ObjectSlotExtra::k1HAxe;
			case TESObjectWEAP::GameData::kType_OneHandMace:
				return ObjectSlotExtra::kMace;
			case TESObjectWEAP::GameData::kType_TwoHandSword:
				return ObjectSlotExtra::k2HSword;
			case TESObjectWEAP::GameData::kType_TwoHandAxe:
				return ObjectSlotExtra::k2HAxe;
			case TESObjectWEAP::GameData::kType_Bow:
				return ObjectSlotExtra::kBow;
			case TESObjectWEAP::GameData::kType_Staff:
				return ObjectSlotExtra::kStaff;
			case TESObjectWEAP::GameData::kType_CrossBow:
				return ObjectSlotExtra::kCrossBow;
			default:
				return ObjectSlotExtra::kNone;
			}
		}

		ObjectSlotExtra ItemData::GetItemSlotExtra(TESObjectLIGH* a_form) noexcept
		{
			return a_form->CanCarry() ? ObjectSlotExtra::kTorch : ObjectSlotExtra::kNone;
		}

		ObjectSlotExtra ItemData::GetItemSlotExtra(TESForm* a_form) noexcept
		{
			switch (a_form->formType)
			{
			case TESObjectWEAP::kTypeID:
				return GetItemSlotExtra(static_cast<TESObjectWEAP*>(a_form));
			case TESObjectARMO::kTypeID:
				return GetItemSlotExtra(static_cast<TESObjectARMO*>(a_form));
			case TESObjectLIGH::kTypeID:
				return GetItemSlotExtra(static_cast<TESObjectLIGH*>(a_form));
			case TESAmmo::kTypeID:
				return ObjectSlotExtra::kAmmo;
			case SpellItem::kTypeID:
				return ObjectSlotExtra::kSpell;
			default:
				return ObjectSlotExtra::kNone;
			}
		}

		ObjectSlotExtra ItemData::GetItemSlotLeftExtra(TESObjectWEAP* a_form) noexcept
		{
			switch (a_form->gameData.type)
			{
			case TESObjectWEAP::GameData::kType_OneHandSword:
				return ObjectSlotExtra::k1HSwordLeft;
			case TESObjectWEAP::GameData::kType_OneHandDagger:
				return ObjectSlotExtra::kDaggerLeft;
			case TESObjectWEAP::GameData::kType_OneHandAxe:
				return ObjectSlotExtra::k1HAxeLeft;
			case TESObjectWEAP::GameData::kType_OneHandMace:
				return ObjectSlotExtra::kMaceLeft;
			case TESObjectWEAP::GameData::kType_TwoHandSword:
				return ObjectSlotExtra::k2HSwordLeft;
			case TESObjectWEAP::GameData::kType_TwoHandAxe:
				return ObjectSlotExtra::k2HAxeLeft;
			case TESObjectWEAP::GameData::kType_Staff:
				return ObjectSlotExtra::kStaffLeft;
			default:
				return ObjectSlotExtra::kNone;
			}
		}

		ObjectSlotExtra ItemData::GetItemSlotLeftExtra(TESForm* a_form) noexcept
		{
			switch (a_form->formType)
			{
			case TESObjectWEAP::kTypeID:
				return ItemData::GetItemSlotLeftExtra(static_cast<TESObjectWEAP*>(a_form));
			case TESObjectARMO::kTypeID:
				return static_cast<TESObjectARMO*>(a_form)->IsShield() ?
                           ObjectSlotExtra::kShield :
                           ObjectSlotExtra::kNone;
			case SpellItem::kTypeID:
				return ObjectSlotExtra::kSpellLeft;
			default:
				return ObjectSlotExtra::kNone;
			}
		}

		ObjectSlot ItemData::GetSlotFromType(ObjectType a_type) noexcept
		{
			switch (a_type)
			{
			case ObjectType::k1HSword:
				return ObjectSlot::k1HSword;
			case ObjectType::kDagger:
				return ObjectSlot::kDagger;
			case ObjectType::k1HAxe:
				return ObjectSlot::k1HAxe;
			case ObjectType::kMace:
				return ObjectSlot::kMace;
			case ObjectType::kStaff:
				return ObjectSlot::kStaff;
			case ObjectType::k2HSword:
				return ObjectSlot::k2HSword;
			case ObjectType::k2HAxe:
				return ObjectSlot::k2HAxe;
			case ObjectType::kBow:
				return ObjectSlot::kBow;
			case ObjectType::kCrossBow:
				return ObjectSlot::kCrossBow;
			case ObjectType::kShield:
				return ObjectSlot::kShield;
			case ObjectType::kTorch:
				return ObjectSlot::kTorch;
			case ObjectType::kAmmo:
				return ObjectSlot::kAmmo;
			default:
				HALT("FIXME");
			}
		}

		ObjectSlotExtra ItemData::GetSlotFromTypeExtra(ObjectTypeExtra a_type) noexcept
		{
			switch (a_type)
			{
			case ObjectTypeExtra::k1HSword:
				return ObjectSlotExtra::k1HSword;
			case ObjectTypeExtra::kDagger:
				return ObjectSlotExtra::kDagger;
			case ObjectTypeExtra::k1HAxe:
				return ObjectSlotExtra::k1HAxe;
			case ObjectTypeExtra::kMace:
				return ObjectSlotExtra::kMace;
			case ObjectTypeExtra::kStaff:
				return ObjectSlotExtra::kStaff;
			case ObjectTypeExtra::k2HSword:
				return ObjectSlotExtra::k2HSword;
			case ObjectTypeExtra::k2HAxe:
				return ObjectSlotExtra::k2HAxe;
			case ObjectTypeExtra::kBow:
				return ObjectSlotExtra::kBow;
			case ObjectTypeExtra::kCrossBow:
				return ObjectSlotExtra::kCrossBow;
			case ObjectTypeExtra::kShield:
				return ObjectSlotExtra::kShield;
			case ObjectTypeExtra::kTorch:
				return ObjectSlotExtra::kTorch;
			case ObjectTypeExtra::kArmor:
				return ObjectSlotExtra::kArmor;
			case ObjectTypeExtra::kAmmo:
				return ObjectSlotExtra::kAmmo;
			case ObjectTypeExtra::kSpell:
				return ObjectSlotExtra::kSpell;
			default:
				HALT("FIXME");
			}
		}

		ObjectTypeExtra ItemData::GetTypeFromSlotExtra(ObjectSlotExtra a_slot) noexcept
		{
			switch (a_slot)
			{
			case ObjectSlotExtra::k1HSword:
			case ObjectSlotExtra::k1HSwordLeft:
				return ObjectTypeExtra::k1HSword;
			case ObjectSlotExtra::k1HAxe:
			case ObjectSlotExtra::k1HAxeLeft:
				return ObjectTypeExtra::k1HAxe;
			case ObjectSlotExtra::k2HSword:
			case ObjectSlotExtra::k2HSwordLeft:
				return ObjectTypeExtra::k2HSword;
			case ObjectSlotExtra::k2HAxe:
			case ObjectSlotExtra::k2HAxeLeft:
				return ObjectTypeExtra::k2HAxe;
			case ObjectSlotExtra::kMace:
			case ObjectSlotExtra::kMaceLeft:
				return ObjectTypeExtra::kMace;
			case ObjectSlotExtra::kDagger:
			case ObjectSlotExtra::kDaggerLeft:
				return ObjectTypeExtra::kDagger;
			case ObjectSlotExtra::kStaff:
			case ObjectSlotExtra::kStaffLeft:
				return ObjectTypeExtra::kStaff;
			case ObjectSlotExtra::kBow:
				return ObjectTypeExtra::kBow;
			case ObjectSlotExtra::kCrossBow:
				return ObjectTypeExtra::kCrossBow;
			case ObjectSlotExtra::kShield:
				return ObjectTypeExtra::kShield;
			case ObjectSlotExtra::kTorch:
				return ObjectTypeExtra::kTorch;
			case ObjectSlotExtra::kArmor:
				return ObjectTypeExtra::kArmor;
			case ObjectSlotExtra::kAmmo:
				return ObjectTypeExtra::kAmmo;
			case ObjectSlotExtra::kSpell:
			case ObjectSlotExtra::kSpellLeft:
				return ObjectTypeExtra::kSpell;
			default:
				return ObjectTypeExtra::kNone;
			}
		}

		ObjectSlot ItemData::GetLeftSlot(ObjectSlot a_slot) noexcept
		{
			switch (a_slot)
			{
			case ObjectSlot::k1HSword:
				return ObjectSlot::k1HSwordLeft;
			case ObjectSlot::k2HSword:
				return ObjectSlot::k2HSwordLeft;
			case ObjectSlot::k1HAxe:
				return ObjectSlot::k1HAxeLeft;
			case ObjectSlot::k2HAxe:
				return ObjectSlot::k2HAxeLeft;
			case ObjectSlot::kDagger:
				return ObjectSlot::kDaggerLeft;
			case ObjectSlot::kMace:
				return ObjectSlot::kMaceLeft;
			case ObjectSlot::kStaff:
				return ObjectSlot::kStaffLeft;
			default:
				return ObjectSlot::kMax;
			}
		}

		ObjectSlotExtra ItemData::GetLeftSlotExtra(ObjectSlotExtra a_slot) noexcept
		{
			switch (a_slot)
			{
			case ObjectSlotExtra::k1HSword:
				return ObjectSlotExtra::k1HSwordLeft;
			case ObjectSlotExtra::k2HSword:
				return ObjectSlotExtra::k2HSwordLeft;
			case ObjectSlotExtra::k1HAxe:
				return ObjectSlotExtra::k1HAxeLeft;
			case ObjectSlotExtra::k2HAxe:
				return ObjectSlotExtra::k2HAxeLeft;
			case ObjectSlotExtra::kDagger:
				return ObjectSlotExtra::kDaggerLeft;
			case ObjectSlotExtra::kMace:
				return ObjectSlotExtra::kMaceLeft;
			case ObjectSlotExtra::kStaff:
				return ObjectSlotExtra::kStaffLeft;
			case ObjectSlotExtra::kSpell:
				return ObjectSlotExtra::kSpellLeft;
			default:
				return ObjectSlotExtra::kNone;
			}
		}

		ObjectSlot ItemData::ExtraSlotToSlot(ObjectSlotExtra a_slot) noexcept
		{
			switch (a_slot)
			{
			case ObjectSlotExtra::k1HSword:
				return ObjectSlot::k1HSword;
			case ObjectSlotExtra::k1HSwordLeft:
				return ObjectSlot::k1HSwordLeft;
			case ObjectSlotExtra::k1HAxe:
				return ObjectSlot::k1HAxe;
			case ObjectSlotExtra::k1HAxeLeft:
				return ObjectSlot::k1HAxeLeft;
			case ObjectSlotExtra::k2HSword:
				return ObjectSlot::k2HSword;
			case ObjectSlotExtra::k2HSwordLeft:
				return ObjectSlot::k2HSwordLeft;
			case ObjectSlotExtra::k2HAxe:
				return ObjectSlot::k2HAxe;
			case ObjectSlotExtra::k2HAxeLeft:
				return ObjectSlot::k2HAxeLeft;
			case ObjectSlotExtra::kMace:
				return ObjectSlot::kMace;
			case ObjectSlotExtra::kMaceLeft:
				return ObjectSlot::kMaceLeft;
			case ObjectSlotExtra::kDagger:
				return ObjectSlot::kDagger;
			case ObjectSlotExtra::kDaggerLeft:
				return ObjectSlot::kDaggerLeft;
			case ObjectSlotExtra::kStaff:
				return ObjectSlot::kStaff;
			case ObjectSlotExtra::kStaffLeft:
				return ObjectSlot::kStaffLeft;
			case ObjectSlotExtra::kBow:
				return ObjectSlot::kBow;
			case ObjectSlotExtra::kCrossBow:
				return ObjectSlot::kCrossBow;
			case ObjectSlotExtra::kShield:
				return ObjectSlot::kShield;
			case ObjectSlotExtra::kTorch:
				return ObjectSlot::kTorch;
			case ObjectSlotExtra::kAmmo:
				return ObjectSlot::kAmmo;
			default:
				return ObjectSlot::kMax;
			}
		}

		ObjectSlotExtra ItemData::SlotToExtraSlot(ObjectSlot a_slot) noexcept
		{
			switch (a_slot)
			{
			case ObjectSlot::k1HSword:
				return ObjectSlotExtra::k1HSword;
			case ObjectSlot::k1HSwordLeft:
				return ObjectSlotExtra::k1HSwordLeft;
			case ObjectSlot::k1HAxe:
				return ObjectSlotExtra::k1HAxe;
			case ObjectSlot::k1HAxeLeft:
				return ObjectSlotExtra::k1HAxeLeft;
			case ObjectSlot::k2HSword:
				return ObjectSlotExtra::k2HSword;
			case ObjectSlot::k2HSwordLeft:
				return ObjectSlotExtra::k2HSwordLeft;
			case ObjectSlot::k2HAxe:
				return ObjectSlotExtra::k2HAxe;
			case ObjectSlot::k2HAxeLeft:
				return ObjectSlotExtra::k2HAxeLeft;
			case ObjectSlot::kMace:
				return ObjectSlotExtra::kMace;
			case ObjectSlot::kMaceLeft:
				return ObjectSlotExtra::kMaceLeft;
			case ObjectSlot::kDagger:
				return ObjectSlotExtra::kDagger;
			case ObjectSlot::kDaggerLeft:
				return ObjectSlotExtra::kDaggerLeft;
			case ObjectSlot::kStaff:
				return ObjectSlotExtra::kStaff;
			case ObjectSlot::kStaffLeft:
				return ObjectSlotExtra::kStaffLeft;
			case ObjectSlot::kBow:
				return ObjectSlotExtra::kBow;
			case ObjectSlot::kCrossBow:
				return ObjectSlotExtra::kCrossBow;
			case ObjectSlot::kShield:
				return ObjectSlotExtra::kShield;
			case ObjectSlot::kTorch:
				return ObjectSlotExtra::kTorch;
			case ObjectSlot::kAmmo:
				return ObjectSlotExtra::kAmmo;
			default:
				return ObjectSlotExtra::kNone;
			}
		}

		bool ItemData::IsLeftWeaponSlot(ObjectSlot a_slot) noexcept
		{
			switch (a_slot)
			{
			case ObjectSlot::k1HSwordLeft:
			case ObjectSlot::k2HSwordLeft:
			case ObjectSlot::k1HAxeLeft:
			case ObjectSlot::k2HAxeLeft:
			case ObjectSlot::kDaggerLeft:
			case ObjectSlot::kMaceLeft:
			case ObjectSlot::kStaffLeft:
				return true;
			default:
				return false;
			}
		}

		bool ItemData::IsLeftHandExtraSlot(ObjectSlotExtra a_slot) noexcept
		{
			switch (a_slot)
			{
			case ObjectSlotExtra::k1HSwordLeft:
			case ObjectSlotExtra::k2HSwordLeft:
			case ObjectSlotExtra::k1HAxeLeft:
			case ObjectSlotExtra::k2HAxeLeft:
			case ObjectSlotExtra::kDaggerLeft:
			case ObjectSlotExtra::kMaceLeft:
			case ObjectSlotExtra::kStaffLeft:
			case ObjectSlotExtra::kSpellLeft:
			case ObjectSlotExtra::kShield:
			case ObjectSlotExtra::kTorch:
				return true;
			default:
				return false;
			}
		}

		NodeDescriptor ItemData::GetDefaultSlotNode(ObjectSlot a_slot) noexcept
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

		TESRace::EquipmentFlag
			ItemData::GetRaceEquipmentFlagFromType(ObjectType a_type) noexcept
		{
			switch (a_type)
			{
			case ObjectType::k1HSword:
				return TESRace::EquipmentFlag::kOneHandSword;
			case ObjectType::kDagger:
				return TESRace::EquipmentFlag::kOneHandDagger;
			case ObjectType::k1HAxe:
				return TESRace::EquipmentFlag::kOneHandAxe;
			case ObjectType::kMace:
				return TESRace::EquipmentFlag::kOneHandMace;
			case ObjectType::kStaff:
				return TESRace::EquipmentFlag::kStaff;
			case ObjectType::k2HSword:
				return TESRace::EquipmentFlag::kTwoHandSword;
			case ObjectType::k2HAxe:
				return TESRace::EquipmentFlag::kTwoHandAxe;
			case ObjectType::kBow:
				return TESRace::EquipmentFlag::kBow;
			case ObjectType::kCrossBow:
				return TESRace::EquipmentFlag::kCrossbow;
			case ObjectType::kShield:
				return TESRace::EquipmentFlag::kShield;
			case ObjectType::kTorch:
				return TESRace::EquipmentFlag::kTorch;
			default:
				return TESRace::EquipmentFlag::kNone;
			}
		}

		Biped::BIPED_OBJECT ItemData::SlotToBipedObject(ObjectSlot a_slot) noexcept
		{
			switch (a_slot)
			{
			case ObjectSlot::k1HSword:
				return Biped::BIPED_OBJECT::kOneHandSword;
			case ObjectSlot::k1HAxe:
				return Biped::BIPED_OBJECT::kOneHandAxe;
			case ObjectSlot::k2HSword:
			case ObjectSlot::k2HAxe:
				return Biped::BIPED_OBJECT::kTwoHandMelee;
			case ObjectSlot::k2HSwordLeft:
			case ObjectSlot::k2HAxeLeft:
				return Biped::BIPED_OBJECT::kNone;
			case ObjectSlot::kMace:
				return Biped::BIPED_OBJECT::kOneHandMace;
			case ObjectSlot::kDagger:
				return Biped::BIPED_OBJECT::kOneHandDagger;
			case ObjectSlot::kStaff:
				return Biped::BIPED_OBJECT::kStaff;
			case ObjectSlot::kBow:
				return Biped::BIPED_OBJECT::kBow;
			case ObjectSlot::kCrossBow:
				return Biped::BIPED_OBJECT::kCrossbow;
			case ObjectSlot::kDaggerLeft:
			case ObjectSlot::k1HSwordLeft:
			case ObjectSlot::k1HAxeLeft:
			case ObjectSlot::kMaceLeft:
			case ObjectSlot::kStaffLeft:
			case ObjectSlot::kShield:
			case ObjectSlot::kTorch:
				return Biped::BIPED_OBJECT::kShield;
			case ObjectSlot::kAmmo:
				return Biped::BIPED_OBJECT::kQuiver;
			default:
				return Biped::BIPED_OBJECT::kNone;
			}
		}

		bool IData::PopulateRaceList()
		{
			m_Instance.m_raceList.clear();

			auto dh = DataHandler::GetSingleton();
			if (!dh)
				return false;

			for (auto race : dh->races)
			{
				if (!race)
					continue;

				if (race->IsDeleted())
					continue;

				m_Instance.m_raceList.try_emplace(
					race->formID,
					LocaleData::ToUTF8(race->fullName.GetName()),
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