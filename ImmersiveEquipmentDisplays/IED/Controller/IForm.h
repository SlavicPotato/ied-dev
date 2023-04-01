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

		const WeatherClassificationFlags classFlags;
	};

	class ExtraFormInfoTESObjectWEAP :
		public BaseExtraFormInfo
	{
	public:
		using FORM_TYPE = TESObjectWEAP;

		ExtraFormInfoTESObjectWEAP(const FORM_TYPE* a_form);

		const WEAPON_TYPE weaponType;
	};
	
	class ExtraFormInfoTESAmmo :
		public BaseExtraFormInfo
	{
	public:
		using FORM_TYPE = TESAmmo;

		ExtraFormInfoTESAmmo(const FORM_TYPE* a_form);

		const bool isBolt;
	};

	struct FormInfoData
	{
		FormInfoData(TESForm* a_form);

		Game::FormID             id;
		std::uint8_t             type;
		std::uint32_t            formFlags;
		stl::flag<FormInfoFlags> flags;
		std::string              name;
		std::uint32_t            extraType;

		std::unique_ptr<BaseExtraFormInfo> extraInfo;
	};

	struct FormInfoResult
	{
		FormInfoData                  form;
		std::unique_ptr<FormInfoData> base;

		[[nodiscard]] constexpr const auto& get_base() const noexcept
		{
			return base ? *base : form;
		}

		[[nodiscard]] inline bool is_ref() const noexcept
		{
			return static_cast<bool>(base.get());
		}
	};

	class IForm
	{
	public:
		using info_result               = std::unique_ptr<FormInfoResult>;
		using form_lookup_result_func_t = std::function<void(info_result)>;

		static std::uint32_t GetFormExtraType(TESForm* a_form);
		static info_result   LookupFormInfo(Game::FormID a_form);
	};
}