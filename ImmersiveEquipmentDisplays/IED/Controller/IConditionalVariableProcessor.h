#pragma once

#include "IED/ConfigConditionalVars.h"

namespace IED
{
	struct ProcessParams;
	class IEquipment;

	class IConditionalVariableProcessor
	{
	public:
		static bool UpdateVariableMap(
			ProcessParams&                                a_params,
			const Data::configConditionalVariablesHolder_t& a_config,
			conditionalVariableMap_t&                       a_map) noexcept;

	private:
		static constexpr const Data::configConditionalVariable_t* GetOverrideVariable(
			ProcessParams&                              a_params,
			const Data::configConditionalVariablesList_t& a_list) noexcept;

		static Game::FormID GetLastEquippedForm(
			ProcessParams&                                  a_params,
			const Data::configConditionalVariableValueData_t& a_data) noexcept;

		static constexpr void UpdateVariable(
			ProcessParams&                                  a_params,
			ConditionalVariableType                           a_type,
			const Data::configConditionalVariableValueData_t& a_src,
			conditionalVariableStorage_t&                     a_dst,
			bool&                                             a_modified) noexcept;
	};
}