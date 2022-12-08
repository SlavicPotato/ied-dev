#pragma once

#include "LocalizationDataManager.h"

#include "CommonStrings.h"

namespace IED
{
	namespace Localization
	{
		class ILocalization
		{
		public:
			ILocalization() = default;
			ILocalization(const LocalizationDataManager::shared_string_table& a_table);

			[[nodiscard]] inline constexpr const auto& GetCurrentLanguageTable() const noexcept
			{
				return m_currentTable;
			}

			bool SetLanguage(const stl::fixed_string& a_lang);

			[[nodiscard]] const std::string& L(StringID a_id) const;

			template <
				class Te,
				class = std::enable_if_t<
					std::is_enum_v<Te> &&
					std::is_same_v<std::underlying_type_t<Te>, StringID>>>
			[[nodiscard]] inline constexpr auto& L(Te a_id) const
			{
				return L(stl::underlying(a_id));
			}

			[[nodiscard]] inline constexpr auto LS(StringID a_id) const
			{
				return L(a_id).c_str();
			}

			template <
				class Te,
				class = std::enable_if_t<
					std::is_enum_v<Te> &&
					std::is_same_v<std::underlying_type_t<Te>, StringID>>>
			inline constexpr auto LS(Te a_id) const
			{
				return L(stl::underlying(a_id)).c_str();
			}

		private:
			LocalizationDataManager::shared_string_table m_currentTable;

		public:
			char m_scBuffer1[2048]{ 0 };

		private:
			[[nodiscard]] const std::string& get_default_str(StringID a_id) const;
		};
	}
}