#include "pch.h"

#include "ConfigConditionsCommon.h"

namespace IED
{
	namespace Conditions
	{
		bool match_extra(
			CommonParams& a_params,
			Data::ExtraConditionType a_type)
		{
			switch (a_type)
			{
			case Data::ExtraConditionType::kCanDualWield:
				return a_params.can_dual_wield();
			case Data::ExtraConditionType::kIsDead:
				return a_params.is_dead();
			}

			return false;
		}

		bool match_form(
			Game::FormID a_formid,
			TESForm* a_form)
		{
			if (a_formid)
			{
				return a_form->formID == a_formid;
			}
			else
			{
				return false;
			}
		}
	}
}