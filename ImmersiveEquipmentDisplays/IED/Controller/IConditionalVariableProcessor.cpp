#include "pch.h"

#include "IConditionalVariableProcessor.h"

#include "IED/ConfigBase.h"
#include "IED/ProcessParams.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	using namespace Data;

	bool IConditionalVariableProcessor::UpdateVariableMap(
		ProcessParams&                            a_params,
		const configConditionalVariablesHolder_t& a_config,
		conditionalVariableMap_t&                 a_map) noexcept
	{
		a_params.useCount.clear();

		a_params.flags.set(ControllerUpdateFlags::kFailVariableCondition);

		bool result = false;

		for (const auto* const e : a_config.data.getvec())
		{
			const auto r = a_map.emplace(
				e->first,
				e->second.defaultValue.value);

			result |= r.second;

			const auto overrideVar = GetOverrideVariable(
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

		a_params.flags.clear(ControllerUpdateFlags::kFailVariableCondition);

		return result;
	}

	constexpr const Data::configConditionalVariable_t* IConditionalVariableProcessor::GetOverrideVariable(
		ProcessParams&                          a_params,
		const configConditionalVariablesList_t& a_list) noexcept
	{
		for (auto& e : a_list)
		{
			if (e.conditions.evaluate_eos(
					a_params,
					true))
			{
				if (e.flags.test(Data::ConditionalVariableFlags::kIsGroup))
				{
					if (const auto result = GetOverrideVariable(a_params, e.group))
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

	namespace detail
	{
		static constexpr void update_use_counts(
			ProcessParams&                                       a_params,
			const bool                                           a_consumeItem,
			const CollectorData::container_type::const_iterator& a_it) noexcept
		{
			const auto r = a_params.useCount.emplace(a_it->first, 0u);

			if (a_it->second.extra.type == ObjectType::kAmmo)
			{
				r.first->second = std::max(a_it->second.itemCount, 0i32);
			}
			else
			{
				r.first->second++;
			}

			if (a_consumeItem)
			{
				a_it->second.consume_one();
			}
		}
	}

	Game::FormID IConditionalVariableProcessor::GetLastEquippedForm(
		ProcessParams&                              a_params,
		const configConditionalVariableValueData_t& a_data) noexcept
	{
		auto& controller = a_params.controller;

		controller.RunUpdateBipedSlotCache(a_params);

		const auto it = controller.DoLastEquippedSelection(
			a_params,
			a_data.lastEquipped,
			[](const auto& a_itemEntry) noexcept [[msvc::forceinline]] {
				return !a_itemEntry.first.IsTemporary();
			});

		if (it != a_params.collector.data.forms.end())
		{
			detail::update_use_counts(
				a_params,
				a_data.flags.test(ConditionalVariableValueDataFlags::kConsumeItem),
				it);

			return it->first;
		}
		else
		{
			return {};
		}
	}

	Game::FormID IConditionalVariableProcessor::GetInventoryForm(
		ProcessParams&                              a_params,
		const configConditionalVariableValueData_t& a_data,
		const conditionalVariableStorage_t&         a_dst) noexcept
	{
		auto& controller = a_params.controller;

		const auto it = controller.SelectInventoryFormDefault(
			a_params,
			a_data.inv,
			a_data.value.form.get_id(),
			a_dst.form.get_id(),
			[](const auto& a_itemEntry) noexcept [[msvc::forceinline]] {
				return true;
			});

		if (it != a_params.collector.data.forms.end())
		{
			detail::update_use_counts(
				a_params,
				a_data.inv.flags.test(InventoryFlags::kEquipmentMode),
				it);

			return it->first;
		}
		else
		{
			return {};
		}
	}

	constexpr void IConditionalVariableProcessor::UpdateVariable(
		ProcessParams&                              a_params,
		ConditionalVariableType                     a_type,
		const configConditionalVariableValueData_t& a_src,
		conditionalVariableStorage_t&               a_dst,
		bool&                                       a_modified) noexcept
	{
		switch (a_type)
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

			switch (a_src.flags.bf().selectionMode)
			{
			case ConditionalVariableSelectionMode::LastEquipped:
				{
					const auto v = GetLastEquippedForm(a_params, a_src);

					if (v == a_dst.form.get_id())
					{
						return;
					}

					a_dst.form = v;
				}
				break;

			case ConditionalVariableSelectionMode::Inventory:
				{
					const auto v = GetInventoryForm(a_params, a_src, a_dst);

					if (v == a_dst.form.get_id())
					{
						return;
					}

					a_dst.form = v;
				}
				break;

			default:

				if (a_src.value.form == a_dst.form)
				{
					return;
				}

				a_dst.form = a_src.value.form;

				break;
			}

			break;

		default:

			return;
		}

		a_modified = true;
	}

}