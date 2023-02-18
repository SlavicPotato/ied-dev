#pragma once

#include "IED/UI/UIData.h"

#include "Localization/Common.h"

namespace IED
{
	namespace UI
	{
		class UIControlKeySelectorWidget
		{
			enum class UIControlKeySelectorContextAction
			{
				None,

				ManualChange
			};

		public:
			static bool DrawKeySelector(
				const char*                      a_strid,
				Localization::StringID           a_label,
				const UIData::controlDescData_t& a_data,
				std::uint32_t&                   a_key,
				bool                             a_allowNone = false);

		private:
			static UIControlKeySelectorContextAction DrawContextMenu(std::uint32_t& a_key, bool a_allowNone);
		};
	}
}
