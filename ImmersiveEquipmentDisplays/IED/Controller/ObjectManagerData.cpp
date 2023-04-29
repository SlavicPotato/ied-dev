#include "pch.h"

#include "ObjectManagerData.h"

#include "ActorObjectHolder.h"

namespace IED
{
	void ObjectManagerData::ClearVariablesOnAll(bool a_requestEval) noexcept
	{
		for (auto& e : m_actorMap.getvec())
		{
			e->second.ClearVariables(a_requestEval);
		}
	}

	void ObjectManagerData::ClearVariables(
		Game::FormID a_handle,
		bool         a_requestEval) noexcept
	{
		auto it = m_actorMap.find(a_handle);
		if (it != m_actorMap.end())
		{
			it->second.ClearVariables(a_requestEval);
		}
	}

	void ObjectManagerData::RequestVariableUpdateOnAll() const noexcept
	{
		for (auto& e : m_actorMap.getvec())
		{
			e->second.RequestVariableUpdate();
		}
	}

	void ObjectManagerData::RequestVariableUpdate(Game::FormID a_handle) const noexcept
	{
		auto it = m_actorMap.find(a_handle);
		if (it != m_actorMap.end())
		{
			it->second.RequestVariableUpdate();
		}
	}

	void ObjectManagerData::RequestEvaluateAll() const noexcept
	{
		for (auto& e : m_actorMap.getvec())
		{
			e->second.RequestEvalDefer();
		}
	}

	void ObjectManagerData::RequestEvaluateAll(bool a_defer) const noexcept
	{
		for (auto& e : m_actorMap.getvec())
		{
			if (a_defer)
			{
				e->second.RequestEvalDefer();
			}
			else
			{
				e->second.RequestEval();
			}
		}
	}

	void ObjectManagerData::RequestLFEvaluateAll() const noexcept
	{
		for (auto& e : m_actorMap.getvec())
		{
			e->second.m_wantLFUpdate = true;
		}
	}

	std::size_t ObjectManagerData::GetNumQueuedModels() const noexcept
	{
		std::size_t result = 0;

		for (auto& e : m_actorMap.getvec())
		{
			result += e->second.GetNumQueuedModels();
		}

		return result;
	}

	void ObjectManagerData::RequestLFEvaluateAll(
		Game::FormID a_skip) const noexcept
	{
		for (auto& e : m_actorMap.getvec())
		{
			if (e->first != a_skip)
			{
				e->second.m_wantLFUpdate = true;
			}
		}
	}

	void ObjectManagerData::RequestHFEvaluateAll() const noexcept
	{
		for (auto& e : m_actorMap.getvec())
		{
			e->second.m_wantHFUpdate = true;
		}
	}

	void ObjectManagerData::RequestHFEvaluateAll(
		Game::FormID a_skip) const noexcept
	{
		for (auto& e : m_actorMap.getvec())
		{
			if (e->first != a_skip)
			{
				e->second.m_wantHFUpdate = true;
			}
		}
	}

}