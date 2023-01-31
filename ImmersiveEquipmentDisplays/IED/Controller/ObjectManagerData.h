#pragma once

#include "ActorObjectHolder.h"
#include "IED/ActorState.h"

namespace IED
{

	/*class AnimationEventFilter
	{
	public:
		SKMP_FORCEINLINE bool test(const BSFixedString& a_event)
		{
			if (m_allow.contains(a_event))
			{
				return true;
			}

			return !m_denyAll &&
			       !m_deny.contains(a_event);
		}

		bool                              m_denyAll{ false };
		stl::unordered_set<BSFixedString> m_allow;
		stl::unordered_set<BSFixedString> m_deny;
	};*/

	/*class AnimationGraphManagerHolderList
	{
	public:
		void Add(RE::WeaponAnimationGraphManagerHolderPtr& a_ptr)
		{
			const stl::lock_guard lock(m_lock);

			m_data.emplace_back(a_ptr);
		}

		void Remove(RE::WeaponAnimationGraphManagerHolderPtr& a_ptr)
		{
			const stl::lock_guard lock(m_lock);

			auto it = std::find(m_data.begin(), m_data.end(), a_ptr);
			if (it != m_data.end())
			{
				m_data.erase(it);
			}
		}

		void Notify(const BSFixedString& a_event) const
		{
			const stl::lock_guard lock(m_lock);

			for (auto& e : m_data)
			{
				e->NotifyAnimationGraph(a_event);
			}
		}

		void Update(const BSAnimationUpdateData& a_data) const;

		void UpdateNoLock(const BSAnimationUpdateData& a_data) const;

		void Clear() noexcept
		{
			const stl::lock_guard lock(m_lock);

			m_data.clear();
		}

		constexpr auto& GetList() const noexcept
		{
			return m_data;
		}

		inline bool Empty() const noexcept
		{
			return m_data.empty();
		}

	private:
		mutable stl::recursive_mutex                       m_lock;
		stl::list<RE::WeaponAnimationGraphManagerHolderPtr> m_data;
	};*/

	using ActorObjectMap = stl::cache_aligned::vectormap<Game::FormID, ActorObjectHolder>;

	class ActorProcessorTask;

	class ObjectManagerData
	{
		friend class ActorProcessorTask;

	public:
		template <class... Args>
		[[nodiscard]] constexpr auto& GetOrCreateObjectHolder(
			Actor* a_actor,
			Args&&... a_args) noexcept
		{
			const auto r = m_actorMap.try_emplace(
				a_actor->formID,
				a_actor,
				std::forward<Args>(a_args)...);

			if (r.second)
			{
				m_actorMap.sortvec(
					[](const auto& a_lhs,
				       const auto& a_rhs) noexcept [[msvc::forceinline]] {
						return a_lhs->first < a_rhs->first;
					});

				OnActorAcquire(r.first->second);

				if (WantGlobalVariableUpdateOnAddRemove())
				{
					RequestVariableUpdateOnAll();
				}
			}

			return r.first->second;
		}

		[[nodiscard]] constexpr auto& GetActorMap() const noexcept
		{
			return m_actorMap;
		}

		[[nodiscard]] constexpr auto& GetActorMap() noexcept
		{
			return m_actorMap;
		}

		void ClearVariablesOnAll(bool a_requestEval) noexcept;
		void ClearVariables(Game::FormID a_handle, bool a_requestEval) noexcept;

		void RequestVariableUpdateOnAll() const noexcept;
		void RequestVariableUpdate(Game::FormID a_handle) const noexcept;

		void RequestEvaluateOnAll() const noexcept;

	private:
		virtual void OnActorAcquire(ActorObjectHolder& a_holder) noexcept = 0;
		virtual bool WantGlobalVariableUpdateOnAddRemove() const noexcept = 0;

	protected:
		void RequestEvaluateAll(bool a_defer) const noexcept;

		void RequestLFEvaluateAll() const noexcept;
		void RequestLFEvaluateAll(Game::FormID a_skip) const noexcept;

		void RequestHFEvaluateAll() const noexcept;
		void RequestHFEvaluateAll(Game::FormID a_skip) const noexcept;

		inline auto EraseActor(ActorObjectMap::const_iterator a_it) noexcept
		{
			const auto result = m_actorMap.erase(a_it);

			if (WantGlobalVariableUpdateOnAddRemove())
			{
				RequestHFEvaluateAll();
			}

			return result;
		}

		inline auto EraseActor(const ActorObjectMap::key_type& a_key) noexcept
		{
			const auto result = m_actorMap.erase(a_key);

			if (result)
			{
				if (WantGlobalVariableUpdateOnAddRemove())
				{
					RequestHFEvaluateAll();
				}
			}

			return result;
		}

		ActorObjectMap m_actorMap;
	};

}