#include "pch.h"

#include "JSONFontGlyphDataParser.h"
#include "JSONFontGlyphRangeParser.h"

namespace IED
{
	namespace Serialization
	{
		constexpr auto get_preset(const char* a_name) noexcept
		{
			switch (stl::fixed_string::key_type::compute_hash(a_name))
			{
			case stl::fixed_string::make_hash("default"):
				return GlyphPresetFlags::kDefault;
			case stl::fixed_string::make_hash("cyrilic"):
				return GlyphPresetFlags::kCyrilic;
			case stl::fixed_string::make_hash("japanese"):
				return GlyphPresetFlags::kJapanese;
			case stl::fixed_string::make_hash("chinese_simplified_common"):
				return GlyphPresetFlags::kChineseSimplifiedCommon;
			case stl::fixed_string::make_hash("chinese_full"):
				return GlyphPresetFlags::kChineseFull;
			case stl::fixed_string::make_hash("korean"):
				return GlyphPresetFlags::kKorean;
			case stl::fixed_string::make_hash("latin_full"):
				return GlyphPresetFlags::kLatinFull;
			case stl::fixed_string::make_hash("thai"):
				return GlyphPresetFlags::kThai;
			case stl::fixed_string::make_hash("vietnamise"):
				return GlyphPresetFlags::kVietnamise;
			case stl::fixed_string::make_hash("greek"):
				return GlyphPresetFlags::kGreek;
			case stl::fixed_string::make_hash("arabic"):
				return GlyphPresetFlags::kArabic;
			case stl::fixed_string::make_hash("arrows"):
				return GlyphPresetFlags::kArrows;
			case stl::fixed_string::make_hash("common"):
				return GlyphPresetFlags::kCommon;
			default:
				return GlyphPresetFlags::kNone;
			}
		}

		template <>
		bool Parser<FontGlyphData>::Parse(
			const Json::Value& a_in,
			FontGlyphData&     a_out) const
		{
			if (auto& gp = a_in["glyph_presets"])
			{
				for (auto& e : gp)
				{
					const auto s = e.asString();
					const auto p = get_preset(s.c_str());

					if (p == GlyphPresetFlags::kNone)
					{
						Warning("%s: unknown glyph preset: %s", __FUNCTION__, s.c_str());
						continue;
					}

					a_out.glyph_preset_flags.set(p);
				}
			}

			if (auto& r = a_in["glyph_ranges"])
			{
				Parser<fontGlyphRange_t> rangeParser(m_state);

				if (!rangeParser.Parse(
						r,
						a_out.glyph_ranges))
				{
					return false;
				}
			}

			a_out.extra_glyphs = a_in["extra_glyphs"].asString();

			return true;
		}

		template <>
		void Parser<FontGlyphData>::Create(
			const FontGlyphData& a_data,
			Json::Value&         a_out) const
		{
			throw parser_exception(__FUNCTION__ ": " PARSER_NOT_IMPL_STR);
		}

	}
}