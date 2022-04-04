#pragma once

#include "Common.h"

#include "Fonts/FontInfo.h"

namespace IED
{
	namespace Localization
	{
		class StringTable
		{
		public:
			using container_type = stl::unordered_map<StringID, std::string>;

			struct data_storage_type
			{
				stl::fixed_string                lang;
				container_type                   data;
				std::shared_ptr<fontGlyphData_t> glyph_data;
			};

			bool Load(const fs::path& a_path);

			inline constexpr const auto& GetData() const noexcept
			{
				return m_data.data;
			}

			inline constexpr const auto& GetLang() const noexcept
			{
				return m_data.lang;
			}

			inline constexpr const auto& GetGlyphData() const noexcept
			{
				return m_data.glyph_data;
			}

			inline constexpr const auto& GetLastException() const noexcept
			{
				return m_lastException;
			}

		private:
			data_storage_type m_data;

			except::descriptor m_lastException;
		};
	}
}