#include "pch.h"

#include "JSONFormParser.h"

#include "IED/Data.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Game::FormID>::Parse(
			const Json::Value& a_in,
			Game::FormID& a_out,
			const std::uint32_t a_version) const
		{
			if (a_in.empty())
			{
				a_out = {};
				return false;
			}

			Game::FormID lower = a_in["id"].asUInt();

			if (!lower || lower.IsTemporary())
			{
				a_out = lower;
				return true;
			}

			auto& plugin = a_in["plugin"];

			auto& data = Data::IData::GetPluginInfo().GetLookupRef();

			auto it = data.find(plugin.asString());
			if (it == data.end())
			{
				a_out = {};
				return false;
			}
			else
			{
				a_out = it->second.GetFormID(lower);
				return true;
			}
		}

		template <>
		void Parser<Game::FormID>::Create(
			const Game::FormID& a_data,
			Json::Value& a_out) const
		{
			std::uint32_t pluginIndex;
			if (a_data && a_data.GetPluginPartialIndex(pluginIndex))
			{
				auto& data = Data::IData::GetPluginInfo().GetIndexMap();

				auto it = data.find(pluginIndex);
				if (it != data.end())
				{
					a_out["plugin"] = static_cast<const std::string&>(it->second.name);
					a_out["id"] = it->second.GetFormIDLower(a_data).get();
				}
				else
				{
					Error("%s: plugin index %X not found", __FUNCTION__, pluginIndex);
				}
			}
			else  // temporary or zero
			{
				a_out["id"] = a_data.get();
			}
		}

		template <>
		void Parser<Game::FormID>::GetDefault(Game::FormID& a_out) const
		{
			a_out = {};
		}

	}  // namespace Serialization
}  // namespace IED