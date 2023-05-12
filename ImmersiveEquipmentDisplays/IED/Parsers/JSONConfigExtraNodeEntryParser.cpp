#include "pch.h"

#include "JSONConfigExtraNodeEntryParser.h"

#include "JSONConfigExtraNodeEntrySkelTransformParser.h"
#include "JSONConfigSkeletonMatchParser.h"

namespace IED
{
	namespace Serialization
	{
		constexpr auto filter_placement_id(Json::UInt a_in) noexcept
		{
			switch (a_in)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				return static_cast<WeaponPlacementID>(a_in);
			default:
				return WeaponPlacementID::None;
			}
		}

		template <>
		bool Parser<Data::configExtraNodeEntry_t>::Parse(
			const Json::Value&            a_in,
			Data::configExtraNodeEntry_t& a_out) const
		{
			a_out.name        = a_in["name"].asString();
			a_out.desc        = a_in["desc"].asString();
			a_out.parent      = a_in["parent"].asString();
			a_out.placementID = filter_placement_id(a_in["placement_id"].asUInt());

			if (auto& skel = a_in["skeleton"])
			{
				Parser<Data::configSkeletonMatch_t>               smparser(m_state);
				Parser<Data::configExtraNodeEntrySkelTransform_t> skelparser(m_state);

				for (auto& e : skel)
				{
					auto& v = a_out.skel.emplace_back();

					if (!smparser.Parse(e["match"], v.match))
					{
						throw parser_exception("bad match");
					}

					if (auto& xh = e["xfrm_mov"])
					{
						if (!skelparser.Parse(xh, v.sxfrms[0]))
						{
							throw parser_exception("bad xfrm_mov");
						}
					}

					if (auto& xh = e["xfrm_node"])
					{
						if (!skelparser.Parse(xh, v.sxfrms[1]))
						{
							throw parser_exception("bad xfrm_node");
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
			throw parser_exception(__FUNCTION__ ": " PARSER_NOT_IMPL_STR);
		}
	}
}