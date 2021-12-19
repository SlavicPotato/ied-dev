#include "pch.h"

#include "JSONSettingsControlsParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::SettingHolder::Controls>::Parse(
			const Json::Value& a_in,
			Data::SettingHolder::Controls& a_out,
			const std::uint32_t a_version) const
		{
			a_out.key = a_in.get("key", 0).asUInt();
			a_out.comboKey = a_in.get("combo_key", 0).asUInt();

			return true;
		}

		template <>
		void Parser<Data::SettingHolder::Controls>::Create(
			const Data::SettingHolder::Controls& a_data,
			Json::Value& a_out) const
		{
			a_out["key"] = a_data.key;
			a_out["combo_key"] = a_data.comboKey;
		}

		template <>
		void Parser<Data::SettingHolder::Controls>::GetDefault(
			Data::SettingHolder::Controls& a_out) const
		{}

	}  // namespace Serialization
}  // namespace IED