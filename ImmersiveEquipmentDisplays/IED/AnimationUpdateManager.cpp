#include "pch.h"

#include "AnimationUpdateManager.h"

#include "Controller/Controller.h"

namespace IED
{
	void AnimationUpdateManager::PrepareAnimationUpdateList(
		Controller* a_controller)
	{
		stl::scoped_lock lock(m_lock);  // REMOVE ME

		assert(!m_running.load(std::memory_order_relaxed));
		assert(m_data.empty());

		/*PerfTimer pt;
		pt.Start();*/

		{
			stl::scoped_lock ctrllock(a_controller->GetLock());

			auto& data = a_controller->GetData();

			m_data.reserve(data.size());

			for (auto& e : data)
			{
				if (e.first == 0x14)
				{
					continue;
				}

				m_data.emplace_back(
					e.first.get(),
					e.second.GetAnimationUpdateList());
			}
		}

		std::sort(
			m_data.begin(),
			m_data.end(),
			[](const auto& a_lhs, const auto& a_rhs) {
				return a_lhs.first < a_rhs.first;
			});

		//_DMESSAGE("%f", pt.Stop());

		m_running.store(true, std::memory_order_relaxed);
	}

	void AnimationUpdateManager::ClearAnimationUpdateList()
	{
		stl::scoped_lock lock(m_lock);  // REMOVE ME

		assert(m_running.load(std::memory_order_relaxed));

		m_running.store(false, std::memory_order_relaxed);

		m_data.clear();
	}

	void AnimationUpdateManager::UpdateQueuedAnimationList(
		Actor*                       a_actor,
		const BSAnimationUpdateData& a_data)
	{
		if (!m_running.load(std::memory_order_relaxed))
		{
			return;
		}

		/*PerfTimer pt;
		pt.Start();*/

		auto it = std::lower_bound(
			m_data.begin(),
			m_data.end(),
			a_actor->formID.get(),
			[](auto& a_lhs, auto& a_rhs) {
				return a_lhs.first < a_rhs;
			});

		if (it != m_data.end() &&
		    it->first == a_actor->formID)
		{
			it->second->Update(a_data);
		}

		//_DMESSAGE("%f", pt.Stop());
	}

	void AnimationUpdateManager::UpdateActorAnimationList(
		Actor*                       a_actor,
		const BSAnimationUpdateData& a_data,
		Controller*                  a_controller)
	{
		if (auto list = GetActorAnimationUpdateList(a_actor, a_controller))
		{
			list->Update(a_data);
		}
	}

	auto AnimationUpdateManager::GetActorAnimationUpdateList(
		Actor*      a_actor,
		Controller* a_controller)
		-> std::shared_ptr<AnimationGraphManagerHolderList>
	{
		stl::scoped_lock lock(a_controller->GetLock());

		auto& data = a_controller->GetData();

		auto it = data.find(a_actor->formID);
		if (it != data.end())
		{
			return it->second.GetAnimationUpdateList();
		}
		else
		{
			return {};
		}
	}

}