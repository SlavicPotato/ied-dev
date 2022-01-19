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
		const char* codepage;

		if (!_stricmp(a_lang, "french"))
		{
			codepage = CODEPAGE_FRENCH;
		}
		else if (!_stricmp(a_lang, "polish"))
		{
			codepage = CODEPAGE_POLISH;
		}
		else if (!_stricmp(a_lang, "czech"))
		{
			codepage = CODEPAGE_CZECH;
		}
		else if (!_stricmp(a_lang, "danish"))
		{
			codepage = CODEPAGE_DANISH;
		}
		else if (!_stricmp(a_lang, "finnish"))
		{
			codepage = CODEPAGE_FINNISH;
		}
		else if (!_stricmp(a_lang, "german"))
		{
			codepage = CODEPAGE_GERMAN;
		}
		else if (!_stricmp(a_lang, "greek"))
		{
			codepage = CODEPAGE_GREEK;
		}
		else if (!_stricmp(a_lang, "italian"))
		{
			codepage = CODEPAGE_ITALIAN;
		}
		else if (!_stricmp(a_lang, "japanese"))
		{
			codepage = CODEPAGE_JAPANESE;
		}
		else if (!_stricmp(a_lang, "norwegian"))
		{
			codepage = CODEPAGE_NORWEGIAN;
		}
		else if (!_stricmp(a_lang, "portugese"))
		{
			codepage = CODEPAGE_PORTUGESE;
		}
		else if (!_stricmp(a_lang, "spanish"))
		{
			codepage = CODEPAGE_SPANISH;
		}
		else if (!_stricmp(a_lang, "swedish"))
		{
			codepage = CODEPAGE_SWEDISH;
		}
		else if (!_stricmp(a_lang, "turkish"))
		{
			codepage = CODEPAGE_TURKISH;
		}
		else if (!_stricmp(a_lang, "russian"))
		{
			codepage = CODEPAGE_RUSSIAN;
		}
		else if (!_stricmp(a_lang, "chinese"))
		{
			codepage = CODEPAGE_CHINESE;
		}
		else if (!_stricmp(a_lang, "hungarian"))
		{
			codepage = CODEPAGE_HUNGARIAN;
		}
		else if (!_stricmp(a_lang, "arabic"))
		{
			codepage = CODEPAGE_ARABIC;
		}
		else
		{
			codepage = CODEPAGE_ENGLISH;
		}

		try
		{
			boost::locale::generator gen;
			m_current = gen.generate(codepage);

			_DMESSAGE("set cp: %s", codepage);

		}
		catch (...)
		{
		}
	}
}