#include "pch.h"

#include "JSONFontGlyphDataParser.h"
#include "JSONFontGlyphRangeParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<fontGlyphData_t>::Parse(
			const Json::Value& a_in,
			fontGlyphData_t& a_out) const
		{
			if (auto& gp = a_in["glyph_presets"])
			{
				stl::iunordered_map<std::string, GlyphPresetFlags> m{
					{ "default", GlyphPresetFlags::kDefault },
					{ "cyrilic", GlyphPresetFlags::kCyrilic },
					{ "japanese", GlyphPresetFlags::kJapanese },
					{ "chinese_simplified_common", GlyphPresetFlags::kChineseSimplifiedCommon },
					{ "chinese_full", GlyphPresetFlags::kChineseFull },
					{ "korean", GlyphPresetFlags::kKorean },
					{ "latin_full", GlyphPresetFlags::kLatinFull },
					{ "thai", GlyphPresetFlags::kThai },
					{ "vietnamise", GlyphPresetFlags::kVietnamise },
					{ "greek", GlyphPresetFlags::kGreek },
					{ "arabic", GlyphPresetFlags::kArabic },
					{ "arrows", GlyphPresetFlags::kArrows },
					{ "common", GlyphPresetFlags::kCommon }
				};

				for (auto& e : gp)
				{
					auto s = e.asString();

					auto it = m.find(s);
					if (it == m.end())
					{
						Error("%s: unknown glyph preset: %s", __FUNCTION__, s.c_str());
						continue;
					}

					a_out.glyph_preset_flags.set(it->second);
				}
			}

			Parser<fontGlyphRange_t> rangeParser(m_state);

			if (!rangeParser.Parse(
					a_in["glyph_ranges"],
					a_out.glyph_ranges))
			{
				return false;
			}

			a_out.extra_glyphs = a_in["extra_glyphs"].asString();

			return true;
		}

		template <>
		void Parser<fontGlyphData_t>::Create(
			const fontGlyphData_t& a_data,
			Json::Value& a_out) const
		{
		}

		template <>
		void Parser<fontGlyphData_t>::GetDefault(
			fontGlyphData_t& a_out) const
		{}

	}
}