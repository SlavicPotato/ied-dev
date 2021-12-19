#include "pch.h"

#include "Serialization.h"

namespace IED
{
	namespace Serialization
	{
		bool ParseFloatArray(
			const Json::Value& a_in,
			float* a_out,
			std::uint32_t a_size)
		{
			if (!a_in.isArray())
				return false;

			if (a_in.size() != a_size)
				return false;

			for (std::uint32_t i = 0; i < a_size; i++)
			{
				auto& v = a_in[i];

				if (!v.isNumeric())
					return false;

				a_out[i] = v.asFloat();
			}

			return true;
		}

		void CreateFloatArray(
			const float* a_in,
			std::uint32_t a_size,
			Json::Value& a_out)
		{
			for (std::uint32_t i = 0; i < a_size; i++)
			{
				a_out.append(a_in[i]);
			}
		}
	}  // namespace Serialization
}  // namespace IED