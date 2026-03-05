#include "pch.h"

#include "JSONConfigInventoryParser.h"
#include "JSONFormListParser.h"
#include "JSONParsersCommon.h"
#include "JSONRangeParser.h"

namespace IED
{
	namespace Serialization
	{
		using namespace Data;

		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configInventory_t>::Parse(
			const Json::Value&       a_in,
			Data::configInventory_t& a_out) const
		{
			JSON_PARSE_VERSION()

			Parser<Data::configRange_t> prange(m_state);

			if (auto& cr = a_in["cr"])
			{
				if (!prange.Parse(cr, a_out.countRange))
				{
					return false;
				}
			}

			if (auto& extra = a_in["ex"])
			{
				Parser<Data::configFormList_t> pformList(m_state);

				if (!pformList.Parse(extra, a_out.extraItems, version))
				{
					return false;
				}
			}

			a_out.flags = a_in.get("fl", stl::underlying(Data::configInventory_t::DEFAULT_FLAGS)).asUInt();

			return true;
		}

		template <>
		void Parser<Data::configInventory_t>::Create(
			const Data::configInventory_t& a_in,
			Json::Value&                   a_out) const
		{
			if (!a_in.countRange.empty())
			{
				Parser<Data::configRange_t> prange(m_state);

				prange.Create(a_in.countRange, a_out["cr"]);
			}

			if (!a_in.extraItems.empty())
			{
				Parser<Data::configFormList_t> pformList(m_state);

				pformList.Create(a_in.extraItems, a_out["ex"]);
			}

			a_out["fl"] = a_in.flags.underlying();
		}

	}
}