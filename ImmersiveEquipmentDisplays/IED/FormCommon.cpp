#include "pch.h"

#include "ConfigCommon.h"
#include "FormCommon.h"
#include "LocaleData.h"

#if defined(IED_ENABLE_OUTFIT_FORM_MANAGER)
#	include "OM/PersistentOutfitFormManager.h"
#endif

namespace IED
{
	bool IFormCommon::IsValidCustomForm(
		const TESForm* a_form) noexcept
	{
		if (a_form->IsDeleted())
		{
			return false;
		}

		return IsValidCustomFormType(a_form->formType);
	}

	bool IFormCommon::IsValidCustomFormType(
		std::uint8_t a_type) noexcept
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
		case BGSHazard::kTypeID:
			return true;
		default:
			return false;
		}
	}

	bool IFormCommon::IsInventoryForm(
		const TESForm* a_form) noexcept
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
			return static_cast<const TESObjectLIGH*>(a_form)->CanCarry();
		default:
			return false;
		}
	}

	bool IFormCommon::IsInventoryFormType(
		std::uint8_t a_type) noexcept
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

	bool IFormCommon::IsValidSlotForm(
		const TESForm* a_form) noexcept
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
			return static_cast<const TESObjectARMO*>(a_form)->IsShield();
		case TESObjectLIGH::kTypeID:
			return static_cast<const TESObjectLIGH*>(a_form)->CanCarry();
		}

		return false;
	}

	bool IFormCommon::IsEquippableForm(
		const TESForm* a_form) noexcept
	{
		switch (a_form->formType)
		{
		case TESObjectLIGH::kTypeID:
			return static_cast<const TESObjectLIGH*>(a_form)->CanCarry();
		case TESObjectWEAP::kTypeID:
		case TESObjectARMO::kTypeID:
		case TESAmmo::kTypeID:
		case SpellItem::kTypeID:
		case ScrollItem::kTypeID:
		case TESShout::kTypeID:
			return true;
		default:
			return false;
		}
	}

	stl::flag<FormInfoFlags> IFormCommon::GetFormFlags(const TESForm* a_form) noexcept
	{
		stl::flag<FormInfoFlags> result{ FormInfoFlags::kNone };

		if (IsInventoryForm(a_form))
		{
			result.set(FormInfoFlags::kInventory);
		}

		if (IsValidCustomForm(a_form))
		{
			result.set(FormInfoFlags::kValidCustom);
		}

		if (IsValidSlotForm(a_form))
		{
			result.set(FormInfoFlags::kValidSlot);
		}

		return result;
	}

	constexpr const char* GetStringImpl(const BGSKeyword* a_form)  //
		noexcept(noexcept(a_form->keyword.c_str()))                //
	{
		return a_form->keyword.__ptr();
	}

	template <class T>
	constexpr const char* GetStringImpl(const T* a_form)  //
		noexcept(noexcept(a_form->editorId.c_str()))      //
		requires(
			stl::is_any_same_v<
				decltype(T::editorId),
				BSFixedString,
				BSString>)
	{
		if constexpr (std::is_same_v<decltype(T::editorId), BSFixedString>)
		{
			return a_form->editorId.__ptr();
		}
		else
		{
			return a_form->editorId.c_str();
		}
	}

	constexpr const char* GetFormNamePtr(TESForm* a_form)
	{
		switch (a_form->formType)
		{
		case BGSKeyword::kTypeID:
			return GetStringImpl(static_cast<const BGSKeyword*>(a_form));
		case TESRace::kTypeID:
			return GetStringImpl(static_cast<const TESRace*>(a_form));
		case TESQuest::kTypeID:
			return GetStringImpl(static_cast<const TESQuest*>(a_form));
		case TESObjectANIO::kTypeID:
			return GetStringImpl(static_cast<const TESObjectANIO*>(a_form));
		case TESGlobal::kTypeID:
			return GetStringImpl(static_cast<const TESGlobal*>(a_form));
		case TESIdleForm::kTypeID:
			return GetStringImpl(static_cast<const TESIdleForm*>(a_form));
		case Actor::kTypeID:
		case TESObjectREFR::kTypeID:
			return static_cast<TESObjectREFR*>(a_form)->GetReferenceName();
		default:
			return a_form->GetName();
		}
	}

	std::string IFormCommon::GetFormName(TESForm* a_form)
	{
#if defined(IED_ENABLE_OUTFIT_FORM_MANAGER)
		/*if (a_form->formType == BGSOutfit::kTypeID)
		{
			if (auto form = OM::PersistentOutfitFormManager::GetSingleton().GetHolder(a_form->formID))
			{
				return LocaleData::ToUTF8(form->get_name().__ptr());
			}
		}*/
#endif
		return LocaleData::ToUTF8(GetFormNamePtr(a_form));
	}

	bool IFormCommon::HasKeyword(
		const TESForm*    a_form,
		const BGSKeyword* a_keyword) noexcept
	{
		if (const auto keywordForm = a_form->As<const BGSKeywordForm>())
		{
			return keywordForm->HasKeyword(a_keyword);
		}
		else
		{
			return false;
		}
	}

	bool IFormCommon::HasKeyword(
		const TESForm* a_form,
		Game::FormID   a_keyword) noexcept
	{
		if (const auto keyword = a_keyword.As<const BGSKeyword>())
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
		const Data::configCachedForm_t& a_keyword) noexcept
	{
		if (const auto keyword = a_keyword.get_form<const BGSKeyword>())
		{
			return HasKeyword(a_form, keyword);
		}
		else
		{
			return false;
		}
	}

}