#pragma once

#include "IED/CommonParams.h"
#include "IED/ConfigCommon.h"
#include "IED/FormCommon.h"
#include "IED/FormHolder.h"
#include "IED/Inventory.h"
#include "NodeOverrideData.h"

namespace IED
{
	namespace Data
	{
		struct configNodeOverride_t;
	}

	class ActorObjectHolder;
	class Controller;
	struct weapNodeEntry_t;
	struct cmeNodeEntry_t;

	class INodeOverride
	{
	public:
		struct armorInfoEntry_t
		{
			TESForm* item;
			Biped::BIPED_OBJECT bip{ Biped::BIPED_OBJECT::kNone };
			float totalWeightAdjust{ 0.0f };
			bool matched{ false };
		};

		struct nodeOverrideParamsArgs_t
		{
			TESNPC* npc;
			TESRace* race;
			NiNode* npcRoot;
			const ActorObjectHolder& objects;
			Controller& controller;
		};

		struct nodeOverrideParams_t :
			nodeOverrideParamsArgs_t,
			CommonParams
		{
		public:
			/*const Data::collectorData_t::container_type* data;
			std::unique_ptr<EquippedArmorCollector> collector;*/
			SetObjectWrapper<float> weaponAdjust;
			SetObjectWrapper<float> weightAdjust;
			SetObjectWrapper<bool> actorDead;
			std::unique_ptr<std::unordered_map<Game::FormID, armorInfoEntry_t>> itemData;
			SetObjectWrapper<Biped*> biped;
			SetObjectWrapper<TESObjectARMO*> actorSkin;

			//std::unique_ptr<stl::set<Game::FormID>> matchedArmors;
			//float matchedWeightAdjust{ 0.0f };

			/*SKMP_FORCEINLINE constexpr auto get_inventory_data()
			{
				if (!data)
				{
					collector = std::make_unique<EquippedArmorCollector>();
					collector->Run(actor);

					data = std::addressof(collector->m_data.forms);
				}

				return data;
			}*/

			SKMP_FORCEINLINE std::unordered_map<Game::FormID, armorInfoEntry_t>* get_item_data();

			SKMP_FORCEINLINE auto get_biped()
			{
				if (!biped)
				{
					if (auto ptrh = actor->GetBiped(false))
					{
						*biped = ptrh->ptr;
					}
					else
					{
						*biped = nullptr;
					}

					biped.mark(true);
				}

				return *biped;
			}

			SKMP_FORCEINLINE float get_weapon_adjust();
			//SKMP_FORCEINLINE float get_weapon_adjust(const stl::set<Game::FormID>* a_armors);

			SKMP_FORCEINLINE constexpr float get_weight_adjust()
			{
				if (!weightAdjust)
				{
					weightAdjust = (actor->GetWeight() * 0.01f) * 0.5f;
				}

				return *weightAdjust;
			}

			SKMP_FORCEINLINE constexpr bool get_actor_dead()
			{
				if (!actorDead)
				{
					actorDead = actor->IsDead();
				}

				return *actorDead;
			}

			SKMP_FORCEINLINE constexpr auto get_actor_skin() noexcept
			{
				if (!actorSkin)
				{
					actorSkin = Game::GetActorSkin(actor);
				}

				return *actorSkin;
			}

			SKMP_FORCEINLINE void clear_matched_items() noexcept
			{
				if (itemData)
				{
					for (auto& e : *itemData)
					{
						e.second.matched = false;
					}
				}
			}

			SKMP_FORCEINLINE float get_matched_weapon_adjust() const noexcept
			{
				float result = 0.0f;

				if (itemData)
				{
					for (auto& e : *itemData)
					{
						if (e.second.matched)
						{
							result += e.second.totalWeightAdjust;
						}
					}
				}

				return result;
			}
		};

	protected:
		static void ApplyNodeOverride(
			const cmeNodeEntry_t& a_entry,
			const Data::configNodeOverride_t& a_data,
			nodeOverrideParams_t& a_params);

		static void ResetNodeOverride(
			const cmeNodeEntry_t& a_entry);

		static void ApplyNodeVisibility(
			NiNode* a_node,
			const Data::configNodeOverride_t& a_data,
			nodeOverrideParams_t& a_params);

		static void ApplyNodePlacement(
			const Data::configNodeOverridePlacement_t& a_data,
			const weapNodeEntry_t& a_entry,
			nodeOverrideParams_t& a_params);

		static void ResetNodePlacement(
			const weapNodeEntry_t& a_entry,
			nodeOverrideParams_t& a_params);

	private:
		SKMP_FORCEINLINE static constexpr const stl::fixed_string& get_target_node(
			const Data::configNodeOverridePlacement_t& a_data,
			const weapNodeEntry_t& a_entry,
			nodeOverrideParams_t& a_params);

		static bool INodeOverride::process_offsets(
			const Data::configNodeOverrideOffsetList_t& a_data,
			NiTransform& a_out,
			NiPoint3& a_posAccum,
			nodeOverrideParams_t& a_params);
	};

}