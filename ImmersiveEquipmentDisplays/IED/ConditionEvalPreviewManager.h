#pragma once

#include "ConfigBase.h"

namespace IED
{
	class ConditionEvalPreviewManager
	{
	public:
		[[nodiscard]] static constexpr auto& GetSingleton() noexcept
		{
			return m_Instance;
		}

		void QueueEvalJob(const Data::equipmentOverrideConditionList_t& a_condition)
		{
			/*ITaskPool::AddTask([&] {
				Data::configBase_t::evaluate_condition_list
			});*/
		}

		using evalResultList_t = stl::vector<luid_tag, bool>;

	private:
		/*struct ActorEntry
		{
			Game::FormID                                   actor;
			stl::unordered_map<luid_tag, evalResultList_t> results;
		};

		ActorEntry data;*/

		static ConditionEvalPreviewManager m_Instance;
	};
}