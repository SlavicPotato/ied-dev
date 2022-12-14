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
		case BGSMaterialObject::kTypeID:
		case BGSProjectile::kTypeID:
		case TESObjectARMO::kTypeID:
			return true;
		case TESObjectLIGH::kTypeID:
			return static_cast<TESObjectLIGH*>(a_form)->CanCarry();
		default:
			return false;
		}
	}

	bool IFormCommon::IsValidCustomFormType(std::uint8_t a_type)
	{
		switch (a_type)
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
		case BGSMaterialObject::kTypeID:
		case BGSProjectile::kTypeID:
		case TESObjectARMO::kTypeID:
		case TESObjectLIGH::kTypeID:
			return true;
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
	inline static constexpr const char* GetFullName(const TESForm* a_form)  //
		requires(std::is_convertible_v<T*, TESFullName*>)
	{
		return static_cast<const T*>(a_form)->GetFullName();
	}

	inline static constexpr const char* GetKeywordString(const BGSKeyword* a_form)
	{
		return a_form->keyword.c_str();
	}

	template <class T>
	inline static constexpr const char* GetEditorID(const T* a_form)
	{
		return a_form->editorId.c_str();
	}

	inline static constexpr const char* GetReferenceName(TESObjectREFR* a_form)
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
		case TESAmmo::kTypeID:
			return GetFullName<TESAmmo>(a_form);
		case TESObjectACTI::kTypeID:
			return GetFullName<TESObjectACTI>(a_form);
		case BGSTalkingActivator::kTypeID:
			return GetFullName<BGSTalkingActivator>(a_form);
		case TESFlora::kTypeID:
			return GetFullName<TESFlora>(a_form);
		case TESFurniture::kTypeID:
			return GetFullName<TESFurniture>(a_form);
		case BGSHeadPart::kTypeID:
			return GetFullName<BGSHeadPart>(a_form);
		case BGSKeyword::kTypeID:
			return GetKeywordString(static_cast<const BGSKeyword*>(a_form));
		case TESRace::kTypeID:
			return GetEditorID(static_cast<const TESRace*>(a_form));
		case TESQuest::kTypeID:
			return GetEditorID(static_cast<const TESQuest*>(a_form));
		case TESObjectANIO::kTypeID:
			return GetEditorID(static_cast<const TESObjectANIO*>(a_form));
		case TESObjectDOOR::kTypeID:
			return GetFullName<TESObjectDOOR>(a_form);
		case BGSExplosion::kTypeID:
			return GetFullName<BGSExplosion>(a_form);
		case BGSProjectile::kTypeID:
			return GetFullName<BGSProjectile>(a_form);
		case BGSLocation::kTypeID:
			return GetFullName<BGSLocation>(a_form);
		case TESWorldSpace::kTypeID:
			return GetFullName<TESWorldSpace>(a_form);
		case TESShout::kTypeID:
			return GetFullName<TESShout>(a_form);
		case TESFaction::kTypeID:
			return GetFullName<TESFaction>(a_form);
		case EffectSetting::kTypeID:
			return GetFullName<EffectSetting>(a_form);
		case EnchantmentItem::kTypeID:
		case SpellItem::kTypeID:
		case ScrollItem::kTypeID:
		case AlchemyItem::kTypeID:
		case IngredientItem::kTypeID:
		case MagicItem::kTypeID:
			return GetFullName<MagicItem>(a_form);
		case TESClass::kTypeID:
			return GetFullName<TESClass>(a_form);
		case TESObjectCELL::kTypeID:
			return GetFullName<TESObjectCELL>(a_form);
		case TESGlobal::kTypeID:
			return GetEditorID(static_cast<TESGlobal*>(a_form));
		case TESIdleForm::kTypeID:
			return GetEditorID(static_cast<TESIdleForm*>(a_form));
		case Actor::kTypeID:
		case TESObjectREFR::kTypeID:
			return GetReferenceName(static_cast<TESObjectREFR*>(a_form));
		default:
			return nullptr;
		}
	}

	std::string IFormCommon::GetFormName(TESForm* a_form)
	{
		return LocaleData::ToUTF8(GetFormNamePtr(a_form));
	}

	bool IFormCommon::HasKeyword(
		const TESForm*    a_form,
		const BGSKeyword* a_keyword)
	{
		switch (a_form->formType)
		{
		case TESObjectWEAP::kTypeID:
			return FormHasKeywordImpl<TESObjectWEAP>(a_form, a_keyword);
		case TESObjectARMO::kTypeID:
			return FormHasKeywordImpl<TESObjectARMO>(a_form, a_keyword);
		case TESSoulGem::kTypeID:
		case TESKey::kTypeID:
		case TESObjectMISC::kTypeID:
			return FormHasKeywordImpl<TESObjectMISC>(a_form, a_keyword);
		case TESObjectBOOK::kTypeID:
			return FormHasKeywordImpl<TESObjectBOOK>(a_form, a_keyword);
		case TESAmmo::kTypeID:
			return FormHasKeywordImpl<TESAmmo>(a_form, a_keyword);
		case TESRace::kTypeID:
			return FormHasKeywordImpl<TESRace>(a_form, a_keyword);
		case TESObjectACTI::kTypeID:
			return FormHasKeywordImpl<TESObjectACTI>(a_form, a_keyword);
		case BGSTalkingActivator::kTypeID:
			return FormHasKeywordImpl<BGSTalkingActivator>(a_form, a_keyword);
		case TESFlora::kTypeID:
			return FormHasKeywordImpl<TESFlora>(a_form, a_keyword);
		case TESFurniture::kTypeID:
			return FormHasKeywordImpl<TESFurniture>(a_form, a_keyword);
		case BGSLocation::kTypeID:
			return FormHasKeywordImpl<BGSLocation>(a_form, a_keyword);
		case TESNPC::kTypeID:
			return FormHasKeywordImpl<TESNPC>(a_form, a_keyword);
		case EffectSetting::kTypeID:
			return FormHasKeywordImpl<EffectSetting>(a_form, a_keyword);
		case EnchantmentItem::kTypeID:
		case SpellItem::kTypeID:
		case ScrollItem::kTypeID:
		case AlchemyItem::kTypeID:
		case IngredientItem::kTypeID:
		case MagicItem::kTypeID:
			return FormHasKeywordImpl<MagicItem>(a_form, a_keyword);
		default:
			return false;
		}
	}

	bool IFormCommon::HasKeyword(
		const TESForm* a_form,
		Game::FormID   a_keyword)
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
		const TESForm*                  a_form,
		const Data::configCachedForm_t& a_keyword)
	{
		if (const auto* const keyword = a_keyword.get_form<BGSKeyword>())
		{
			return HasKeyword(a_form, keyword);
		}
		else
		{
			return false;
		}
	}

}