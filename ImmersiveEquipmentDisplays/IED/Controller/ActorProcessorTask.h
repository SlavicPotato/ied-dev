#pragma once

#include "IED/ConfigCommon.h"
#include "IED/GlobalParams.h"
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

		[[nodiscard]] constexpr auto NodeProcessorGetTime() const noexcept
		{
			return m_currentTime;
		}

#if defined(IED_ENABLE_CONDITION_EN)
		[[nodiscard]] constexpr auto PlayerHasEnemiesNearby() const noexcept
		{
			return m_state.playerEnemiesNearby;
		}
#endif

		constexpr void SetProcessorTaskRunState(bool a_state) noexcept
		{
			m_run = a_state;
		}

		constexpr void SetProcessorTaskParallelUpdates(bool a_switch) noexcept
		{
			m_parallelProcessing = a_switch;
		}

		[[nodiscard]] constexpr auto& GetGlobalState() const noexcept
		{
			return m_globalState;
		}

		[[nodiscard]] constexpr const auto& GetOrCreateGlobalParams() noexcept
		{
			if (!m_globalParams)
			{
				m_globalParams.emplace();
			}

			return *m_globalParams;
		}

	private:
		struct GlobalState
		{
			constexpr GlobalState(long long a_ct) noexcept :
				nextRun(a_ct),
				nextRunMF(a_ct),
				nextRunLF(a_ct)
			{
			}

			long long nextRun;
			long long nextRunMF;
			long long nextRunLF;

			RE::TESWeather*       currentWeather{ nullptr };
			RE::Calendar::Day     dayOfWeek{ RE::Calendar::Day::kSundas };
			Data::TimeOfDay       timeOfDay{ Data::TimeOfDay::kDay };
			BGSLightingTemplate*  roomLightingTemplate{ nullptr };
			Game::ActorHandle     playerLastRidden;
			Game::ObjectRefHandle talkingHandle;
			float                 sunAngle{ 0.0f };
			bool                  inFirstPerson{ false };
			bool                  inDialogue{ false };
			bool                  isExteriorDark{ false };
			bool                  isPlayerInDarkInterior{ false };
			bool                  isDaytime{ true };
#if defined(IED_ENABLE_CONDITION_EN)
			bool playerEnemiesNearby{ false };
#endif
		};

		static constexpr auto COMMON_STATE_CHECK_INTERVAL    = 1000000ll;
		static constexpr auto COMMON_STATE_CHECK_INTERVAL_MF = 5150000ll;
		static constexpr auto COMMON_STATE_CHECK_INTERVAL_LF = 15270000ll;

		[[nodiscard]] constexpr bool ParallelProcessingEnabled() const noexcept
		{
			return m_parallelProcessing;
		}

		virtual void Run() noexcept override;

		SKMP_FORCEINLINE Controller&       GetController() noexcept;
		SKMP_FORCEINLINE const Controller& GetController() const noexcept;

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

		void RunPreUpdates(bool a_effectUpdates) noexcept;

		GlobalState                 m_globalState;
		std::optional<GlobalParams> m_globalParams;

		PerfTimerInt m_timer{ 1000000LL };
		long long    m_currentTime{ 0LL };
		bool         m_run{ false };
		bool         m_runAnimationUpdates{ false };
		bool         m_parallelProcessing{ false };

		template <class T>
		class PostMTTaskQueue
		{
		public:
			template <class... Args>
			constexpr decltype(auto) emplace(Args&&... a_args)
			{
				stl::lock_guard lock(m_lock);
				return m_queue.emplace_back(std::forward<Args>(a_args)...);
			}

			template <class Tf>
			constexpr void process(Tf a_func)  //
				noexcept(std::is_nothrow_invocable_v<Tf, const T&>)
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
			stl::cache_aligned::vector<T> m_queue;
			stl::fast_spin_lock           m_lock;
		};

		PostMTTaskQueue<std::pair<ActorObjectHolder*, ObjectEntryBase*>> m_syncRefParentQueue;
	};

}