#pragma once

#include "Common.h"

#include "Fonts/FontInfo.h"

namespace IED
{
	namespace Localization
	{
		class StringTable :
			public stl::intrusive_ref_counted
		{
		public:
			SKMP_REDEFINE_NEW_PREF();

			using container_type = stl::unordered_map<StringID, std::string>;

			struct table_data
			{
				stl::fixed_string              lang;
				container_type                 data;
				stl::smart_ptr<FontGlyphData>  glyph_data;
			};

			bool Load(const fs::path& a_path);

			constexpr const auto& GetData() const noexcept
			{
				return m_data.data;
			}

			constexpr const auto& GetLang() const noexcept
			{
				return m_data.lang;
			}

			constexpr const auto& GetGlyphData() const noexcept
			{
				return m_data.glyph_data;
			}

			constexpr const auto& GetLastException() const noexcept
			{
				return m_lastException;
			}

		private:
			table_data m_data;

			except::descriptor m_lastException;
		};
	}
}