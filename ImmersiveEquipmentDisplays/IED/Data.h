#pragma once

#include "CollectorData.h"
#include "ConfigData.h"
#include "ConfigINI.h"
#include "ConfigSlot.h"
#include "ConfigStore.h"

namespace IED
{
	namespace Data
	{
		namespace ItemData
		{
			inline constexpr ObjectSlot GetObjectSlot(const TESObjectWEAP* a_form) noexcept
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
			
			inline constexpr ObjectSlot GetObjectSlotNoBound(const TESObjectWEAP* a_form) noexcept
			{
				if (a_form->IsBound())
				{
					return ObjectSlot::kMax;
				}
				else
				{
					return GetObjectSlot(a_form);
				}
			}

			inline constexpr ObjectSlot GetObjectSlotLeft(const TESObjectWEAP* a_form) noexcept
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
			
			inline constexpr ObjectSlot GetObjectSlotLeftNoBound(const TESObjectWEAP* a_form) noexcept
			{
				if (a_form->IsBound())
				{
					return ObjectSlot::kMax;
				}
				else
				{
					return GetObjectSlotLeft(a_form);
				}
			}

			inline constexpr ObjectSlot GetObjectSlotLeft(const TESObjectLIGH* a_form) noexcept
			{
				return a_form->CanCarry() ? ObjectSlot::kTorch : ObjectSlot::kMax;
			}

			inline constexpr ObjectSlot GetObjectSlot(const TESObjectARMO* a_form) noexcept
			{
				return a_form->IsShield() ? ObjectSlot::kShield : ObjectSlot::kMax;
			}

			inline constexpr ObjectSlot GetObjectSlot(const TESAmmo* a_form) noexcept
			{
				return ObjectSlot::kAmmo;
			}

			inline constexpr ObjectSlot GetObjectSlot(const TESForm* a_form) noexcept
			{
				switch (a_form->formType)
				{
				case TESObjectWEAP::kTypeID:
					return GetObjectSlot(static_cast<const TESObjectWEAP*>(a_form));
				case TESAmmo::kTypeID:
					return GetObjectSlot(static_cast<const TESAmmo*>(a_form));
				default:
					return ObjectSlot::kMax;
				}
			}
			
			inline constexpr ObjectSlot GetObjectSlotNoBound(const TESForm* a_form) noexcept
			{
				switch (a_form->formType)
				{
				case TESObjectWEAP::kTypeID:
					return GetObjectSlotNoBound(static_cast<const TESObjectWEAP*>(a_form));
				case TESAmmo::kTypeID:
					return GetObjectSlot(static_cast<const TESAmmo*>(a_form));
				default:
					return ObjectSlot::kMax;
				}
			}

			inline constexpr ObjectSlot GetObjectSlotLeft(const TESForm* a_form) noexcept
			{
				switch (a_form->formType)
				{
				case TESObjectWEAP::kTypeID:
					return GetObjectSlotLeft(static_cast<const TESObjectWEAP*>(a_form));
				case TESObjectARMO::kTypeID:
					return GetObjectSlot(static_cast<const TESObjectARMO*>(a_form));
				case TESObjectLIGH::kTypeID:
					return GetObjectSlotLeft(static_cast<const TESObjectLIGH*>(a_form));
				default:
					return ObjectSlot::kMax;
				}
			}
			
			inline constexpr ObjectSlot GetObjectSlotLeftNoBound(const TESForm* a_form) noexcept
			{
				switch (a_form->formType)
				{
				case TESObjectWEAP::kTypeID:
					return GetObjectSlotLeftNoBound(static_cast<const TESObjectWEAP*>(a_form));
				case TESObjectARMO::kTypeID:
					return GetObjectSlot(static_cast<const TESObjectARMO*>(a_form));
				case TESObjectLIGH::kTypeID:
					return GetObjectSlotLeft(static_cast<const TESObjectLIGH*>(a_form));
				default:
					return ObjectSlot::kMax;
				}
			}

			inline constexpr ObjectType GetItemType(const TESObjectARMO* a_form) noexcept
			{
				return a_form->IsShield() ? ObjectType::kShield : ObjectType::kMax;
			}

			inline constexpr ObjectType GetItemType(const TESObjectLIGH* a_form) noexcept
			{
				return a_form->CanCarry() ? ObjectType::kTorch : ObjectType::kMax;
			}

			inline constexpr ObjectType GetItemType(const TESAmmo* a_form) noexcept
			{
				return ObjectType::kAmmo;
			}

			inline constexpr ObjectType GetItemType(const TESObjectWEAP* a_form) noexcept
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

			inline constexpr ObjectType GetItemType(const TESForm* a_form) noexcept
			{
				switch (a_form->formType)
				{
				case TESObjectWEAP::kTypeID:
					return GetItemType(static_cast<const TESObjectWEAP*>(a_form));
				case TESObjectARMO::kTypeID:
					return GetItemType(static_cast<const TESObjectARMO*>(a_form));
				case TESObjectLIGH::kTypeID:
					return GetItemType(static_cast<const TESObjectLIGH*>(a_form));
				case TESAmmo::kTypeID:
					return GetItemType(static_cast<const TESAmmo*>(a_form));
				default:
					return ObjectType::kMax;
				}
			}

			inline constexpr ObjectTypeExtra GetItemTypeExtra(const TESObjectARMO* a_form) noexcept
			{
				return a_form->IsShield() ? ObjectTypeExtra::kShield : ObjectTypeExtra::kArmor;
			}

			inline constexpr ObjectTypeExtra GetItemTypeExtra(const TESObjectLIGH* a_form) noexcept
			{
				return a_form->CanCarry() ? ObjectTypeExtra::kTorch : ObjectTypeExtra::kNone;
			}

			inline constexpr ObjectTypeExtra GetItemTypeExtra(const TESObjectWEAP* a_form) noexcept
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

			inline constexpr ObjectTypeExtra GetItemTypeExtra(const TESForm* a_form) noexcept
			{
				switch (a_form->formType)
				{
				case TESObjectWEAP::kTypeID:
					return GetItemTypeExtra(static_cast<const TESObjectWEAP*>(a_form));
				case TESObjectARMO::kTypeID:
					return GetItemTypeExtra(static_cast<const TESObjectARMO*>(a_form));
				case TESObjectLIGH::kTypeID:
					return GetItemTypeExtra(static_cast<const TESObjectLIGH*>(a_form));
				case TESAmmo::kTypeID:
					return ObjectTypeExtra::kAmmo;
				case SpellItem::kTypeID:
					return ObjectTypeExtra::kSpell;
				default:
					return ObjectTypeExtra::kNone;
				}
			}

			SKMP_FORCEINLINE constexpr CollectorData::ItemExtraData GetItemTypePair(const TESForm* a_form) noexcept
			{
				switch (a_form->formType)
				{
				case SpellItem::kTypeID:
					return CollectorData::ItemExtraData(ObjectType::kMax, ObjectTypeExtra::kSpell);
				case TESObjectARMO::kTypeID:
					return static_cast<const TESObjectARMO*>(a_form)->IsShield() ?
					           CollectorData::ItemExtraData(ObjectType::kShield, ObjectTypeExtra::kShield) :
					           CollectorData::ItemExtraData(ObjectType::kMax, ObjectTypeExtra::kArmor);
				case TESObjectLIGH::kTypeID:
					return static_cast<const TESObjectLIGH*>(a_form)->CanCarry() ?
					           CollectorData::ItemExtraData(ObjectType::kTorch, ObjectTypeExtra::kTorch) :
					           CollectorData::ItemExtraData(ObjectType::kMax, ObjectTypeExtra::kNone);
				case TESObjectWEAP::kTypeID:
					switch (static_cast<const TESObjectWEAP*>(a_form)->type())
					{
					case WEAPON_TYPE::kOneHandSword:
						return CollectorData::ItemExtraData(ObjectType::k1HSword, ObjectTypeExtra::k1HSword);
					case WEAPON_TYPE::kOneHandDagger:
						return CollectorData::ItemExtraData(ObjectType::kDagger, ObjectTypeExtra::kDagger);
					case WEAPON_TYPE::kOneHandAxe:
						return CollectorData::ItemExtraData(ObjectType::k1HAxe, ObjectTypeExtra::k1HAxe);
					case WEAPON_TYPE::kOneHandMace:
						return CollectorData::ItemExtraData(ObjectType::kMace, ObjectTypeExtra::kMace);
					case WEAPON_TYPE::kTwoHandSword:
						return CollectorData::ItemExtraData(ObjectType::k2HSword, ObjectTypeExtra::k2HSword);
					case WEAPON_TYPE::kTwoHandAxe:
						return CollectorData::ItemExtraData(ObjectType::k2HAxe, ObjectTypeExtra::k2HAxe);
					case WEAPON_TYPE::kBow:
						return CollectorData::ItemExtraData(ObjectType::kBow, ObjectTypeExtra::kBow);
					case WEAPON_TYPE::kStaff:
						return CollectorData::ItemExtraData(ObjectType::kStaff, ObjectTypeExtra::kStaff);
					case WEAPON_TYPE::kCrossbow:
						return CollectorData::ItemExtraData(ObjectType::kCrossBow, ObjectTypeExtra::kCrossBow);
					default:
						return CollectorData::ItemExtraData(ObjectType::kMax, ObjectTypeExtra::kNone);
					}
				case TESAmmo::kTypeID:
					return CollectorData::ItemExtraData(ObjectType::kAmmo, ObjectTypeExtra::kAmmo);
				default:
					return CollectorData::ItemExtraData(ObjectType::kMax, ObjectTypeExtra::kNone);
				}
			}

			inline constexpr ObjectSlotExtra GetItemSlotExtra(const TESObjectARMO* a_form) noexcept
			{
				return !a_form->IsShield() ? ObjectSlotExtra::kArmor : ObjectSlotExtra::kNone;
			}

			inline constexpr ObjectSlotExtra GetItemSlotExtra(const TESObjectWEAP* a_form) noexcept
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

			inline constexpr ObjectSlotExtra GetItemSlotExtra(const TESObjectLIGH* a_form) noexcept
			{
				return a_form->CanCarry() ? ObjectSlotExtra::kTorch : ObjectSlotExtra::kNone;
			}

			inline constexpr ObjectSlotExtra GetItemSlotExtra(const TESForm* a_form) noexcept
			{
				switch (a_form->formType)
				{
				case TESObjectWEAP::kTypeID:
					return GetItemSlotExtra(static_cast<const TESObjectWEAP*>(a_form));
				case TESObjectARMO::kTypeID:
					return GetItemSlotExtra(static_cast<const TESObjectARMO*>(a_form));
				case TESObjectLIGH::kTypeID:
					return GetItemSlotExtra(static_cast<const TESObjectLIGH*>(a_form));
				case TESAmmo::kTypeID:
					return ObjectSlotExtra::kAmmo;
				case SpellItem::kTypeID:
					return ObjectSlotExtra::kSpell;
				default:
					return ObjectSlotExtra::kNone;
				}
			}

			inline constexpr ObjectSlotExtra GetItemSlotExtraGeneric(const TESObjectARMO* a_form) noexcept
			{
				return a_form->IsShield() ? ObjectSlotExtra::kShield : ObjectSlotExtra::kArmor;
			}

			inline constexpr ObjectSlotExtra GetItemSlotExtraGeneric(const TESForm* a_form) noexcept
			{
				switch (a_form->formType)
				{
				case TESObjectWEAP::kTypeID:
					return GetItemSlotExtra(static_cast<const TESObjectWEAP*>(a_form));
				case TESObjectARMO::kTypeID:
					return GetItemSlotExtraGeneric(static_cast<const TESObjectARMO*>(a_form));
				case TESObjectLIGH::kTypeID:
					return GetItemSlotExtra(static_cast<const TESObjectLIGH*>(a_form));
				case TESAmmo::kTypeID:
					return ObjectSlotExtra::kAmmo;
				case SpellItem::kTypeID:
					return ObjectSlotExtra::kSpell;
				default:
					return ObjectSlotExtra::kNone;
				}
			}

			inline constexpr ObjectSlotExtra GetItemSlotLeftExtra(const TESObjectWEAP* a_form) noexcept
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

			inline constexpr ObjectSlotExtra GetItemSlotLeftExtra(const TESForm* a_form) noexcept
			{
				switch (a_form->formType)
				{
				case TESObjectWEAP::kTypeID:
					return GetItemSlotLeftExtra(static_cast<const TESObjectWEAP*>(a_form));
				case TESObjectARMO::kTypeID:
					return static_cast<const TESObjectARMO*>(a_form)->IsShield() ?
					           ObjectSlotExtra::kShield :
					           ObjectSlotExtra::kNone;
				case SpellItem::kTypeID:
					return ObjectSlotExtra::kSpellLeft;
				default:
					return ObjectSlotExtra::kNone;
				}
			}

			inline constexpr ObjectSlot GetSlotFromType(ObjectType a_type) noexcept
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
					return ObjectSlot::kMax;
				}
			}

			inline constexpr ObjectSlotExtra GetSlotFromTypeExtra(ObjectTypeExtra a_type) noexcept
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
					return ObjectSlotExtra::kNone;
				}
			}

			inline constexpr ObjectTypeExtra GetTypeFromSlotExtra(ObjectSlotExtra a_slot) noexcept
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

			inline constexpr ObjectSlot GetLeftSlot(ObjectSlot a_slot) noexcept
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

			inline constexpr ObjectSlotExtra GetLeftSlotExtra(ObjectSlotExtra a_slot) noexcept
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

			inline constexpr ObjectSlot ExtraSlotToSlot(ObjectSlotExtra a_slot) noexcept
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

			inline constexpr ObjectSlotExtra SlotToExtraSlot(ObjectSlot a_slot) noexcept
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

			inline constexpr bool IsLeftWeaponSlot(ObjectSlot a_slot) noexcept
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

			inline constexpr bool IsLeftHandExtraSlot(ObjectSlotExtra a_slot) noexcept
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

			NodeDescriptor GetDefaultSlotNode(ObjectSlot a_slot);

			inline constexpr TESRace::EquipmentFlag GetRaceEquipmentFlagFromType(ObjectType a_type) noexcept
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

			inline constexpr BIPED_OBJECT SlotToBipedObject(
				Actor*     a_actor,
				ObjectSlot a_slot) noexcept
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
			IPluginInfoA                                       m_pluginInfo;

			Game::FormID m_playerRefID{ 0x14 };
			Game::FormID m_playerBaseID{ 0x7 };

			static IData m_Instance;
		};

		inline constexpr const char* GetSlotName(ObjectSlot a_slot)
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

		inline constexpr const char* GetSlotName(ObjectSlotExtra a_slot)
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

		inline constexpr const char* GetObjectTypeName(ObjectType a_type)
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

		inline constexpr const char* GetObjectTypeName(ObjectTypeExtra a_type)
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

		inline constexpr const char* GetConfigClassName(ConfigClass a_class)
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

		std::unique_ptr<configSlotHolder_t::data_type> CreateDefaultSlotConfig(
			ObjectSlot a_slot);

		std::unique_ptr<configSlotHolderCopy_t::data_type> CreateDefaultSlotConfig(
			ObjectSlot  a_slot,
			ConfigClass a_class);

	}
}