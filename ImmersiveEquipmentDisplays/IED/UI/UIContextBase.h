#pragma once

#include "Window/UIWindowBase.h"

#include "Drivers/Input/Handlers.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIContextBase :
			public UIChildWindowBase
		{
		public:
			UIContextBase(Controller& a_controller) :
				m_controller(a_controller)
			{
			}

			virtual ~UIContextBase() noexcept = default;

			virtual void Initialize(){};
			virtual void Draw() = 0;
			virtual void PrepareGameData(){};
			virtual void Render(){};
			virtual void Reset(){};
			virtual void OnMouseMove(const Handlers::MouseMoveEvent& a_evn){};

			virtual void OnOpen(){};
			virtual void OnClose(){};

		protected:
			Controller& m_controller;
		};
	}
}