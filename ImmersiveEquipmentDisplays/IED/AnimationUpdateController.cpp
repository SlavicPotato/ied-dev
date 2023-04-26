#include "pch.h"

#include "AnimationUpdateController.h"

#include "Controller/ObjectCloningTask.h"
#include "StringHolder.h"

#include <ext/GarbageCollector.h>

namespace IED
{
	AnimationUpdateController AnimationUpdateController::m_Instance;

	void AnimationUpdateController::Initialize() noexcept
	{
		m_initialized = true;
	}

	std::size_t AnimationUpdateController::GetNumObjects() const noexcept
	{
		const read_lock_guard lock(m_lock);

		std::size_t i = 0;

		for (auto& e : m_data)
		{
			std::for_each(
				e.second.begin(),
				e.second.end(),
				[&](auto&) [[msvc::forceinline]] {
					i++;
				});
		}

		return i;
	}

	void AnimationUpdateController::OnUpdate(
		Actor*                       a_actor,
		const BSAnimationUpdateData& a_data) const noexcept
	{
		const read_lock_guard lock(m_lock);

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
		if (IsInitialized())
		{
			const write_lock_guard lock(m_lock);

			auto& e = m_data.try_emplace(a_actor).first->second;

			e.emplace_back(a_ptr);
		}
	}

	void AnimationUpdateController::RemoveObject(
		Game::FormID                                    a_actor,
		const RE::WeaponAnimationGraphManagerHolderPtr& a_ptr) noexcept
	{
		if (IsInitialized())
		{
			const write_lock_guard lock(m_lock);

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
		if (IsInitialized())
		{
			const write_lock_guard lock(m_lock);

			m_data.erase(a_actor);
		}
	}

	bool AnimationUpdateController::CreateWeaponBehaviorGraph(
		NiAVObject*                               a_object,
		RE::WeaponAnimationGraphManagerHolderPtr& a_out,
		std::function<bool(const char*)>          a_allowFunc)
	{
		const auto sh = BSStringHolder::GetSingleton();

		auto bged = a_object->GetExtraDataSafe<BSBehaviorGraphExtraData>(sh->m_bged);
		if (!bged)
		{
			return false;
		}

		if (bged->controlsBaseSkeleton)
		{
			return false;
		}

		if (bged->behaviorGraphFile.empty())
		{
			return false;
		}

		const auto bgf = bged->behaviorGraphFile;

		if (!a_allowFunc(bgf.c_str()))
		{
			return false;
		}

		auto result = RE::WeaponAnimationGraphManagerHolder::Create();

		if (!LoadAnimationBehahaviorGraph(
				*result,
				bgf.c_str()))
		{
			return false;
		}

		if (!BindAnimationObject(*result, a_object))
		{
			gLog.Warning(
				"%s: binding animation object failed [0x%p | %s]",
				__FUNCTION__,
				a_object,
				a_object->m_name.c_str());
		}

		a_out = std::move(result);

		return true;
	}

	bool AnimationUpdateController::LoadWeaponBehaviorGraph(
		NiAVObject*                                                         a_object,
		std::pair<BSFixedString, RE::WeaponAnimationGraphManagerHolderPtr>& a_out)
	{
		const auto sh = BSStringHolder::GetSingleton();

		auto bged = a_object->GetExtraDataSafe<BSBehaviorGraphExtraData>(sh->m_bged);
		if (!bged)
		{
			return false;
		}

		if (bged->controlsBaseSkeleton)
		{
			return false;
		}

		if (bged->behaviorGraphFile.empty())
		{
			return false;
		}

		auto result = std::make_pair(
			bged->behaviorGraphFile,
			RE::WeaponAnimationGraphManagerHolder::Create());

		if (!LoadAnimationBehahaviorGraph(
				*result.second,
				result.first.c_str()))
		{
			return false;
		}

		a_out = std::move(result);

		return true;
	}

	bool AnimationUpdateController::CreateWeaponBehaviorGraph(
		NiAVObject*                               a_object,
		ObjectCloningTask&                        a_in,
		RE::WeaponAnimationGraphManagerHolderPtr& a_out,
		std::function<bool(const char*)>          a_allowFunc)
	{
		if (!a_allowFunc(a_in.GetAnimPath().c_str()))
		{
			return false;
		}

		a_out = std::move(a_in.GetGraphHolderManager());

		if (!BindAnimationObject(*a_out, a_object))
		{
			gLog.Warning(
				"%s: binding animation object failed [0x%p | %s]",
				__FUNCTION__,
				a_object,
				a_object->m_name.c_str());
		}

		return true;
	}

	void AnimationUpdateController::CleanupWeaponBehaviorGraph(
		const RE::WeaponAnimationGraphManagerHolderPtr& a_graph) noexcept
	{
		if (a_graph)
		{
			RE::BSAnimationGraphManagerPtr manager;
			if (a_graph->GetAnimationGraphManagerImpl(manager))
			{
				RE::GarbageCollector::GetSingleton()->QueueBehaviorGraph(manager);
			}
		}
	}

}