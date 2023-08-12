#include "pch.h"

#include "GearNodeData.h"

namespace IED
{
	GearNodeData GearNodeData::m_Instance;

	void GearNodeData::SetPlacement(
		Game::FormID      a_actor,
		GearNodeID        a_nodeID,
		WeaponPlacementID a_placementID)
	{
		const write_lock_guard lock(m_lock);

		const auto r = m_data
		                   .try_emplace(a_actor)
		                   .first->second.nodes
		                   .try_emplace(a_nodeID);

		r.first->second.currentPlacement = a_placementID;
	}

	auto GearNodeData::GetNodeEntry(
		Game::FormID a_actor,
		GearNodeID   a_nodeID) const
		-> std::optional<Entry::Node>
	{
		const read_lock_guard lock(m_lock);

		auto ita = m_data.find(a_actor);
		if (ita == m_data.end())
		{
			return {};
		}

		auto ite = ita->second.nodes.find(a_nodeID);
		return ite != ita->second.nodes.end() ? 
			ite->second : 
			std::optional<Entry::Node>{};
	}

	void GearNodeData::RemoveActor(Game::FormID a_actor) noexcept
	{
		const write_lock_guard lock(m_lock);

		m_data.erase(a_actor);
	}
}