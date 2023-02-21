#include "pch.h"

#include "JSONConfigLastEquippedParser.h"

#include "JSONConfigBipedObjectListParser.h"
#include "JSONConfigObjectSlotListParser.h"
#include "JSONEquipmentOverrideConditionListParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 2;

		template <>
		bool Parser<Data::configLastEquipped_t>::Parse(
			const Json::Value&          a_in,
			Data::configLastEquipped_t& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			if (auto& d = data["sl"])
			{
				Parser<Data::configBipedObjectList_t> parser(m_state);

				if (!parser.Parse(d, a_out.bipedSlots))
				{
					return false;
				}
			}

			if (auto& d = data["fc"])
			{
				Parser<Data::equipmentOverrideConditionList_t> parser(m_state);

				if (!parser.Parse(d, a_out.filterConditions))
				{
					return false;
				}
			}

			a_out.flags = data.get(
								  "flags",
								  stl::underlying(Data::configLastEquipped_t::DEFAULT_FLAGS))
			                  .asUInt();

			if (version >= 2)
			{
				if (auto& d = data["esl"])
				{
					Parser<Data::configObjectSlotList_t> parser(m_state);

					if (!parser.Parse(d, a_out.slots))
					{
						return false;
					}
				}
			}
			else
			{
				const auto slot = static_cast<Data::ObjectSlot>(
					data.get("es", stl::underlying(Data::ObjectSlot::kNone)).asUInt());

				if (slot < Data::ObjectSlot::kMax)
				{
					a_out.slots.emplace_back(slot);
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configLastEquipped_t>::Create(
			const Data::configLastEquipped_t& a_in,
			Json::Value&                      a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			if (!a_in.bipedSlots.empty())
			{
				Parser<Data::configBipedObjectList_t> parser(m_state);

				parser.Create(a_in.bipedSlots, data["sl"]);
			}

			if (!a_in.filterConditions.empty())
			{
				Parser<Data::equipmentOverrideConditionList_t> parser(m_state);

				parser.Create(a_in.filterConditions, data["fc"]);
			}

			data["flags"] = a_in.flags.underlying();

			if (!a_in.slots.empty())
			{
				Parser<Data::configObjectSlotList_t> parser(m_state);

				parser.Create(a_in.slots, data["esl"]);
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}