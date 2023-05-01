#include "pch.h"

#include "JSONBulletVector3Parser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<btVector3>::Parse(
			const Json::Value& a_in,
			btVector3&         a_out) const
		{
			if (!ParseFloatArray(a_in, a_out, 3))
			{
				throw parser_exception(__FUNCTION__ ": malformed array");
			}

			a_out.setW(0);

			return true;
		}

		template <>
		void Parser<btVector3>::Create(
			const btVector3& a_data,
			Json::Value&     a_out) const
		{
			CreateFloatArray(a_data, 3, a_out);
		}

	}
}