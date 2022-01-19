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
		
	}

	std::string LocaleData::ToUTF8(const char* a_in)
	{
		if (m_Instance)
		{
			return m_Instance->ToUTF8Impl(a_in);
		}
		else
		{
			return a_in;
		}
	}

	std::string LocaleData::ToUTF8Impl(const char* a_in)
	{
		try
		{
			using namespace boost::locale;

			if (m_current)
			{
				return conv::to_utf<char>(a_in, *m_current, conv::skip);
			}
		}
		catch (...)
		{
		}

		return a_in;
	}

}