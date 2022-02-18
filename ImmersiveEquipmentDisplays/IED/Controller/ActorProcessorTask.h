#pragma once

#include "IED/ConfigCommon.h"

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
		public TaskDelegateFixed
	{
	public:
		ActorProcessorTask(
			Controller& a_controller);

		[[nodiscard]] inline constexpr auto NodeProcessorGetTime() const noexcept
		{
			return m_currentTime;
		}

	private:
		inline static constexpr long long STATE_CHECK_INTERVAL_LOW = 1250000;

		virtual void Run() override;

		SKMP_FORCEINLINE void UpdateNode(
			const ActorObjectHolder& a_record,
			objectEntryBase_t&       a_entry);

		SKMP_FORCEINLINE void ProcessTransformUpdateRequest(
			const ActorObjectHolder& a_data);

		SKMP_FORCEINLINE void ProcessEvalRequest(
			ActorObjectHolder& a_data);

		SKMP_FORCEINLINE bool CheckMonitorNodes(
			ActorObjectHolder& a_data);

		PerfTimerInt m_timer{ 1000000LL };
		long long    m_currentTime{ 0LL };

		Controller& m_controller;
	};

}