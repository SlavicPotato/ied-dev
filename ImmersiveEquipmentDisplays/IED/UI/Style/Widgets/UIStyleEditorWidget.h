#pragma once

#include "IED/UI/Widgets/Filters/UIGenericFilter.h"

#include "IED/ImGuiStyleHolder.h"

namespace IED
{
	namespace UI
	{
		class UIStyleEditorWidget
		{
		public:
			UIStyleEditorWidget();

			bool DrawStyleEditor(Data::ImGuiStyleHolder& a_data);

		private:
			bool DrawTabBar(ImGuiStyle& a_data);
			bool DrawSizesTab(ImGuiStyle& a_data);
			bool DrawColorsTab(ImGuiStyle& a_data);
			bool DrawRenderingTab(ImGuiStyle& a_data);

			void DrawColorFilterTree();

			UIGenericFilter m_colorFilter;
		};
	}
}