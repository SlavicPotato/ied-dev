#pragma once

#include "UITipsData.h"
#include "UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UITipsInterface : 
			public virtual UILocalizationInterface
		{
		public:
			UITipsInterface(Controller& a_controller);

		protected:
			void DrawTip(UITip a_id) const;
			void DrawTip(const char* a_text) const;
			const char* GetTipText(UITip a_id) const;

		private:
			void DrawTipText(const char* a_text) const;
		};

	}
}