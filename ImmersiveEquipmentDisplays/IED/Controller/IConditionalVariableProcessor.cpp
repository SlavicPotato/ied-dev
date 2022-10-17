#include "pch.h"

#include "IConditionalVariableProcessor.h"

#include "IED/ConfigBase.h"
#include "IED/ProcessParams.h"

namespace IED
{
	using namespace Data;

	bool IConditionalVariableProcessor::UpdateVariableMap(
		processParams_t&                          a_params,
		const configConditionalVariablesHolder_t& a_config,
		conditionalVariableMap_t&                 a_map)
	{
		bool result = false;

		for (auto& e : a_config.data)
		{
			auto r = a_map.emplace(
				e.first,
				e.second.defaultValue);

			result |= r.second;

			auto overrideVar = GetOverrideVariable(
				a_params,
				e.second.vars);

			UpdateVariable(
				e.second.defaultValue.type,
				overrideVar ?
					overrideVar->value :
                    e.second.defaultValue,
				r.first->second,
				result);
		}

		return result;
	}

	constexpr const Data::configConditionalVariable_t* IConditionalVariableProcessor::GetOverrideVariable(
		processParams_t&                              a_params,
		const Data::configConditionalVariablesList_t& a_list)
	{
		for (auto& e : a_list)
		{
			if (configBase_t::do_match(
					a_params.collector.data,
					e.conditions,
					a_params,
					true))
			{
				if (e.flags.test(Data::ConditionalVariableFlags::kIsGroup))
				{
					if (auto result = GetOverrideVariable(a_params, e.group))
					{
						return result;
					}

					if (!e.flags.test(Data::ConditionalVariableFlags::kContinue))
					{
						break;
					}
				}
				else
				{
					return std::addressof(e);
				}
			}
		}

		return nullptr;
	}

	constexpr void IConditionalVariableProcessor::UpdateVariable(
		ConditionalVariableType             a_defaultType,
		const conditionalVariableStorage_t& a_srcvar,
		conditionalVariableStorage_t&       a_dstval,
		bool&                               a_modified)
	{
		switch (a_defaultType)
		{
		case ConditionalVariableType::kInt32:
			if (a_srcvar.i32 == a_dstval.i32)
			{
				return;
			}
			break;
		case ConditionalVariableType::kFloat:
			if (a_srcvar.f32 == a_dstval.f32)
			{
				return;
			}
			break;
		}

		a_dstval   = a_srcvar;
		a_modified = true;
	}

}