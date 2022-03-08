#include "pch.h"

#include "JSONConfigSexParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::ConfigSex>::Parse(
			const Json::Value& a_in,
			Data::ConfigSex& a_out,
			const std::uint32_t a_version) const
		{
			auto tmp =
				a_in.get("sex", stl::underlying(Data::ConfigSex::Male)).asUInt();

			a_out = tmp == 1 ? Data::ConfigSex::Female : Data::ConfigSex::Male;

			return true;
		}

		template <>
		void Parser<Data::ConfigSex>::Create(
			const Data::ConfigSex& a_data,
			Json::Value& a_out) const
		{
			a_out["sex"] = stl::underlying(a_data);
		}

	}
}