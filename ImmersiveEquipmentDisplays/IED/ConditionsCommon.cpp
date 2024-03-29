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
			BGSLocation* a_loc) noexcept
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
			BGSLocation* a_matched) noexcept
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
			BGSKeyword*  a_keyword) noexcept
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
			bool           a_matchParent) noexcept
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

		const ActorObjectMap& get_actor_object_map(CommonParams& a_params) noexcept
		{
			return a_params.controller.GetActorMap();
		}

		bool is_in_first_person(CommonParams& a_params) noexcept
		{
			return a_params.objects.IsPlayer() &&
			       a_params.controller.IsInFirstPerson();
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

		bool is_in_dialogue(CommonParams& a_params) noexcept
		{
			if (a_params.objects.IsPlayer())
			{
				return MenuTopicManager::GetSingleton()->HasDialogueTarget();
			}
			else
			{
				return MenuTopicManager::GetSingleton()->talkingHandle == a_params.objects.GetHandle();
			}
		}

		bool is_cell_owner(
			CommonParams&            a_params,
			const CachedFactionData& a_cachedFactionData) noexcept
		{
			if (const auto ownerForm = a_params.get_parent_cell_owner())
			{
				switch (ownerForm->formType)
				{
				case TESFaction::kTypeID:
					{
						auto& data = a_cachedFactionData.GetFactionContainer();

						auto it = data.find(static_cast<TESFaction*>(ownerForm));
						if (it != data.end())
						{
							return it->second > -1;
						}
					}
					break;
				case TESNPC::kTypeID:
					return ownerForm == a_params.npc;
				}
			}

			return false;
		}

		BGSLightingTemplate* get_active_lighting_template(
			CommonParams& a_params) noexcept
		{
			const auto cell = a_params.actor->GetParentCell();
			if (!cell ||
			    !cell->IsInterior() ||
			    cell->cellFlags.test(
					TESObjectCELL::Flag::kShowSky |
					TESObjectCELL::Flag::kUseSkyLighting))
			{
				return nullptr;
			}

			if (a_params.objects.IsPlayer())
			{
				if (const auto rlt = a_params.controller.GetGlobalState().roomLightingTemplate)
				{
					return rlt;
				}
			}

			return cell->lightingTemplate;
		}

#if defined(IED_ENABLE_CONDITION_EN)
		bool enemies_nearby(CommonParams& a_params) noexcept
		{
			return a_params.objects.IsPlayer() &&
			       a_params.controller.PlayerHasEnemiesNearby();
		}
#endif
	}
}