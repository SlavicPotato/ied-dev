#include "pch.h"

#include "ObjectManagerData.h"

#include "ActorObjectHolder.h"

namespace IED
{
	void ObjectManagerData::StorePlayerState(ActorObjectHolder& a_holder)
	{
		if (m_playerState)
		{
			*m_playerState = a_holder;
		}
		else
		{
			m_playerState = std::make_unique<Data::actorStateEntry_t>(a_holder);
		}
	}

	void ObjectManagerData::ApplyActorState(
		ActorObjectHolder& a_holder)
	{
		if (a_holder.m_actor == *g_thePlayer && m_playerState)
		{
			a_holder.ApplyActorState(*m_playerState);
		}
		else
		{
			auto it = m_storedActorStates.data.find(a_holder.m_actor->formID);
			if (it != m_storedActorStates.data.end())
			{
				a_holder.ApplyActorState(it->second);

				m_storedActorStates.data.erase(it);
			}
		}
	}

	/*void AnimationGraphManagerHolderList::Update(const BSAnimationUpdateData& a_data) const
	{
		stl::scoped_lock lock(m_lock);

		UpdateNoLock(a_data);
	}

	void AnimationGraphManagerHolderList::UpdateNoLock(const BSAnimationUpdateData& a_data) const
	{
		for (auto& e : m_data)
		{
			EngineExtensions::UpdateAnimationGraph(e.get(), a_data);
		}
	}*/

}