#include "pch.h"

#include "LocaleData.h"

namespace IED
{
	std::unique_ptr<LocaleData> LocaleData::m_Instance;

	void LocaleData::CreateSingleton()
	{
		m_Instance = std::make_unique<LocaleData>();
	}

	void LocaleData::SetFromLang(const char* a_lang)
	{
		if (!_stricmp(a_lang, "english"))
		{
			try
			{
				boost::locale::generator gen;
				m_current = gen.generate(CODEPAGE_ENGLISH);
			}
			catch (...)
			{
			}
		}

		// in SE all other language strings are UTF-8 encoded, assume no conversion is necessary
	}

	std::string LocaleData::ToUTF8(const char* a_in)
	{
		if (!a_in)
		{
			return {};
		}

		if (m_Instance)
		{
			try
			{
				if (m_Instance->m_current)
				{
					using namespace boost::locale;

					return conv::to_utf<char>(
						a_in,
						*m_Instance->m_current,
						conv::skip);
				}
			}
			catch (...)
			{
			}
		}

		return a_in;
	}

}