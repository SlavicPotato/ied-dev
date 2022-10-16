#include "pch.h"

#include "IConditionalVariableProcessor.h"

#include "IED/ConfigBase.h"
#include "IED/ProcessParams.h"

namespace IED
{
	using namespace Data;

	bool IConditionalVariableProcessor::Process(
		processParams_t&                          a_params,
		const configConditionalVariablesHolder_t& a_config,
		conditionalVariableMap_t&                 a_map)
	{
		return Process(a_params, a_config.data, a_map);
	}

	/*bool IConditionalVariableProcessor::Process(
		processParams_t&                                a_params,
		const Data::configConditionalVariablesHolder_t& a_config)
	{
		return Process(a_params, a_config.data, m_varStorage);
	}*/

	constexpr bool IConditionalVariableProcessor::Process(
		processParams_t&                             a_params,
		const configConditionalVariablesEntryList_t& a_config,
		conditionalVariableMap_t&                    a_map)
	{
		bool result = false;

		for (auto& e : a_config)
		{
			if (configBase_t::do_match(
					a_params.collector.data,
					e.conditions,
					a_params,
					true))
			{
				if (e.flags.test(ConditionalVariablesEntryFlags::kIsGroup))
				{
					result |= Process(a_params, e.group, a_map);
				}
				else
				{
					result |= ProcessConditionTrue(e, a_map);
				}
			}
			else
			{
				RecursiveEraseVariables(e, a_map);
			}
		}

		return result;
	}

	bool IConditionalVariableProcessor::ProcessConditionTrue(
		const configConditionalVariablesEntry_t& a_config,
		conditionalVariableMap_t&                a_map)
	{
		bool result = false;

		for (auto& e : a_config.vars)
		{
			auto r = a_map.try_emplace(e.name, e.storage, true);
			if (r.second)
			{
				result = true;
			}
			else
			{
				if (!r.first->second.second ||
				    r.first->second.first != e.storage)
				{
					r.first->second.first  = e.storage;
					r.first->second.second = true;
					result                 = true;
				}
			}
		}

		return result;
	}

	bool IConditionalVariableProcessor::RecursiveEraseVariables(
		const Data::configConditionalVariablesEntry_t& a_config,
		conditionalVariableMap_t&                      a_map) noexcept
	{
		bool result = false;

		if (a_config.flags.test(ConditionalVariablesEntryFlags::kIsGroup))
		{
			for (auto& e : a_config.group)
			{
				result |= RecursiveEraseVariables(e, a_map);
			}
		}
		else
		{
			for (auto& e : a_config.vars)
			{
				auto it = a_map.find(e.name);
				if (it != a_map.end())
				{
					it->second.second = false;
					result            = true;
				}
			}
		}

		return result;
	}
}