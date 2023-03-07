#include "pch.h"

#include "OutfitController.h"

#include "ConfigStoreOutfit.h"

#include "IED/Controller/ActorObjectHolder.h"
#include "IED/ProcessParams.h"

#if defined(IED_ENABLE_OUTFIT_FORM_MANAGER)
#	include "PersistentOutfitFormManager.h"
#endif

namespace IED
{
	namespace OM
	{
		OutfitController::OutfitController()
		{
		}

		void OutfitController::EvaluateOutfit(
			const Data::OM::configStoreOutfit_t& a_config,
			IED::processParams_t&                a_params) noexcept
		{
			if (!a_params.objects.IsActive())
			{
				return;
			}

			auto& config = a_config.GetNPC(
									   a_params.npcOrTemplate->formID,
									   a_params.race->formID)
			                   .data.get(a_params.configSex)
			                   .get_config(a_params);

			TrySetOutfit(a_params, config);
		}

		void OutfitController::TrySetOutfit(
			IED::processParams_t&           a_params,
			const Data::OM::configOutfit_t& a_config) noexcept
		{
			const auto outfit = GetOutfit(a_params, a_config);
			if (!outfit)
			{
				return;
			}

			const auto actor = a_params.actor;
			const auto npc   = a_params.npc;

			Game::RemoveOutfitItems(actor, npc->defaultOutfit);
			npc->SetDefaultOutfit(outfit);

			Game::unk1D8D10(actor, false);

			const auto currentFrame         = *Game::g_frameCounter;
			const auto lastQueuedEquipFrame = a_params.objects.GetLastQueuedOutfitEquipFrame();

			const bool backgroundEquip = (currentFrame != lastQueuedEquipFrame) && !outfit->outfitItems.empty();

			if (backgroundEquip)
			{
				a_params.objects.SetLastQueuedOutfitEquipFrame(currentFrame);
			}

			Game::EquipOutfit(actor, outfit, backgroundEquip);
		}

		BGSOutfit* OutfitController::GetOutfit(
			IED::processParams_t&           a_params,
			const Data::OM::configOutfit_t& a_config) noexcept
		{
#if defined(IED_ENABLE_OUTFIT_FORM_MANAGER)
			if (a_config.flags.test(Data::OM::ConfigOutfitFlags::kUseID))
			{
				if (a_config.id.empty())
				{
					return nullptr;
				}

				const auto holder = PersistentOutfitFormManager::GetSingleton().GetHolder(a_config.id);

				if (!holder)
				{
					return nullptr;
				}

				const auto currentOutfit = a_params.npc->defaultOutfit;
				const auto form          = holder->get();

				if (currentOutfit &&
				    currentOutfit == form)
				{
					return nullptr;
				}

				return form;
			}
			else
			{
#endif
				if (!a_config.outfit)
				{
					return nullptr;
				}

				const auto currentOutfit = a_params.npc->defaultOutfit;

				if (currentOutfit && 
					currentOutfit->formID == a_config.outfit)
				{
					return nullptr;
				}

				return a_config.outfit.Lookup<BGSOutfit>();

#if defined(IED_ENABLE_OUTFIT_FORM_MANAGER)
			}
#endif
		}
	}
}