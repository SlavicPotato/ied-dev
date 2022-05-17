#include "pch.h"

#include "UIFormBrowserCommonFilters.h"

namespace IED
{
	namespace UI
	{
		UIFormBrowserCommonFilters UIFormBrowserCommonFilters::m_Instance;

		UIFormBrowserCommonFilters::UIFormBrowserCommonFilters() :
			m_filters{ {

				std::make_unique<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>{
						TESFurniture::kTypeID }),

				std::make_unique<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESRace::kTypeID }),

				std::make_unique<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ Actor::kTypeID }),

				std::make_unique<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESNPC::kTypeID }),

				std::make_unique<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESQuest::kTypeID }),

				std::make_unique<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ BGSLocation::kTypeID }),

				std::make_unique<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ BGSKeyword::kTypeID }),

				std::make_unique<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESWorldSpace::kTypeID }),

				std::make_unique<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESPackage::kTypeID }),

				std::make_unique<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESShout::kTypeID }),

				std::make_unique<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESFaction::kTypeID }),

				std::make_unique<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESCombatStyle::kTypeID }),

				std::make_unique<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESClass::kTypeID }),

				std::make_unique<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ RE::TESWeather::kTypeID }),

				std::make_unique<
					UIFormBrowser::tab_filter_type,
					std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
					{ TESGlobal::kTypeID })

			} }
		{
		}
	}
}