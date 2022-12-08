#pragma once

#include "IED/ConfigNodeMonitor.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	namespace UI
	{
		class UINodeMonitorSelectorWidget 
		{
		public:
			UINodeMonitorSelectorWidget() = default;

			bool DrawNodeMonitorSelector(std::uint32_t& a_uid) const;

		protected:
			const char* get_nodemon_desc(std::uint32_t a_uid) const;
			const char* get_nodemon_desc(const Data::configNodeMonitorEntryBS_t &a_entry) const;

		private:
			mutable char m_buf[256]{ 0 };
		};
	}

}
