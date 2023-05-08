#pragma once

#include "IED/FormCommon.h"

#include "IED/WeatherClassificationFlags.h"

namespace IED
{
	class BaseExtraFormInfo
	{
	public:
		explicit constexpr BaseExtraFormInfo(
			const TESForm* a_form) noexcept :
			type(a_form->formType)
		{
		}

		virtual ~BaseExtraFormInfo() noexcept = default;

		[[nodiscard]] static std::unique_ptr<BaseExtraFormInfo> Create(const TESForm* a_form);

		const std::uint8_t type;
	};

	class ExtraFormInfoTESWeather :
		public BaseExtraFormInfo
	{
	public:
		using FORM_TYPE = RE::TESWeather;

		ExtraFormInfoTESWeather(const FORM_TYPE* a_form);

		WeatherClassificationFlags classFlags;
	};

	class ExtraFormInfoTESObjectWEAP :
		public BaseExtraFormInfo
	{
	public:
		using FORM_TYPE = TESObjectWEAP;

		ExtraFormInfoTESObjectWEAP(const FORM_TYPE* a_form);

		WEAPON_TYPE                           weaponType;
		stl::flag<TESObjectWEAP::Data::Flag>  flags;
		stl::flag<TESObjectWEAP::Data::Flag2> flags2;
	};

	class ExtraFormInfoTESAmmo :
		public BaseExtraFormInfo
	{
	public:
		using FORM_TYPE = TESAmmo;

		ExtraFormInfoTESAmmo(const FORM_TYPE* a_form);

		stl::flag<AMMO_DATA::Flag> flags;
	};
	
	class ExtraFormInfoTESObjectARMA :
		public BaseExtraFormInfo
	{
	public:
		using FORM_TYPE = TESObjectARMA;

		ExtraFormInfoTESObjectARMA(const FORM_TYPE* a_form);

		float weaponAdjust;
	};

	struct FormInfoData
	{
		FormInfoData(TESForm* a_form);

		Game::FormID             id;
		std::uint32_t            extraType;
		std::uint32_t            formFlags;
		std::uint8_t             type;
		stl::flag<FormInfoFlags> flags;
		std::string              name;

		std::unique_ptr<const BaseExtraFormInfo> extraInfo;
	};

	struct FormInfoResult
	{
		const FormInfoData                  form;
		std::unique_ptr<const FormInfoData> base;

		[[nodiscard]] constexpr const auto& get_base() const noexcept
		{
			return base ? *base : form;
		}

		[[nodiscard]] SKMP_143_CONSTEXPR bool is_ref() const noexcept
		{
			return static_cast<bool>(base);
		}
	};

	class IForm
	{
	public:
		using info_result               = std::unique_ptr<FormInfoResult>;
		using form_lookup_result_func_t = std::function<void(info_result)>;

		static std::uint32_t GetFormExtraType(const TESForm* a_form) noexcept;
		static info_result   LookupFormInfo(Game::FormID a_form);
	};
}