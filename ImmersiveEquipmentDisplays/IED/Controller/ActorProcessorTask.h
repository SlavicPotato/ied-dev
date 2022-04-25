#pragma once

#include "IED/ConfigCommon.h"
#include "IED/TimeOfDay.h"

#include "IFirstPersonState.h"

namespace RE
{
	class TESWeather;
}

namespace IED
{
	enum class TransformUpdateFlags
	{
		kSkipNext = 1u << 0
	};

	DEFINE_ENUM_CLASS_BITWISE(TransformUpdateFlags);

	class Controller;
	class ActorObjectHolder;

	struct objectEntryBase_t;

	class ActorProcessorTask :
		public TaskDelegateFixed,
		public IFirstPersonState
	{
		struct animUpdateData_t
		{
			Game::Unk2f6b948::Steps steps;
			BSAnimationUpdateData   data;
		};

	public:
		ActorProcessorTask(
			Controller& a_controller);

		[[nodiscard]] inline constexpr auto NodeProcessorGetTime() const noexcept
		{
			return m_currentTime;
		}

	protected:
#if defined(IED_ENABLE_1D10T_SAFEGUARDS)
		bool m_activeWriteCMETransforms{ false };
#endif

	private:
		struct State
		{
			long long lastRun;

			RE::TESWeather* currentWeather{ nullptr };
			Data::TimeOfDay timeOfDay{ Data::TimeOfDay::kDay };
			bool            inFirstPerson{ false };
		};

		inline static constexpr long long STATE_CHECK_INTERVAL_LOW = 1250000;

		virtual void Run() override;

		SKMP_FORCEINLINE void UpdateNode(
			ActorObjectHolder&                          a_record,
			objectEntryBase_t&                          a_entry,
			const std::optional<animUpdateData_t>& a_animUpdateData);

		SKMP_FORCEINLINE void ProcessTransformUpdateRequest(
			ActorObjectHolder& a_data);

		SKMP_FORCEINLINE void ProcessEvalRequest(
			ActorObjectHolder& a_data);

		SKMP_FORCEINLINE bool CheckMonitorNodes(
			ActorObjectHolder& a_data);

#if defined(IED_ENABLE_1D10T_SAFEGUARDS)
		void WriteCMETransforms(
			ActorObjectHolder& a_data);
#endif

		void UpdateState();

		State m_state;

		PerfTimerInt m_timer{ 1000000LL };
		long long    m_currentTime{ 0LL };

		Controller& m_controller;
	};

}