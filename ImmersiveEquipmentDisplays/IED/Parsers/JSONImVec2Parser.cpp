#include "pch.h"

#include "JSONImVec2Parser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<ImVec2>::Parse(
			const Json::Value& a_in,
			ImVec2&            a_out) const
		{
			if (!ParseFloatArray(a_in, reinterpret_cast<float*>(std::addressof(a_out)), 2))
			{
				throw parser_exception(__FUNCTION__ ": malformed array");
			}

			return true;
		}

		template <>
		void Parser<ImVec2>::Create(
			const ImVec2& a_data,
			Json::Value&  a_out) const
		{
			CreateFloatArray(reinterpret_cast<const float*>(std::addressof(a_data)), 2, a_out);
		}

	}
}