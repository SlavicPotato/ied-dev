#include "pch.h"

#include "ILocalization.h"

namespace IED
{
	namespace Localization
	{
		ILocalization::ILocalization(
			const LocalizationDataManager::shared_string_table& a_table) :
			m_currentTable(a_table)
		{
		}

		bool ILocalization::SetLanguage(const stl::fixed_string& a_lang)
		{
			return LocalizationDataManager::GetSingleton().LookupTable(a_lang, m_currentTable);
		}

		const std::string& ILocalization::L(StringID a_id) const
		{
			if (m_currentTable)
			{
				auto& data = m_currentTable->GetData();

				auto it = data.find(a_id);
				return it != data.end() ?
				           it->second :
                           get_default_str(a_id);
			}
			else
			{
				return get_default_str(a_id);
			}
		}

		const std::string& ILocalization::get_default_str(StringID a_id) const
		{
			if (auto& def = LocalizationDataManager::GetSingleton().GetDefaultTable())
			{
				auto& data = def->GetData();
				if (auto it = data.find(a_id); it != data.end())
				{
					return it->second;
				}
			}

			auto& empty = LocalizationDataManager::GetSingleton().GetEmptyTable();

			auto r = empty->try_emplace(a_id);
			if (r.second)
			{
				r.first->second = '$';
				r.first->second += std::to_string(a_id);
			}

			return r.first->second;
		}
	}
}