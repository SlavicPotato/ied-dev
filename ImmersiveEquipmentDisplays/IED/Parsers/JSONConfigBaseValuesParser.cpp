#include "pch.h"

#include "JSONConfigBaseValuesParser.h"

#include "JSONConfigCachedFormParser.h"
#include "JSONConfigExtraLight.h"
#include "JSONConfigNodePhysicsValuesParser.h"
#include "JSONConfigTransformParser.h"
#include "JSONConfigVSSParser.h"
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

			Parser<Data::configTransform_t> xparser(m_state);

			if (auto& data = a_in["xfrm"])
			{
				if (!xparser.Parse(data, a_out, a_version))
				{
					return false;
				}
			}

			if (auto& data = a_in["gxfrm"])
			{
				if (!xparser.Parse(data, a_out.geometryTransform, a_version))
				{
					return false;
				}
			}

			if (auto& data = a_in["node"])
			{
				Parser<Data::NodeDescriptor> parser(m_state);

				if (!parser.Parse(data, a_out.targetNode, a_version))
				{
					return false;
				}
			}

			a_out.targetNode.lookup_flags();

			if (auto& data = a_in["nics"])
			{
				a_out.niControllerSequence = data.asString();
			}

			if (auto& data = a_in["aev"])
			{
				a_out.animationEvent = data.asString();
			}

			if (auto& data = a_in["fmd"])
			{
				Parser<Data::configCachedForm_t> parser(m_state);

				if (!parser.Parse(data, a_out.forceModel))
				{
					return false;
				}
			}

			if (auto& data = a_in["phy"])
			{
				Parser<Data::configNodePhysicsValues_t> parser(m_state);

				a_out.physicsValues.data =
					std::make_unique<Data::configNodePhysicsValues_t>();

				if (!parser.Parse(data, *a_out.physicsValues.data))
				{
					return false;
				}
			}

			if (auto& data = a_in["exl"])
			{
				Parser<Data::configExtraLight_t> parser(m_state);

				if (!parser.Parse(data, a_out.extraLightConfig))
				{
					return false;
				}
			}

			if (auto& data = a_in["fm_vss"])
			{
				Parser<Data::configVariableSourceSelector_t> parser(m_state);

				if (!parser.Parse(data, a_out.fmVss))
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

			Parser<Data::configTransform_t> xparser(m_state);

			if (!a_data.empty())
			{
				xparser.Create(a_data, a_out["xfrm"]);
			}

			if (!a_data.geometryTransform.empty())
			{
				xparser.Create(a_data.geometryTransform, a_out["gxfrm"]);
			}

			if (a_data.targetNode)
			{
				Parser<Data::NodeDescriptor> parser(m_state);

				parser.Create(a_data.targetNode, a_out["node"]);
			}

			if (!a_data.niControllerSequence.empty())
			{
				a_out["nics"] = *a_data.niControllerSequence;
			}

			if (!a_data.animationEvent.empty())
			{
				a_out["aev"] = *a_data.animationEvent;
			}

			if (a_data.forceModel.get_id())
			{
				Parser<Data::configCachedForm_t> parser(m_state);

				parser.Create(a_data.forceModel, a_out["fmd"]);
			}

			if (auto& data = a_data.physicsValues.data)
			{
				Parser<Data::configNodePhysicsValues_t> parser(m_state);

				parser.Create(*data, a_out["phy"]);
			}

			Parser<Data::configExtraLight_t> parserexl(m_state);

			parserexl.Create(a_data.extraLightConfig, a_out["exl"]);

			Parser<Data::configVariableSourceSelector_t> vssparser(m_state);

			vssparser.Create(a_data.fmVss, a_out["fm_vss"]);
		}

	}
}