#include "pch.h"

#include "ConfigCommon.h"
#include "FormCommon.h"
#include "LocaleData.h"

namespace IED
{
	bool IFormCommon::IsValidCustomForm(TESForm* a_form)
	{
		if (a_form->IsDeleted())
		{
			return false;
		}

		switch (a_form->formType)
		{
		case TESObjectMISC::kTypeID:
		case TESObjectSTAT::kTypeID:
		case BGSMovableStatic::kTypeID:
		case TESObjectTREE::kTypeID:
		case TESGrass::kTypeID:
		case TESObjectWEAP::kTypeID:
		case TESObjectBOOK::kTypeID:
		case TESObjectACTI::kTypeID:
		case TESObjectANIO::kTypeID:
		case TESObjectDOOR::kTypeID:
		case BGSTalkingActivator::kTypeID:
		case TESSoulGem::kTypeID:
		case TESKey::kTypeID:
		case TESAmmo::kTypeID:
		case BGSArtObject::kTypeID:
		case IngredientItem::kTypeID:
		case AlchemyItem::kTypeID:
		case ScrollItem::kTypeID:
		case TESFlora::kTypeID:
		case TESFurniture::kTypeID:
		case BGSStaticCollection::kTypeID:
		case BGSExplosion::kTypeID:
		case TESObjectARMO::kTypeID:
			return true;
		case TESObjectLIGH::kTypeID:
			return static_cast<TESObjectLIGH*>(a_form)->CanCarry();
		default:
			return false;
		}
	}

	bool IFormCommon::IsInventoryForm(TESForm* a_form)
	{
		if (a_form->IsDeleted())
		{
			return false;
		}

		switch (a_form->formType)
		{
		case TESObjectMISC::kTypeID:
		case TESObjectWEAP::kTypeID:
		case TESObjectBOOK::kTypeID:
		case TESSoulGem::kTypeID:
		case TESKey::kTypeID:
		case TESAmmo::kTypeID:
		case IngredientItem::kTypeID:
		case AlchemyItem::kTypeID:
		case ScrollItem::kTypeID:
		case TESObjectARMO::kTypeID:
			return true;
		case TESObjectLIGH::kTypeID:
			return static_cast<TESObjectLIGH*>(a_form)->CanCarry();
		default:
			return false;
		}
	}

	bool IFormCommon::IsInventoryFormType(std::uint8_t a_type)
	{
		switch (a_type)
		{
		case TESObjectMISC::kTypeID:
		case TESObjectWEAP::kTypeID:
		case TESObjectBOOK::kTypeID:
		case TESSoulGem::kTypeID:
		case TESKey::kTypeID:
		case TESAmmo::kTypeID:
		case IngredientItem::kTypeID:
		case AlchemyItem::kTypeID:
		case ScrollItem::kTypeID:
		case TESObjectARMO::kTypeID:
		case TESObjectLIGH::kTypeID:
			return true;
		default:
			return false;
		}
	}

	bool IFormCommon::IsValidSlotForm(TESForm* a_form)
	{
		if (a_form->IsDeleted())
		{
			return false;
		}

		switch (a_form->formType)
		{
		case TESObjectWEAP::kTypeID:
		case TESAmmo::kTypeID:
			return true;
		case TESObjectARMO::kTypeID:
			return static_cast<TESObjectARMO*>(a_form)->IsShield();
		case TESObjectLIGH::kTypeID:
			return static_cast<TESObjectLIGH*>(a_form)->CanCarry();
		}

		return false;
	}

	bool IFormCommon::IsEquippableForm(TESForm* a_form)
	{
		switch (a_form->formType)
		{
		case TESObjectLIGH::kTypeID:
			return static_cast<TESObjectLIGH*>(a_form)->CanCarry();
		case TESObjectWEAP::kTypeID:
		case TESObjectARMO::kTypeID:
		case TESAmmo::kTypeID:
		case SpellItem::kTypeID:
		case TESShout::kTypeID:
			return true;
		default:
			return false;
		}
	}

	stl::flag<FormInfoFlags> IFormCommon::GetFormFlags(TESForm* a_form)
	{
		FormInfoFlags flags{ FormInfoFlags::kNone };

		if (IsInventoryForm(a_form))
		{
			flags |= FormInfoFlags::kInventory;
		}

		if (IsValidCustomForm(a_form))
		{
			flags |= FormInfoFlags::kValidCustom;
		}

		if (IsValidSlotForm(a_form))
		{
			flags |= FormInfoFlags::kValidSlot;
		}

		return flags;
	}

	template <class T>
	inline static constexpr const char* GetFullName(TESForm* a_form)
	{
		return static_cast<T*>(a_form)->fullName.name.c_str();
	}

	inline static const char* GetKeywordString(BGSKeyword* a_form)
	{
		return a_form->keyword.c_str();
	}

	template <class T>
	inline static constexpr const char* GetEditorID(T* a_form)
	{
		return a_form->editorId.c_str();
	}

	inline static const char* GetReferenceName(TESObjectREFR* a_form)
	{
		return a_form->GetReferenceName();
	}

	inline static constexpr const char* GetFormNamePtr(TESForm* a_form)
	{
		switch (a_form->formType)
		{
		case TESObjectWEAP::kTypeID:
			return GetFullName<TESObjectWEAP>(a_form);
		case TESObjectARMO::kTypeID:
			return GetFullName<TESObjectARMO>(a_form);
		case TESNPC::kTypeID:
			return GetFullName<TESNPC>(a_form);
		case TESSoulGem::kTypeID:
		case TESKey::kTypeID:
		case TESObjectMISC::kTypeID:
			return GetFullName<TESObjectMISC>(a_form);
		case TESObjectLIGH::kTypeID:
			return GetFullName<TESObjectLIGH>(a_form);
		case TESObjectBOOK::kTypeID:
			return GetFullName<TESObjectBOOK>(a_form);
		case AlchemyItem::kTypeID:
		case IngredientItem::kTypeID:
		case SpellItem::kTypeID:
		case ScrollItem::kTypeID:
			return GetFullName<MagicItem>(a_form);
		case TESAmmo::kTypeID:
			return GetFullName<TESAmmo>(a_form);
		case TESObjectACTI::kTypeID:
		case BGSTalkingActivator::kTypeID:
		case TESFlora::kTypeID:
		case TESFurniture::kTypeID:
			return GetFullName<TESObjectACTI>(a_form);
		case BGSHeadPart::kTypeID:
			return GetFullName<BGSHeadPart>(a_form);
		case BGSKeyword::kTypeID:
			return GetKeywordString(static_cast<BGSKeyword*>(a_form));
		case TESRace::kTypeID:
			return GetEditorID(static_cast<TESRace*>(a_form));
		case TESQuest::kTypeID:
			return GetEditorID(static_cast<TESQuest*>(a_form));
		case TESObjectANIO::kTypeID:
			return GetEditorID(static_cast<TESObjectANIO*>(a_form));
		case TESObjectDOOR::kTypeID:
			return GetFullName<TESObjectDOOR>(a_form);
		case BGSExplosion::kTypeID:
			return GetFullName<BGSExplosion>(a_form);
		case BGSLocation::kTypeID:
			return GetFullName<BGSLocation>(a_form);
		case TESWorldSpace::kTypeID:
			return GetFullName<TESWorldSpace>(a_form);
		case TESShout::kTypeID:
			return GetFullName<TESShout>(a_form);
		case TESFaction::kTypeID:
			return GetFullName<TESFaction>(a_form);
		case TESClass::kTypeID:
			return GetFullName<TESClass>(a_form);
		case TESGlobal::kTypeID:
			return static_cast<TESGlobal*>(a_form)->formEditorID.c_str();
		case TESObjectREFR::kTypeID:
		case Actor::kTypeID:
			return GetReferenceName(static_cast<TESObjectREFR*>(a_form));
		default:
			return nullptr;
		}
	}

	std::string IFormCommon::GetFormName(TESForm* a_form)
	{
		return LocaleData::ToUTF8(GetFormNamePtr(a_form));
	}

	const char* IFormCommon::GetFormTypeDesc(std::uint8_t a_type)
	{
		switch (a_type)
		{
		case TESObjectWEAP::kTypeID:
			return "Weapon";
		case TESObjectARMO::kTypeID:
			return "Armor";
		case TESNPC::kTypeID:
			return "NPC";
		case TESSoulGem::kTypeID:
			return "Soul Gem";
		case TESKey::kTypeID:
			return "Key";
		case TESObjectMISC::kTypeID:
			return "Misc";
		case TESObjectLIGH::kTypeID:
			return "Light";
		case TESObjectBOOK::kTypeID:
			return "Book";
		case AlchemyItem::kTypeID:
			return "Potion/Food";
		case IngredientItem::kTypeID:
			return "Ingredient";
		case ScrollItem::kTypeID:
			return "Scroll";
		case TESObjectSTAT::kTypeID:
			return "Static";
		case BGSMovableStatic::kTypeID:
			return "Movable Static";
		case TESObjectTREE::kTypeID:
			return "Tree";
		case TESGrass::kTypeID:
			return "Grass";
		case BGSHeadPart::kTypeID:
			return "Head Part";
		case TESAmmo::kTypeID:
			return "Ammo";
		case BGSKeyword::kTypeID:
			return "Keyword";
		case TESFlora::kTypeID:
			return "Flora";
		case TESFurniture::kTypeID:
			return "Furniture";
		case TESObjectACTI::kTypeID:
			return "Activator";
		case BGSTalkingActivator::kTypeID:
			return "Talking Activator";
		case TESObjectREFR::kTypeID:
			return "Reference";
		case Actor::kTypeID:
			return "Actor";
		case TESQuest::kTypeID:
			return "Quest";
		case SpellItem::kTypeID:
			return "Spell";
		case TESRace::kTypeID:
			return "Race";
		case BGSArtObject::kTypeID:
			return "Art Object";
		case BGSSoundDescriptorForm::kTypeID:
			return "Sound Descriptor";
		case TESObjectANIO::kTypeID:
			return "Anim Object";
		case TESObjectDOOR::kTypeID:
			return "Door";
		case BGSExplosion::kTypeID:
			return "Explosion";
		case BGSLocation::kTypeID:
			return "Location";
		case TESWorldSpace::kTypeID:
			return "Worldspace";
		case TESPackage::kTypeID:
			return "Package";
		case TESShout::kTypeID:
			return "Shout";
		case TESFaction::kTypeID:
			return "Faction";
		case TESCombatStyle::kTypeID:
			return "Combat Style";
		case TESClass::kTypeID:
			return "Class";
		case TESWeather::kTypeID:
			return "Weather";
		case TESGlobal::kTypeID:
			return "Global";
		default:
			return nullptr;
		}
	}

	template <class T>
	inline static constexpr bool HasKeywordImpl(TESForm* a_form, BGSKeyword* a_keyword) noexcept
	{
		return static_cast<T*>(a_form)->keyword.HasKeyword(a_keyword);
	}

	bool IFormCommon::HasKeyword(TESForm* a_form, BGSKeyword* a_keyword)
	{
		switch (a_form->formType)
		{
		case TESObjectWEAP::kTypeID:
			return HasKeywordImpl<TESObjectWEAP>(a_form, a_keyword);
		case TESObjectARMO::kTypeID:
			return HasKeywordImpl<TESObjectARMO>(a_form, a_keyword);
		case TESSoulGem::kTypeID:
		case TESKey::kTypeID:
		case TESObjectMISC::kTypeID:
			return HasKeywordImpl<TESObjectMISC>(a_form, a_keyword);
		case TESObjectBOOK::kTypeID:
			return HasKeywordImpl<TESObjectBOOK>(a_form, a_keyword);
		case AlchemyItem::kTypeID:
			return HasKeywordImpl<AlchemyItem>(a_form, a_keyword);
		case IngredientItem::kTypeID:
			return HasKeywordImpl<IngredientItem>(a_form, a_keyword);
		case ScrollItem::kTypeID:
			return HasKeywordImpl<ScrollItem>(a_form, a_keyword);
		case TESAmmo::kTypeID:
			return HasKeywordImpl<TESAmmo>(a_form, a_keyword);
		case SpellItem::kTypeID:
			return HasKeywordImpl<SpellItem>(a_form, a_keyword);
		case TESRace::kTypeID:
			return HasKeywordImpl<TESRace>(a_form, a_keyword);
		case TESObjectACTI::kTypeID:
		case BGSTalkingActivator::kTypeID:
		case TESFlora::kTypeID:
		case TESFurniture::kTypeID:
			return HasKeywordImpl<TESObjectACTI>(a_form, a_keyword);
		case BGSLocation::kTypeID:
			return HasKeywordImpl<BGSLocation>(a_form, a_keyword);
		default:
			return false;
		}
	}

	bool IFormCommon::HasKeyword(
		TESForm*     a_form,
		Game::FormID a_keyword)
	{
		if (auto keyword = a_keyword.As<BGSKeyword>())
		{
			return HasKeyword(a_form, keyword);
		}
		else
		{
			return false;
		}
	}

	bool IFormCommon::HasKeyword(
		TESForm*                        a_form,
		const Data::configCachedForm_t& a_keyword)
	{
		if (auto form = a_keyword.get_form())
		{
			if (auto keyword = form->As<BGSKeyword>())
			{
				return HasKeyword(a_form, keyword);
			}
		}

		return false;
	}

}