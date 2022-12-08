#pragma once

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UINPCInfoAddInterface
		{
		public:
			UINPCInfoAddInterface(Controller& a_controller);

			void DrawNPCInfoAdd(Game::FormID a_current = {});

			void QueueUpdateNPCInfo(Game::FormID a_handle);

		private:
			virtual void OnNPCInfoAdded(Game::FormID a_npc) = 0;

			Controller& m_controller;
		};
	}
}