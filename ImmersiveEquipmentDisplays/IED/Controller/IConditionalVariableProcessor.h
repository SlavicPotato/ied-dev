#pragma once

#include "IED/ConfigConditionalVars.h"

namespace IED
{
	struct processParams_t;

	class IConditionalVariableProcessor
	{
	public:
		static bool UpdateVariableMap(
			processParams_t&                                a_params,
			const Data::configConditionalVariablesHolder_t& a_config,
			conditionalVariableMap_t&                       a_map);

	private:
		static constexpr const Data::configConditionalVariable_t* GetOverrideVariable(
			processParams_t&                              a_params,
			const Data::configConditionalVariablesList_t& a_list);

		static constexpr void UpdateVariable(
			ConditionalVariableType             a_defaultType,
			const conditionalVariableStorage_t& a_srcvar,
			conditionalVariableStorage_t&       a_dstval,
			bool&                               a_modified);
	};
}