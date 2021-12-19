#include "pch.h"

#include "JSONFontGlyphDataParser.h"
#include "JSONFontInfoEntryParser.h"

#include <ext/IOS.h>

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<fontInfoEntry_t>::Parse(
			const Json::Value& a_in,
			fontInfoEntry_t& a_out,
			std::uint32_t a_version) const
		{
			auto file = a_in["file"].asString();

			if (file.empty())
			{
				Error("%s: empty font filename", __FUNCTION__);
				return false;
			}

			bool win_font = a_in.get("win_font", false).asBool();

			if (win_font)
			{
				if (fs::path(file).is_absolute())
				{
					a_out.path = std::move(file);
				}
				else
				{
					fs::path tmp;

					if (!WinApi::get_windows_path(tmp))
					{
						Error("%s: failed getting windows path", __FUNCTION__);
						return false;
					}

					tmp /= "Fonts";
					tmp /= file;

					a_out.path = tmp.string();
				}
			}
			else
			{
				a_out.path = std::move(file);
			}

			Parser<fontGlyphData_t> gparser;

			if (!gparser.Parse(a_in, a_out))
			{
				return false;
			}

			a_out.size = std::max(a_in["size"].asFloat(), 0.1f);

			return true;
		}

		template <>
		void Parser<fontInfoEntry_t>::Create(
			const fontInfoEntry_t& a_data,
			Json::Value& a_out) const
		{
		}

		template <>
		void Parser<fontInfoEntry_t>::GetDefault(
			fontInfoEntry_t& a_out) const
		{}

	}
}