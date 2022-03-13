#pragma once

#include "Window/UIWindow.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIContextBase :
			public UIWindow
		{
		public:
			UIContextBase(Controller& a_controller) :
				m_controller(a_controller)
			{
			}

			virtual ~UIContextBase() noexcept = default;

			virtual void Initialize(){};
			virtual void Draw() = 0;
			virtual void Reset(){};

			virtual void OnOpen(){};
			virtual void OnClose(){};

		protected:
			Controller& m_controller;
		};
	}
}