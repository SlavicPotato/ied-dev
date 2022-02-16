#pragma once

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UINPCInfoAddInterface :
			public virtual UILocalizationInterface
		{
		public:
			UINPCInfoAddInterface(Controller& a_controller);

			void DrawNPCInfoAdd(Game::FormID a_current = {});

		private:
			virtual void OnNPCInfoAdded(Game::FormID a_npc) = 0;

			Controller& m_controller;
		};
	}
}