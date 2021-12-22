#include "pch.h"

#include "JSONConfigSlotHolderParser.h"
#include "JSONConfigSlotParser.h"
#include "JSONParsersCommon.h"

namespace IED
{
	namespace Serialization
	{
		using namespace Data;

		static SlotKeyParser s_slotKeyParser;

		template <>
		bool Parser<Data::configSlotHolder_t>::Parse(
			const Json::Value& a_in,
			Data::configSlotHolder_t& a_out) const
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				Error("%s: bad version data", __FUNCTION__);
				return false;
			}

			Parser<Data::configSlot_t> pslot(m_state);

			auto& data = a_in["data"];

			for (auto it = data.begin(); it != data.end(); ++it)
			{
				auto key = it.key().asString();

				auto slot = s_slotKeyParser.KeyToSlot(key);

				if (slot == ObjectSlot::kMax)
				{
					Error("%s: unrecognized slot key: '%s'", __FUNCTION__, key.c_str());
					return false;
				}

				auto& v = a_out.get(slot);

				if (!v)
				{
					v = std::make_unique<Data::configSlotHolder_t::data_type>();
				}

				parserDesc_t<Data::configSlot_t> desc[]{
					{ "m", (*v)(ConfigSex::Male) },
					{ "f", (*v)(ConfigSex::Female) }
				};

				for (auto& e : desc)
				{
					if (!pslot.Parse((*it)[e.member], e.data, version))
					{
						return false;
					}
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configSlotHolder_t>::Create(
			const Data::configSlotHolder_t& a_data,
			Json::Value& a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			using enum_type = std::underlying_type_t<ObjectSlot>;

			Parser<Data::configSlot_t> pslot(m_state);

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				auto slot = static_cast<ObjectSlot>(i);

				auto& v = a_data.get(slot);

				if (!v)
				{
					continue;
				}

				auto key = s_slotKeyParser.SlotToKey(slot);

				ASSERT(key);

				parserDesc_t<Data::configSlot_t> desc[]{
					{ "m", (*v)(ConfigSex::Male) },
					{ "f", (*v)(ConfigSex::Female) }
				};

				auto& e = data[key];

				for (auto& f : desc)
				{
					pslot.Create(f.data, e[f.member]);
				}
			}

			a_out["version"] = 1u;
		}

		template <>
		void Parser<Data::configSlotHolder_t>::GetDefault(
			Data::configSlotHolder_t& a_out) const
		{
			/*for (auto& e : a_out.m_data)
      {
          e = std::make_unique<Data::configSlotHolder_t::data_type>();
      }*/
		}

	}  // namespace Serialization
}  // namespace IED