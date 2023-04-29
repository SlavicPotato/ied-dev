#include "pch.h"

#include "JSONStringTableDataParser.h"

#include "JSONFontGlyphDataParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Localization::StringTable::table_data>::Parse(
			const Json::Value&                     a_in,
			Localization::StringTable::table_data& a_out) const
		{
			JSON_PARSE_VERSION()

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

				unsigned long lid;

				try
				{
					lid = std::stoul(key);
				}
				catch (const std::exception& e)
				{
					Error("%s: bad key '%s': %s", __FUNCTION__, key.c_str(), e.what());
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
				Parser<FontGlyphData> gparser(m_state);

				auto tmp = std::make_shared<FontGlyphData>();

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
		void Parser<Localization::StringTable::table_data>::Create(
			const Localization::StringTable::table_data& a_data,
			Json::Value&                                 a_out) const
		{
		}

	}
}