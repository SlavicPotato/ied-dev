#pragma once

#include "IED/ConfigConditionalVars.h"

namespace IED
{
	struct processParams_t;

	class IConditionalVariableProcessor
	{
	public:
		static bool Process(
			processParams_t&                                a_params,
			const Data::configConditionalVariablesHolder_t& a_config,
			conditionalVariableMap_t&                       a_map);

		bool Process(
			processParams_t&                                a_params,
			const Data::configConditionalVariablesHolder_t& a_config);

		/*inline void ClearConditionalVariables() noexcept
		{
			m_varStorage.clear();
		}

		[[nodiscard]] inline constexpr auto& GetVariableStorage() const noexcept
		{
			return m_varStorage;
		}

		[[nodiscard]] inline constexpr auto& GetVariableStorage() noexcept
		{
			return m_varStorage;
		}*/

	private:
		static constexpr bool Process(
			processParams_t&                                   a_params,
			const Data::configConditionalVariablesEntryList_t& a_config,
			conditionalVariableMap_t&                          a_map);

		static bool ProcessConditionTrue(
			const Data::configConditionalVariablesEntry_t& a_config,
			conditionalVariableMap_t&                      a_map);

		static bool RecursiveEraseVariables(
			const Data::configConditionalVariablesEntry_t& a_config,
			conditionalVariableMap_t&                      a_map) noexcept;

		//conditionalVariableMap_t m_varStorage;
	};
}