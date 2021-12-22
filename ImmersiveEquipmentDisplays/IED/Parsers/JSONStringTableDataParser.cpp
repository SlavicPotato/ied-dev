#include "pch.h"

#include "JSONStringTableDataParser.h"

#include "JSONFontGlyphDataParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Localization::StringTable::data_storage_type>::Parse(
			const Json::Value& a_in,
			Localization::StringTable::data_storage_type& a_out) const
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				Error("%s: bad version data", __FUNCTION__);
				return false;
			}

			a_out.lang = a_in["lang"].asString();

			if (a_out.lang.empty())
			{
				Error("%s: missing language", __FUNCTION__);
				return false;
			}

			auto& data = a_in["data"];

			for (auto it = data.begin(); it != data.end(); ++it)
			{
				auto key = it.key().asString();

				char* end = nullptr;

				auto lid = std::strtoul(key.c_str(), &end, 0);

				if (!(end > key.c_str()))
				{
					Error("%s: bad key: %s", __FUNCTION__, key.c_str());
					SetHasErrors();
					continue;
				}

				if (lid > std::numeric_limits<Localization::StringID>::max())
				{
					Error("%s: [%s] id out of range: %lu", __FUNCTION__, key.c_str(), lid);
					SetHasErrors();
					continue;
				}

				auto r = a_out.data.emplace(static_cast<Localization::StringID>(lid), it->asString());

				if (!r.second)
				{
					Warning("%s: duplicate id %hu", __FUNCTION__, r.first->first);
				}
			}

			if (a_in.isMember("glyphs"))
			{
				Parser<fontGlyphData_t> gparser(m_state);

				auto tmp = std::make_unique<fontGlyphData_t>();

				if (!gparser.Parse(
						a_in["glyphs"],
						*tmp))
				{
					return false;
				}

				a_out.glyph_data = std::move(tmp);
			}

			return true;
		}

		template <>
		void Parser<Localization::StringTable::data_storage_type>::Create(
			const Localization::StringTable::data_storage_type& a_data,
			Json::Value& a_out) const
		{
			/*auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			for (auto& e : a_data.data)
			{
				a_out[std::to_string(e.first)] = e.second;
			}

			a_out["lang"] = *a_data.lang;
			a_out["version"] = 1u;*/
		}

		template <>
		void Parser<Localization::StringTable::data_storage_type>::GetDefault(
			Localization::StringTable::data_storage_type& a_out) const
		{}

	}
}