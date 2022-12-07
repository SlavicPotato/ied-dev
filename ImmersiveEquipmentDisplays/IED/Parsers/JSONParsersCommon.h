#pragma once

#include "IED/ConfigCommon.h"

#include "JSONFormParser.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		class SlotKeyParser
		{
		public:
			SlotKeyParser();

			const char*      SlotToKey(Data::ObjectSlot a_slot) const noexcept;
			Data::ObjectSlot KeyToSlot(const std::string& a_key) const;

		private:
			stl::unordered_map<std::string, Data::ObjectSlot> m_keyToSlot;
		};

		template <class Th, class Tm>
		constexpr void ParseConfigMap(
			const Json::Value& a_in,
			Tm&                a_out,
			std::uint32_t      a_version,
			ParserState&       a_state)
		{
			Parser<Game::FormID> pform(a_state);
			Parser<Th>           pholder(a_state);

			auto& data = a_in["data"];

			for (auto& e : data)
			{
				Game::FormID form;

				if (!pform.Parse(e["form"], form))
				{
					gLog.Warning("%s: missing form entry", __FUNCTION__);
					continue;
				}

				Th tmp;

				if (!pholder.Parse(e["data"], tmp))
				{
					gLog.Warning("%s: failed to parse record data", __FUNCTION__);
					continue;
				}

				auto r = a_out.try_emplace(form, std::move(tmp));

				if (!r.second)
				{
					gLog.Warning("%s: duplicate form %.8X in list", __FUNCTION__, form);
				}
			}
		}

		template <class Th, class Tm>
		constexpr void CreateConfigMap(
			const Tm&     a_data,
			Json::Value&  a_out,
			std::uint32_t a_version,
			ParserState&  a_state)
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::arrayValue));

			Parser<Game::FormID> pform(a_state);
			Parser<Th>           pholder(a_state);

			for (auto& [i, e] : a_data)
			{
				auto& v = data.append(Json::Value());

				pform.Create(i, v["form"]);
				pholder.Create(e, v["data"]);
			}

			a_out["version"] = a_version;
		}

		template <class Th, class Tm, class Ts>
		[[nodiscard]] constexpr bool ParseConfigStore(
			const Json::Value& a_in,
			Ts&                a_out,
			std::uint32_t      a_version,
			ParserState&       a_state)
		{
			Parser<Tm> pmap(a_state);
			Parser<Th> pholder(a_state);

			auto& data = a_in["data"];

			auto& g = a_out.GetFormMaps();

			if (!pmap.Parse(data["actor"], g[stl::underlying(Data::ConfigClass::Actor)]))
			{
				return false;
			}

			if (!pmap.Parse(data["npc"], g[stl::underlying(Data::ConfigClass::NPC)]))
			{
				return false;
			}

			if (!pmap.Parse(data["race"], g[stl::underlying(Data::ConfigClass::Race)]))
			{
				return false;
			}

			if (!pholder.Parse(
					data["default_player"],
					a_out.GetGlobalData(Data::GlobalConfigType::Player)))
			{
				return false;
			}

			if (!pholder.Parse(
					data["default_npc"],
					a_out.GetGlobalData(Data::GlobalConfigType::NPC)))
			{
				return false;
			}

			return true;
		}

		template <class Th, class Tm, class Ts>
		constexpr void CreateConfigStore(
			const Ts&     a_data,
			Json::Value&  a_out,
			std::uint32_t a_version,
			ParserState&  a_state)
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			Parser<Tm> pmap(a_state);
			Parser<Th> pholder(a_state);

			auto& g = a_data.GetFormMaps();

			pmap.Create(g[stl::underlying(Data::ConfigClass::Actor)], data["actor"]);
			pmap.Create(g[stl::underlying(Data::ConfigClass::NPC)], data["npc"]);
			pmap.Create(g[stl::underlying(Data::ConfigClass::Race)], data["race"]);

			pholder.Create(
				a_data.GetGlobalData(Data::GlobalConfigType::Player),
				data["default_player"]);

			pholder.Create(
				a_data.GetGlobalData(Data::GlobalConfigType::NPC),
				data["default_npc"]);

			a_out["version"] = a_version;
		}

	}
}