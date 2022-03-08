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
			fontInfoEntry_t&   a_out,
			std::uint32_t      a_version) const
		{
			fs::path path(str_conv::str_to_wstr(a_in["file"].asString()));

			if (path.empty())
			{
				Error("%s: empty font filename", __FUNCTION__);
				return false;
			}

			bool win_font = a_in.get("win_font", false).asBool();

			if (win_font && !path.is_absolute())
			{
				fs::path tmp;

				if (!WinApi::get_windows_path(tmp))
				{
					Error("%s: failed getting windows path", __FUNCTION__);
					return false;
				}

				tmp /= "Fonts";
				tmp /= path;

				a_out.path = str_conv::wstr_to_str(tmp.wstring());
			}
			else
			{
				a_out.path = str_conv::wstr_to_str(path.wstring());
			}

			Parser<fontGlyphData_t> gparser(m_state);

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
			Json::Value&           a_out) const
		{
		}

	}
}