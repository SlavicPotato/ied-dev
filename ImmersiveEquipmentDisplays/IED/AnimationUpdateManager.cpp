#include "pch.h"

#include "AnimationUpdateManager.h"

#include "Controller/Controller.h"

namespace IED
{
	void AnimationUpdateManager::BeginAnimationUpdate(
		Controller* a_controller)
	{
		auto prev = m_running.exchange(true);
		ASSERT(prev == false);

		a_controller->GetLock().lock();
	}

	void AnimationUpdateManager::EndAnimationUpdate(
		Controller* a_controller)
	{
		auto prev = m_running.exchange(false);
		ASSERT(prev == true);

		a_controller->GetLock().unlock();
	}

	void AnimationUpdateManager::ProcessAnimationUpdateList(
		Actor*                       a_actor,
		const BSAnimationUpdateData& a_data,
		Controller*                  a_controller)
	{
		ASSERT(m_running.load() == true);

		auto& data = a_controller->GetData();

		auto it = data.find(a_actor->formID);
		if (it != data.end())
		{
			it->second.UpdateAllAnimationGraphs(a_data);
		}
	}

	void AnimationUpdateManager::UpdateActorAnimationList(
		Actor*                       a_actor,
		const BSAnimationUpdateData& a_data,
		Controller*                  a_controller)
	{
		stl::scoped_lock lock(a_controller->GetLock());

		auto& data = a_controller->GetData();

		auto it = data.find(a_actor->formID);
		if (it != data.end())
		{
			it->second.UpdateAllAnimationGraphs(a_data);
		}
	}

}