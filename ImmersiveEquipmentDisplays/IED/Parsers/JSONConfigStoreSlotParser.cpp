#include "pch.h"

#include "JSONConfigMapSlotParser.h"
#include "JSONConfigSlotHolderParser.h"
#include "JSONConfigStoreSlotParser.h"
#include "JSONParsersCommon.h"

namespace IED
{
	namespace Serialization
	{
		using namespace Data;

		template <>
		bool Parser<Data::configStoreSlot_t>::Parse(
			const Json::Value& a_in,
			Data::configStoreSlot_t& a_out) const
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				Error("%s: bad version data", __FUNCTION__);
				return false;
			}

			return ParseConfigStore<configSlotHolder_t, configMapSlot_t>(a_in, a_out, version);

		}

		template <>
		void Parser<Data::configStoreSlot_t>::Create(
			const Data::configStoreSlot_t& a_data,
			Json::Value& a_out) const
		{
			CreateConfigStore<configSlotHolder_t, configMapSlot_t>(a_data, a_out, 1u);
		}

		template <>
		void Parser<Data::configStoreSlot_t>::GetDefault(
			Data::configStoreSlot_t& a_out) const
		{}

	}  // namespace Serialization
}  // namespace IED