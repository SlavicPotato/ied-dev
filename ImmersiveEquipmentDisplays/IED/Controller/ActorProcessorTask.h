#pragma once

#include "IED/ConfigCommon.h"
#include "IED/TimeOfDay.h"

#include "EffectController.h"
#include "IFirstPersonState.h"

namespace RE
{
	class TESWeather;
}

namespace IED
{
	/*enum class TransformUpdateFlags : std::uint32_t
	{
		kSkipNext = 1u << 0
	};

	DEFINE_ENUM_CLASS_BITWISE(TransformUpdateFlags);*/

	class Controller;
	class ActorObjectHolder;

	struct ObjectEntryBase;

	class ActorProcessorTask :
		public EffectController,
		public IFirstPersonState,
		public TaskDelegateFixed
	{
		struct animUpdateData_t
		{
			Game::Unk2f6b948::Steps steps;
			//BSAnimationUpdateData   data;
		};

	public:
		ActorProcessorTask();

		[[nodiscard]] inline constexpr auto NodeProcessorGetTime() const noexcept
		{
			return m_currentTime;
		}

#if defined(IED_ENABLE_CONDITION_EN)
		[[nodiscard]] inline constexpr auto PlayerHasEnemiesNearby() const noexcept
		{
			return m_state.playerEnemiesNearby;
		}
#endif

		inline constexpr void SetProcessorTaskRunState(bool a_state) noexcept
		{
			m_run = a_state;
		}

		[[nodiscard]] void SetProcessorTaskRunAUState(bool a_state) noexcept;

		inline constexpr void SetProcessorTaskParallelUpdates(bool a_switch) noexcept
		{
			m_parallelProcessing = a_switch;
		}
		
		[[nodiscard]] inline constexpr auto& GetGlobalState() const noexcept
		{
			return m_globalState;
		}

	private:
		struct GlobalState
		{
			constexpr GlobalState(long long a_ct) noexcept :
				nextRun(a_ct),
				nextRunLF(a_ct)
			{
			}

			long long nextRun;
			long long nextRunLF;

			RE::TESWeather*   currentWeather{ nullptr };
			RE::Calendar::Day dayOfWeek{ RE::Calendar::Day::kSundas };
			Data::TimeOfDay   timeOfDay{ Data::TimeOfDay::kDay };
			Game::ActorHandle playerLastRidden;
			bool              inFirstPerson{ false };
			bool              inDialogue{ false };
#if defined(IED_ENABLE_CONDITION_EN)
			bool playerEnemiesNearby{ false };
#endif
		};

		inline static constexpr auto COMMON_STATE_CHECK_INTERVAL    = 1000000ll;
		inline static constexpr auto COMMON_STATE_CHECK_INTERVAL_LF = 15000000ll;

		[[nodiscard]] inline constexpr bool ParallelProcessingEnabled() const noexcept
		{
			return m_parallelProcessing;
		}

		virtual void Run() noexcept override;

		SKMP_FORCEINLINE Controller& GetController() noexcept;

		bool SyncRefParentNode(
			ActorObjectHolder& a_record,
			ObjectEntryBase&   a_entry) noexcept;

		SKMP_FORCEINLINE void DoObjectRefSync(
			ActorObjectHolder& a_record,
			ObjectEntryBase&   a_entry) noexcept;

		SKMP_FORCEINLINE void DoObjectRefSyncMTSafe(
			ActorObjectHolder& a_record,
			ObjectEntryBase&   a_entry) noexcept;

		//const std::optional<animUpdateData_t>& a_animUpdateData);

		void ProcessTransformUpdateRequest(
			ActorObjectHolder& a_data) noexcept;

		void ProcessEvalRequest(
			ActorObjectHolder& a_data) noexcept;

		void UpdateGlobalState() noexcept;

		template <bool _Par>
		void DoActorUpdate(
			const float                             a_interval,
			const Game::Unk2f6b948::Steps&          a_stepMuls,
			const std::optional<PhysicsUpdateData>& a_physUpdData,
			ActorObjectHolder&                      a_holder,
			bool                                    a_updateEffects) noexcept;

		void RunPreUpdates(const Game::Unk2f6b948::Steps& a_stepMuls) noexcept;
		void RunSequentialAnimUpdates(const Game::Unk2f6b948::Steps& a_stepMuls) noexcept;

		GlobalState m_globalState;

		PerfTimerInt m_timer{ 1000000LL };
		long long    m_currentTime{ 0LL };
		bool         m_run{ false };
		bool         m_runAnimationUpdates{ false };
		bool         m_parallelProcessing{ false };

		template <class T>
		class PostMTUpdateQueue
		{
		public:
			template <class... Args>
			constexpr decltype(auto) emplace(Args&&... a_args)
			{
				stl::lock_guard lock(m_lock);
				return m_queue.emplace_back(std::forward<Args>(a_args)...);
			}

			template <class Tf>
			constexpr void process(Tf a_func) noexcept
			{
				if (m_queue.empty())
				{
					return;
				}

				for (const auto& e : m_queue)
				{
					a_func(e);
				}

				m_queue.clear();
			}

		private:
			stl::vector<T>      m_queue;
			stl::fast_spin_lock m_lock;
		};

		PostMTUpdateQueue<std::pair<ActorObjectHolder*, ObjectEntryBase*>> m_syncRefParentQueue;

		/*stl::fast_spin_lock                                          m_unloadQueueWRLock;
		stl::vector<std::pair<ActorObjectHolder*, ObjectEntryBase*>> m_unloadQueue;*/
	};

}