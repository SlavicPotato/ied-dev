#include "pch.h"

#include "ConfigCommon.h"
#include "FormCommon.h"
#include "LocaleData.h"

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

	stl::flag<FormInfoFlags> IFormCommon::GetFormFlags(TESForm* a_form) noexcept
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

	constexpr const char* GetKeywordString(const BGSKeyword* a_form) noexcept
	{
		return a_form->keyword.c_str();
	}

	template <class T>
	constexpr const char* GetEditorID(const T* a_form) noexcept
	{
		return a_form->editorId.c_str();
	}

	constexpr const char* GetFormNamePtr(TESForm* a_form) noexcept
	{
		switch (a_form->formType)
		{
		case BGSKeyword::kTypeID:
			return GetKeywordString(static_cast<const BGSKeyword*>(a_form));
		case TESRace::kTypeID:
			return GetEditorID(static_cast<const TESRace*>(a_form));
		case TESQuest::kTypeID:
			return GetEditorID(static_cast<const TESQuest*>(a_form));
		case TESObjectANIO::kTypeID:
			return GetEditorID(static_cast<const TESObjectANIO*>(a_form));
		case TESGlobal::kTypeID:
			return GetEditorID(static_cast<const TESGlobal*>(a_form));
		case TESIdleForm::kTypeID:
			return GetEditorID(static_cast<const TESIdleForm*>(a_form));
		case Actor::kTypeID:
		case TESObjectREFR::kTypeID:
			return static_cast<TESObjectREFR*>(a_form)->GetReferenceName();
		default:
			return a_form->GetName();
		}
	}

	std::string IFormCommon::GetFormName(TESForm* a_form)
	{
		return LocaleData::ToUTF8(GetFormNamePtr(a_form));
	}

	bool IFormCommon::HasKeyword(
		const TESForm*    a_form,
		const BGSKeyword* a_keyword) noexcept
	{
		if (const auto keywordForm = a_form->As<BGSKeywordForm>())
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