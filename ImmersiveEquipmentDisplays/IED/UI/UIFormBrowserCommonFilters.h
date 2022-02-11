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
			Keyword
		};

		class UIFormBrowserCommonFilters
		{
		public:
			[[nodiscard]] inline static constexpr const auto& Get(UIFormBrowserFilter a_id)
			{
				return m_Instance.m_filters[stl::underlying(a_id)];
			}

		private:
			UIFormBrowserCommonFilters();

			std::array<std::shared_ptr<const UIFormBrowser::tab_filter_type>, 7> m_filters;

			static UIFormBrowserCommonFilters m_Instance;
		};

	}
}