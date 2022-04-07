#include "pch.h"

#include "JSONConfigSoundPairParser.h"
#include "JSONConfigSoundParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 2;

		template <>
		bool Parser<Data::ConfigSound<Game::FormID>>::Parse(
			const Json::Value&               a_in,
			Data::ConfigSound<Game::FormID>& a_out) const
		{
			JSON_PARSE_VERSION();

			Parser<Data::ConfigSound<Game::FormID>::soundPair_t> pparser(m_state);

			auto& data = a_in["data"];

			if (auto& forms = data["forms"])
			{
				if (version > 1)
				{
					for (auto it = forms.begin(); it != forms.end(); ++it)
					{
						auto key = it.key().asString();

						unsigned long formType;

						try
						{
							formType = std::stoul(key);
						}
						catch (const std::exception& e)
						{
							Error("%s: bad key '%s': %s", __FUNCTION__, key.c_str(), e.what());
							SetHasErrors();
							continue;
						}

						if (formType > std::numeric_limits<std::uint8_t>::max())
						{
							Error("%s: [%s] form type out of range: %lu", __FUNCTION__, key.c_str(), formType);
							SetHasErrors();
							continue;
						}

						if (!pparser.Parse(*it, a_out.data.try_emplace(static_cast<std::uint8_t>(formType)).first->second))
						{
							return false;
						}
					}
				}
				else
				{
					if (!pparser.Parse(forms["armor"], a_out.data.try_emplace(TESObjectARMO::kTypeID).first->second))
					{
						return false;
					}

					if (!pparser.Parse(forms["arrow"], a_out.data.try_emplace(TESAmmo::kTypeID).first->second))
					{
						return false;
					}

					if (!pparser.Parse(forms["weapon"], a_out.data.try_emplace(TESObjectWEAP::kTypeID).first->second))
					{
						return false;
					}

					if (!pparser.Parse(forms["generic"], a_out.data.try_emplace(TESForm::kTypeID).first->second))
					{
						return false;
					}
				}
			}

			a_out.enabled = data.get("enable", false).asBool();
			a_out.npc     = data.get("npc", false).asBool();

			return true;
		}

		template <>
		void Parser<Data::ConfigSound<Game::FormID>>::Create(
			const Data::ConfigSound<Game::FormID>& a_data,
			Json::Value&                           a_out) const
		{
			auto& data  = (a_out["data"] = Json::Value(Json::ValueType::objectValue));
			auto& forms = (data["forms"] = Json::Value(Json::ValueType::objectValue));

			Parser<Data::ConfigSound<Game::FormID>::soundPair_t> pparser(m_state);

			for (auto& e : a_data.data)
			{
				pparser.Create(e.second, forms[std::to_string(e.first)]);
			}

			data["enable"] = a_data.enabled;
			data["npc"]    = a_data.npc;

			a_out["version"] = CURRENT_VERSION;
		}

	}
}