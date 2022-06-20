#pragma once

#include "Window/UIWindowBase.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIContextBase :
			public UIWindowBase
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

			virtual void OnOpen(){};
			virtual void OnClose(){};

		protected:
			Controller& m_controller;
		};
	}
}