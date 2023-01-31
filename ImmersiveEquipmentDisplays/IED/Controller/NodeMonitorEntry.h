#pragma once

#include "IED/ConfigNodeMonitor.h"

namespace IED
{
	class NodeMonitorEntry
	{
	public:
		inline NodeMonitorEntry(
			NiNode*                                 a_parent,
			const Data::configNodeMonitorEntryBS_t& a_config) noexcept :
			m_parent(a_parent),
			m_config(a_config)
		{
		}

		bool Update() noexcept;

		[[nodiscard]] constexpr bool IsPresent() const noexcept
		{
			return m_present;
		}

	private:
		bool m_present{ false };

		const NiPointer<NiNode>                 m_parent;
		const Data::configNodeMonitorEntryBS_t& m_config;
	};
}