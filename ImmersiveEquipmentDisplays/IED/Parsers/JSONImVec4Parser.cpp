#include "pch.h"

#include "JSONImVec4Parser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<ImVec4>::Parse(
			const Json::Value& a_in,
			ImVec4&            a_out) const
		{
			if (!ParseFloatArray(a_in, reinterpret_cast<float*>(std::addressof(a_out)), 4))
			{
				throw parser_exception(__FUNCTION__ ": malformed array");
			}

			return true;
		}

		template <>
		void Parser<ImVec4>::Create(
			const ImVec4& a_data,
			Json::Value&  a_out) const
		{
			CreateFloatArray(reinterpret_cast<const float*>(std::addressof(a_data)), 4, a_out);
		}

	}
}