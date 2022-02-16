#pragma once

namespace IED
{
	class IGlyphData
	{
	public:
		static const ImWchar* get_glyph_ranges_latin_full() noexcept;
		static const ImWchar* get_glyph_ranges_greek() noexcept;
		static const ImWchar* get_glyph_ranges_arabic() noexcept;
		static const ImWchar* get_glyph_ranges_cyrilic() noexcept;
		static const ImWchar* get_glyph_ranges_arrows() noexcept;
		static const ImWchar* get_glyph_ranges_common() noexcept;
	};
}