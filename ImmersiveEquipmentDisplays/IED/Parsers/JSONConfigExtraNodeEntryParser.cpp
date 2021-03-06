#include "pch.h"

#include "JSONConfigExtraNodeEntryParser.h"

#include "JSONConfigTransformParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configExtraNodeEntry_t>::Parse(
			const Json::Value&            a_in,
			Data::configExtraNodeEntry_t& a_out) const
		{
			Parser<Data::configTransform_t> xfrmparser(m_state);

			a_out.name   = a_in["name"].asString();
			a_out.desc   = a_in["desc"].asString();
			a_out.parent = a_in["parent"].asString();

			if (auto& skel = a_in["skeleton"])
			{
				for (auto& e : skel)
				{
					auto& ids = e["ids"];
					if (!ids || ids.size() == 0)
					{
						continue;
					}

					auto& v = a_out.skel.emplace_back();

					for (auto& f : ids)
					{
						v.ids.emplace_back(f.asInt());
					}

					if (auto& xh = e["xfrm_mov"])
					{
						if (!xfrmparser.Parse(xh, v.transform_mov, 1u))
						{
							return false;
						}
					}

					if (auto& xh = e["xfrm_node"])
					{
						if (!xfrmparser.Parse(xh, v.transform_node, 1u))
						{
							return false;
						}
					}
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configExtraNodeEntry_t>::Create(
			const Data::configExtraNodeEntry_t& a_data,
			Json::Value&                        a_out) const
		{
		}
	}
}