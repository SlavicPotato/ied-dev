#pragma once

#include "IED/SettingHolder.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UISettingsInterface
		{
		public:
			UISettingsInterface(Controller& a_controller);

		protected:
			Data::SettingHolder& GetSettings() const noexcept;
			Data::configStore_t& GetConfigStore() const noexcept;

		private:
			Controller& m_controller;
		};

	}
}