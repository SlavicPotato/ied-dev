#include "pch.h"

#include "JSONBulletVector4Parser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<btVector4>::Parse(
			const Json::Value& a_in,
			btVector4&         a_out) const
		{
			if (!ParseFloatArray(a_in, a_out.mVec128.m128_f32))
			{
				throw std::exception(__FUNCTION__ ": malformed array");
			}

			return true;
		}

		template <>
		void Parser<btVector4>::Create(
			const btVector4& a_data,
			Json::Value&     a_out) const
		{
			CreateFloatArray(a_data.mVec128.m128_f32, a_out);
		}

	}
}