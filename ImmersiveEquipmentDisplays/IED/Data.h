#pragma once

#include "ConfigData.h"
#include "ConfigINI.h"
#include "ConfigStore.h"

namespace IED
{
	namespace Data
	{
		class ItemData
		{
		public:
			inline static constexpr ObjectSlot GetObjectSlot(TESObjectWEAP* a_form) noexcept
			{
				switch (a_form->type())
				{
				case WEAPON_TYPE::kOneHandSword:
					return ObjectSlot::k1HSword;
				case WEAPON_TYPE::kOneHandDagger:
					return ObjectSlot::kDagger;
				case WEAPON_TYPE::kOneHandAxe:
					return ObjectSlot::k1HAxe;
				case WEAPON_TYPE::kOneHandMace:
					return ObjectSlot::kMace;
				case WEAPON_TYPE::kTwoHandSword:
					return ObjectSlot::k2HSword;
				case WEAPON_TYPE::kTwoHandAxe:
					return ObjectSlot::k2HAxe;
				case WEAPON_TYPE::kBow:
					return ObjectSlot::kBow;
				case WEAPON_TYPE::kStaff:
					return ObjectSlot::kStaff;
				case WEAPON_TYPE::kCrossbow:
					return ObjectSlot::kCrossBow;
				default:
					return ObjectSlot::kMax;
				}
			}

			inline static constexpr ObjectSlot GetObjectSlotLeft(TESObjectWEAP* a_form) noexcept
			{
				switch (a_form->type())
				{
				case WEAPON_TYPE::kOneHandSword:
					return ObjectSlot::k1HSwordLeft;
				case WEAPON_TYPE::kOneHandAxe:
					return ObjectSlot::k1HAxeLeft;
				case WEAPON_TYPE::kTwoHandSword:
					return ObjectSlot::k2HSwordLeft;
				case WEAPON_TYPE::kTwoHandAxe:
					return ObjectSlot::k2HAxeLeft;
				case WEAPON_TYPE::kOneHandDagger:
					return ObjectSlot::kDaggerLeft;
				case WEAPON_TYPE::kOneHandMace:
					return ObjectSlot::kMaceLeft;
				case WEAPON_TYPE::kStaff:
					return ObjectSlot::kStaffLeft;
				default:
					return ObjectSlot::kMax;
				}
			}

			inline static constexpr ObjectSlot GetObjectSlotLeft(TESObjectLIGH* a_form) noexcept
			{
				return a_form->CanCarry() ? ObjectSlot::kTorch : ObjectSlot::kMax;
			}

			inline static constexpr ObjectSlot GetObjectSlot(TESObjectARMO* a_form) noexcept
			{
				return a_form->IsShield() ? ObjectSlot::kShield : ObjectSlot::kMax;
			}

			inline static constexpr ObjectSlot GetObjectSlot(TESAmmo* a_form) noexcept
			{
				return ObjectSlot::kAmmo;
			}

			inline static constexpr ObjectSlot GetObjectSlot(TESForm* a_form) noexcept
			{
				switch (a_form->formType)
				{
				case TESObjectWEAP::kTypeID:
					return GetObjectSlot(static_cast<TESObjectWEAP*>(a_form));
				case TESAmmo::kTypeID:
					return GetObjectSlot(static_cast<TESAmmo*>(a_form));
				}

				return ObjectSlot::kMax;
			}

			inline static constexpr ObjectSlot GetObjectSlotLeft(TESForm* a_form) noexcept
			{
				switch (a_form->formType)
				{
				case TESObjectWEAP::kTypeID:
					return GetObjectSlotLeft(static_cast<TESObjectWEAP*>(a_form));
				case TESObjectARMO::kTypeID:
					return GetObjectSlot(static_cast<TESObjectARMO*>(a_form));
				case TESObjectLIGH::kTypeID:
					return GetObjectSlotLeft(static_cast<TESObjectLIGH*>(a_form));
				}

				return ObjectSlot::kMax;
			}

			inline static constexpr ObjectType GetItemType(TESObjectARMO* a_form) noexcept
			{
				return a_form->IsShield() ? ObjectType::kShield : ObjectType::kMax;
			}

			inline static constexpr ObjectType GetItemType(TESObjectLIGH* a_form) noexcept
			{
				return a_form->CanCarry() ? ObjectType::kTorch : ObjectType::kMax;
			}

			inline static constexpr ObjectType GetItemType(TESAmmo* a_form) noexcept
			{
				return ObjectType::kAmmo;
			}

			inline static constexpr ObjectType GetItemType(TESObjectWEAP* a_form) noexcept
			{
				switch (a_form->type())
				{
				case WEAPON_TYPE::kOneHandSword:
					return ObjectType::k1HSword;
				case WEAPON_TYPE::kOneHandDagger:
					return ObjectType::kDagger;
				case WEAPON_TYPE::kOneHandAxe:
					return ObjectType::k1HAxe;
				case WEAPON_TYPE::kOneHandMace:
					return ObjectType::kMace;
				case WEAPON_TYPE::kTwoHandSword:
					return ObjectType::k2HSword;
				case WEAPON_TYPE::kTwoHandAxe:
					return ObjectType::k2HAxe;
				case WEAPON_TYPE::kBow:
					return ObjectType::kBow;
				case WEAPON_TYPE::kStaff:
					return ObjectType::kStaff;
				case WEAPON_TYPE::kCrossbow:
					return ObjectType::kCrossBow;
				default:
					return ObjectType::kMax;
				}
			}

			inline static constexpr ObjectType GetItemType(TESForm* a_form) noexcept
			{
				switch (a_form->formType)
				{
				case TESObjectWEAP::kTypeID:
					return GetItemType(static_cast<TESObjectWEAP*>(a_form));
				case TESObjectARMO::kTypeID:
					return GetItemType(static_cast<TESObjectARMO*>(a_form));
				case TESObjectLIGH::kTypeID:
					return GetItemType(static_cast<TESObjectLIGH*>(a_form));
				case TESAmmo::kTypeID:
					return GetItemType(static_cast<TESAmmo*>(a_form));
				}

				return ObjectType::kMax;
			}

			inline static constexpr ObjectTypeExtra GetItemTypeExtra(TESObjectARMO* a_form) noexcept
			{
				return a_form->IsShield() ? ObjectTypeExtra::kShield : ObjectTypeExtra::kArmor;
			}

			inline static constexpr ObjectTypeExtra GetItemTypeExtra(TESObjectLIGH* a_form) noexcept
			{
				return a_form->CanCarry() ? ObjectTypeExtra::kTorch : ObjectTypeExtra::kNone;
			}

			inline static constexpr ObjectTypeExtra GetItemTypeExtra(TESObjectWEAP* a_form) noexcept
			{
				switch (a_form->type())
				{
				case WEAPON_TYPE::kOneHandSword:
					return ObjectTypeExtra::k1HSword;
				case WEAPON_TYPE::kOneHandDagger:
					return ObjectTypeExtra::kDagger;
				case WEAPON_TYPE::kOneHandAxe:
					return ObjectTypeExtra::k1HAxe;
				case WEAPON_TYPE::kOneHandMace:
					return ObjectTypeExtra::kMace;
				case WEAPON_TYPE::kTwoHandSword:
					return ObjectTypeExtra::k2HSword;
				case WEAPON_TYPE::kTwoHandAxe:
					return ObjectTypeExtra::k2HAxe;
				case WEAPON_TYPE::kBow:
					return ObjectTypeExtra::kBow;
				case WEAPON_TYPE::kStaff:
					return ObjectTypeExtra::kStaff;
				case WEAPON_TYPE::kCrossbow:
					return ObjectTypeExtra::kCrossBow;
				default:
					return ObjectTypeExtra::kNone;
				}
			}

			inline static constexpr ObjectTypeExtra GetItemTypeExtra(TESForm* a_form) noexcept
			{
				switch (a_form->formType)
				{
				case TESObjectWEAP::kTypeID:
					return GetItemTypeExtra(static_cast<TESObjectWEAP*>(a_form));
				case TESObjectARMO::kTypeID:
					return GetItemTypeExtra(static_cast<TESObjectARMO*>(a_form));
				case TESObjectLIGH::kTypeID:
					return GetItemTypeExtra(static_cast<TESObjectLIGH*>(a_form));
				case TESAmmo::kTypeID:
					return ObjectTypeExtra::kAmmo;
				case SpellItem::kTypeID:
					return ObjectTypeExtra::kSpell;
				default:
					return ObjectTypeExtra::kNone;
				}
			}

			inline static constexpr ObjectSlotExtra GetItemSlotExtra(TESObjectARMO* a_form) noexcept
			{
				return !a_form->IsShield() ? ObjectSlotExtra::kArmor : ObjectSlotExtra::kNone;
			}

			inline static constexpr ObjectSlotExtra GetItemSlotExtra(TESObjectWEAP* a_form) noexcept
			{
				switch (a_form->type())
				{
				case WEAPON_TYPE::kOneHandSword:
					return ObjectSlotExtra::k1HSword;
				case WEAPON_TYPE::kOneHandDagger:
					return ObjectSlotExtra::kDagger;
				case WEAPON_TYPE::kOneHandAxe:
					return ObjectSlotExtra::k1HAxe;
				case WEAPON_TYPE::kOneHandMace:
					return ObjectSlotExtra::kMace;
				case WEAPON_TYPE::kTwoHandSword:
					return ObjectSlotExtra::k2HSword;
				case WEAPON_TYPE::kTwoHandAxe:
					return ObjectSlotExtra::k2HAxe;
				case WEAPON_TYPE::kBow:
					return ObjectSlotExtra::kBow;
				case WEAPON_TYPE::kStaff:
					return ObjectSlotExtra::kStaff;
				case WEAPON_TYPE::kCrossbow:
					return ObjectSlotExtra::kCrossBow;
				default:
					return ObjectSlotExtra::kNone;
				}
			}

			inline static constexpr ObjectSlotExtra GetItemSlotExtra(TESObjectLIGH* a_form) noexcept
			{
				return a_form->CanCarry() ? ObjectSlotExtra::kTorch : ObjectSlotExtra::kNone;
			}

			inline static constexpr ObjectSlotExtra GetItemSlotExtra(TESForm* a_form) noexcept
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
			
			inline static constexpr ObjectSlotExtra GetItemSlotExtraGeneric(TESObjectARMO* a_form) noexcept
			{
				return a_form->IsShield() ? ObjectSlotExtra::kShield : ObjectSlotExtra::kArmor;
			}

			inline static constexpr ObjectSlotExtra GetItemSlotExtraGeneric(TESForm* a_form) noexcept
			{
				switch (a_form->formType)
				{
				case TESObjectWEAP::kTypeID:
					return GetItemSlotExtra(static_cast<TESObjectWEAP*>(a_form));
				case TESObjectARMO::kTypeID:
					return GetItemSlotExtraGeneric(static_cast<TESObjectARMO*>(a_form));
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

			inline static constexpr ObjectSlotExtra GetItemSlotLeftExtra(TESObjectWEAP* a_form) noexcept
			{
				switch (a_form->type())
				{
				case WEAPON_TYPE::kOneHandSword:
					return ObjectSlotExtra::k1HSwordLeft;
				case WEAPON_TYPE::kOneHandDagger:
					return ObjectSlotExtra::kDaggerLeft;
				case WEAPON_TYPE::kOneHandAxe:
					return ObjectSlotExtra::k1HAxeLeft;
				case WEAPON_TYPE::kOneHandMace:
					return ObjectSlotExtra::kMaceLeft;
				case WEAPON_TYPE::kTwoHandSword:
					return ObjectSlotExtra::k2HSwordLeft;
				case WEAPON_TYPE::kTwoHandAxe:
					return ObjectSlotExtra::k2HAxeLeft;
				case WEAPON_TYPE::kStaff:
					return ObjectSlotExtra::kStaffLeft;
				default:
					return ObjectSlotExtra::kNone;
				}
			}

			inline static constexpr ObjectSlotExtra GetItemSlotLeftExtra(TESForm* a_form) noexcept
			{
				switch (a_form->formType)
				{
				case TESObjectWEAP::kTypeID:
					return GetItemSlotLeftExtra(static_cast<TESObjectWEAP*>(a_form));
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

			inline static constexpr ObjectSlot GetSlotFromType(ObjectType a_type) noexcept
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

			inline static constexpr ObjectSlotExtra GetSlotFromTypeExtra(ObjectTypeExtra a_type) noexcept
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

			inline static constexpr ObjectTypeExtra GetTypeFromSlotExtra(ObjectSlotExtra a_slot) noexcept
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

			inline static constexpr ObjectSlot GetLeftSlot(ObjectSlot a_slot) noexcept
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

			inline static constexpr ObjectSlotExtra GetLeftSlotExtra(ObjectSlotExtra a_slot) noexcept
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

			inline static constexpr ObjectSlot ExtraSlotToSlot(ObjectSlotExtra a_slot) noexcept
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

			inline static constexpr ObjectSlotExtra SlotToExtraSlot(ObjectSlot a_slot) noexcept
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

			inline static constexpr bool IsLeftWeaponSlot(ObjectSlot a_slot) noexcept
			{
				switch (a_slot)
				{
				case ObjectSlot::k1HSwordLeft:
				case ObjectSlot::k1HAxeLeft:
				case ObjectSlot::k2HSwordLeft:
				case ObjectSlot::k2HAxeLeft:
				case ObjectSlot::kDaggerLeft:
				case ObjectSlot::kMaceLeft:
				case ObjectSlot::kStaffLeft:
					return true;
				default:
					return false;
				}
			}

			inline static constexpr bool IsLeftHandExtraSlot(ObjectSlotExtra a_slot) noexcept
			{
				switch (a_slot)
				{
				case ObjectSlotExtra::k1HSwordLeft:
				case ObjectSlotExtra::k1HAxeLeft:
				case ObjectSlotExtra::k2HSwordLeft:
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

			static NodeDescriptor GetDefaultSlotNode(ObjectSlot a_slot);

			inline static constexpr TESRace::EquipmentFlag GetRaceEquipmentFlagFromType(ObjectType a_type) noexcept
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

			inline static constexpr BIPED_OBJECT SlotToBipedObject(Actor* a_actor, ObjectSlot a_slot) noexcept
			{
				switch (a_slot)
				{
				case ObjectSlot::k1HSword:
					return BIPED_OBJECT::kOneHandSword;
				case ObjectSlot::k1HAxe:
					return BIPED_OBJECT::kOneHandAxe;
				case ObjectSlot::k2HSword:
				case ObjectSlot::k2HAxe:
					return BIPED_OBJECT::kTwoHandMelee;
				case ObjectSlot::k2HSwordLeft:
				case ObjectSlot::k2HAxeLeft:
					return BIPED_OBJECT::kNone;
				case ObjectSlot::kMace:
					return BIPED_OBJECT::kOneHandMace;
				case ObjectSlot::kDagger:
					return BIPED_OBJECT::kOneHandDagger;
				case ObjectSlot::kStaff:
					return BIPED_OBJECT::kStaff;
				case ObjectSlot::kBow:
					return BIPED_OBJECT::kBow;
				case ObjectSlot::kCrossBow:
					return BIPED_OBJECT::kCrossbow;
				case ObjectSlot::kDaggerLeft:
				case ObjectSlot::k1HSwordLeft:
				case ObjectSlot::k1HAxeLeft:
				case ObjectSlot::kMaceLeft:
				case ObjectSlot::kStaffLeft:
				case ObjectSlot::kShield:
				case ObjectSlot::kTorch:
					return a_actor->GetShieldBipedObject();
				case ObjectSlot::kAmmo:
					return BIPED_OBJECT::kQuiver;
				default:
					return BIPED_OBJECT::kNone;
				}
			}

		private:
		};

		struct raceCacheEntry_t
		{
			std::string              fullname;
			std::string              edid;
			stl::flag<TESRace::Flag> flags;
		};

		class IData
		{
		public:
			[[nodiscard]] static bool PopulateRaceList();
			[[nodiscard]] static bool PopulatePluginInfo();
			[[nodiscard]] static bool PopulateMiscInfo();

			[[nodiscard]] inline constexpr static const auto& GetRaceList() noexcept
			{
				return m_Instance.m_raceList;
			}

			[[nodiscard]] inline constexpr static const auto& GetPluginInfo() noexcept
			{
				return m_Instance.m_pluginInfo;
			}

			[[nodiscard]] inline constexpr static const auto& GetPlayerRefID() noexcept
			{
				return m_Instance.m_playerRefID;
			}

			[[nodiscard]] inline constexpr static const auto& GetPlayerBaseID() noexcept
			{
				return m_Instance.m_playerBaseID;
			}

		private:
			stl::unordered_map<Game::FormID, raceCacheEntry_t> m_raceList;
			IPluginInfo                                        m_pluginInfo;

			Game::FormID m_playerRefID{ 0x14 };
			Game::FormID m_playerBaseID{ 0x7 };

			static IData m_Instance;
		};

		const char* GetSlotName(ObjectSlot a_slot);
		const char* GetSlotName(ObjectSlotExtra a_slot);
		const char* GetObjectTypeName(ObjectType a_type);
		const char* GetObjectTypeName(ObjectTypeExtra a_type);
		const char* GetConfigClassName(ConfigClass a_class);

		std::unique_ptr<configSlotHolder_t::data_type>
			CreateDefaultSlotConfig(ObjectSlot a_slot);

		std::unique_ptr<configSlotHolderCopy_t::data_type>
			CreateDefaultSlotConfig(ObjectSlot a_slot, ConfigClass a_class);

	}
}