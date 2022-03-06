#pragma once

namespace IED
{
	namespace UI
	{
		class UIPopupToggleButtonWidget
		{
		public:
			static bool DrawPopupToggleButton(
				const char* a_id,
				const char* a_popupId);

			static bool DrawPopupToggleButtonNoOpen(
				const char* a_id,
				const char* a_popupId);

		private:
		};
	}
}