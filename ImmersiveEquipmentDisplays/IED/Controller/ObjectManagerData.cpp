#include "pch.h"

#include "ObjectManagerData.h"

#include "ActorObjectHolder.h"

namespace IED
{
	void ObjectManagerData::ClearVariablesOnAll(bool a_requestEval) noexcept
	{
		for (auto& e : m_objects)
		{
			e.second.ClearVariables(a_requestEval);
		}
	}

	void ObjectManagerData::ClearVariables(
		Game::FormID a_handle,
		bool         a_requestEval) noexcept
	{
		auto it = m_objects.find(a_handle);
		if (it != m_objects.end())
		{
			it->second.ClearVariables(a_requestEval);
		}
	}

	void ObjectManagerData::RequestVariableUpdateOnAll() const noexcept
	{
		for (auto& e : m_objects)
		{
			e.second.RequestVariableUpdate();
		}
	}

	void ObjectManagerData::RequestVariableUpdate(Game::FormID a_handle) const noexcept
	{
		auto it = m_objects.find(a_handle);
		if (it != m_objects.end())
		{
			it->second.RequestVariableUpdate();
		}
	}

	void ObjectManagerData::RequestEvaluateOnAll() const noexcept
	{
		for (auto& e : m_objects)
		{
			e.second.RequestEvalDefer();
		}
	}

	void ObjectManagerData::RequestEvaluateAll(bool a_defer) const noexcept
	{
		for (auto& e : m_objects)
		{
			if (a_defer)
			{
				e.second.RequestEvalDefer();
			}
			else
			{
				e.second.RequestEval();
			}
		}
	}

	void ObjectManagerData::RequestLFEvaluateAll() noexcept
	{
		for (auto& e : m_objects)
		{
			e.second.m_wantLFUpdate = true;
		}
	}

	void ObjectManagerData::RequestLFEvaluateAll(Game::FormID a_skip) noexcept
	{
		for (auto& [i, e] : m_objects)
		{
			if (i != a_skip)
			{
				e.m_wantLFUpdate = true;
			}
		}
	}

	void ObjectManagerData::RequestHFEvaluateAll() noexcept
	{
		for (auto& e : m_objects)
		{
			e.second.m_wantHFUpdate = true;
		}
	}

	void ObjectManagerData::RequestHFEvaluateAll(Game::FormID a_skip) noexcept
	{
		for (auto& [i, e] : m_objects)
		{
			if (i != a_skip)
			{
				e.m_wantHFUpdate = true;
			}
		}
	}

	/*void ObjectManagerData::StorePlayerState(ActorObjectHolder& a_holder)
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
	}*/

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