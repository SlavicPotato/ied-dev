#pragma once

namespace IED
{
	class LocaleData
	{
	public:
		static constexpr auto CODEPAGE_ENGLISH   = "en_US.CP1252";
		static constexpr auto CODEPAGE_FRENCH    = "fr_FR.UTF-8";
		static constexpr auto CODEPAGE_POLISH    = "pl_PL.UTF-8";
		static constexpr auto CODEPAGE_CZECH     = "cs_CS.UTF-8";
		static constexpr auto CODEPAGE_DANISH    = "da_DA.UTF-8";
		static constexpr auto CODEPAGE_FINNISH   = "fi_FI.UTF-8";
		static constexpr auto CODEPAGE_GERMAN    = "de_DE.UTF-8";
		static constexpr auto CODEPAGE_GREEK     = "el_EL.UTF-8";
		static constexpr auto CODEPAGE_ITALIAN   = "it_IT.UTF-8";
		static constexpr auto CODEPAGE_JAPANESE  = "ja_JA.UTF-8";
		static constexpr auto CODEPAGE_NORWEGIAN = "no_NO.UTF-8";
		static constexpr auto CODEPAGE_PORTUGESE = "pt_BR.UTF-8";
		static constexpr auto CODEPAGE_SPANISH   = "es_ES.UTF-8";
		static constexpr auto CODEPAGE_SWEDISH   = "sv_SE.UTF-8";
		static constexpr auto CODEPAGE_TURKISH   = "tr_TR.UTF-8";
		static constexpr auto CODEPAGE_RUSSIAN   = "ru_RU.UTF-8";
		static constexpr auto CODEPAGE_CHINESE   = "zh_CN.UTF-8";
		static constexpr auto CODEPAGE_HUNGARIAN = "hu_HU.UTF-8";
		static constexpr auto CODEPAGE_ARABIC    = "ar_AE.UTF-8";

		static constexpr auto DEFAULT_CODEPAGE = CODEPAGE_ENGLISH;

		inline static auto GetSingleton() noexcept
		{
			return m_Instance.get();
		}

		static void CreateSingleton();

		constexpr auto& GetCurrent() const noexcept
		{
			return m_current;
		}

		void SetFromLang(const char* a_lang);

		static std::string ToUTF8(const char* a_in) noexcept;

	private:
		std::optional<std::locale> m_current;

		static std::unique_ptr<LocaleData> m_Instance;
	};
}