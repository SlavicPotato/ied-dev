#include "pch.h"

#include "AnimationUpdateManager.h"

//#include "EngineExtensions.h"

namespace IED
{
	AnimationUpdateController AnimationUpdateController::m_Instance;

	void AnimationUpdateController::Initialize() noexcept
	{
		m_enabled = true;
	}

	std::size_t AnimationUpdateController::GetNumObjects() const noexcept
	{
		const shared_lock lock(m_lock);

		std::size_t i = 0;

		for (auto& e : m_data)
		{
			std::for_each(
				e.second.begin(),
				e.second.end(),
				[&](auto&) { i++; });
		}

		return i;
	}

	void AnimationUpdateController::OnUpdate(
		Actor*                       a_actor,
		const BSAnimationUpdateData& a_data) const noexcept
	{
		const shared_lock lock(m_lock);

		auto it = m_data.find(a_actor->formID);
		if (it != m_data.end())
		{
			for (auto& e : it->second)
			{
				UpdateAnimationGraph(e.get(), a_data);
			}
		}
	}

	void AnimationUpdateController::AddObject(
		Game::FormID                                    a_actor,
		const RE::WeaponAnimationGraphManagerHolderPtr& a_ptr) noexcept
	{
		if (m_enabled)
		{
			const unique_lock lock(m_lock);

			auto& e = m_data.try_emplace(a_actor).first->second;

			e.emplace_front(a_ptr);
		}
	}

	void AnimationUpdateController::RemoveObject(
		Game::FormID                                    a_actor,
		const RE::WeaponAnimationGraphManagerHolderPtr& a_ptr) noexcept
	{
		if (m_enabled)
		{
			const unique_lock lock(m_lock);

			auto it = m_data.find(a_actor);
			if (it != m_data.end())
			{
				std::erase(it->second, a_ptr);
			}
		}
	}

	void AnimationUpdateController::RemoveActor(
		Game::FormID a_actor) noexcept
	{
		if (m_enabled)
		{
			const unique_lock lock(m_lock);

			m_data.erase(a_actor);
		}
	}

}