#include "pch.h"

#include "JSONConfigBaseValuesParser.h"
#include "JSONConfigFixedStringSetParser.h"
#include "JSONConfigTransformParser.h"
#include "JSONNodeMapValueParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configBaseValues_t>::Parse(
			const Json::Value&        a_in,
			Data::configBaseValues_t& a_out,
			const std::uint32_t       a_version) const
		{
			a_out.flags = static_cast<Data::BaseFlags>(
				a_in.get("flags", stl::underlying(Data::configBaseValues_t::DEFAULT_FLAGS)).asUInt());

			if (auto& xfrm = a_in["xfrm"])
			{
				Parser<Data::configTransform_t> tfParser(m_state);

				if (!tfParser.Parse(xfrm, a_out, a_version))
				{
					return false;
				}
			}

			if (auto& node = a_in["node"])
			{
				Parser<Data::NodeDescriptor> nvParser(m_state);

				if (!nvParser.Parse(node, a_out.targetNode, a_version))
				{
					return false;
				}
			}

			a_out.targetNode.lookup_flags();

			if (auto& nics = a_in["nics"])
			{
				a_out.niControllerSequence = nics.asString();
			}

			if (auto& aev = a_in["aev"])
			{
				a_out.animationEvent = aev.asString();
			}

			if (auto& hkxflt = a_in["hkxflt"])
			{
				Parser<Data::configFixedStringSet_t> fssparser(m_state);

				if (!fssparser.Parse(hkxflt, a_out.hkxFilter))
				{
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configBaseValues_t>::Create(
			const Data::configBaseValues_t& a_data,
			Json::Value&                    a_out) const
		{
			a_out["flags"] = a_data.flags.underlying();

			if (!a_data.empty())
			{
				Parser<Data::configTransform_t> tfParser(m_state);

				tfParser.Create(a_data, a_out["xfrm"]);
			}

			if (a_data.targetNode)
			{
				Parser<Data::NodeDescriptor> nvParser(m_state);

				nvParser.Create(a_data.targetNode, a_out["node"]);
			}

			if (!a_data.niControllerSequence.empty())
			{
				a_out["nics"] = *a_data.niControllerSequence;
			}

			if (!a_data.animationEvent.empty())
			{
				a_out["aev"] = *a_data.animationEvent;
			}

			if (!a_data.hkxFilter.empty())
			{
				Parser<Data::configFixedStringSet_t> fssparser(m_state);

				auto& outset = (a_out["hkxflt"] = Json::Value(Json::ValueType::arrayValue));

				fssparser.Create(a_data.hkxFilter, outset);
			}
		}

	}
}