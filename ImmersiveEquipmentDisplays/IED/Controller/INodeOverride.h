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
		struct configNodeOverrideTransform_t;
	}

	class ActorObjectHolder;
	class Controller;
	struct weapNodeEntry_t;
	struct cmeNodeEntry_t;

	class INodeOverride
	{
	public:
		struct bipedInfoEntry_t
		{
			TESForm* item;
			Biped::BIPED_OBJECT bip{ Biped::BIPED_OBJECT::kNone };
			float weaponAdjust{ 0.0f };
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
			using item_container_type = std::unordered_map<Game::FormID, bipedInfoEntry_t>;

			SetObjectWrapper<float> weaponAdjust;
			SetObjectWrapper<float> weightAdjust;
			SetObjectWrapper<bool> actorDead;
			std::unique_ptr<item_container_type> itemData;
			SetObjectWrapper<Biped*> biped;
			SetObjectWrapper<TESObjectARMO*> actorSkin;
			SetObjectWrapper<bool> bipedHasArmor;

			auto get_biped_has_armor()
			{
				if (!bipedHasArmor)
				{
					bipedHasArmor = equipped_armor_visitor([](auto*) { return true; });
				}

				return *bipedHasArmor;
			}

			std::unordered_map<Game::FormID, bipedInfoEntry_t>* get_item_data();

			auto get_biped()
			{
				if (!biped)
				{
					if (auto ptrh = actor->GetBiped(false))
					{
						biped = ptrh->ptr;
					}
					else
					{
						biped = nullptr;
					}
				}

				return *biped;
			}

			float get_weapon_adjust();
			//SKMP_FORCEINLINE float get_weapon_adjust(const stl::set<Game::FormID>* a_armors);

			constexpr float get_weight_adjust()
			{
				if (!weightAdjust)
				{
					weightAdjust = (actor->GetWeight() * 0.01f) * 0.5f;
				}

				return *weightAdjust;
			}

			constexpr bool get_actor_dead()
			{
				if (!actorDead)
				{
					actorDead = actor->IsDead();
				}

				return *actorDead;
			}

			constexpr auto get_actor_skin() noexcept
			{
				if (!actorSkin)
				{
					actorSkin = Game::GetActorSkin(actor);
				}

				return *actorSkin;
			}

			void clear_matched_items() noexcept
			{
				if (itemData)
				{
					for (auto& e : *itemData)
					{
						e.second.matched = false;
					}
				}
			}

			float get_matched_weapon_adjust() const noexcept
			{
				float result = 0.0f;

				if (itemData)
				{
					for (auto& e : *itemData)
					{
						if (e.second.matched)
						{
							result += e.second.weaponAdjust;
						}
					}
				}

				return result;
			}

			template <class Tf>
			bool equipped_armor_visitor(
				Tf a_func)
			{
				auto biped = get_biped();
				if (!biped)
				{
					return false;
				}

				auto skin = get_actor_skin();

				using enum_type = std::underlying_type_t<Biped::BIPED_OBJECT>;

				for (enum_type i = Biped::kHead; i < Biped::kEditorTotal; i++)
				{
					if (is_av_ignored_slot(i))
					{
						continue;
					}

					auto& e = biped->objects[i];

					if (e.item &&
					    e.item != e.addon &&
					    e.item != skin &&
					    e.item->IsArmor())
					{
						if (a_func(e.item))
						{
							return true;
						}
					}
				}

				return false;
			}

		private:
			inline static constexpr bool is_av_ignored_slot(
				std::underlying_type_t<Biped::BIPED_OBJECT> a_slot) noexcept
			{
				switch (a_slot)
				{
				case Biped::kHair:
				case Biped::kShield:
				case Biped::kTail:
				case Biped::kLongHair:
				case Biped::kDecapitateHead:
				case Biped::kDecapitate:
				case Biped::kFX01:
					return true;
				default:
					return false;
				}
			}
		};

		static void ResetNodeOverride(
			const cmeNodeEntry_t& a_entry);

	protected:
		static void ApplyNodeOverride(
			const cmeNodeEntry_t& a_entry,
			const Data::configNodeOverrideTransform_t& a_data,
			nodeOverrideParams_t& a_params);

		static void ApplyNodeVisibility(
			NiNode* a_node,
			const Data::configNodeOverrideTransform_t& a_data,
			nodeOverrideParams_t& a_params);

		static void ApplyNodePlacement(
			const Data::configNodeOverridePlacement_t& a_data,
			const weapNodeEntry_t& a_entry,
			nodeOverrideParams_t& a_params);

		static void ResetNodePlacement(
			const weapNodeEntry_t& a_entry,
			nodeOverrideParams_t& a_params);

	private:
		static constexpr const stl::fixed_string& get_target_node(
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