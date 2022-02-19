#include "pch.h"

#include "IForm.h"

#include "IFormDatabase.h"

namespace IED
{
	std::uint32_t IForm::GetFormExtraType(TESForm* a_form)
	{
		if (auto armor = RTTI<TESObjectARMO>()(a_form))
		{
			if (!armor->IsShield())
			{
				return IFormDatabase::EXTRA_TYPE_ARMOR;
			}
		}

		return 0;
	}

	auto IForm::LookupFormInfo(
		Game::FormID a_form)
		-> info_result
	{
		auto form = a_form.Lookup();
		if (!form)
		{
			return nullptr;
		}

		if (form->IsDeleted())
		{
			return nullptr;
		}

		TESForm* base;

		if (auto ref = RTTI<TESObjectREFR>()(form))
		{
			base = ref->baseForm;
		}
		else
		{
			base = nullptr;
		}

		return std::make_unique<formInfoResult_t>(
			form,
			base ?
				std::make_unique<formInfo_t>(base) :
                nullptr);
	}

	formInfo_t::formInfo_t(TESForm* a_form) :
		id(a_form->formID),
		type(a_form->formType),
		flags(IFormCommon::GetFormFlags(a_form)),
		name(IFormCommon::GetFormName(a_form)),
		extraType(IForm::GetFormExtraType(a_form))
	{
	}

}