#include "pch.h"

#include "UIFormTypeSelectorWidget.h"

namespace IED
{
	namespace UI
	{
		UIFormTypeSelectorWidget::data_type UIFormTypeSelectorWidget::m_data{ {

			{ TESObjectWEAP::kTypeID, UIFormTypeSelectorWidgetStrings::Weapon },
			{ TESObjectARMO::kTypeID, UIFormTypeSelectorWidgetStrings::Armor },
			{ TESNPC::kTypeID, UIFormTypeSelectorWidgetStrings::NPC },
			{ TESSoulGem::kTypeID, UIFormTypeSelectorWidgetStrings::SoulGem },
			{ TESKey::kTypeID, UIFormTypeSelectorWidgetStrings::Key },
			{ TESObjectMISC::kTypeID, UIFormTypeSelectorWidgetStrings::Misc },
			{ TESObjectLIGH::kTypeID, UIFormTypeSelectorWidgetStrings::Light },
			{ TESObjectBOOK::kTypeID, UIFormTypeSelectorWidgetStrings::Book },
			{ AlchemyItem::kTypeID, UIFormTypeSelectorWidgetStrings::PotionFood },
			{ IngredientItem::kTypeID, UIFormTypeSelectorWidgetStrings::Ingredient },
			{ ScrollItem::kTypeID, UIFormTypeSelectorWidgetStrings::Scroll },
			{ TESObjectSTAT::kTypeID, UIFormTypeSelectorWidgetStrings::Static },
			{ TESObjectTREE::kTypeID, UIFormTypeSelectorWidgetStrings::Tree },
			{ TESGrass::kTypeID, UIFormTypeSelectorWidgetStrings::Grass },
			{ BGSHeadPart::kTypeID, UIFormTypeSelectorWidgetStrings::HeadPart },
			{ TESAmmo::kTypeID, UIFormTypeSelectorWidgetStrings::Ammo },
			{ BGSKeyword::kTypeID, UIFormTypeSelectorWidgetStrings::Keyword },
			{ TESFlora::kTypeID, UIFormTypeSelectorWidgetStrings::Flora },
			{ TESFurniture::kTypeID, UIFormTypeSelectorWidgetStrings::Furniture },
			{ TESObjectACTI::kTypeID, UIFormTypeSelectorWidgetStrings::Activator },
			{ BGSTalkingActivator::kTypeID, UIFormTypeSelectorWidgetStrings::TalkingActivator },
			{ TESObjectREFR::kTypeID, UIFormTypeSelectorWidgetStrings::Reference },
			{ Actor::kTypeID, UIFormTypeSelectorWidgetStrings::Actor },
			{ TESQuest::kTypeID, UIFormTypeSelectorWidgetStrings::Quest },
			{ SpellItem::kTypeID, UIFormTypeSelectorWidgetStrings::Quest },
			{ TESRace::kTypeID, UIFormTypeSelectorWidgetStrings::Race },
			{ BGSArtObject::kTypeID, UIFormTypeSelectorWidgetStrings::ArtObject },
			{ BGSSoundDescriptorForm::kTypeID, UIFormTypeSelectorWidgetStrings::SoundDescriptor },
			{ TESObjectANIO::kTypeID, UIFormTypeSelectorWidgetStrings::AnimObject },
			{ TESObjectDOOR::kTypeID, UIFormTypeSelectorWidgetStrings::Door },
			{ BGSExplosion::kTypeID, UIFormTypeSelectorWidgetStrings::Explosion },
			{ BGSMaterialObject::kTypeID, UIFormTypeSelectorWidgetStrings::Material },
			{ BGSLocation::kTypeID, UIFormTypeSelectorWidgetStrings::Location },
			{ TESWorldSpace::kTypeID, UIFormTypeSelectorWidgetStrings::Worldspace },
			{ TESPackage::kTypeID, UIFormTypeSelectorWidgetStrings::Package },
			{ TESShout::kTypeID, UIFormTypeSelectorWidgetStrings::Shout },
			{ TESFaction::kTypeID, UIFormTypeSelectorWidgetStrings::Faction },
			{ TESCombatStyle::kTypeID, UIFormTypeSelectorWidgetStrings::CombatStyle },
			{ TESClass::kTypeID, UIFormTypeSelectorWidgetStrings::Class },
			{ RE::TESWeather::kTypeID, UIFormTypeSelectorWidgetStrings::Weather },
			{ TESGlobal::kTypeID, UIFormTypeSelectorWidgetStrings::Global },
			{ TESForm::kTypeID, UIFormTypeSelectorWidgetStrings::Form },

		} };

		UIFormTypeSelectorWidget::UIFormTypeSelectorWidget(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

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
				for (auto& [i, e] : m_data)
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
							LS<UIFormTypeSelectorWidgetStrings, 3>(e, "1"),
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
				return LS(UIFormTypeSelectorWidgetStrings::Weapon);
			case TESObjectARMO::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Armor);
			case TESNPC::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::NPC);
			case TESSoulGem::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::SoulGem);
			case TESKey::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Key);
			case TESObjectMISC::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Misc);
			case TESObjectLIGH::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Light);
			case TESObjectBOOK::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Book);
			case AlchemyItem::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::PotionFood);
			case IngredientItem::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Ingredient);
			case ScrollItem::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Scroll);
			case TESObjectSTAT::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Static);
			case BGSMovableStatic::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::MovableStatic);
			case TESObjectTREE::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Tree);
			case TESGrass::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Grass);
			case BGSHeadPart::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::HeadPart);
			case TESAmmo::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Ammo);
			case BGSKeyword::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Keyword);
			case TESFlora::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Flora);
			case TESFurniture::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Furniture);
			case TESObjectACTI::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Activator);
			case BGSTalkingActivator::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::TalkingActivator);
			case TESObjectREFR::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Reference);
			case Actor::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Actor);
			case TESQuest::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Quest);
			case SpellItem::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Spell);
			case TESRace::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Race);
			case BGSArtObject::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::ArtObject);
			case BGSSoundDescriptorForm::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::SoundDescriptor);
			case TESObjectANIO::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::AnimObject);
			case TESObjectDOOR::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Door);
			case BGSExplosion::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Explosion);
			case BGSMaterialObject::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Material);
			case BGSLocation::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Location);
			case TESWorldSpace::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Worldspace);
			case TESPackage::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Package);
			case TESShout::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Shout);
			case TESFaction::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Faction);
			case TESCombatStyle::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::CombatStyle);
			case TESClass::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Class);
			case RE::TESWeather::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Weather);
			case TESGlobal::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Global);
			case TESForm::kTypeID:
				return LS(UIFormTypeSelectorWidgetStrings::Form);
			default:
				return nullptr;
			}
		}
	}
}