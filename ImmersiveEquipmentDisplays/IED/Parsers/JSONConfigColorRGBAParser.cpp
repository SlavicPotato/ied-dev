#include "pch.h"

#include "JSONConfigColorRGBAParser.h"
#include "JSONConfigColorRGBParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configColorRGBA_t>::Parse(
			const Json::Value&       a_in,
			Data::configColorRGBA_t& a_out,
			float                    a_defaultAlpha) const
		{
			Parser<Data::configColorRGB_t> rgbparser(m_state);

			rgbparser.Parse(a_in, a_out);

			a_out.a = std::clamp(a_in.get("a", a_defaultAlpha).asFloat(), 0.0f, 1.0f);

			return true;
		}

		template <>
		void Parser<Data::configColorRGBA_t>::Create(
			const Data::configColorRGBA_t& a_data,
			Json::Value&                   a_out) const
		{
			Parser<Data::configColorRGB_t> rgbparser(m_state);

			rgbparser.Create(a_data, a_out);

			a_out["a"] = a_data.a;
		}

	}
}