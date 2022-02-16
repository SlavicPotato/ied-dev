#pragma once

#include "IFormDatabase.h"

namespace IED
{
	struct formInfo_t
	{
		formInfo_t() = delete;
		formInfo_t(TESForm* a_form);

		Game::FormID             id;
		std::uint8_t             type;
		stl::flag<FormInfoFlags> flags;
		std::string              name;
		std::uint32_t            extraType;
	};

	struct formInfoResult_t
	{
		formInfo_t                  form;
		std::unique_ptr<formInfo_t> base;

		[[nodiscard]] inline constexpr const auto& get_base() const noexcept
		{
			return base ? *base : form;
		}

		[[nodiscard]] inline bool is_ref() const noexcept
		{
			return static_cast<bool>(base.get());
		}
	};

	class IForm : public IFormDatabase
	{
	public:
		using info_result               = std::unique_ptr<formInfoResult_t>;
		using form_lookup_result_func_t = std::function<void(info_result)>;

		static std::uint32_t GetFormExtraType(TESForm* a_form);

	protected:
		static info_result LookupFormInfo(Game::FormID a_form);
	};
}