#include "pch.h"

#include "JSONConfigCachedFormParser.h"
#include "JSONConfigOverrideModelGroupEntryParser.h"
#include "JSONConfigTransformParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configModelGroupEntry_t>::Parse(
			const Json::Value& a_in,
			Data::configModelGroupEntry_t& a_out) const
		{
			JSON_PARSE_VERSION();

			auto& data = a_in["data"];

			Parser<Data::configCachedForm_t> fparser(m_state);
			Parser<Data::configTransform_t> tparser(m_state);

			if (!fparser.Parse(data["form"], a_out.form))
			{
				return false;
			}

			if (!tparser.Parse(data["xfrm"], a_out.transform, version))
			{
				return false;
			}

			a_out.flags = static_cast<Data::ConfigModelGroupEntryFlags>(
				data.get("flags", stl::underlying(Data::ConfigModelGroupEntryFlags::kNone)).asUInt());

			return true;
		}

		template <>
		void Parser<Data::configModelGroupEntry_t>::Create(
			const Data::configModelGroupEntry_t& a_data,
			Json::Value& a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			Parser<Data::configCachedForm_t> fparser(m_state);
			Parser<Data::configTransform_t> tparser(m_state);

			fparser.Create(a_data.form, data["form"]);
			tparser.Create(a_data.transform, data["xfrm"]);

			data["flags"] = stl::underlying(a_data.flags.value);

			a_out["version"] = CURRENT_VERSION;
		}

		template <>
		void Parser<Data::configModelGroupEntry_t>::GetDefault(
			Data::configModelGroupEntry_t& a_out) const
		{
		}

	}
}