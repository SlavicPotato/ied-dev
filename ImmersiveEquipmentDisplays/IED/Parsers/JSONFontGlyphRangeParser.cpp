#include "pch.h"

#include "JSONFontGlyphRangeParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<fontGlyphRange_t>::Parse(
			const Json::Value& a_in,
			fontGlyphRange_t&  a_out) const
		{
			for (auto& e : a_in)
			{
				if (!e.isArray() || e.size() != 2)
				{
					throw parser_exception(__FUNCTION__ ": bad range data");
				}

				const std::uint64_t tmp[2]{
					e[0].asUInt64(),
					e[1].asUInt64()
				};

				for (auto& f : tmp)
				{
					if (f > std::numeric_limits<ImWchar>::max() ||
					    f < 0x20u)
					{
						throw parser_exception(__FUNCTION__ ": glyph out of range");
					}
				}

				if (tmp[0] > tmp[1])
				{
					throw parser_exception(__FUNCTION__ ": bad glyph range (low > high)");
				}

				a_out.emplace_back(
					static_cast<ImWchar>(tmp[0]),
					static_cast<ImWchar>(tmp[1]));
			}

			return true;
		}

		template <>
		void Parser<fontGlyphRange_t>::Create(
			const fontGlyphRange_t& a_data,
			Json::Value&            a_out) const
		{
			throw parser_exception(__FUNCTION__ ": " PARSER_NOT_IMPL_STR);
		}

	}
}