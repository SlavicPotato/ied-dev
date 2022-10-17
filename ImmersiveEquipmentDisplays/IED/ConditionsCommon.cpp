#include "pch.h"

#include "ConditionsCommon.h"

#include "Controller/Controller.h"
#include "Controller/ObjectManagerData.h"

#include "SkeletonID.h"

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

		const ActorObjectMap& get_actor_object_map(CommonParams& a_params)
		{
			return a_params.controller.GetData();
		}

		const SkeletonID& get_skeleton_id(CommonParams& a_params) noexcept
		{
			return a_params.objects.GetSkeletonID();
		}

		bool is_in_first_person(CommonParams& a_params) noexcept
		{
			return a_params.is_player() &&
			       a_params.controller.IsInFirstPerson();
		}

		bool check_node_monitor_value(CommonParams& a_params, std::uint32_t a_uid) noexcept
		{
			return a_params.objects.GetNodeMonitorResult(a_uid);
		}

		bool is_player_last_ridden_mount(CommonParams& a_params) noexcept
		{
			const auto handle = (*g_thePlayer)->lastRiddenHorseHandle;

			return handle && handle.IsValid() && a_params.objects.GetHandle() == handle;
		}

		bool is_sds_shield_on_back_enabled(CommonParams& a_params) noexcept
		{
			PluginInterfaceSDS* intfc;
			a_params.controller.GetPluginInterface(intfc);

			if (intfc)
			{
				return intfc->GetShieldOnBackEnabled(a_params.actor);
			}
			else
			{
				return false;
			}
		}

		bool match_random_percent(
			CommonParams&   a_params,
			const luid_tag& a_luid,
			float           a_percent) noexcept
		{
			return a_params.objects.GetRandomPercent(a_luid) <= a_percent;
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