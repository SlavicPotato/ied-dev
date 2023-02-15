#include "pch.h"

#include "JSONConfigCachedFormParser.h"
#include "JSONConfigExtraLight.h"
#include "JSONConfigOverrideModelGroupEntryParser.h"
#include "JSONConfigTransformParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 2;

		template <>
		bool Parser<Data::configModelGroupEntry_t>::Parse(
			const Json::Value&             a_in,
			Data::configModelGroupEntry_t& a_out) const
		{
			JSON_PARSE_VERSION();

			auto& data = a_in["data"];

			Parser<Data::configCachedForm_t> fparser(m_state);

			if (!fparser.Parse(data["form"], a_out.form))
			{
				return false;
			}

			if (auto& xfrm = data["xfrm"])
			{
				Parser<Data::configTransform_t> tparser(m_state);

				if (!tparser.Parse(xfrm, a_out.transform, version))
				{
					return false;
				}
			}

			a_out.flags = static_cast<Data::ConfigModelGroupEntryFlags>(
				data.get("flags", stl::underlying(Data::ConfigModelGroupEntryFlags::kNone)).asUInt());

			if (auto& nics = data["nics"])
			{
				a_out.niControllerSequence = nics.asString();
			}

			if (auto& aev = data["aev"])
			{
				a_out.animationEvent = aev.asString();
			}

			if (auto& el = data["exl"])
			{
				Parser<Data::configExtraLight_t> parser(m_state);

				if (!parser.Parse(el, a_out.extraLightConfig))
				{
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configModelGroupEntry_t>::Create(
			const Data::configModelGroupEntry_t& a_data,
			Json::Value&                         a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			Parser<Data::configCachedForm_t> fparser(m_state);

			fparser.Create(a_data.form, data["form"]);

			if (!a_data.transform.empty())
			{
				Parser<Data::configTransform_t> tparser(m_state);

				tparser.Create(a_data.transform, data["xfrm"]);
			}

			data["flags"] = a_data.flags.underlying();

			if (!a_data.niControllerSequence.empty())
			{
				data["nics"] = *a_data.niControllerSequence;
			}

			if (!a_data.animationEvent.empty())
			{
				data["aev"] = *a_data.animationEvent;
			}

			Parser<Data::configExtraLight_t> parserexl(m_state);

			parserexl.Create(a_data.extraLightConfig, data["exl"]);

			a_out["version"] = CURRENT_VERSION;
		}

	}
}