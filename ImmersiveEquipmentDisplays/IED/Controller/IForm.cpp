#include "pch.h"

#include "IForm.h"

#include "IFormDatabase.h"

namespace IED
{
	std::uint32_t IForm::GetFormExtraType(const TESForm* a_form) noexcept
	{
		switch (a_form->formType)
		{
		case TESObjectARMO::kTypeID:
			if (!static_cast<const TESObjectARMO*>(a_form)->IsShield())
			{
				return IFormDatabase::EXTRA_TYPE_ARMOR;
			}
			break;
		case TESObjectLIGH::kTypeID:
			if (!static_cast<const TESObjectLIGH*>(a_form)->CanCarry())
			{
				return IFormDatabase::EXTRA_TYPE_LIGHT;
			}
			break;
		}

		return 0;
	}

	auto IForm::LookupFormInfo(
		Game::FormID a_form)
		-> info_result
	{
		const auto form = a_form.Lookup();
		if (!form)
		{
			return nullptr;
		}

		/*if (form->IsDeleted())
		{
			return nullptr;
		}*/

		const auto ref  = ::RTTI<const TESObjectREFR>()(form);
		auto       base = ref ? ref->baseForm : nullptr;

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
			EXTRA_FORM_INFO_CREATE(ExtraFormInfoTESObjectARMA);
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
		weaponType(a_form->type()),
		flags(a_form->weaponData.flags),
		flags2(a_form->weaponData.flags2)
	{
	}

	ExtraFormInfoTESAmmo::ExtraFormInfoTESAmmo(
		const FORM_TYPE* a_form) :
		BaseExtraFormInfo(a_form),
		flags(a_form->settings.flags)
	{
	}

	ExtraFormInfoTESObjectARMA::ExtraFormInfoTESObjectARMA(
		const FORM_TYPE* a_form) :
		BaseExtraFormInfo(a_form),
		weaponAdjust(a_form->data.weaponAdjust)
	{
	}

}