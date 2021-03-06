#include "pch.h"

#include "ConditionsCommon.h"

#include "Controller/Controller.h"
#include "Controller/ObjectManagerData.h"

namespace IED
{
	namespace Conditions
	{
		bool is_in_location(
			BGSLocation* a_current,
			BGSLocation* a_loc)
		{
			for (auto it = a_current; it; it = it->parentLoc)
			{
				if (a_loc == it)
				{
					return true;
				}
			}

			return false;
		}

		bool is_in_location(
			BGSLocation* a_current,
			BGSKeyword*  a_keyword,
			BGSLocation* a_matched)
		{
			for (auto it = a_current; it; it = it->parentLoc)
			{
				if (it->HasKeyword(a_keyword))
				{
					return true;
				}

				if (it == a_matched)
				{
					break;
				}
			}

			return false;
		}

		bool is_in_location(
			BGSLocation* a_current,
			BGSKeyword*  a_keyword)
		{
			for (auto it = a_current; it; it = it->parentLoc)
			{
				if (it->HasKeyword(a_keyword))
				{
					return true;
				}
			}

			return false;
		}

		bool match_worldspace(
			TESWorldSpace* a_current,
			Game::FormID   a_wsId,
			bool           a_matchParent)
		{
			if (a_current->formID == a_wsId)
			{
				return true;
			}

			if (a_matchParent)
			{
				if (auto parent = a_current->GetParentWorldspace())
				{
					return parent->formID == a_wsId;
				}
			}

			return false;
		}

		bool is_in_first_person(CommonParams& a_params) noexcept
		{
			return a_params.is_player() &&
			       a_params.controller.IsInFirstPerson();
		}

		bool is_female(CommonParams& a_params) noexcept
		{
			return a_params.objects.IsFemale();
		}

		bool has_humanoid_skeleton(CommonParams& a_params) noexcept
		{
			return a_params.objects.HasHumanoidSkeleton();
		}

#if defined(IED_ENABLE_CONDITION_EN)
		bool enemies_nearby(CommonParams& a_params) noexcept
		{
			return a_params.is_player() &&
			       a_params.controller.PlayerHasEnemiesNearby();
		}
#endif
	}
}