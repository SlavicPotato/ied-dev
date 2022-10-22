#include "pch.h"

#include "IConditionalVariableProcessor.h"

#include "IED/ConfigBase.h"
#include "IED/ProcessParams.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	using namespace Data;

	bool IConditionalVariableProcessor::UpdateVariableMap(
		processParams_t&                          a_params,
		const configConditionalVariablesHolder_t& a_config,
		conditionalVariableMap_t&                 a_map)
	{
		a_params.useCount.clear();

		bool result = false;

		for (auto& e : a_config.data.getvec())
		{
			auto r = a_map.emplace(
				e->first,
				e->second.defaultValue.value);

			result |= r.second;

			auto overrideVar = GetOverrideVariable(
				a_params,
				e->second.vars);

			UpdateVariable(
				a_params,
				e->second.defaultValue.value.type,
				overrideVar ?
					overrideVar->value :
                    e->second.defaultValue,
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
			if (configBase_t::do_match_eos(
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

	Game::FormID IConditionalVariableProcessor::EvaluateLastEquippedForm(
		processParams_t&                                  a_params,
		const Data::configConditionalVariableValueData_t& a_data)
	{
		auto& controller = a_params.controller;

		controller.RunUpdateBipedSlotCache(a_params);

		auto it = controller.DoLastEquippedSelection(
			a_params,
			a_data.lastEquipped,
			[](auto&) { return true; });

		if (it != a_params.collector.data.forms.end())
		{
			auto r = a_params.useCount.emplace(it->first, 0);

			r.first->second++;

			return it->first;
		}
		else
		{
			return {};
		}
	}

	constexpr void IConditionalVariableProcessor::UpdateVariable(
		processParams_t&                                  a_params,
		ConditionalVariableType                           a_defaultType,
		const Data::configConditionalVariableValueData_t& a_src,
		conditionalVariableStorage_t&                     a_dst,
		bool&                                             a_modified)
	{
		switch (a_defaultType)
		{
		case ConditionalVariableType::kInt32:

			if (a_src.value.i32 == a_dst.i32)
			{
				return;
			}

			a_dst.i32 = a_src.value.i32;

			break;
		case ConditionalVariableType::kFloat:

			if (a_src.value.f32 == a_dst.f32)
			{
				return;
			}

			a_dst.f32 = a_src.value.f32;

			break;
		case ConditionalVariableType::kForm:

			if (a_src.flags.test(Data::ConditionalVariableValueDataFlags::kLastEquipped))
			{
				auto v = EvaluateLastEquippedForm(a_params, a_src);

				//_DMESSAGE("le: %X", v);

				if (v == a_dst.form.get_id())
				{
					return;
				}

				a_dst.form = v;
			}
			else
			{
				if (a_src.value.form == a_dst.form)
				{
					return;
				}

				a_dst.form = a_src.value.form;
			}
			break;
		}

		a_modified = true;
	}

}