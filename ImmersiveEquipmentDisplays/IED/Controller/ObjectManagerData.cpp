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

	void ObjectManagerData::RequestLFEvaluateAll() const noexcept
	{
		for (auto& e : m_objects)
		{
			e.second.m_wantLFUpdate = true;
		}
	}

	void ObjectManagerData::RequestLFEvaluateAll(
		Game::FormID a_skip) const noexcept
	{
		for (auto& [i, e] : m_objects)
		{
			if (i != a_skip)
			{
				e.m_wantLFUpdate = true;
			}
		}
	}

	void ObjectManagerData::RequestHFEvaluateAll() const noexcept
	{
		for (auto& e : m_objects)
		{
			e.second.m_wantHFUpdate = true;
		}
	}

	void ObjectManagerData::RequestHFEvaluateAll(
		Game::FormID a_skip) const noexcept
	{
		for (auto& [i, e] : m_objects)
		{
			if (i != a_skip)
			{
				e.m_wantHFUpdate = true;
			}
		}
	}

}