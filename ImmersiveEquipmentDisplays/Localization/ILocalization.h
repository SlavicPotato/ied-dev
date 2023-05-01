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

			[[nodiscard]] inline auto GetCurrentLanguageTable() const noexcept
			{
				return m_currentTable;
			}

			bool SetLanguage(const stl::fixed_string& a_lang);

			[[nodiscard]] const std::string& L(StringID a_id) const;

			template <class Te>
			[[nodiscard]] constexpr auto& L(Te a_id) const  //
				requires(std::is_same_v<std::underlying_type_t<Te>, StringID>)
			{
				return L(stl::underlying(a_id));
			}

			[[nodiscard]] constexpr const char* LS(StringID a_id) const;

			template <class Te>
			constexpr auto LS(Te a_id) const  //
				requires(std::is_same_v<std::underlying_type_t<Te>, StringID>)
			{
				return L(stl::underlying(a_id)).c_str();
			}

		private:
			LocalizationDataManager::shared_string_table m_currentTable;

		public:
			static constexpr std::size_t SC_BUFFER_SIZE = 1024 * 8;

			const std::unique_ptr<char[]> m_scBuffer1{ std::make_unique<char[]>(SC_BUFFER_SIZE) };

		private:
			[[nodiscard]] const std::string& get_default_str(StringID a_id) const;
		};
	}
}