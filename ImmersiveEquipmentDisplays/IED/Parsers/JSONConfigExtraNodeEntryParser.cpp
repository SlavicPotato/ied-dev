#include "pch.h"

#include "JSONConfigExtraNodeEntryParser.h"

#include "JSONConfigTransformParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configExtraNodeEntry_t>::Parse(
			const Json::Value&       a_in,
			Data::configExtraNodeEntry_t& a_out) const
		{
			Parser<Data::configTransform_t> xfrmparser(m_state);

			a_out.desc   = a_in["desc"].asString();
			a_out.parent = a_in["parent"].asString();

			if (auto& xh = a_in["xfrm_mov"])
			{
				if (!xfrmparser.Parse(xh["f"], a_out.xfrm_mov_f, 1u))
				{
					return false;
				}

				if (!xfrmparser.Parse(xh["m"], a_out.xfrm_mov_m, 1u))
				{
					return false;
				}
			}

			if (auto& xh = a_in["xfrm_node"])
			{
				if (!xfrmparser.Parse(xh["f"], a_out.xfrm_node_f, 1u))
				{
					return false;
				}

				if (!xfrmparser.Parse(xh["m"], a_out.xfrm_node_m, 1u))
				{
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configExtraNodeEntry_t>::Create(
			const Data::configExtraNodeEntry_t& a_data,
			Json::Value&                   a_out) const
		{
			Parser<Data::configTransform_t> xfrmparser(m_state);

			a_out["desc"]   = *a_data.desc;
			a_out["parent"] = *a_data.parent;

			auto& xh_mov = a_out["xfrm_mov"];

			xfrmparser.Create(a_data.xfrm_mov_f, xh_mov["f"]);
			xfrmparser.Create(a_data.xfrm_mov_m, xh_mov["m"]);

			auto& xh_node = a_out["xfrm_node"];

			xfrmparser.Create(a_data.xfrm_node_f, xh_node["f"]);
			xfrmparser.Create(a_data.xfrm_node_m, xh_node["m"]);
		}
	}
}