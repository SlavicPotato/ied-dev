#pragma once

namespace IED
{
	enum class GlyphPresetFlags : std::uint32_t
	{
		kNone = 0,

		kDefault                 = 1u << 0,
		kCyrilic                 = 1u << 1,
		kJapanese                = 1u << 2,
		kChineseSimplifiedCommon = 1u << 3,
		kChineseFull             = 1u << 4,
		kKorean                  = 1u << 5,
		kLatinFull               = 1u << 6,
		kThai                    = 1u << 7,
		kVietnamise              = 1u << 8,
		kGreek                   = 1u << 9,
		kArabic                  = 1u << 10,
		kArrows                  = 1u << 21,
		kCommon                  = 1u << 22,

		kAll = static_cast<std::underlying_type_t<GlyphPresetFlags>>(-1)
	};

	DEFINE_ENUM_CLASS_BITWISE(GlyphPresetFlags);

	using fontGlyphRange_t = stl::vector<std::pair<ImWchar, ImWchar>>;

	struct fontGlyphData_t
	{
		stl::flag<GlyphPresetFlags> glyph_preset_flags{ GlyphPresetFlags::kNone };
		std::string                 extra_glyphs;
		fontGlyphRange_t            glyph_ranges;
	};

	struct fontInfoEntry_t :
		public fontGlyphData_t
	{
		std::string path;
		float       size;
	};

	struct fontInfoMap_t
	{
		using font_map_t = stl::unordered_map<
			stl::fixed_string,
			fontInfoEntry_t>;

		font_map_t      fonts;
		fontGlyphData_t default_glyph_data;
		float           default_font_size{ 13.0f };
	};

}