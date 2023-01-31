#include "pch.h"

#include "ObjectManagerData.h"

#include "ActorObjectHolder.h"

namespace IED
{
	void ObjectManagerData::ClearVariablesOnAll(bool a_requestEval) noexcept
	{
		for (auto& e : m_actorMap)
		{
			e.second.ClearVariables(a_requestEval);
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
		for (auto& e : m_actorMap)
		{
			e.second.RequestVariableUpdate();
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

	void ObjectManagerData::RequestEvaluateOnAll() const noexcept
	{
		for (auto& e : m_actorMap)
		{
			e.second.RequestEvalDefer();
		}
	}

	void ObjectManagerData::RequestEvaluateAll(bool a_defer) const noexcept
	{
		for (auto& e : m_actorMap)
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

	void ObjectManagerData::RequestLFEvaluateAll() const noexcept
	{
		for (auto& e : m_actorMap)
		{
			e.second.m_wantLFUpdate = true;
		}
	}

	void ObjectManagerData::RequestLFEvaluateAll(
		Game::FormID a_skip) const noexcept
	{
		for (auto& [i, e] : m_actorMap)
		{
			if (i != a_skip)
			{
				e.m_wantLFUpdate = true;
			}
		}
	}

	void ObjectManagerData::RequestHFEvaluateAll() const noexcept
	{
		for (auto& e : m_actorMap)
		{
			e.second.m_wantHFUpdate = true;
		}
	}

	void ObjectManagerData::RequestHFEvaluateAll(
		Game::FormID a_skip) const noexcept
	{
		for (auto& [i, e] : m_actorMap)
		{
			if (i != a_skip)
			{
				e.m_wantHFUpdate = true;
			}
		}
	}

}