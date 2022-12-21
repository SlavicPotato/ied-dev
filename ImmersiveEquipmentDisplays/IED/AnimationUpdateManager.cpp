#include "pch.h"

#include "AnimationUpdateManager.h"

#include "Controller/Controller.h"

namespace IED
{
	void AnimationUpdateManager::BeginAnimationUpdate(
		Controller* a_controller) noexcept
	{
		assert(m_running.exchange(true) == false);

		a_controller->GetLock().lock();
	}

	void AnimationUpdateManager::EndAnimationUpdate(
		Controller* a_controller) noexcept
	{
		assert(m_running.exchange(false) == true);

		a_controller->GetLock().unlock();
	}

	void AnimationUpdateManager::ProcessAnimationUpdateList(
		Actor*                       a_actor,
		const BSAnimationUpdateData& a_data,
		const Controller&            a_controller) noexcept
	{
		assert(m_running.load() == true);

		const auto& data = a_controller.GetObjects();

		auto it = data.find(a_actor->formID);
		if (it != data.end())
		{
			it->second.UpdateAllAnimationGraphs(a_data);
		}
	}

	void AnimationUpdateManager::UpdateActorAnimationList(
		Actor*                       a_actor,
		const BSAnimationUpdateData& a_data,
		const Controller&            a_controller) noexcept
	{
		const boost::lock_guard lock(a_controller.GetLock());

		auto& data = a_controller.GetObjects();

		auto it = data.find(a_actor->formID);
		if (it != data.end())
		{
			it->second.UpdateAllAnimationGraphs(a_data);
		}
	}

}