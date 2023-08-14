#include "pch.h"

#include "ConditionsCommon.h"
#include "ConfigBase.h"
#include "FormCommon.h"

#include "Controller/ObjectManagerData.h"

#include "ProcessParams.h"

namespace IED
{
	namespace Data
	{
		const equipmentOverride_t* configBase_t::get_equipment_override(
			ProcessParams& a_params) const noexcept
		{
			return get_equipment_override(
				a_params,
				equipmentOverrides);
		}

		const equipmentOverride_t* configBase_t::get_equipment_override(
			ProcessParams&                 a_params,
			const equipmentOverrideList_t& a_list) noexcept
		{
			for (auto& e : a_list)
			{
				if (e.conditions.evaluate(a_params, true))
				{
					if (e.overrideFlags.test(Data::ConfigOverrideFlags::kIsGroup))
					{
						if (auto result = get_equipment_override(
								a_params,
								e.group))
						{
							return result;
						}

						if (!e.overrideFlags.test(Data::ConfigOverrideFlags::kContinue))
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

		const equipmentOverride_t* configBase_t::get_equipment_override_fp(
			const FormSlotPair& a_checkForm,
			ProcessParams&      a_params) const noexcept
		{
			return get_equipment_override_fp(
				a_checkForm,
				a_params,
				equipmentOverrides);
		}

		const equipmentOverride_t* configBase_t::get_equipment_override_fp(
			const FormSlotPair&            a_checkForm,
			ProcessParams&                 a_params,
			const equipmentOverrideList_t& a_list) noexcept
		{
			for (auto& e : a_list)
			{
				if (e.conditions.evaluate_fp(a_checkForm, a_params, true))
				{
					if (e.overrideFlags.test(Data::ConfigOverrideFlags::kIsGroup))
					{
						if (auto result = get_equipment_override_fp(
								a_checkForm,
								a_params,
								e.group))
						{
							return result;
						}

						if (!e.overrideFlags.test(Data::ConfigOverrideFlags::kContinue))
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

		const equipmentOverride_t* configBase_t::get_equipment_override_sfp(
			const FormSlotPair& a_checkForm,
			ProcessParams&      a_params) const noexcept
		{
			return get_equipment_override_sfp(
				a_checkForm,
				a_params,
				equipmentOverrides);
		}

		const equipmentOverride_t* configBase_t::get_equipment_override_sfp(
			const FormSlotPair&            a_checkForm,
			ProcessParams&                 a_params,
			const equipmentOverrideList_t& a_list) noexcept
		{
			for (auto& e : a_list)
			{
				if (e.conditions.evaluate_sfp(a_checkForm, a_params, true))
				{
					if (e.overrideFlags.test(Data::ConfigOverrideFlags::kIsGroup))
					{
						if (auto result = get_equipment_override_sfp(
								a_checkForm,
								a_params,
								e.group))
						{
							return result;
						}

						if (!e.overrideFlags.test(Data::ConfigOverrideFlags::kContinue))
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

		const configEffectShaderHolder_t* configBase_t::get_effect_shader(
			ProcessParams& a_params) const noexcept
		{
			for (auto& e : effectShaders.data)
			{
				if (!e.enabled())
				{
					continue;
				}

				if (e.conditions.evaluate(a_params, true))
				{
					return std::addressof(e);
				}
			}

			return nullptr;
		}

		const configEffectShaderHolder_t* configBase_t::get_effect_shader_fp(
			const FormSlotPair& a_checkForm,
			ProcessParams&      a_params) const noexcept
		{
			for (auto& e : effectShaders.data)
			{
				if (!e.enabled())
				{
					continue;
				}

				if (e.conditions.evaluate_fp(a_checkForm, a_params, true))
				{
					return std::addressof(e);
				}
			}

			return nullptr;
		}

		const configEffectShaderHolder_t* configBase_t::get_effect_shader_sfp(
			const FormSlotPair& a_checkForm,
			ProcessParams&      a_params) const noexcept
		{
			for (auto& e : effectShaders.data)
			{
				if (!e.enabled())
				{
					continue;
				}

				if (e.conditions.evaluate_sfp(a_checkForm, a_params, true))
				{
					return std::addressof(e);
				}
			}

			return nullptr;
		}

		bool configBaseFiltersHolder_t::run_filters(
			const CommonParams& a_params) const noexcept
		{
			if (filters)
			{
				return filters->actorFilter.test(a_params.actor->formID) &&
				       filters->npcFilter.test(a_params.npc->formID) &&
				       filters->raceFilter.test(a_params.race->formID);
			}
			else
			{
				return true;
			}
		}
	}
}