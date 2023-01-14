#pragma once

#include "StringTable.h"

namespace IED
{
	namespace Localization
	{
		class LocalizationDataManager : ILog
		{
		public:
			static constexpr auto DEFAULT_LANG = "English";

			using shared_string_table = std::shared_ptr<const StringTable>;

			bool Load(const fs::path& a_path);

			[[nodiscard]] static constexpr auto& GetSingleton() noexcept
			{
				return m_Instance;
			}

			[[nodiscard]] constexpr auto& GetData() const noexcept
			{
				return m_data;
			}

			[[nodiscard]] inline auto& GetEmptyTable() const noexcept
			{
				if (!m_emptyTable)
				{
					m_emptyTable = std::make_unique<StringTable::container_type>();
				}

				return m_emptyTable;
			}

			[[nodiscard]] constexpr auto& GetDefaultTable() const noexcept
			{
				return m_defaultTable;
			}

			[[nodiscard]] constexpr auto IsLoaded() const noexcept
			{
				return m_loaded;
			}

			[[nodiscard]] constexpr auto& GetLastException() const noexcept
			{
				return m_lastException;
			}

			[[nodiscard]] bool LookupTable(
				const stl::fixed_string& a_lang,
				shared_string_table&     a_out) const;

			FN_NAMEPROC("LocalizationDataManager");

		private:
			stl::unordered_map<stl::fixed_string, std::shared_ptr<StringTable>> m_data;

			mutable std::unique_ptr<StringTable::container_type> m_emptyTable;
			shared_string_table                                  m_defaultTable;

			bool m_loaded{ false };

			except::descriptor m_lastException;

			static LocalizationDataManager m_Instance;
		};
	}
}
