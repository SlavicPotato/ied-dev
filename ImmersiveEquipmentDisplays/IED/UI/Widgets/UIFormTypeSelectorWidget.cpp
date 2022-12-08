#include "pch.h"

#include "UIFormTypeSelectorWidget.h"

namespace IED
{
	namespace UI
	{
		static constexpr auto s_data = stl::make_array<
			std::pair<
				std::uint8_t,
				UIFormTypeSelectorWidgetStrings>>(

			std::make_pair(TESObjectWEAP::kTypeID, UIFormTypeSelectorWidgetStrings::Weapon),
			std::make_pair(TESObjectARMO::kTypeID, UIFormTypeSelectorWidgetStrings::Armor),
			std::make_pair(TESNPC::kTypeID, UIFormTypeSelectorWidgetStrings::NPC),
			std::make_pair(TESSoulGem::kTypeID, UIFormTypeSelectorWidgetStrings::SoulGem),
			std::make_pair(TESKey::kTypeID, UIFormTypeSelectorWidgetStrings::Key),
			std::make_pair(TESObjectMISC::kTypeID, UIFormTypeSelectorWidgetStrings::Misc),
			std::make_pair(TESObjectLIGH::kTypeID, UIFormTypeSelectorWidgetStrings::Light),
			std::make_pair(TESObjectBOOK::kTypeID, UIFormTypeSelectorWidgetStrings::Book),
			std::make_pair(AlchemyItem::kTypeID, UIFormTypeSelectorWidgetStrings::PotionFood),
			std::make_pair(IngredientItem::kTypeID, UIFormTypeSelectorWidgetStrings::Ingredient),
			std::make_pair(ScrollItem::kTypeID, UIFormTypeSelectorWidgetStrings::Scroll),
			std::make_pair(TESObjectSTAT::kTypeID, UIFormTypeSelectorWidgetStrings::Static),
			std::make_pair(TESObjectTREE::kTypeID, UIFormTypeSelectorWidgetStrings::Tree),
			std::make_pair(TESGrass::kTypeID, UIFormTypeSelectorWidgetStrings::Grass),
			std::make_pair(BGSHeadPart::kTypeID, UIFormTypeSelectorWidgetStrings::HeadPart),
			std::make_pair(TESAmmo::kTypeID, UIFormTypeSelectorWidgetStrings::Ammo),
			std::make_pair(BGSKeyword::kTypeID, UIFormTypeSelectorWidgetStrings::Keyword),
			std::make_pair(TESFlora::kTypeID, UIFormTypeSelectorWidgetStrings::Flora),
			std::make_pair(TESFurniture::kTypeID, UIFormTypeSelectorWidgetStrings::Furniture),
			std::make_pair(TESObjectACTI::kTypeID, UIFormTypeSelectorWidgetStrings::Activator),
			std::make_pair(BGSTalkingActivator::kTypeID, UIFormTypeSelectorWidgetStrings::TalkingActivator),
			std::make_pair(TESObjectREFR::kTypeID, UIFormTypeSelectorWidgetStrings::Reference),
			std::make_pair(Actor::kTypeID, UIFormTypeSelectorWidgetStrings::Actor),
			std::make_pair(TESQuest::kTypeID, UIFormTypeSelectorWidgetStrings::Quest),
			std::make_pair(SpellItem::kTypeID, UIFormTypeSelectorWidgetStrings::Quest),
			std::make_pair(TESRace::kTypeID, UIFormTypeSelectorWidgetStrings::Race),
			std::make_pair(BGSArtObject::kTypeID, UIFormTypeSelectorWidgetStrings::ArtObject),
			std::make_pair(BGSSoundDescriptorForm::kTypeID, UIFormTypeSelectorWidgetStrings::SoundDescriptor),
			std::make_pair(TESObjectANIO::kTypeID, UIFormTypeSelectorWidgetStrings::AnimObject),
			std::make_pair(TESObjectDOOR::kTypeID, UIFormTypeSelectorWidgetStrings::Door),
			std::make_pair(BGSExplosion::kTypeID, UIFormTypeSelectorWidgetStrings::Explosion),
			std::make_pair(BGSMaterialObject::kTypeID, UIFormTypeSelectorWidgetStrings::Material),
			std::make_pair(BGSProjectile::kTypeID, UIFormTypeSelectorWidgetStrings::Material),
			std::make_pair(BGSLocation::kTypeID, UIFormTypeSelectorWidgetStrings::Location),
			std::make_pair(TESWorldSpace::kTypeID, UIFormTypeSelectorWidgetStrings::Worldspace),
			std::make_pair(TESPackage::kTypeID, UIFormTypeSelectorWidgetStrings::Package),
			std::make_pair(TESShout::kTypeID, UIFormTypeSelectorWidgetStrings::Shout),
			std::make_pair(TESFaction::kTypeID, UIFormTypeSelectorWidgetStrings::Faction),
			std::make_pair(EffectSetting::kTypeID, UIFormTypeSelectorWidgetStrings::Effect),
			std::make_pair(TESCombatStyle::kTypeID, UIFormTypeSelectorWidgetStrings::CombatStyle),
			std::make_pair(TESClass::kTypeID, UIFormTypeSelectorWidgetStrings::Class),
			std::make_pair(RE::TESWeather::kTypeID, UIFormTypeSelectorWidgetStrings::Weather),
			std::make_pair(TESGlobal::kTypeID, UIFormTypeSelectorWidgetStrings::Global),
			std::make_pair(TESIdleForm::kTypeID, UIFormTypeSelectorWidgetStrings::Idle),
			std::make_pair(BGSOutfit::kTypeID, UIFormTypeSelectorWidgetStrings::Outfit),
			std::make_pair(EnchantmentItem::kTypeID, UIFormTypeSelectorWidgetStrings::Enchantment),
			std::make_pair(TESForm::kTypeID, UIFormTypeSelectorWidgetStrings::Form)
		);

		bool UIFormTypeSelectorWidget::DrawFormTypeSelector(
			stl::optional<std::uint8_t>& a_type,
			filter_func_t                a_filter)
		{
			bool result = false;

			auto preview = a_type ?
			                   form_type_to_desc(*a_type) :
                               nullptr;

			if (ImGui::BeginCombo(
					"##ex_ft_sel",
					preview,
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& [i, e] : s_data)
				{
					if (a_filter && !a_filter(i))
					{
						continue;
					}

					ImGui::PushID(i);

					bool selected = (a_type == i);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(
							UIL::LS<UIFormTypeSelectorWidgetStrings, 3>(e, "1"),
							selected))
					{
						a_type = i;
						result = true;
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			return result;
		}

		const char* UIFormTypeSelectorWidget::form_type_to_desc(
			std::uint8_t a_type)
		{
			switch (a_type)
			{
			case TESObjectWEAP::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Weapon);
			case TESObjectARMO::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Armor);
			case TESNPC::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::NPC);
			case TESSoulGem::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::SoulGem);
			case TESKey::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Key);
			case TESObjectMISC::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Misc);
			case TESObjectLIGH::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Light);
			case TESObjectBOOK::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Book);
			case AlchemyItem::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::PotionFood);
			case IngredientItem::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Ingredient);
			case ScrollItem::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Scroll);
			case TESObjectSTAT::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Static);
			case BGSMovableStatic::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::MovableStatic);
			case TESObjectTREE::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Tree);
			case TESGrass::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Grass);
			case BGSHeadPart::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::HeadPart);
			case TESAmmo::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Ammo);
			case BGSKeyword::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Keyword);
			case TESFlora::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Flora);
			case TESFurniture::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Furniture);
			case TESObjectACTI::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Activator);
			case BGSTalkingActivator::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::TalkingActivator);
			case TESObjectREFR::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Reference);
			case Actor::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Actor);
			case TESQuest::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Quest);
			case SpellItem::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Spell);
			case TESRace::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Race);
			case BGSArtObject::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::ArtObject);
			case BGSSoundDescriptorForm::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::SoundDescriptor);
			case TESObjectANIO::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::AnimObject);
			case TESObjectDOOR::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Door);
			case BGSExplosion::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Explosion);
			case BGSProjectile::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Projectile);
			case BGSMaterialObject::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Material);
			case BGSLocation::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Location);
			case TESWorldSpace::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Worldspace);
			case TESPackage::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Package);
			case TESShout::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Shout);
			case TESFaction::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Faction);
			case EffectSetting::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Effect);
			case TESCombatStyle::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::CombatStyle);
			case TESClass::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Class);
			case RE::TESWeather::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Weather);
			case TESGlobal::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Global);
			case TESIdleForm::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Idle);
			case BGSOutfit::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Outfit);
			case EnchantmentItem::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Enchantment);
			case TESForm::kTypeID:
				return UIL::LS(UIFormTypeSelectorWidgetStrings::Form);
			default:
				return nullptr;
			}
		}
	}
}