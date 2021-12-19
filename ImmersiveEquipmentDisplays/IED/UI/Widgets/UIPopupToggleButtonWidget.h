#pragma once

namespace IED
{
	namespace UI
	{
		class UIPopupToggleButtonWidget
		{
		public:
			bool DrawPopupToggleButton(
				const char* a_id,
				const char* a_popupId);
			
			bool DrawPopupToggleButtonNoOpen(
				const char* a_id,
				const char* a_popupId);

		private:
		};
	}
}