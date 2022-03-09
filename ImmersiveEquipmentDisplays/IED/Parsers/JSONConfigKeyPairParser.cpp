#include "pch.h"

#include "JSONConfigKeyPairParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::ConfigKeyPair>::Parse(
			const Json::Value&   a_in,
			Data::ConfigKeyPair& a_out) const
		{
			a_out.key      = a_in.get("key", 0).asUInt();
			a_out.comboKey = a_in.get("combo_key", 0).asUInt();

			return true;
		}

		template <>
		void Parser<Data::ConfigKeyPair>::Create(
			const Data::ConfigKeyPair& a_data,
			Json::Value&               a_out) const
		{
			a_out["key"]       = a_data.key;
			a_out["combo_key"] = a_data.comboKey;
		}

	}
}