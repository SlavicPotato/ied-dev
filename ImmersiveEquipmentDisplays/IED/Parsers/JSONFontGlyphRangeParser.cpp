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
					Error("%s: bad range data", __FUNCTION__);
					return false;
				}

				std::uint64_t tmp[2]{
					e[0].asUInt64(),
					e[1].asUInt64()
				};

				for (auto& f : tmp)
				{
					if (f > std::numeric_limits<ImWchar>::max() ||
					    f < 0x20u)
					{
						Error("%s: glyph %.X out of range", __FUNCTION__, f);
						return false;
					}
				}

				if (tmp[0] > tmp[1])
				{
					Error("%s: bad glyph range (low > high)", __FUNCTION__);
					return false;
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
		}

	}
}