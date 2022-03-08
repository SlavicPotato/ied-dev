#include "pch.h"

#include "JSONConfigTransformParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configTransform_t>::Parse(
			const Json::Value& a_in,
			Data::configTransform_t& a_out,
			const std::uint32_t a_version) const
		{
			if (ParseFloatArray(a_in["pos"], *a_out.position, 3))
			{
				a_out.position.mark(true);
			}

			if (ParseFloatArray(a_in["rot"], *a_out.rotation, 3))
			{
				a_out.rotation.mark(true);
			}

			if (a_in.isMember("scale"))
			{
				a_out.scale = a_in.get("scale", 1.0f).asFloat();
			}

			a_out.clamp();

			return true;
		}

		template <>
		void Parser<Data::configTransform_t>::Create(
			const Data::configTransform_t& a_data,
			Json::Value& a_out) const
		{
			if (a_data.position)
			{
				CreateFloatArray(*a_data.position, 3, a_out["pos"]);
			}

			if (a_data.rotation)
			{
				CreateFloatArray(*a_data.rotation, 3, a_out["rot"]);
			}

			if (a_data.scale)
			{
				a_out["scale"] = *a_data.scale;
			}
		}

	}  // namespace Serialization
}  // namespace IED