#pragma once

#include "IED/ConfigCommon.h"

namespace IED
{
	namespace UI
	{
		class UICountRangeWidget
		{
		public:
			UICountRangeWidget() noexcept = default;

			static bool DrawCountRange(
				Data::configRange_t& a_configRange);

		private:
			static bool DrawCountRangeContextMenu(
				Data::configRange_t& a_configRange);
		};

	}
}