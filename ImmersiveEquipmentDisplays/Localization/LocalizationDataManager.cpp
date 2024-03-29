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
				const stl::fixed_string defaultLang(DEFAULT_LANG);

				const fs::path allowedExt{ ".json" };

				for (const auto& entry : fs::directory_iterator(a_path))
				{
					try
					{
						if (!entry.is_regular_file())
						{
							continue;
						}

						auto& path = entry.path();

						if (!path.has_extension() ||
						    path.extension() != allowedExt)
						{
							continue;
						}

						auto tmp = stl::make_smart_for_overwrite<StringTable>();
						if (!tmp->Load(path))
						{
							Exception(
								tmp->GetLastException(),
								__FUNCTION__ ": [%s]",
								Serialization::SafeGetPath(path).c_str());

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

				if (m_data.empty())
				{
					Warning("No string tables were loaded");
				}
				else
				{
					if (!m_defaultTable)
					{
						Warning("Default language string table could not be loaded [%s]", DEFAULT_LANG);
					}

					Debug("%zu string table(s) loaded", m_data.size());
				}

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
			shared_string_table&     a_out) const
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
