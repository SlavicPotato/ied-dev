#pragma once

#include "../Form/UIFormSelectorWidget.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIActorInfoAddInterface 
		{
		public:
			UIActorInfoAddInterface(Controller& a_controller);

			void DrawActorInfoAdd();
			void QueueUpdateActorInfo(Game::FormID a_handle);

		private:
			virtual void OnActorInfoAdded(Game::FormID a_handle) = 0;

			UIFormSelectorWidget m_formSelector;

			Game::FormID m_form;

			Controller& m_controller;
		};
	}
}