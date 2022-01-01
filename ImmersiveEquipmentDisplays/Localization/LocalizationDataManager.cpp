#include "pch.h"

#include "LocalizationDataManager.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Localization
	{
		LocalizationDataManager LocalizationDataManager::m_Instance;

		bool LocalizationDataManager::Load(const fs::path& a_path)
		{
			try
			{
				stl::fixed_string defaultLang(DEFAULT_LANG);

				for (const auto& entry : fs::directory_iterator(a_path))
				{
					try
					{
						if (!entry.is_regular_file())
						{
							continue;
						}

						auto& path = entry.path();

						auto tmp(std::make_unique<StringTable>());
						if (!tmp->Load(path))
						{
							Error(
								"%s: %s",
								Serialization::SafeGetPath(path).c_str(),
								tmp->GetLastException().what());

							continue;
						}

						auto r = m_data.emplace(
							tmp->GetLang(),
							std::move(tmp));

						if (!r.second)
						{
							Warning(
								"%s: duplicate language key: %s",
								Serialization::SafeGetPath(path).c_str(),
								r.first->first.c_str());
						}
						else
						{
							if (r.first->first == defaultLang)
							{
								m_defaultTable = r.first->second;
							}

							Debug(
								"Loaded string table: '%s' [%zu]",
								r.first->first.c_str(),
								r.first->second->GetData().size());
						}
					}
					catch (const std::exception& e)
					{
						Error(
							"%s: exception occured while processing string table: %s",
							Serialization::SafeGetPath(entry.path()).c_str(),
							e.what());
					}
				}

				m_loaded = true;

				if (!m_defaultTable)
				{
					Warning("Default language table could not be loaded [%s]", DEFAULT_LANG);
				}

				Debug("%zu string table(s) loaded", m_data.size());

				return true;
			}
			catch (const std::exception& e)
			{
				m_lastException = e;
				return false;
			}
		}

		bool LocalizationDataManager::LookupTable(
			const stl::fixed_string& a_lang,
			shared_string_table& a_out) const
		{
			auto it = m_data.find(a_lang);
			if (it != m_data.end())
			{
				a_out = it->second;
				return true;
			}
			else
			{
				return false;
			}
		}

	}
}
