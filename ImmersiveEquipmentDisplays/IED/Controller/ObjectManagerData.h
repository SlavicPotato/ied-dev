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
			const std::lock_guard lock(m_lock);

			m_data.emplace_back(a_ptr);
		}

		void Remove(RE::WeaponAnimationGraphManagerHolderPtr& a_ptr)
		{
			const std::lock_guard lock(m_lock);

			auto it = std::find(m_data.begin(), m_data.end(), a_ptr);
			if (it != m_data.end())
			{
				m_data.erase(it);
			}
		}

		void Notify(const BSFixedString& a_event) const
		{
			const std::lock_guard lock(m_lock);

			for (auto& e : m_data)
			{
				e->NotifyAnimationGraph(a_event);
			}
		}

		void Update(const BSAnimationUpdateData& a_data) const;

		void UpdateNoLock(const BSAnimationUpdateData& a_data) const;

		void Clear() noexcept
		{
			const std::lock_guard lock(m_lock);

			m_data.clear();
		}

		inline constexpr auto& GetList() const noexcept
		{
			return m_data;
		}

		inline bool Empty() const noexcept
		{
			return m_data.empty();
		}

	private:
		mutable std::recursive_mutex                       m_lock;
		stl::list<RE::WeaponAnimationGraphManagerHolderPtr> m_data;
	};*/

	using ActorObjectMap = stl::unordered_map<Game::FormID, ActorObjectHolder>;

	class ActorProcessorTask;

	class ObjectManagerData
	{
		friend class ActorProcessorTask;

	public:
		template <class... Args>
		[[nodiscard]] inline constexpr auto& GetObjectHolder(
			Actor* a_actor,
			Args&&... a_args)
		{
			auto r = m_objects.try_emplace(
				a_actor->formID,
				a_actor,
				std::forward<Args>(a_args)...);

			if (r.second)
			{
				//ApplyActorState(r.first->second);
				OnActorAcquire(r.first->second);
				RequestVariableUpdateOnAll();
			}

			return r.first->second;
		}

		[[nodiscard]] inline constexpr auto& GetObjects() const noexcept
		{
			return m_objects;
		}

		[[nodiscard]] inline constexpr auto& GetObjects() noexcept
		{
			return m_objects;
		}

		/*inline void ClearPlayerState() noexcept
		{
			m_playerState.reset();
		}*/

		void ClearVariablesOnAll(bool a_requestEval) noexcept;
		void ClearVariables(Game::FormID a_handle, bool a_requestEval) noexcept;

		void RequestVariableUpdateOnAll() const noexcept;
		void RequestVariableUpdate(Game::FormID a_handle) const noexcept;

		void RequestEvaluateOnAll() const noexcept;

		//void StorePlayerState(ActorObjectHolder& a_holder);

	private:
		//void ApplyActorState(ActorObjectHolder& a_holder);

		virtual void OnActorAcquire(ActorObjectHolder& a_holder) = 0;

	protected:
		void RequestEvaluateAll(bool a_defer) const noexcept;

		void RequestLFEvaluateAll() noexcept;
		void RequestLFEvaluateAll(Game::FormID a_skip) noexcept;

		void RequestHFEvaluateAll() noexcept;
		void RequestHFEvaluateAll(Game::FormID a_skip) noexcept;

		inline auto EraseActor(ActorObjectMap::const_iterator a_it)
		{
			auto result = m_objects.erase(a_it);

			RequestHFEvaluateAll();

			return result;
		}

		inline auto EraseActor(const ActorObjectMap::key_type& a_key)
		{
			auto result = m_objects.erase(a_key);

			if (result)
			{
				RequestHFEvaluateAll();
			}

			return result;
		}

		ActorObjectMap m_objects;
	};

}