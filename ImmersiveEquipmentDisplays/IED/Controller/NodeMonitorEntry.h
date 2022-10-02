#pragma once

#include "IED/ConfigNodeMonitor.h"

namespace IED
{
	class NodeMonitorEntry
	{
	public:
		inline NodeMonitorEntry(
			NiNode*                                 a_parent,
			const Data::configNodeMonitorEntryBS_t& a_config) :
			m_parent(a_parent),
			m_config(a_config)
		{
		}

		bool Update();

		[[nodiscard]] inline constexpr bool IsPresent() const noexcept
		{
			return m_present;
		}

	private:
		bool m_present{ false };

		NiPointer<NiNode>                       m_parent;
		const Data::configNodeMonitorEntryBS_t& m_config;
	};
}