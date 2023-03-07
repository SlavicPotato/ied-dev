#include "pch.h"

#include "JSONConfigOutfitParser.h"

#include "IED/Parsers/JSONFormParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::OM::configOutfit_t>::Parse(
			const Json::Value&        a_in,
			Data::OM::configOutfit_t& a_out,
			const std::uint32_t       a_version) const
		{
			a_out.flags = a_in.get("flags", stl::underlying(Data::OM::configOutfit_t::DEFAULT_FLAGS)).asUInt();

			if (!make<Game::FormID>().Parse(a_in["ofrm"], a_out.outfit))
			{
				return false;
			}

			if (auto& id = a_in["id"])
			{
				a_out.id = id.asString();
			}

			return true;
		}

		template <>
		void Parser<Data::OM::configOutfit_t>::Create(
			const Data::OM::configOutfit_t& a_data,
			Json::Value&                    a_out) const
		{
			a_out["flags"] = a_data.flags.underlying();

			make<Game::FormID>().Create(a_data.outfit, a_out["ofrm"]);

			a_out["id"] = *a_data.id;
		}
	}
}