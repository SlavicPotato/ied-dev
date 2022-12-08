#pragma once

#include "UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIAboutModal
		{
		public:
			UIAboutModal(Controller& a_controller);

			void QueueAboutPopup();

		private:
			Controller& m_controller;
		};
	}
}