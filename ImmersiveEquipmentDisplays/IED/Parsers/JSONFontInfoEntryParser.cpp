#include "pch.h"

#include "JSONFontGlyphDataParser.h"
#include "JSONFontInfoEntryParser.h"

#include <ext/IOS.h>

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<FontInfoEntry>::Parse(
			const Json::Value& a_in,
			FontInfoEntry&     a_out,
			std::uint32_t      a_version) const
		{
			a_out.path = a_in["file"].asString();

			if (a_out.path.empty())
			{
				throw parser_exception("empty font filename");
			}

			a_out.win_font = a_in.get("win_font", false).asBool();

			if (a_version >= 2)
			{
				if (auto& glyphs = a_in["glyphs"])
				{
					Parser<FontGlyphData> gparser(m_state);

					auto tmp = stl::make_smart_for_overwrite<FontGlyphData>();

					if (!gparser.Parse(glyphs, *tmp))
					{
						return false;
					}

					a_out.glyph_data = std::move(tmp);
				}
			}
			else
			{
				Parser<FontGlyphData> gparser(m_state);

				auto tmp = stl::make_smart_for_overwrite<FontGlyphData>();

				if (!gparser.Parse(a_in, *tmp))
				{
					return false;
				}

				a_out.glyph_data = std::move(tmp);
			}

			a_out.size = std::max(a_in["size"].asFloat(), 0.1f);

			return true;
		}

		template <>
		void Parser<FontInfoEntry>::Create(
			const FontInfoEntry& a_data,
			Json::Value&         a_out) const
		{
			throw parser_exception(__FUNCTION__ ": " PARSER_NOT_IMPL_STR);
		}

	}
}