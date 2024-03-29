#include "pch.h"

#include "UIFormBrowserCommonFilters.h"

#include "UIAllowedModelTypes.h"

namespace IED
{
	namespace UI
	{
		const UIFormBrowserCommonFilters UIFormBrowserCommonFilters::m_Instance;

		UIFormBrowserCommonFilters::UIFormBrowserCommonFilters() :
			m_filters{

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESFurniture::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESRace::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ Actor::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESNPC::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESQuest::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ BGSLocation::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ BGSKeyword::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESWorldSpace::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESPackage::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESShout::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESFaction::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESCombatStyle::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESClass::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ RE::TESWeather::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESGlobal::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESIdleForm::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ EffectSetting::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ SpellItem::kTypeID,
			          TESObjectARMO::kTypeID,
			          IFormDatabase::EXTRA_TYPE_ARMOR }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ BGSPerk::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ BGSEquipSlot::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESObjectCELL::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESShout::kTypeID,
			          SpellItem::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ BGSLightingTemplate::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ BGSOutfit::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ IFormDatabase::EXTRA_TYPE_ARMOR,
			          TESLevItem::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESObjectWEAP::kTypeID,
			          TESObjectARMO::kTypeID,
			          TESObjectLIGH::kTypeID,
			          ScrollItem::kTypeID,
			          SpellItem::kTypeID }),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESObjectWEAP::kTypeID,
			          TESObjectARMO::kTypeID,
			          TESAmmo::kTypeID,
			          TESObjectLIGH::kTypeID,
			          TESObjectMISC::kTypeID,
			          IngredientItem::kTypeID,
			          AlchemyItem::kTypeID,
			          TESKey::kTypeID,
			          TESObjectBOOK::kTypeID,
			          TESSoulGem::kTypeID,
			          ScrollItem::kTypeID,
			          SpellItem::kTypeID,
			          IFormDatabase::EXTRA_TYPE_ARMOR }),

				std::make_shared<
					UIFormBrowser::tab_filter_type>(g_allowedModelTypes),

				std::make_shared<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ BGSVoiceType::kTypeID }),

			}
		{
		}
	}
}