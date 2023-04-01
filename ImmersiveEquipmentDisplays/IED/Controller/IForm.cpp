#include "pch.h"

#include "IForm.h"

#include "IFormDatabase.h"

namespace IED
{
	std::uint32_t IForm::GetFormExtraType(TESForm* a_form)
	{
		if (auto armor = ::RTTI<TESObjectARMO>()(a_form))
		{
			if (!armor->IsShield())
			{
				return IFormDatabase::EXTRA_TYPE_ARMOR;
			}
		}
		else if (auto light = ::RTTI<TESObjectLIGH>()(a_form))
		{
			if (!light->CanCarry())
			{
				return IFormDatabase::EXTRA_TYPE_LIGHT;
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

		/*if (form->IsDeleted())
		{
			return nullptr;
		}*/

		auto ref  = ::RTTI<TESObjectREFR>()(form);
		auto base = ref ? ref->baseForm : nullptr;

		return std::make_unique<FormInfoResult>(
			form,
			base ?
				std::make_unique<FormInfoData>(base) :
				nullptr);
	}

	FormInfoData::FormInfoData(TESForm* a_form) :
		id(a_form->formID),
		type(a_form->formType),
		formFlags(a_form->flags),
		flags(IFormCommon::GetFormFlags(a_form)),
		name(IFormCommon::GetFormName(a_form)),
		extraType(IForm::GetFormExtraType(a_form)),
		extraInfo(BaseExtraFormInfo::Create(a_form))
	{
	}

#define EXTRA_FORM_INFO_CREATE(cl) \
	case cl::FORM_TYPE::kTypeID:   \
		return std::make_unique<cl>(static_cast<const cl::FORM_TYPE*>(a_form));

	std::unique_ptr<BaseExtraFormInfo> BaseExtraFormInfo::Create(const TESForm* a_form)
	{
		switch (a_form->formType)
		{
			EXTRA_FORM_INFO_CREATE(ExtraFormInfoTESWeather);
			EXTRA_FORM_INFO_CREATE(ExtraFormInfoTESObjectWEAP);
			EXTRA_FORM_INFO_CREATE(ExtraFormInfoTESAmmo);
		default:
			return nullptr;
		}
	}

#undef EXTRA_FORM_INFO_CREATE

	ExtraFormInfoTESWeather::ExtraFormInfoTESWeather(
		const FORM_TYPE* a_form) :
		BaseExtraFormInfo(a_form),
		classFlags(
			static_cast<WeatherClassificationFlags>(
				a_form->data.flags & RE::TESWeather::WeatherDataFlag::kWeatherMask))
	{
	}

	ExtraFormInfoTESObjectWEAP::ExtraFormInfoTESObjectWEAP(
		const FORM_TYPE* a_form) :
		BaseExtraFormInfo(a_form),
		weaponType(a_form->type())
	{
	}

	ExtraFormInfoTESAmmo::ExtraFormInfoTESAmmo(
		const FORM_TYPE* a_form) :
		BaseExtraFormInfo(a_form),
		isBolt(a_form->isBolt())
	{
	}

}