#pragma once

#include "GearNodeID.h"
#include "WeaponPlacementID.h"

namespace IED
{
	// acquiring the (non-shared) global lock in oar condition eval is a no-no due to thread contention, hence this
	class GearNodeData
	{
	public:
		struct Entry
		{
			struct Node
			{
				NiPointer<NiNode> node;
				WeaponPlacementID currentPlacement{ WeaponPlacementID::None };
			};

			stl::cache_aligned::flat_map<GearNodeID, Node> nodes;
		};

		using lock_type        = stl::shared_mutex;
		using read_lock_guard  = stl::read_lock_guard<lock_type>;
		using write_lock_guard = stl::write_lock_guard<lock_type>;

		[[nodiscard]] static constexpr auto& GetSingleton() noexcept
		{
			return m_Instance;
		}

		template <class... Args>
		void SetNodeEntry(Game::FormID a_actor, GearNodeID a_nodeID, Args&&... a_args);

		void                       SetPlacement(Game::FormID a_actor, GearNodeID a_nodeID, WeaponPlacementID a_placementID);
		std::optional<Entry::Node> GetNodeEntry(Game::FormID a_actor, GearNodeID a_nodeID) const;

		void RemoveActor(Game::FormID a_actor) noexcept;

	private:
		stl::unordered_map<Game::FormID, Entry> m_data;

		mutable lock_type m_lock;

		static GearNodeData m_Instance;
	};

	template <class... Args>
	void GearNodeData::SetNodeEntry(Game::FormID a_actor, GearNodeID a_nodeID, Args&&... a_args)
	{
		const write_lock_guard lock(m_lock);

		const auto r = m_data
		                   .try_emplace(a_actor)
		                   .first->second.nodes
		                   .insert_or_assign(a_nodeID, Entry::Node{ std::forward<Args>(a_args)... });
	}
}