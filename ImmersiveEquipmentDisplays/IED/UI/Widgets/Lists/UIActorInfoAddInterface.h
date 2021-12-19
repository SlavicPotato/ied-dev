#pragma once

#include "../Form/UIFormSelectorWidget.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIActorInfoAddInterface : 
			public virtual UILocalizationInterface
		{
		public:
			UIActorInfoAddInterface(Controller& a_controller);

			void DrawActorInfoAdd();

		private:
			virtual void OnActorInfoAdded(Game::FormID a_handle) = 0;

			UIFormSelectorWidget m_formSelector;

			Game::FormID m_form;

			Controller& m_controller;
		};
	}
}