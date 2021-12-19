#include "pch.h"

#include "IGlyphData.h"

namespace IED
{
	const ImWchar* IGlyphData::get_glyph_ranges_latin_full() noexcept
	{
		static const ImWchar ranges[] = {
			0x0020,
			0x02AF,  // Basic Latin, Latin-1 Supplement, Latin Extended-A, Latin Extended-B, IPA Extensions
			0x2C60,
			0x2C7F,  // Latin Extended-C
			0xAB30,
			0xAB6F,  // Latin Extended-E
			0x1E00,
			0x1EFF,  // Latin Extended Additional
			0,
		};
		return ranges;
	}

	const ImWchar* IGlyphData::get_glyph_ranges_greek() noexcept
	{
		static const ImWchar ranges[] = {
			0x0020,
			0x007F,  // Basic Latin
			0x0370,
			0x03FF,  // Greek and Coptic
			0x1F00,
			0x1FFF,  // Greek Extended
			0,
		};
		return ranges;
	}

	const ImWchar* IGlyphData::get_glyph_ranges_arabic() noexcept
	{
		static const ImWchar ranges[] = {
			0x0020,
			0x007F,  // Basic Latin
			0x0600,
			0x06FF,  // Arabic
			0x0750,
			0x077F,  // Arabic Supplement
			0x0870,
			0x089F,  // Arabic Extended-B
			0x08A0,
			0x08FF,  // Arabic Extended-B
			0,
		};
		return ranges;
	}
	const ImWchar* IGlyphData::get_glyph_ranges_cyrilic() noexcept
	{
		static const ImWchar ranges[] = {
			0x0020,
			0x00FF,  // Basic Latin + Latin Supplement
			0x0400,
			0x052F,  // Cyrillic + Cyrillic Supplement
			0x2DE0,
			0x2DFF,  // Cyrillic Extended-A
			0xA640,
			0xA69F,  // Cyrillic Extended-B
			0x1C80,
			0x1C8F,  // Cyrillic Extended-C
			0,
		};
		return ranges;
	}
	const ImWchar* IGlyphData::get_glyph_ranges_arrows() noexcept
	{
		static const ImWchar ranges[] = {
			0x0020,
			0x00FF,  // Basic Latin + Latin Supplement
			0x2190,
			0x21FF,  // Arrows
			0x27F0,
			0x27FF,  // Supplemental Arrows-A
			0x2900,
			0x297F,  // Supplemental Arrows-B
			0,
		};
		return ranges;
	}

	const ImWchar* IGlyphData::get_glyph_ranges_common() noexcept
	{
		static const ImWchar ranges[] = {
			0x0020,
			0x00FF,  // Basic Latin + Latin Supplement
			0x02B0,
			0x02FF,  // Spacing Modifier Letters
			0x2000,
			0x206F,  // General Punctuation
			0x20A0,
			0x20CF,  // Currency Symbols
			0x2100,
			0x214F,  // Letterlike Symbols
			0x2190,
			0x21FF,  // Arrows
			0x2200,
			0x22FF,  // Mathematical Operators
			0x2300,
			0x23FF,  // Miscellaneous Technical
			0x2400,
			0x243F,  // Control Pictures
			0x2440,
			0x245F,  // Optical Character Recognition
			0x2460,
			0x24FF,  // Enclosed Alphanumerics
			0x2500,
			0x257F,  // Box Drawing
			0x2580,
			0x259F,  // Block Elements
			0x25A0,
			0x25FF,  // Geometric Shapes
			0x2600,
			0x26FF,  // Miscellaneous Symbols
			0x27C0,
			0x27EF,  // Miscellaneous Mathematical Symbols-A
			0x2980,
			0x29FF,  // Miscellaneous Mathematical Symbols-B
			0x27F0,
			0x27FF,  // Supplemental Arrows-A
			0x2900,
			0x297F,  // Supplemental Arrows-B
			0x2B00,
			0x2BFF,  // Miscellaneous Symbols and Arrows
			0x2E00,
			0x2E7F,  // Supplemental Punctuation
			0,
		};
		return ranges;
	}
}