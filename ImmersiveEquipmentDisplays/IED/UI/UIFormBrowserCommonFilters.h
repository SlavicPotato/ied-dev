#pragma once

#include "UIFormBrowser.h"

namespace IED
{
	namespace UI
	{
		enum class UIFormBrowserFilter : std::uint32_t
		{
			Furniture,
			Race,
			Actor,
			NPC,
			Quest,
			Location,
			Keyword,
			Worldspace,
			Package,
			Shout,
			Faction,
			CombatStyle,
			Class,
			Weather,
			Global,
			Idle,
			Effect,
			EffectSource,
			Perk,
			EquipSlot,
			Cell,
			ShoutAndSpell,
			LightingTemplate,
			Outfit,
			OutfitItems,
			HandEquippable,
			Common,
			ModelTypes,
		};

		class UIFormBrowserCommonFilters
		{
		public:
			[[nodiscard]] static constexpr auto& Get(UIFormBrowserFilter a_id) noexcept
			{
				return m_Instance.m_filters[stl::underlying(a_id)];
			}

		private:
			UIFormBrowserCommonFilters();

			const std::array<std::shared_ptr<const UIFormBrowser::tab_filter_type>, 28> m_filters;

			static const UIFormBrowserCommonFilters m_Instance;
		};

	}
}