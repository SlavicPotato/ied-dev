#include "pch.h"

#include "LocalizationDataManager.h"

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

						std::string fn;
						try
						{
							fn = path.string();
						}
						catch (const std::exception&)
						{
							fn = ".";
						}

						StringTable tmp;
						if (!tmp.Load(path))
						{
							Error(
								"%s: %s",
								fn.c_str(),
								tmp.GetLastException().what());

							continue;
						}

						auto name(tmp.GetLang());

						auto r = m_data.emplace(
							std::move(name),
							std::make_unique<StringTable>(std::move(tmp)));

						if (!r.second)
						{
							Warning(
								"%s: duplicate laIEDage key: %s",
								fn.c_str(),
								name.c_str());
						}
						else
						{
							if (r.first->first == defaultLang) {
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
							"Exception occured while processing string table: %s",
							e.what());
					}
				}

				m_loaded = true;

				Debug("Loaded %zu string table(s)", m_data.size());

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
			shared_string_table& a_out)
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
